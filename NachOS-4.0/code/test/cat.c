#include "syscall.h"

int main()
{
  char filename[100];
  char buffer[100];
  OpenFileId id;

  PrintString("Enter path: ");
  ReadString(filename, 100);

  id = Open(filename);

  if (id == -1)
  {
    PrintString("Open file failed.\n");
  }
  else
  {
    while (Read(buffer, 100, id) > 0)
    {
      // PrintString(buffer);
      Write(buffer, 100, ConsoleOutputId);
    }
  }

  Close(id);

  Halt();
}
