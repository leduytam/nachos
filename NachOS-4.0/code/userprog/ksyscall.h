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
#include <vector>

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
    return 0;
}

void SysPrintNum(int number)
{
    vector<char> buffer;

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

void SysReadString(char buffer[], int length);

void SysPrintString(char buffer[]);

int SysCreate(char* name);

OpenFileId SysOpen(char* name);

int SysClose(OpenFileId id);

int SysRead(char* buffer, int size, OpenFileId id);

int SysWrite(char* buffer, int size, OpenFileId id);

int SysSeek(int position, OpenFileId id);

int SysRemove(char* name);

#endif /* ! __USERPROG_KSYSCALL_H__ */
