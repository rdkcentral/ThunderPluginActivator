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

include(FindPackageHandleStandardArgs)
include(GNUInstallDirs)

find_path(ThunderExtensions_INCLUDE_DIR
    NAMES extensions/IPluginAsyncStateControl.h
    PATHS
        "${CMAKE_SYSROOT}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${NAMESPACE}"
        "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${NAMESPACE}"
        "${CMAKE_SYSROOT}/usr/${CMAKE_INSTALL_INCLUDEDIR}/${NAMESPACE}"
        "${CMAKE_SYSROOT}/${CMAKE_INSTALL_INCLUDEDIR}/${NAMESPACE}"
    PATH_SUFFIXES
        "${CMAKE_INSTALL_INCLUDEDIR}/${NAMESPACE}"
)

find_package_handle_standard_args(ThunderExtensions
    REQUIRED_VARS ThunderExtensions_INCLUDE_DIR
)

if(ThunderExtensions_FOUND AND NOT TARGET ThunderExtensions::ThunderExtensions)
    add_library(ThunderExtensions::ThunderExtensions INTERFACE IMPORTED)
    set_target_properties(ThunderExtensions::ThunderExtensions PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ThunderExtensions_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(ThunderExtensions_INCLUDE_DIR)