#include "syscall.h"

/*

Input: ../test/input.txt

Case 1: ../test/input.txt is not exist
Output: ../test/input.txt is created

Case 2: ../test/input.txt is exist
Output: Do nothing (not changed anything in the file)

Case 3: ../test/input.txt is opening
Output: Do nothing

PASSED

*/

int main()
{
    char buffer[100];
    OpenFileId id;

    PrintString("Enter file name: ");
    ReadString(buffer, 100);

    id = Create(buffer);

    PrintString(buffer);

    if (id == -1)
        PrintString(" is exist or opening \n");
    else
        PrintString(" is created successfully.\n");

    Halt();
}
