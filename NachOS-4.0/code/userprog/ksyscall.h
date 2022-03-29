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

        if (c == ' ' || c == '\n' || c == EOF || c == ' ')
            break;

        buffer.push_back(c);
    }

    if (buffer.length() == 0 || (buffer.length() == 1 && buffer[0] == '-'))
        return 0;

    if (buffer == "-2147483648")
        return INT_MIN;

    bool isNegative = buffer[0] == '-';
    bool isLeadingZero = true;
    long long result = 0;

    for (int i = isNegative; i < buffer.length(); i++)
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

    return isNegative ? -result : result;
}

void SysPrintNum(int number)
{
    string buffer;

    if (number == 0)
        buffer.push_back('0');
    else if (number < 0)
    {
        buffer.push_back('-');
        number = -number;
    }

    while (number != 0)
    {
        buffer.push_back(number % 10 + '0');
        number /= 10;
    }

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
    return random();
}

void SysReadString(char buffer[], int length)
{
    buffer[length] = '\0';

    for (int i = 0; i < length; i++)
        buffer[i] = kernel->synchConsoleIn->GetChar();
}

void SysPrintString(char buffer[])
{
    int len = strlen(buffer);

    for (int i = 0; i < len; i++)
        kernel->synchConsoleOut->PutChar(buffer[i]);
}

int SysCreate(char* name)
{
    if (name == NULL)
    {
        DEBUG(dbgFile, "SysCreate: Not enough memory to create file");
        return -1;
    }

    if (strlen(name) == 0)
    {
        DEBUG(dbgFile, "SysCreate: File name is empty\n");
    }

    if (!kernel->fileSystem->Create(name))
    {
        DEBUG(dbgFile, "SysCreate: Failed to create " << name << "\n");
        return -1;
    }

    DEBUG(dbgFile, "SysCreate: Created " << name << "\n");

    return 0;
}

OpenFileId SysOpen(char* name)
{
    if (name == NULL)
    {
        DEBUG(dbgFile, "SysOpen: Not enough memory to open file");
        return -1;
    }

    if (strlen(name) == 0)
    {
        DEBUG(dbgFile, "SysOpen: File name is empty\n");
        return -1;
    }

    int id = kernel->fileSystem->GetOpenFileId();

    if (id == -1)
    {
        DEBUG(dbgFile, "SysOpen: Failed to open " << name << ". Number of opening files is maximum\n");
        return -1;
    }

    OpenFile* file = kernel->fileSystem->Open(name);

    if (file == NULL)
    {
        DEBUG(dbgFile, "SysOpen: Failed to open " << name << "\n");
        return -1;
    }

    kernel->fileSystem->openingFiles[id] = file;

    return 0;
}

int SysClose(OpenFileId id)
{
    if (id < 2 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgFile, "SysClose: Invalid file id\n");
        return -1;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgFile, "SysClose: File with id " << id << " is not opened\n");
        return -1;
    }

    kernel->fileSystem->Close(id);

    return 0;
}

int SysRead(char* buffer, int size, OpenFileId id)
{
    if (buffer == NULL)
    {
        DEBUG(dbgFile, "SysRead: Not enough memory to read file");
        return -1;
    }

    if (size <= 0)
    {
        DEBUG(dbgFile, "SysRead: Invalid size\n");
        return -1;
    }

    if (id < 2 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgFile, "SysRead: Invalid file id\n");
        return -1;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgFile, "SysRead: File with id " << id << " is not opened\n");
        return -1;
    }

    return kernel->fileSystem->openingFiles[id]->Read(buffer, size);
}

int SysWrite(char* buffer, int size, OpenFileId id)
{
    return 0;
}

int SysSeek(int position, OpenFileId id)
{
    if (id < 2 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgFile, "SysSeek: Invalid file id\n");
        return -1;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgFile, "SysSeek: File with id " << id << " is not opened\n");
        return -1;
    }

    return kernel->fileSystem->Seek(position, id);
}

int SysRemove(char* name)
{
    if (name == NULL)
    {
        DEBUG(dbgFile, "SysRemove: Not enough memory to remove file");
        return -1;
    }

    if (strlen(name) == 0)
    {
        DEBUG(dbgFile, "SysRemove: File name is empty\n");
        return -1;
    }

    if (!kernel->fileSystem->Remove(name))
    {
        DEBUG(dbgFile, "SysRemove: Failed to remove " << name << "\n");
        return -1;
    }

    DEBUG(dbgFile, "SysRemove: Removed " << name << "\n");

    return 0;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
