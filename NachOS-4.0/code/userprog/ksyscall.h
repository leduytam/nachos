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

    if (buffer.length() == 0)
    {
        DEBUG(dbgSys, "SysReadNum: Empty Input\n");
        return 0;
    }

    if (buffer.length() == 1 && buffer[0] == '-')
    {
        DEBUG(dbgSys, "SysReadNum: Invalid number " << buffer << "\n");
        return 0;
    }

    if (buffer == "-2147483648")
    {
        DEBUG(dbgSys, "SysReadNum: " << buffer << "\n");
        return INT_MIN;
    }

    bool isNegative = buffer[0] == '-';
    bool isLeadingZero = true;
    long long result = 0;
    int size = (int)buffer.length();

    for (int i = isNegative; i < size; i++)
    {
        if (!isdigit(buffer[i]))
        {
            DEBUG(dbgSys, "SysReadNum: Invalid number " << buffer << "\n");
            return 0;
        }

        if (isLeadingZero && buffer[i] != '0')
            isLeadingZero = false;

        if (!isLeadingZero)
        {
            result = result * 10 + (buffer[i] - '0');

            if (result > INT_MAX)
            {
                DEBUG(dbgSys, "SysReadNum: Number " << buffer << " exceeds MAX_INT" << "\n");
                return 0;
            }
        }
    }

    result = isNegative ? -result : result;

    DEBUG(dbgSys, "SysReadNum: " << result << "\n");

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
    srand(time(NULL));
    return random();
}

int SysReadString(char* buffer, int length)
{
    if (buffer == NULL)
    {
        DEBUG(dbgSys, "SysReadString: Failed to allocate memory buffer\n");
        return -1;
    }

    if (length <= 0)
    {
        DEBUG(dbgSys, "SysReadString: Invalid length " << length << "\n");
        return -1;
    }

    for (int i = 0; i < length; i++)
    {
        buffer[i] = kernel->synchConsoleIn->GetChar();

        if (buffer[i] == '\n' || buffer[i] == EOF)
        {
            buffer[i] = '\0';
            break;
        }
    }

    if (buffer[length - 1] == '\0')
        return length - 1;

    return 0;
}

void SysPrintString(char* buffer)
{
    if (buffer == NULL)
    {
        DEBUG(dbgSys, "SysPrintString: Failed to allocate memory buffer\n");
        SysHalt();
        ASSERTNOTREACHED();
    }

    int len = strlen(buffer);

    for (int i = 0; i < len; i++)
        kernel->synchConsoleOut->PutChar(buffer[i]);
}

// TODO: check file is already exist
int SysCreate(char* name)
{
    if (name == NULL)
    {
        DEBUG(dbgSys, "SysCreate: Not enough memory to create file");
        return -1;
    }

    if (strlen(name) == 0)
    {
        DEBUG(dbgSys, "SysCreate: File name is empty\n");
        return -1;
    }

    if (!kernel->fileSystem->Create(name))
    {
        DEBUG(dbgSys, "SysCreate: Failed to create " << name << "\n");
        return -1;
    }

    DEBUG(dbgSys, "SysCreate: Created " << name << "\n");

    return 0;
}

OpenFileId SysOpen(char* name)
{
    if (name == NULL)
    {
        DEBUG(dbgSys, "SysOpen: Not enough memory to open file");
        return -1;
    }

    if (strlen(name) == 0)
    {
        DEBUG(dbgSys, "SysOpen: File name is empty\n");
        return -1;
    }

    int id = kernel->fileSystem->GetOpenFileId();

    if (id == -1)
    {
        DEBUG(dbgSys, "SysOpen: Failed to open " << name << ". Number of opening files is maximum\n");
        return -1;
    }

    OpenFile* file = kernel->fileSystem->Open(name);

    if (file == NULL)
    {
        DEBUG(dbgSys, "SysOpen: Failed to open " << name << "\n");
        return -1;
    }

    kernel->fileSystem->openingFiles[id] = file;

    DEBUG(dbgSys, "SysOpen: Opened " << name << " with id " << id << "\n");

    return id;
}

int SysClose(OpenFileId id)
{
    if (id < 2 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgSys, "SysClose: Invalid file id\n");
        return -1;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgSys, "SysClose: File with id " << id << " is not opened\n");
        return -1;
    }

    kernel->fileSystem->Close(id);

    DEBUG(dbgSys, "SysClose: Closed file with id " << id << "\n");

    return 0;
}

int SysRead(char* buffer, int size, OpenFileId id)
{
    if (buffer == NULL)
    {
        DEBUG(dbgSys, "SysRead: Not enough memory to read file");
        return -1;
    }

    if (size <= 0)
    {
        DEBUG(dbgSys, "SysRead: Invalid size\n");
        return -1;
    }

    if (id < 0 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgSys, "SysRead: Invalid file id\n");
        return -1;
    }

    if (id == CONSOLE_OUTUT)
    {
        DEBUG(dbgSys, "SysRead: Can't read from stdout\n");
        return -1;
    }

    if (id == CONSOLE_INPUT)
    {
        DEBUG(dbgSys, "SysRead: Read successfully from stdin\n");
        SysReadString(buffer, size);
        return 0;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgSys, "SysRead: File with id " << id << " is not opened\n");
        return -1;
    }

    DEBUG(dbgSys, "SysRead: Read successfully from " << kernel->fileSystem->openingFiles[id]->GetName() << "\n");

    return kernel->fileSystem->Read(buffer, size, id);
}

int SysWrite(char* buffer, int size, OpenFileId id)
{
    if (buffer == NULL)
    {
        DEBUG(dbgSys, "SysWrite: Not enough memory to write file");
        return -1;
    }

    if (size <= 0)
    {
        DEBUG(dbgSys, "SysWrite: Invalid size\n");
        return -1;
    }

    if (id < 0 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgSys, "SysWrite: Invalid file id\n");
        return -1;
    }

    if (id == CONSOLE_INPUT)
    {
        DEBUG(dbgSys, "SysWrite: Can't write to stdin\n");
        return -1;
    }

    if (id == CONSOLE_OUTUT)
    {
        DEBUG(dbgSys, "SysWrite: Write successfully to stdout\n");
        SysPrintString(buffer);
        return 0;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgSys, "SysWrite: File with id " << id << " is not opened\n");
        return -1;
    }

    DEBUG(dbgSys, "SysWrite: Write successfully to " << kernel->fileSystem->openingFiles[id]->GetName() << "\n");

    return kernel->fileSystem->Write(buffer, size, id);
}

int SysSeek(int position, OpenFileId id)
{
    if (id < 0 || id >= MAX_OPEN_FILES)
    {
        DEBUG(dbgSys, "SysSeek: Invalid file id\n");
        return -1;
    }

    if (id == CONSOLE_INPUT)
    {
        DEBUG(dbgSys, "SysSeek: Can't seek in stdin\n");
        return -1;
    }

    if (id == CONSOLE_OUTUT)
    {
        DEBUG(dbgSys, "SysSeek: Can't seek in stdout\n");
        return -1;
    }

    if (kernel->fileSystem->openingFiles[id] == NULL)
    {
        DEBUG(dbgSys, "SysSeek: File with id " << id << " is not opened\n");
        return -1;
    }

    return kernel->fileSystem->Seek(position, id);
}

int SysRemove(char* name)
{
    if (name == NULL)
    {
        DEBUG(dbgSys, "SysRemove: Not enough memory to remove file");
        return -1;
    }

    if (strlen(name) == 0)
    {
        DEBUG(dbgSys, "SysRemove: File name is empty\n");
        return -1;
    }

    for (int i = 2; i < MAX_OPEN_FILES; i++)
    {
        if (kernel->fileSystem->openingFiles[i] != NULL &&
            strcmp(kernel->fileSystem->openingFiles[i]->GetName(), name) == 0)
        {
            DEBUG(dbgSys, "SysRemove: File " << name << " is opening\n");
            return -1;
        }
    }

    if (!kernel->fileSystem->Remove(name))
    {
        DEBUG(dbgSys, "SysRemove: Failed to remove " << name << "\n");
        return -1;
    }

    DEBUG(dbgSys, "SysRemove: Removed " << name << "\n");

    return 0;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
