/*
 * ex1b1.c
 *
 *  Created on: Nov 6, 2021
 *      Author: Rani Abu Raia
 *      user: abura
 *      id: 316396787
 */
#include <stdio.h>
#include <stdlib.h>
/*
 * This program gets as an input a file name and reads it line by line
 * and print the output to the stdout. if the input file isn't found
 * it print an error message
 */
int main(int argc, char* argv[])
{
   FILE *fp;
   char ch;
   fp = fopen(argv[1], "r"); // read mode

   if (fp == NULL)
   {
      printf("Error: no input file was found.\n");
   }
   else
   {
	   while((ch = fgetc(fp)) != EOF)
	      printf("%c", ch);

	   fclose(fp);
   }

   exit(1);
}
