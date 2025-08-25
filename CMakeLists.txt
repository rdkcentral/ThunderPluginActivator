# If not stated otherwise in this file or this component's license file the
# following copyright and licenses apply:
#
# Copyright 2020 Metrological
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.10.3)

project(PluginActivator)

set(CMAKE_CXX_STANDARD 11)

find_package(WPEFramework)

find_package(${NAMESPACE}Core REQUIRED)
find_package(${NAMESPACE}COM REQUIRED)
find_package(${NAMESPACE}Plugins REQUIRED)
find_package(CompileSettingsDebug CONFIG REQUIRED)

add_executable(PluginActivator
    source/Module.cpp
    source/Log.cpp
    source/main.cpp
    source/COMRPCStarter.cpp
)

target_link_libraries(PluginActivator
    PRIVATE
    ${NAMESPACE}Core::${NAMESPACE}Core
    ${NAMESPACE}COM::${NAMESPACE}COM
    ${NAMESPACE}Plugins::${NAMESPACE}Plugins
    CompileSettingsDebug::CompileSettingsDebug
)

target_compile_options(PluginActivator
    PRIVATE
    -Wall -Wextra
)

install(
    TARGETS PluginActivator
    RUNTIME DESTINATION bin
)
