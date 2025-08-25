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

#include "IPluginStarter.h"

using namespace WPEFramework;

/**
 * @brief COM-RPC implementation of a plugin starter
 *
 * Connects to Thunder over COM-RPC and attempts to start a given plugin
 */
class COMRPCStarter : public IPluginStarter {
public:
    explicit COMRPCStarter(const string& pluginName);
    ~COMRPCStarter() override = default;

    bool activatePlugin(const uint8_t maxRetries, const uint16_t retryDelayMs) override;
    bool deactivatePlugin(const uint8_t maxRetries, const uint16_t retryDelayMs) override;

private:
    using ControllerConnector = RPC::SmartControllerInterfaceType<Exchange::Controller::ILifeTime>;

private:
    ControllerConnector _connector;
    const string _pluginName;
};
