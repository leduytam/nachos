#include "syscall.h"

#define LENGTH 10

/*
Input: abcd
Output: abcd

Input: 0123456789
Output: 0123456789

Input: 01234567890
Output: 0123456789

Input: <Enter>
Output: <Empty>

Input: <Space><Space>d
Output: <Space><Space>d

LENGTH <= 0 => str = ""

PASSED
*/
int main()
{
  char str[LENGTH];

  PrintString("Enter a string: ");
  ReadString(str, LENGTH);
  PrintString("You entered: ");
  PrintString(str);
  PrintString("\n");

  Halt();
}
