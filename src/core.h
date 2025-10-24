#pragma once

#ifndef CORE_H
#define CORE_H

// Version of the build system
#define VERSION "1.0.3.1"

// Function to get the version string
const char* get_version();

typedef enum
{
    S_OK,
    S_ERROR_RESOURCE_NOT_FOUND,
    S_ERROR_INVALID_ARGUMENT,
    S_BUILD_FAILED,
    S_INVALID_CONFIGURATION,
    S_GIT_ERROR,
    S_FILE_OPERATION_FAILED,
    S_DIRECTORY_OPERATION_FAILED,
    S_COMMAND_EXECUTION_FAILED,
    S_FAILURE,
    S_TARGET_NOT_FOUND

} E_RESULT;

#endif