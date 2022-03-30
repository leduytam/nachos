#include "syscall.h"

int main()
{
  char filename[100];
  int result;
  OpenFileId id;

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
    Close(id);
  }

  Halt();
}
