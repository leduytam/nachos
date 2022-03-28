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

// TODO: prototypes of syscall handler
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
void SysSeekHandler();
void SysRemoveHandler();

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
        // TODO: implement all syscall exceptions
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

// increase program counter to next instruction
void IncreasePC() {
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

// TODO: implements all syscall handlers here
void SysHaltHandler()
{
    DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

    SysHalt();

    ASSERTNOTREACHED();
}

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

void SysReadNumHandler()
{
    return IncreasePC();
}

void SysPrintNumHandler()
{
    return IncreasePC();
}

void SysReadCharHandler()
{
    return IncreasePC();
}

void SysPrintCharHandler()
{
    return IncreasePC();
}

void SysRandomNumHandler()
{
    return IncreasePC();
}

void SysReadStringHandler()
{
    return IncreasePC();
}

void SysPrintStringHandler()
{
    return IncreasePC();
}

void SysCreateHandler()
{
    return IncreasePC();
}

void SysOpenHandler()
{
    return IncreasePC();
}

void SysCloseHandler()
{
    return IncreasePC();
}

void SysReadHandler()
{
    return IncreasePC();
}

void SysSeekHandler()
{
    return IncreasePC();
}

void SysRemoveHandler()
{
    return IncreasePC();
}
