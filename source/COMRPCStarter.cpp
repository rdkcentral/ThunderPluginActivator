/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 Metrological
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "COMRPCStarter.h"

#include "Log.h"

#include <chrono>
#include <thread>
#include <inttypes.h>

void COMRPCStarter::PluginActivatorCallback::Finished(const string& callsign, const Exchange::IPluginAsyncStateControl::IActivationCallback::state state, const uint8_t numberofretries) 
{
    LOG_INF(callsign.c_str(), "Plugin activation async result received %u, retries %u", static_cast<uint8_t>(state), numberofretries);
    _resultpromise.set_value(state);
}

COMRPCStarter::COMRPCStarter(const string& pluginName)
    : IPluginStarter()
#ifdef __DEBUG__
    , _timeoutvalue(1000) // also for debug you don't want this to be forever
#else
    , _timeoutvalue(RPC::CommunicationTimeOut)
#endif
    , _connector(_timeoutvalue)
    , _pluginName(pluginName)
{  
}

/**
 * @brief Attempt to activate the plugin and automatically retry on failure
 *
 * @param[in]   maxRetries                  Maximum amount of times to retry activation if it fails
 * @param[in]   retryDelayMs                Delay in ms between retry attempts
 * @param[in]   pluginActivatorCallsign     Callsign of the plugin that implements IPluginAsyncStateControl and will be used for requests
 *
 * @return True if plugin successfully activated, false if failed to activate (or was aborted)
 */

bool COMRPCStarter::activatePlugin(const uint8_t maxRetries, const uint16_t retryDelayMs, const string& pluginActivatorCallsign)
{
    // Attempt to open the plugin shell
    bool success = false;
    bool retry = true;
    int currentRetry = 0;

    while ((retry == true) && (currentRetry <= maxRetries)) { // first attempt not included, that is not a retry...
        LOG_INF(_pluginName.c_str(), "Attempting to activate plugin - attempt %d/%d", currentRetry, maxRetries);

        Core::StopWatch stopwatch;

        if (_connector.IsOperational() == false) {
            uint32_t result = _connector.Open(_timeoutvalue, ControllerConnector::Connector());
            if (result != Core::ERROR_NONE) {
                LOG_ERROR(_pluginName.c_str(), "Failed to get controller interface, error %u (%s)", result, Core::ErrorToString(result));
                // Sleep, then try again
                currentRetry++;
                SleepMs(retryDelayMs);
                continue;
            }
        }

        PluginHost::IShell* controller = _connector.ControllerInterface();
        ASSERT(controller != nullptr);
        Exchange::IPluginAsyncStateControl* asyncpluginstarter = controller->QueryInterfaceByCallsign<Exchange::IPluginAsyncStateControl>(pluginActivatorCallsign);
        controller->Release();
        controller = nullptr;

        if (asyncpluginstarter == nullptr) {
            LOG_ERROR(_pluginName.c_str(), "Failed to get IPluginAsyncStateControl interface, will retry after %dms", retryDelayMs);
            currentRetry++;

            // Sleep, then try again
            SleepMs(retryDelayMs);
        } else {
            PluginActivatorCallback::PluginActivatorPromise pluginActivateAsyncResultPromise;
            std::future<Exchange::IPluginAsyncStateControl::IActivationCallback::state> pluginActivateAsyncResultFuture = pluginActivateAsyncResultPromise.get_future();       
            Core::SinkType<PluginActivatorCallback> sink(std::move(pluginActivateAsyncResultPromise));
            Core::OptionalType<uint8_t> retries(maxRetries - currentRetry);
            Core::OptionalType<uint16_t> delay(retryDelayMs);

            Core::hresult result = asyncpluginstarter->Activate(_pluginName, retries, delay, &sink);

            if (result == Core::ERROR_NONE) {
                LOG_INF(_pluginName.c_str(), "Plugin activation async request sent, waiting for result");

                Exchange::IPluginAsyncStateControl::IActivationCallback::state resultState = pluginActivateAsyncResultFuture.get();
                success = (resultState == Exchange::IPluginAsyncStateControl::IActivationCallback::state::SUCCESS);

                auto duration = stopwatch.Elapsed() / Core::Time::TicksPerMillisecond;

                if (success == true) {
                    // Our work here is done!
                    LOG_INF(_pluginName.c_str(), "Successfully activated plugin after %" PRIu64 "ms", duration);
                    retry = false;
                }
                else {
                    if (resultState == Exchange::IPluginAsyncStateControl::IActivationCallback::state::FAILURE) {
                        LOG_ERROR(_pluginName.c_str(), "Failed to activate plugin after %" PRIu64 "ms", duration);
                    }
                    else {
                        LOG_ERROR(_pluginName.c_str(), "Activate of plugin aborted (explicitly or implicitly, e.g. due to IPluginAsyncStateControl plugin shutdown) after %" PRIu64 "ms", duration);
                    }
                    retry = false; // do not retry, that is what the IPluginAsyncStateControl already did...
                }

                sink.WaitReleased(RPC::CommunicationTimeOut);
            }
            else if((result & COM_ERROR) != 0) { // we have a COM error, let's retry, connection might be down
                auto duration = stopwatch.Elapsed() / Core::Time::TicksPerMillisecond;
                LOG_ERROR(_pluginName.c_str(), "Failed to send activate plugin request, COM error code %u (%s) after %" PRIu64 "ms", result, Core::ErrorToString(result), duration);

                currentRetry++;
                // Sleep, then try again
                SleepMs(retryDelayMs);
            }
            else {
                auto duration = stopwatch.Elapsed() / Core::Time::TicksPerMillisecond;
                switch (result) {
                    case Core::ERROR_INPROGRESS :
                        LOG_ERROR(_pluginName.c_str(), "Activate plugin request failed, activation request already being handled (from other PluginActivator?), error code %u (%s) after %" PRIu64 "ms", result, Core::ErrorToString(result), duration);
                        break;
                    case Core::ERROR_ILLEGAL_STATE:
                        LOG_ERROR(_pluginName.c_str(), "Activate plugin request failed, plugin is in an invalid state to be started, error code %u (%s) after %" PRIu64 "ms", result, Core::ErrorToString(result), duration);
                        break;
                    case Core::ERROR_NOT_EXIST:
                        LOG_ERROR(_pluginName.c_str(), "Activate plugin request failed, plugin is unknown to Thunder, error code %u (%s) after %" PRIu64 "ms", result, Core::ErrorToString(result), duration);
                        break;
                    default:
                        LOG_ERROR(_pluginName.c_str(), "Activate plugin request failed for unexpected reason, error code %u (%s) after %" PRIu64 "ms", result, Core::ErrorToString(result), duration);
                        break;
                }
                // for the above does not make sense to try again...
                retry = false;
            }

            asyncpluginstarter->Release();
            asyncpluginstarter = nullptr;
        }
    }

    if (!success) {
        LOG_ERROR(_pluginName.c_str(), "Max retries hit or startup aborted - giving up activating the plugin");
    }

    if (_connector.IsOperational() == true) {
        _connector.Close(_timeoutvalue);
    }

    return success;
}
