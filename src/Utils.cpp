#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include "Utils.hpp"

std::size_t mlib::GetFileSize(const char* path)
{
    struct stat result = {};
    stat(path, &result);

    return (std::size_t)result.st_size;
}

char* mlib::ReadFileToBuf(const char* filePath)
{
    if (!filePath)
        return nullptr;

    std::size_t fileSize = GetFileSize(filePath);

    char* buf = (char*)calloc(fileSize + 2, 1);
    if (!buf)
        return nullptr;

    FILE* file = fopen(filePath, "rb");
    if (!file)
        return nullptr;

    if (fread(buf, 1, fileSize, file) != fileSize)
        return nullptr;

    return buf;
}

void mlib::SetConsoleColor(std::ostream& out, ConsoleColor color)
{
    out << "\033[0;" << static_cast<int>(color) << "m";
}
