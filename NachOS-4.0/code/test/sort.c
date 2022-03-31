#include "syscall.h"

#define SIZE 100

int main()
{
  int numbers[SIZE];
  int n, choice, i, j, tmp;

  PrintString("n = ");
  n = ReadNum();

  if (n > SIZE)
    n = SIZE;

  PrintString("Array: ");
  for (i = 0; i < n; i++)
    numbers[i] = ReadNum();

  PrintString("1: Ascending - Other: Descending\nYour choice: ");
  choice = ReadNum();

  PrintString("Array: ");
  for (i = 0; i < n; i++)
  {
    PrintNum(numbers[i]);
    PrintChar(' ');
  }
  PrintChar('\n');

  if (choice == 1)
    PrintString("Ascending: ");
  else
    PrintString("Descending: ");

  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n - i - 1; j++)
    {
      if (choice == 1)
      {
        if (numbers[j] > numbers[j + 1])
        {
          tmp = numbers[j];
          numbers[j] = numbers[j + 1];
          numbers[j + 1] = tmp;
        }
      }
      else
      {
        if (numbers[j] < numbers[j + 1])
        {
          tmp = numbers[j];
          numbers[j] = numbers[j + 1];
          numbers[j + 1] = tmp;
        }
      }
    }
  }

  for (i = 0; i < n; i++)
  {
    PrintNum(numbers[i]);
    PrintChar(' ');
  }
  PrintChar('\n');

  Halt();
}
