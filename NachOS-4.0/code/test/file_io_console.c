#include "syscall.h"

#define SIZE 100

int main()
{
  char buffer[SIZE];

  PrintString("Enter a string: ");
  Read(buffer, SIZE, ConsoleInputId);

  PrintString("You entered: ");
  Write(buffer, SIZE, ConsoleOutputId);
  PrintString("\n");

  Halt();
}
