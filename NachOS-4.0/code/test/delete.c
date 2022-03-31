#include "syscall.h"

int main()
{
  char filePath[] = "../test/test_delete.txt";
  int removeResult;
  OpenFileId id;

  PrintString("Deleting file: ");
  PrintString(filePath);

  // ------------------------------------------------------------
  PrintString("\n+ Case 1: File is not exist\n");
  removeResult = Remove(filePath);

  if (removeResult == -1)
    PrintString("File is not exist or opening.\n");
  else
    PrintString("File is deleted successfully.\n");
  // ------------------------------------------------------------


  // ------------------------------------------------------------
  PrintString("\n+ Case 2: File is exist but opening\n");
  Create(filePath);
  id = Open(filePath);

  removeResult = Remove(filePath);

  if (removeResult == -1)
    PrintString("File is not exist or opening.\n");
  else
    PrintString("File is deleted successfully.\n");

  Close(id);
  // ------------------------------------------------------------


  // ------------------------------------------------------------
  PrintString("\n+ Case 3: File is exist and not opening\n");
  removeResult = Remove(filePath);

  if (removeResult == -1)
    PrintString("File is not exist or opening.\n");
  else
    PrintString("File is deleted successfully.\n");
  // ------------------------------------------------------------

  Halt();
}
