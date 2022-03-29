#include "syscall.h"

int main()
{
  int num;

  PrintString("Enter a number: \n");
  num = ReadNum();

  PrintString("You entered: ");
  PrintNum(num);
  PrintString("\n");

  Halt();
}
