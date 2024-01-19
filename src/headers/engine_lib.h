#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// #############################
//      Defines
// #############################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linux__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

#define BIT(x) 1 << (x)
#define KB(x) ((unsigned long long)x * 1024)
#define MB(x) ((unsigned long long)KB(x) * 1024)
#define GB(x) ((unsigned long long)MB(x) * 1024)

// #############################
//      Logging and Asserts
// #############################
enum TextColor
{
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

template <typename... Args>

void _log(const char *prefix, const char *msg, TextColor textColor, Args... args)
{
    static const char *TextColorTable[TEXT_COLOR_COUNT] = {
        "\x1b[30m",
        "\x1b[31m",
        "\x1b[32m",
        "\x1b[33m",
        "\x1b[34m",
        "\x1b[35m",
        "\x1b[36m",
        "\x1b[37m",
        "\x1b[90m",
        "\x1b[91m",
        "\x1b[92m",
        "\x1b[93m",
        "\x1b[94m",
        "\x1b[95m",
        "\x1b[96m",
    };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

    char textBuffer[8912] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define SM_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);

#define SM_ASSERT(x, msg, ...)            \
    {                                     \
        if (!(x))                         \
        {                                 \
            SM_ERROR(msg, ##__VA_ARGS__); \
            DEBUG_BREAK();                \
            SM_ERROR("Assertion HIT!")    \
        }                                 \
    }

// #############################
//          Bump Allocator
// #############################
struct BumpAllocator
{
    size_t capacity;
    size_t used;
    char *memory;
};

BumpAllocator make_bump_allocator(size_t size)
{
    BumpAllocator ba = {};

    ba.memory = (char *)malloc(size);

    if (ba.memory)
    {
        ba.capacity = size;
        memset(ba.memory, 0, size); // sets all bytes to 0
    }
    else
    {
        SM_ERROR("Failed to allocate memory for bump allocator");
    }

    return ba;
}

char *bump_alloc(BumpAllocator *bumpAllocator, size_t size)
{
    char *result = nullptr;

    size_t alignedSize = (size + 7) & ~7; // align to 8 bytes

    if (bumpAllocator->used + alignedSize <= bumpAllocator->capacity)
    {
        result = bumpAllocator->memory + bumpAllocator->used;
        bumpAllocator->used += alignedSize;
    }
    else
    {
        SM_ERROR("Bump allocator out of memory");
    }

    return result;
}

// #############################
//          File IO
// #############################

// Get the file timestamp
// user stat from the std for compatibility with linux and windows
long long get_timestamp(char *file)
{
    struct stat file_stat = {};
    stat(file, &file_stat);
    return file_stat.st_mtime;
}

// Check if the file exists
// user fopen from the std for compatibility with linux and windows
bool file_exists(char *filePath)
{
    SM_ASSERT(filePath, "File path is null");

    auto file = fopen(filePath, "rb");

    if (!file)
    {
        return false;
    }

    fclose(file);
    return true;
}

// Get the file size
// user fopen from the std for compatibility with linux and windows
long get_file_size(char *filePath)
{
    SM_ASSERT(filePath, "File path is null");

    auto file = fopen(filePath, "rb");

    if (!file)
    {
        SM_ERROR("Failed to open file: %s", filePath);
        return -1;
    }

    fseek(file, 0, SEEK_END); // move to the end of the file
    long size = ftell(file);  // get the position of the file pointer
    fseek(file, 0, SEEK_SET); // move to the start of the file
    fclose(file);             // close the file

    return size;
}

// Reads a file into a buffer. We manage the memory ant therefore want more control over it
// user fopen from the std for compatibility with linux and windows
char *read_file(char *filePath, int *fileSize, char *buffer)
{
    SM_ASSERT(filePath, "File path is null");
    SM_ASSERT(fileSize, "File size is null");
    SM_ASSERT(buffer, "Buffer is null");

    *fileSize = 0;
    auto file = fopen(filePath, "rb");

    if (!file)
    {
        SM_ERROR("Failed to open file: %s", filePath);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(buffer, 0, *fileSize + 1);
    fread(buffer, sizeof(char), *fileSize, file);
    fclose(file);

    return buffer;
}

char *read_file(char *filePath, int *fileSize, BumpAllocator *bumpAllocator)
{
    char *file = nullptr;

    bool fileExists = file_exists(filePath);

    if (!fileExists)
    {
        SM_ERROR("File does not exist: %s", filePath);
        return file;
    }

    long fileSize2 = get_file_size(filePath);

    if (fileSize2)
    {
        char *buffer = bump_alloc(bumpAllocator, fileSize2 + 1);

        file = read_file(filePath, fileSize, buffer);
    }

    return file;
}

void write_file(char *filePath, char *buffer, int size)
{
    SM_ASSERT(filePath, "File path is null");
    SM_ASSERT(buffer, "Buffer is null");

    auto file = fopen(filePath, "wb");

    if (!file)
    {
        SM_ERROR("Failed to open file: %s", filePath);
        return;
    }

    fwrite(buffer, sizeof(char), size, file);
    fclose(file);
}

bool copy_file(char *fileName, char *outputName, char *buffer)
{
    SM_ASSERT(fileName, "File name is null");
    SM_ASSERT(outputName, "Output name is null");
    SM_ASSERT(buffer, "Buffer is null");

    int fileSize = 0;
    char *data = read_file(fileName, &fileSize, buffer);

    auto outputFile = fopen(outputName, "wb");
    if (!outputFile)
    {
        SM_ERROR("Failed to open file: %s", outputName);
        return false;
    }

    int result = fwrite(data, sizeof(char), fileSize, outputFile);
    if (!result)
    {
        SM_ERROR("Failed to write to file: %s", outputName);
        return false;
    }

    fclose(outputFile);
    return true;
}

bool copy_file(char *fileName, char *outputName, BumpAllocator *bumpAllocator)
{
    SM_ASSERT(fileName, "File name is null");
    SM_ASSERT(outputName, "Output name is null");

    char *file = nullptr;
    long fileSize2 = get_file_size(fileName);

    if (fileSize2)
    {
        char *buffer = bump_alloc(bumpAllocator, fileSize2 + 1);

        return copy_file(fileName, outputName, buffer);
    }

    return false;
}