#include "syscall.h"

int main()
{
  PrintString("Number: ");
  PrintNum(RandomNum());
  PrintString("\n");

  Halt();
}
