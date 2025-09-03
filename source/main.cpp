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

#define PROCESS_NAME "WPEFramework"

static int gRetryCount = 100;
static int gRetryDelayMs = 500;
static string gPluginName;
static int gLogLevel = LEVEL_INFO;

static bool gDeactivate = false;

/**
 * @brief Display a help message for the tool
 */
static void displayUsage()
{
    printf("Usage: PluginActivator <option(s)> [callsign]\n");
    printf("    Utility that starts a given thunder plugin\n\n");
    printf("    -h, --help          Print this help and exit\n");
    printf("    -r, --retries       Maximum amount of retries to attempt to start the plugin before giving up\n");
    printf("    -d, --delay         Delay (in ms) between each attempt to start the plugin if it fails\n");
    printf("    -v, --verbose       Increase log level\n");
    printf("    -x, --deactivate    Deactivate the plugin instead of activating\n");
    printf("\n");
    printf("    [callsign]          Callsign of the plugin to activate (Required)\n");
}

/**
 * @brief Parse the provided command line arguments
 *
 * Must be given the name of the plugin to activate, everything else
 * is optional and will fallback to sane defaults
 */
static void parseArgs(const int argc, char** argv)
{
    if (argc == 1) {
        displayUsage();
        exit(EXIT_SUCCESS);
    }

    struct option longopts[] = {
        { "help", no_argument, nullptr, (int)'h' },
        { "retries", required_argument, nullptr, (int)'r' },
        { "delay", required_argument, nullptr, (int)'d' },
        { "verbose", no_argument, nullptr, (int)'v' },
        { "deactivate", no_argument, nullptr, (int)'x' },
        { nullptr, 0, nullptr, 0 }
    };

    opterr = 0;

    int option;
    int longindex;

    while ((option = getopt_long(argc, argv, "hr:d:v:x", longopts, &longindex)) != -1) {
        switch (option) {
        case 'h':
            displayUsage();
            exit(EXIT_SUCCESS);
            break;
        case 'r':
            gRetryCount = std::atoi(optarg);
            if (gRetryCount < 0) {
                fprintf(stderr, "Error: Retry count must be > 0\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'd':
            gRetryDelayMs = std::atoi(optarg);
            if (gRetryDelayMs < 0) {
                fprintf(stderr, "Error: Delay ms must be > 0\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'v':
            if (gLogLevel < LEVEL_DEBUG) {
                gLogLevel++;
            }
            break;
        case 'x':
            gDeactivate = true;
            break;
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Warning: Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Warning: Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Warning: Unknown option character `\\x%x'.\n", optopt);

            exit(EXIT_FAILURE);
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (optind == argc) {
        fprintf(stderr, "Error: Must provide plugin name to activate\n");
        exit(EXIT_FAILURE);
    }

    gPluginName = argv[optind];

    optind++;
    for (int i = optind; i < argc; i++) {
        printf("Warning: Non-option argument %s ignored\n", argv[i]);
    }
}


uint32_t getPID(const char* processName)
{
    char command[128] = {};
    char buffer[128] = {};
    uint32_t pid = 0;

    snprintf(command, sizeof(command), "pidof %s", processName);
    FILE *fp = popen(command, "r");
    if (!fp) {
        fprintf(stderr, "popen for pidof %s failed\n", processName);
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        pid = (uint32_t) atoi(buffer);
    }
    pclose(fp);
    return pid;
}

bool isRunning( uint32_t pid)
{
    int rc = kill(pid, 0);
    if(rc == 0 && pid != -1)
        return true;
    else
        return false;
}

int main(int argc, char* argv[])
{
    parseArgs(argc, argv);

    initLogging(gLogLevel);

    // Check if either WPEFramework or Thunder is running before activating or deactivating
    uint32_t pid = getPID("WPEFramework");
    if (!isRunning(pid))
        pid = getPID("Thunder");

    if (!isRunning(pid)) {
        fprintf(stderr, "Neither WPEFramework nor Thunder is running. Cannot activate/deactivate plugin.\n");
        return 0;
    }

    // For now, we only implement the starter in COM-RPC but could do a JSON-RPC version
    // in the future
    bool success;

    {
        auto starter = std::unique_ptr<IPluginStarter>(new COMRPCStarter(gPluginName));
        // Explicitly check for deactivate or activate
        if (gDeactivate) {
            success = starter->deactivatePlugin(gRetryCount, gRetryDelayMs);
        } else {
            success = starter->activatePlugin(gRetryCount, gRetryDelayMs);
        }
    }

    Core::Singleton::Dispose();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
