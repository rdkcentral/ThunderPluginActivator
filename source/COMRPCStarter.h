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

#pragma once
#include "Module.h"

#include <interfaces/IPluginAsyncStateControl.h>

#include "IPluginStarter.h"

#include <future>

using namespace Thunder;

/**
 * @brief COM-RPC implementation of a plugin starter
 *
 * Connects to Thunder over COM-RPC and attempts to start a given plugin
 */
class COMRPCStarter : public IPluginStarter {
public:
    explicit COMRPCStarter(const string& pluginName);
    ~COMRPCStarter() override = default;

    bool activatePlugin(const uint8_t maxRetries, const uint16_t retryDelayMs, const string& pluginActivatorCallsign) override;

private:
    using ControllerConnector = RPC::SmartControllerInterfaceType<Exchange::Controller::ILifeTime>; 

    class PluginActivatorCallback : public Exchange::IPluginAsyncStateControl::IActivationCallback {
    public:
        using PluginActivatorPromise = std::promise<Exchange::IPluginAsyncStateControl::IActivationCallback::state>;

        explicit PluginActivatorCallback(PluginActivatorPromise&& resultpromise) : _resultpromise(std::move(resultpromise)) {}
        ~PluginActivatorCallback() override = default;

        PluginActivatorCallback(const PluginActivatorCallback&) = delete;
        PluginActivatorCallback& operator=(const PluginActivatorCallback&) = delete;
        PluginActivatorCallback (PluginActivatorCallback&&) = delete;
        PluginActivatorCallback& operator=(PluginActivatorCallback&&) = delete;

        BEGIN_INTERFACE_MAP(PluginActivatorCallback)
        INTERFACE_ENTRY(Exchange::IPluginAsyncStateControl::IActivationCallback)
        END_INTERFACE_MAP

        void Finished(const string& callsign, const Exchange::IPluginAsyncStateControl::IActivationCallback::state state, const uint8_t numberofretries) override;

    private:
        PluginActivatorPromise _resultpromise;
    };

private:
    const uint32_t _timeoutvalue; // note order is important here
    ControllerConnector _connector;
    const string _pluginName;
};