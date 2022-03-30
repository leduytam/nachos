#include "syscall.h"

/*

Input: 0
Output: 0

Input: -5
Output: -5

Input: -00000010
Output: -10

Input: -
Output: 0

Input: 23ab
Output: 0

Input: 2147483647
Output: 2147483647

Input: 2147483648
Output: 0 (Overflow)

Input: -2147483648
Output: -2147483648

Input: -2147483649
Output: 0 (Overflow)

Input: +55
Output: 55

PASSED
*/

int main()
{
  int num;

  PrintString("Enter a number: ");
  num = ReadNum();

  PrintString("You entered: ");
  PrintNum(num);
  PrintString("\n");

  Halt();
}
