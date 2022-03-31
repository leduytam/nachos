/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include <cstring>
#include <string>
#include <climits>

void SysHalt()
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

int SysReadNum()
{
    string buffer;

    while (true)
    {
        char c = kernel->synchConsoleIn->GetChar();

        if ((c == ' ' && buffer != "") || c == '\n' || c == EOF)
            break;

        if (c != ' ')
            buffer.push_back(c);
    }

    if (buffer.length() == 0)
        return 0;

    if (buffer == "-2147483648")
        return INT_MIN;

    bool isStartWithSign = buffer[0] == '-' || buffer[0] == '+';
    bool isNegative = buffer[0] == '-';
    bool isLeadingZero = true;
    long long result = 0;
    int size = (int)buffer.length();

    for (int i = isStartWithSign; i < size; i++)
    {
        if (!isdigit(buffer[i]))
            return 0;

        if (isLeadingZero && buffer[i] != '0')
            isLeadingZero = false;

        if (!isLeadingZero)
        {
            result = result * 10 + (buffer[i] - '0');

            if (result > INT_MAX)
                return 0;
        }
    }

    result = isNegative ? -result : result;

    return result;
}

void SysPrintNum(int number)
{
    string buffer;
    bool isNegative = number < 0;

    if (number == 0)
        buffer.push_back('0');

    while (number != 0)
    {
        // using abs to avoid negative sign and MIN_INT
        buffer.push_back(abs(number % 10) + '0');
        number /= 10;
    }

    if (isNegative)
        buffer.push_back('-');

    for (int i = buffer.size() - 1; i >= 0; i--)
        kernel->synchConsoleOut->PutChar(buffer[i]);
}

char SysReadChar()
{
    return kernel->synchConsoleIn->GetChar();
}

void SysPrintChar(char character)
{
    kernel->synchConsoleOut->PutChar(character);
}

int SysRandomNum()
{
    return RandomNumber();
}

char* SysReadString(int length)
{
    if (length < 0)
        return NULL;

    char* buffer = new char[length + 1];

    int i = 0;
    for (; i < length; i++)
    {
        buffer[i] = kernel->synchConsoleIn->GetChar();

        if (buffer[i] == '\n' || buffer[i] == EOF)
        {
            buffer[i] = '\0';
            break;
        }
    }

    return buffer;
}

void SysPrintString(char* buffer)
{
    if (buffer == NULL)
        return;

    int len = strlen(buffer);

    for (int i = 0; i < len; i++)
        kernel->synchConsoleOut->PutChar(buffer[i]);
}

int SysCreate(char* name)
{
    return kernel->fileSystem->Create(name);
}

OpenFileId SysOpen(char* name)
{
    return kernel->fileSystem->OpenGetId(name);
}

int SysClose(OpenFileId id)
{
    return kernel->fileSystem->Close(id);
}

char* SysRead(int length, OpenFileId id)
{
    if (id == CONSOLE_OUTUT)
        return NULL;

    if (id == CONSOLE_INPUT)
        return SysReadString(length);

    char* buffer = new char[length + 1];

    int count = kernel->fileSystem->Read(buffer, length, id);

    if (count == 0)
    {
        delete[] buffer;
        return NULL;
    }

    if (count != length)
        buffer[count] = '\0';

    return buffer;
}

int SysWrite(char* buffer, OpenFileId id)
{
    if (buffer == NULL || id == CONSOLE_INPUT)
        return 0;

    if (id == CONSOLE_OUTUT)
    {
        SysPrintString(buffer);
        return strlen(buffer);
    }

    return kernel->fileSystem->Write(buffer, strlen(buffer), id);
}

int SysSeek(int position, OpenFileId id)
{
    return kernel->fileSystem->Seek(position, id);
}

int SysRemove(char* name)
{
    return kernel->fileSystem->Remove(name);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
