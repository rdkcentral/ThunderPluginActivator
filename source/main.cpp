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

#include "Module.h"

#include "Log.h"
#include "COMRPCStarter.h"
#include <memory>

static int gRetryCount = 10;
static int gRetryDelayMs = 500;
static string gPluginName;
//static int gLogLevel = LEVEL_INFO;
static int gLogLevel = LEVEL_DEBUG;
static string gPluginActivatorCallsign{ "PluginInitializerService" };

/**
 * @brief Display a help message for the tool
 */
static void displayUsage()
{
    printf("Usage: PluginActivator <option(s)> [callsign]\n");
    printf("    Utility that starts a given Thunder plugin\n\n");
    printf("    -h, --help                Print this help and exit\n");
    printf("    -r, --retries             Maximum amount of retries to attempt to start the plugin before giving up\n");
    printf("    -d, --delay               Delay (in ms) between each attempt to start the plugin if it fails\n");
    printf("    -p, --plugininitcallsign  Callsign of the PluginInitializerService (default: PluginInitializerService)\n");
    printf("    -v, --verbose             Increase log level\n");
    printf("\n");
    printf("    [callsign]                Callsign of the plugin to activate (Required)\n");
}

/**
 * @brief Parse the provided command line arguments
 *
 * Must be given the name of the plugin to activate, everything else
 * is optional and will fallback to sane defaults
 */
static bool parseArgs(const int argc, char** argv)
{
    bool success = true;
    if (argc == 1) {
        displayUsage();
        success = false;
    }
    else {

        /*    struct option longopts[] = {
                { "help", no_argument, nullptr, (int)'h' },
                { "retries", required_argument, nullptr, (int)'r' },
                { "delay", required_argument, nullptr, (int)'d' },
                { "plugininitcallsign", required_argument, nullptr, (int)'p' },
                { "verbose", no_argument, nullptr, (int)'v' },
                { nullptr, 0, nullptr, 0 }
            };

            opterr = 0;

            int option;
            int longindex;

            while ((option = getopt_long(argc, argv, "hr:d:p:v", longopts, &longindex)) != -1) {
                switch (option) {
                case 'h':
                    displayUsage();
                    success = false;
                    break;
                case 'r':
                    gRetryCount = std::atoi(optarg);
                    if (gRetryCount < 0) {
                        fprintf(stderr, "Error: Retry count must be > 0\n");
                        success = false;
                    }
                    break;
                case 'd':
                    gRetryDelayMs = std::atoi(optarg);
                    if (gRetryDelayMs < 0) {
                        fprintf(stderr, "Error: Delay ms must be > 0\n");
                        success = false;
                    }
                    break;
                case 'p':
                    gpluginactivatorcallsign = optarg;
                    break;
                case 'v':
                   if (gLogLevel < LEVEL_DEBUG) {
                        gLogLevel++;
                    }                    
                    break;
                case '?':
                    if (optopt == 'c')
                        fprintf(stderr, "Warning: Option -%c requires an argument.\n", optopt);
                    else if (isprint(optopt))
                        fprintf(stderr, "Warning: Unknown option `-%c'.\n", optopt);
                    else
                        fprintf(stderr, "Warning: Unknown option character `\\x%x'.\n", optopt);

                    success = false;
                    break;
                default:
                    success = false;
                    break;
                }
            }

            if ((success == true) && (optind == argc)) {
                fprintf(stderr, "Error: Must provide plugin name to activate\n");
                success = false;
            } else {

                gPluginName = argv[optind];

                optind++;
                for (int i = optind; i < argc; i++) {
                    printf("Warning: Non-option argument %s ignored\n", argv[i]);
                }

                success = true;
            }

            */

        // huppel to remove
        gPluginName = argv[1];
    }

    return success;
}

int main(int argc, char* argv[])
{
    bool success = false;
    if (parseArgs(argc, argv) == true) {

        initLogging(gLogLevel);

        // For now, we only implement the starter in COM-RPC but could do a JSON-RPC version
        // in the future
#if 0
        {
            auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter(gPluginName));
            success = starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        }

    }
#endif


    std::thread t1([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t2([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary1"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t3([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary2"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t4([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary3"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t5([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary4"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t6([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary5"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t7([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary6"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t8([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary7"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });
    std::thread t9([]() {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter("Dictionary8"));
        starter->activatePlugin(gRetryCount, gRetryDelayMs, gPluginActivatorCallsign);
        });

    t1.join();


    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();

    }

    Core::Singleton::Dispose();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}