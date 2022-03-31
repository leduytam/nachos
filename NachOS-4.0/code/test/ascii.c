#include "syscall.h"

int main()
{
  int i;

  // 32 is space
  for (i = 33; i <= 126; i++)
  {
    PrintNum(i);
    PrintString(" - ");
    PrintChar(i);
    PrintString("\n");
  }

  Halt();
}
