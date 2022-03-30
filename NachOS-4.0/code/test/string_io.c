#include "syscall.h"

int main()
{
  char str[100];

  PrintString("Enter a string: ");
  ReadString(str, 100);
  PrintString("You entered: ");
  PrintString(str);
  PrintString("\n");

  Halt();
}
