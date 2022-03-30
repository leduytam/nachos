#include "syscall.h"

/*

Input: c
Output: c

Input: <Space>
Output: <Space>

Input: <Enter>
Output: <Enter>

PASSED
*/

int main()
{
  char c;

  PrintString("Enter a character: ");
  c = ReadChar();

  PrintString("You entered: ");
  PrintChar(c);
  PrintString("\n");

  Halt();
}
