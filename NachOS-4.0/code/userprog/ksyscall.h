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

 /** Stop Nachos, and print out performance stats */
void SysHalt()
{
    kernel->interrupt->Halt();
}

/** Sum two numbers
 *
 * @param a first number
 * @param b second number
 * @return a+b
 */
int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

/** Read a number from keyboard
*
* @return number read if valid, 0 otherwise
* @idea using synchConsoleIn to read each character until a space, new line, or EOF is reached
*       and check if the number is valid then convert it to an int and return it
*/
int SysReadNum()
{
    string buffer;

    while (true)
    {
        char c = kernel->synchConsoleIn->GetChar();

        // first condition skips leading space characters. E.g. "  123" -> "123"
        if ((c == ' ' && buffer != "") || c == '\n' || c == EOF)
            break;

        if (c != ' ')
            buffer.push_back(c);
    }

    // no characters were read
    if (buffer.length() == 0)
        return 0;

    // special case for below algorithm
    if (buffer == "-2147483648")
        return INT_MIN;

    bool isStartWithSign = buffer[0] == '-' || buffer[0] == '+'; // skip first character if it is a sign
    bool isNegative = buffer[0] == '-';
    bool isLeadingZero = true;
    long long result = 0;
    int length = (int)buffer.length();

    for (int i = isStartWithSign; i < length; i++)
    {
        // not a number
        if (!isdigit(buffer[i]))
            return 0;

        // skip leading zeros. E.g. "0123" -> "123"
        if (isLeadingZero && buffer[i] != '0')
            isLeadingZero = false;

        if (!isLeadingZero)
        {
            result = result * 10 + (buffer[i] - '0');

            // number is overflow
            if (result > INT_MAX)
                return 0;
        }
    }

    result = isNegative ? -result : result;

    return result;
}

/** Print a number to the console
 *
 * @param number number to print
 * @idea convert to string in reverse order and print each character in reverse order by using synchConsoleOut
 *       if the number is negative, add a minus sign at the last position of the string before printing
 */
void SysPrintNum(int number)
{
    string buffer;
    bool isNegative = number < 0;

    // special case for below algorithm
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

/** Read a character from keyboard
 *
 * @return character read
 * @idea using synchConsoleIn to read a character from keyboard and return it
 */
char SysReadChar()
{
    return kernel->synchConsoleIn->GetChar();
}

/** Print a character to the console
*
* @param character character to print
* @idea using synchConsoleOut to print a character to the console
*/
void SysPrintChar(char character)
{
    kernel->synchConsoleOut->PutChar(character);
}

/** Create a random number
 *
 * @return random number
 * @idea using RandomInit to seed the random number generator when the program starts
 *       then using RandomNumber to generate a random number and return it
 */
int SysRandomNum()
{
    return RandomNumber(); // helper function in lib/sysdep.h
}

/** Read a line from keyboard
 *
 * @param length maximum characters to read
 * @return string read
 * @idea using synchConsoleIn to read each character until a new line or EOF is reached or the length is reached
 *       and store each character in the buffer then return the buffer
 */
char* SysReadString(int length)
{
    if (length < 0)
        return NULL;

    char* buffer = new char[length + 1];

    if (buffer == NULL)
        return NULL;

    for (int i = 0; i < length; i++)
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

/** Print a string to the console
*
* @param buffer string to print
* @idea using synchConsoleOut to print each character in the string to the console
*       if the string is NULL, do nothing
*/
void SysPrintString(char* buffer)
{
    if (buffer == NULL)
        return;

    int len = strlen(buffer);

    for (int i = 0; i < len; i++)
        kernel->synchConsoleOut->PutChar(buffer[i]);
}

/** Create a new file
*
* @param name file name
* @return 0 if successful, -1 otherwise (e.g. file already exists)
* @idea using fileSystem->Create function to create a new file
* @note we changed fileSytem->Create function in filesys/filesys.h
*/
int SysCreate(char* name)
{
    return kernel->fileSystem->Create(name);
}

/** Open a file
 *
 * @param name file name
 * @return 0 if successful, -1 otherwise (e.g. file does not exist)
 * @idea using fileSystem->OpenGetId function to open a file
 * @note we added fileSytem->OpenGetId function in filesys/filesys.h
 */
OpenFileId SysOpen(char* name)
{
    return kernel->fileSystem->OpenGetId(name);
}

/** Close a file
 *
 * @param id file id
 * @return 0 if successful, -1 otherwise (e.g. id is invalid or is not opened)
 * @idea using fileSystem->Close function to close a file
 * @note we changed fileSytem->Close function in filesys/filesys.h
 */
int SysClose(OpenFileId id)
{
    return kernel->fileSystem->Close(id);
}

/** Read from a file or from stdin if id is CONSOLE_INPUT
 *
 * @param length maximum characters to read
 * @param id file id
 * @return characters read if successful, NULL otherwise (e.g. id is invalid or is not opened)
 * @idea using fileSystem->Read function to read from a file
 * @note we changed fileSytem->Read function in filesys/filesys.h
 */
char* SysRead(int length, OpenFileId id)
{
    // if id is stdout return NULL
    if (id == CONSOLE_OUTUT)
        return NULL;

    // if id is stdin then read from keyboard
    if (id == CONSOLE_INPUT)
        return SysReadString(length);

    char* buffer = new char[length + 1];

    int count = kernel->fileSystem->Read(buffer, length, id);

    // read nothing then return NULL
    if (count == 0)
    {
        delete[] buffer;
        return NULL;
    }

    // read less than maximum length then at position count add a null character to skip the rest of the characters
    if (count != length)
        buffer[count] = '\0';

    return buffer;
}

/** Write to a file or to stdout if id is CONSOLE_OUTPUT
 *
 * @param buffer string to write
 * @param id file id
 * @return number of characters written
 * @idea using fileSystem->Write function to write to a file
 * @note we changed fileSytem->Write function in filesys/filesys.h
 */
int SysWrite(char* buffer, OpenFileId id)
{
    if (buffer == NULL || id == CONSOLE_INPUT)
        return 0;

    // if id is stdout then print to console and return the length of the string
    if (id == CONSOLE_OUTUT)
    {
        SysPrintString(buffer);
        return strlen(buffer);
    }

    return kernel->fileSystem->Write(buffer, strlen(buffer), id);
}

/** Seek a file
 *
 * @param position position to seek to
 * @param id file id
 * @return actual position after seeking, -1 if failed (e.g. id is invalid or is not opened)
 * @idea using fileSystem->Seek function to seek to a position in a file
 *       if position is -1 then seek to the end of the file
 * @note we added fileSytem->Seek function in filesys/filesys.h
 */
int SysSeek(int position, OpenFileId id)
{
    return kernel->fileSystem->Seek(position, id);
}

/** Remove a file
 *
 * @param name file name
 * @return 0 if successful, -1 otherwise (e.g. file does not exist)
 * @idea using fileSystem->Remove function to remove a file
 * @note we changed fileSytem->Remove function in filesys/filesys.h
 */
int SysRemove(char* name)
{
    return kernel->fileSystem->Remove(name);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
