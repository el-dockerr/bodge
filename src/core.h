#pragma once

#ifndef CORE_H
#define CORE_H

// Version of the build system
#define VERSION "0.0.4"

// Function to get the version string
const char* get_version();

typedef enum
{
    S_OK,
    S_ERROR_RESOURCE_NOT_FOUND

} E_RESULT;

#endif