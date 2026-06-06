#include "FormatUtils.h"

std::string FormatLastError(DWORD dwLastError)
{
    LPSTR messageBuffer = nullptr;

    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    std::string message(messageBuffer, size);

    // Ազատում ենք համակարգի հատկացրած հիշողությունը
    LocalFree(messageBuffer);

    return message;
}