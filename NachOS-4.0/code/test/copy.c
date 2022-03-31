#include "syscall.h"

int main()
{
  char sourcePath[100];
  char destinationPath[100];
  char buffer[100];
  OpenFileId sourceId, destinationId;
  int createFileResult;

  PrintString("Enter source path: ");
  ReadString(sourcePath, 100);

  sourceId = Open(sourcePath);

  if (sourceId == -1)
  {
    PrintString("Error: source file not found.\n");
    Halt();
  }

  PrintString("Enter destination path: ");
  ReadString(destinationPath, 100);

  createFileResult = Create(destinationPath);

  if (createFileResult == -1)
  {
    PrintString("Error: destination file already exists.\n");
    Halt();
  }

  destinationId = Open(destinationPath);

  while (Read(buffer, 100, sourceId) > 0)
    Write(buffer, 100, destinationId);

  Close(sourceId);
  Close(destinationId);

  Halt();
}
