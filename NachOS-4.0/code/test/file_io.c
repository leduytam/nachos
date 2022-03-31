#include "syscall.h"

int main()
{
    char filename[] = "../test/input.txt";
    int result;
    OpenFileId id;
    int i;
    char s[100];

    // PrintString("Enter file name: ");
    // ReadString(filename, 100);

    // Create file if not exist
    if (Create(filename) == 0)
    {
        PrintString(filename);
        PrintString(" is created.\n");
    }

    // Open file
    id = Open(filename);

    if (id == -1)
    {
        PrintString("Open file failed.\n");
        Halt();
    }

    Seek(-1, id);

    // Write to file
    PrintString("Enter a string to write to file: ");
    ReadString(s, 100);
    Write(s, 100, id);
    Write("\n", 100, id);

    Seek(0, id);

    while (Read(s, 100, id) != 0)
        PrintString(s);

    if (Remove(filename) == -1)
        PrintString("Remove file failed.\n");
    else
        PrintString("File removed.\n");

    Close(id);

    Halt();
}
