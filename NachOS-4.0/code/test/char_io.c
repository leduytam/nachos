#include "syscall.h"

int main()
{
  char c;

  PrintString("Enter a character: \n");
  c = ReadChar();

  PrintString("You entered: ");
  PrintChar(c);
  PrintString("\n");

  Halt();
}
