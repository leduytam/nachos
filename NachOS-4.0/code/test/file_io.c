#include "syscall.h"

int main()
{
  char filename[100];
  int result;
  OpenFileId id;
  int i;
  char buffer[100];

  // PrintString("Enter file name: ");
  // ReadString(filename, 100);

  // result = Create(filename);

  // if (result == -1)
  // {
  //   PrintString("Create file failed.\n");
  // }
  // else
  // {
  //   PrintString("Created file.\n");
  // }

  PrintString("Enter file name: ");
  ReadString(filename, 100);

  id = Open(filename);

  if (id == -1)
  {
    PrintString("Open file failed.\n");
  }
  else
  {
    PrintString("Opened file.\n");

    // for (i = 0; i < 10; i++)
    // {
    //   Write("Hello World!\n", 13, id);
    // }

    while (Read(buffer, 100, id) > 0)
      PrintString(buffer);
    PrintString("\n");

    Close(id);
  }

  Halt();
}
