#include "FormatUtils.h"

std::string FormatLastError(DWORD dwError)
{
    // 1. Создаем временный указатель для буфера текстового сообщения
    LPSTR messageBuffer = nullptr;

    // 2. Вызываем системную функцию Windows API для получения текста ошибки по её коду
    size_t size = FormatMessageA
        // Флаги: автоматически выделить память, искать в системе, игнорировать вставки
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,    // Переданный код ошибки (например, 10061 или 10054)
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Использовать язык операционной системы
        (LPSTR)&messageBuffer,  // Сюда функция запишет адрес выделенной памяти с текстом
        0,
        NULL
    );
    // Текст по умолчанию, если описание ошибки не будет найдено в системе
    std::string errorMessage = "Unknown error";

    // 3. Если текстовое описание успешно найдено и получено
    if (size > 0 && messageBuffer != nullptr)
    {
        errorMessage = messageBuffer;   // Копируем полученный текст в стандартную строку (std::string)

        // 4. Удаляем символы переноса строки (\n и \r) с конца, чтобы вывод в консоль не ломал разметку
        if (!errorMessage.empty() && errorMessage.back() == '\n') errorMessage.pop_back();
        if (!errorMessage.empty() && errorMessage.back() == '\r') errorMessage.pop_back();
        // 5. Обязательно освобождаем память, которую операционная система Windows выделила под messageBuffer
        LocalFree(messageBuffer);
    }

    return errorMessage;
}