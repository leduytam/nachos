#include "syscall.h"

int main()
{
  PrintString("========================================================\n");
  PrintString("||                  NachOS Project                    ||\n");
  PrintString("||                                                    ||\n");
  PrintString("|| Instructors: Le Giang Thanh                        ||\n");
  PrintString("||              Nguyen Thanh Quan                     ||\n");
  PrintString("||              Le Ha Minh                            ||\n");
  PrintString("||                                                    ||\n");
  PrintString("|| Students: Le Duy Tam - 20127619                    ||\n");
  PrintString("||           Hoang Quoc Nam - 20127566                ||\n");
  PrintString("||           Bui Tan Sang - 20127611                  ||\n");
  PrintString("||                                                    ||\n");
  PrintString("========================================================\n");

  PrintString("List of commands:\n");
  PrintString("1. nachos -x ../test/sort\n");
  PrintString("Description: Input n numbers and bubble sort them in ascending or descending order.\n\n");
  PrintString("2. nachos -x ../test/ascii\n");
  PrintString("Description: Print all readable ascii characters to console.\n\n");
  PrintString("3. nachos -x ../test/create_file\n");
  PrintString("Description: Input a file path and create it\n\n");
  PrintString("4. nachos -x ../test/cat\n");
  PrintString("Description: Input a file path, read it and print to console\n\n");
  PrintString("5. nachos -x ../test/copy\n");
  PrintString("Description: Input two file paths, copy the first file to the second file\n\n");
  PrintString("6. nachos -x ../test/delete\n");
  PrintString("Description: Input a file path and delete it\n\n");

  Halt();
}
