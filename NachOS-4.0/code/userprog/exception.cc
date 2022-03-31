// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

char* User2System(int addr);
void System2User(int addr, char* buffer);

void SysHaltHandler();
void SysAddHandler();
void SysReadNumHandler();
void SysPrintNumHandler();
void SysReadCharHandler();
void SysPrintCharHandler();
void SysRandomNumHandler();
void SysReadStringHandler();
void SysPrintStringHandler();
void SysCreateHandler();
void SysOpenHandler();
void SysCloseHandler();
void SysReadHandler();
void SysWriteHandler();
void SysSeekHandler();
void SysRemoveHandler();

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case NoException:
        kernel->interrupt->setStatus(SystemMode);
        DEBUG(dbgSys, "Switch to system mode\n");
        break;
    case SyscallException:
        switch (type) {
        case SC_Halt:
            return SysHaltHandler();
        case SC_Add:
            return SysAddHandler();
        case SC_ReadNum:
            return SysReadNumHandler();
        case SC_PrintNum:
            return SysPrintNumHandler();
        case SC_ReadChar:
            return SysReadCharHandler();
        case SC_PrintChar:
            return SysPrintCharHandler();
        case SC_RandomNum:
            return SysRandomNumHandler();
        case SC_ReadString:
            return SysReadStringHandler();
        case SC_PrintString:
            return SysPrintStringHandler();
        case SC_Create:
            return SysCreateHandler();
        case SC_Open:
            return SysOpenHandler();
        case SC_Close:
            return SysCloseHandler();
        case SC_Read:
            return SysReadHandler();
        case SC_Write:
            return SysWriteHandler();
        case SC_Seek:
            return SysSeekHandler();
        case SC_Remove:
            return SysRemoveHandler();
        default:
            cerr << "Unexpected system call " << type << "\n";
            break;
        }
        break;

    case PageFaultException:
    case ReadOnlyException:
    case BusErrorException:
    case AddressErrorException:
    case OverflowException:
    case IllegalInstrException:
    case NumExceptionTypes:
        cerr << "An error occurs. Error Code: " << which << "\n";
        SysHalt();
        ASSERTNOTREACHED();
    default:
        cerr << "Unexpected user mode exception" << (int)which << "\n";
        break;
    }
    ASSERTNOTREACHED();
}

/** Get string from user space.
 *  @param addr The address of the string in user space.
 *  @return string but in kernel space.
 *  @idea count the length of the string through kernel->machine->ReadMem()
 *        then allocate dynamic memory for the string
 *        then read the string from user space to kernel space
 *        then return it
 */
char* User2System(int addr)
{
    char* buffer = NULL;
    int length = 0;
    int c;

    while (true)
    {
        if (!kernel->machine->ReadMem(addr + length, 1, &c))
            return NULL;

        if (c == '\0')
            break;

        length++;
    }

    buffer = new char[length + 1];

    if (buffer == NULL)
        return NULL;

    buffer[length] = '\0';

    for (int i = 0; i < length; i++)
    {
        if (!kernel->machine->ReadMem(addr + i, 1, &c))
        {
            delete[] buffer;
            return NULL;
        }

        buffer[i] = (char)c;
    }

    return buffer;
}

/** Put string to user space.
 *  @param addr The address of the string in user space.
 *  @param buffer The string to be put in user space.
 *  @idea if buffer is NULL, do nothing (the result string in user space is empty or NULL)
 *        else put the string to user space
 */
void System2User(int addr, char* buffer)
{
    if (buffer == NULL)
        return;

    int length = strlen(buffer) + 1;

    for (int i = 0; i < length; i++)
        kernel->machine->WriteMem(addr + i, 1, buffer[i]);
}

/** Increase program counter to next instruction. */
void IncreasePC()
{
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

/** Handle halt system call. */
void SysHaltHandler()
{
    DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

    SysHalt();

    ASSERTNOTREACHED();
}

/** Handle add system call. */
void SysAddHandler()
{
    DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

    /* Process SysAdd Systemcall*/
    int result;
    result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
        /* int op2 */(int)kernel->machine->ReadRegister(5));

    DEBUG(dbgSys, "Add returning with " << result << "\n");
    /* Prepare Result */
    kernel->machine->WriteRegister(2, (int)result);

    return IncreasePC();
}

/** Handle read number system call.
 * @idea read number from keyboard by using SysReadNum() and add it to register 2
 *       increase pc
 */
void SysReadNumHandler()
{
    kernel->machine->WriteRegister(2, (int)SysReadNum());
    return IncreasePC();
}

/** Handle print number system call.
 * @idea get number from register 4 and print it to console by using SysPrintNum()
 *       increase pc
 */
void SysPrintNumHandler()
{
    SysPrintNum((int)kernel->machine->ReadRegister(4));
    return IncreasePC();
}

/** Handle read character system call.
 * @idea read character from keyboard by using SysReadChar() and add it to register 2
 *       increase pc
 */
void SysReadCharHandler()
{
    kernel->machine->WriteRegister(2, (int)SysReadChar());
    return IncreasePC();
}

/** Handle print character system call.
 * @idea get character from register 4 and print it to console by using SysPrintChar()
 *       increase pc
 */
void SysPrintCharHandler()
{
    SysPrintChar((char)kernel->machine->ReadRegister(4));
    return IncreasePC();
}

/** Handle random number system call.
 * @idea get random number by using SysRandomNum() and add it to register 2
 *       increase pc
 */
void SysRandomNumHandler()
{
    kernel->machine->WriteRegister(2, (int)SysRandomNum());
    return IncreasePC();
}

/** Handle read string system call.
 * @idea get virtual address of string from register 4
 *       get length of string from register 5
 *       get string from keyboard by using SysReadString()
 *       put string to user space by using System2User()
 *       delete string buffer in kernel space
 *       increase pc
 */
void SysReadStringHandler()
{
    int addr = kernel->machine->ReadRegister(4);
    int length = kernel->machine->ReadRegister(5);

    char* buffer = SysReadString(length);

    System2User(addr, buffer);

    delete[] buffer;

    return IncreasePC();
}

/** Handle print string system call.
 * @idea get virtual address of string from register 4
 *       get string from user space by using User2System()
 *       print string to console by using SysPrintString()
 *       delete string buffer in kernel space
 *       increase pc
 */
void SysPrintStringHandler()
{
    int addr = kernel->machine->ReadRegister(4);
    char* buffer = User2System(addr);

    SysPrintString(buffer);

    delete[] buffer;

    return IncreasePC();
}

/** Handle create file system call.
 * @idea get virtual address of file name from register 4
 *       get file name from user space by using User2System()
 *       create file by using SysCreateFile()
 *       delete file name buffer in kernel space
 *       increase pc
 */
void SysCreateHandler()
{
    int addr = kernel->machine->ReadRegister(4);
    char* buffer = User2System(addr);

    kernel->machine->WriteRegister(2, (int)SysCreate(buffer));

    delete[] buffer;

    return IncreasePC();
}

/** Handle open file system call.
 * @idea get virtual address of file name from register 4
 *       get file name from user space by using User2System()
 *       open file by using SysOpenFile()
 *       delete file name buffer in kernel space
 *       increase pc
 */
void SysOpenHandler()
{
    int addr = kernel->machine->ReadRegister(4);
    char* buffer = User2System(addr);

    kernel->machine->WriteRegister(2, (int)SysOpen(buffer));

    delete[] buffer;

    return IncreasePC();
}

/** Handle close file system call.
 * @idea get file id from register 4
 *       close file by using SysCloseFile()
 *       increase pc
 */
void SysCloseHandler()
{
    int id = kernel->machine->ReadRegister(4);

    kernel->machine->WriteRegister(2, (int)SysClose(id));

    return IncreasePC();
}

/** Handle read file system call.
 * @idea get virtual address of buffer from register 4
 *       get length of buffer from register 5
 *       get file id from register 6
 *       read file by using SysReadFile()
 *       put buffer to user space by using System2User()
 *       delete buffer buffer in kernel space
 *       increase pc
 */
void SysReadHandler()
{
    int addr = kernel->machine->ReadRegister(4);
    int length = kernel->machine->ReadRegister(5);
    int id = kernel->machine->ReadRegister(6);

    char* buffer = SysRead(length, id);

    kernel->machine->WriteRegister(2, buffer ? strlen(buffer) : 0);

    System2User(addr, buffer);

    delete[] buffer;

    return IncreasePC();
}

/** Handle write file system call.
 * @idea get virtual address of buffer from register 4
 *       get length of buffer from register 5
 *       get file id from register 6
 *       get string to write from user space by using User2System()
 *       write file by using SysWriteFile()
 *       increase pc
 */
void SysWriteHandler()
{
    int addr = kernel->machine->ReadRegister(4);
    int length = kernel->machine->ReadRegister(5);
    int id = kernel->machine->ReadRegister(6);

    char* buffer = User2System(addr);
    buffer[length] = '\0';

    kernel->machine->WriteRegister(2, (int)SysWrite(buffer, id));

    delete[] buffer;

    return IncreasePC();
}

/** Handle seek file system call.
 * @idea get position from register 4
 *       get file id from register 5
 *       seek file by using SysSeekFile()
 *       increase pc
 */
void SysSeekHandler()
{
    int position = kernel->machine->ReadRegister(4);
    int id = kernel->machine->ReadRegister(5);

    kernel->machine->WriteRegister(2, (int)SysSeek(position, id));

    return IncreasePC();
}

/** Handle remove file system call.
 * @idea get virtual address of file name from register 4
 *       get file name from user space by using User2System()
 *       remove file by using SysRemoveFile()
 *       delete file name buffer in kernel space
 *       increase pc
 */
void SysRemoveHandler()
{
    int addr = kernel->machine->ReadRegister(4);

    char* buffer = User2System(addr);

    kernel->machine->WriteRegister(2, (int)SysRemove(buffer));

    delete[] buffer;

    return IncreasePC();
}
