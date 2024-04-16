/*
 * ex0a.c
 *
 *  Created on: Oct 16, 2021
 *      Author: Rani Abu Raia
 *      Id: 316396787
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//------------------------structs------------------------------

struct Data {
    int _num_of_lines ; // num of lines that contain actual data
    int ** _the_data ; // a matrix who stores the data from the user
    int * _lines_len ; // a 1d array which stores each row's column number
};

int main(int argc, char * argv[])
{

    struct Data matrix;
    int current_matrix_lines = 0; // number of lines allocated for the matrix (current_matrix_lines >= matrix._num_of_lines)
    int current_line_len_colomns= 0; //// number of colomns allocated for matrix._lines_len array

    matrix._the_data = NULL;
    matrix._lines_len = NULL;
    matrix._num_of_lines = 0;

    FILE * in = (fopen(argv[1],"r"));
    FILE * out = (fopen(argv[2],"w"));
	if (in == NULL || argc != 3)
	{
		printf("Input Error");
		exit(1);
	}

	int line_size; //number of numbers in the line
	while (fscanf(in, "%d",&line_size) != EOF)
	{
		if(matrix._the_data == NULL)
		{
			matrix._the_data = (int **) malloc (sizeof(int *));
			if(matrix._the_data == NULL)

			{
				printf("Allocation Memory Failed");
				exit(1);
			}

			matrix._the_data[0] = (int *) malloc (line_size * sizeof(int));
			if (matrix._the_data[0] == NULL)
			{
				printf("Allocation Memory Failed");
				exit(1);
			}
			current_matrix_lines += 1;

			matrix._lines_len = (int *) malloc(sizeof(int));
			if(matrix._lines_len == NULL)
			{
				printf("Allocation Memory Failed");
				exit(1);
			}
			current_line_len_colomns++; // update number of allocated columns

			matrix._lines_len[0] = line_size; // Enter number of colomns for the first row

		}

		else
		{
			if(current_line_len_colomns == matrix._num_of_lines)
			{
			matrix._lines_len = (int *)realloc(matrix._lines_len,current_line_len_colomns * 2 * sizeof(int));
			}
			matrix._lines_len[matrix._num_of_lines] = line_size;
			current_line_len_colomns *=2;
		}
		if (current_matrix_lines == matrix._num_of_lines)
		{
			matrix._the_data = (int **)realloc(matrix._the_data,current_matrix_lines * 2 * sizeof(int *));
			current_matrix_lines *= 2;
		}

		// add columns to the new line
		matrix._the_data[matrix._num_of_lines] =(int *) malloc (line_size * sizeof(int));
		if(matrix._the_data[matrix._num_of_lines] == NULL)
		{
			printf("Could not allocate memory");
			exit(1);
		}
		int i,num;

		// input data from the user into a row
		for (i = 0; i < line_size; i++)
		{
			fscanf(in, "%d",&num);
			matrix._the_data[matrix._num_of_lines][i] = num;
		}
		matrix._num_of_lines++; // update number of lines added

    }

	for(int i = matrix._num_of_lines; i <current_matrix_lines;i++)
	{
		//free(matrix._the_data[i]);
	}


	fprintf(out,"%d",find(matrix));

	fclose(in);
	fclose(out);
}

bool is_bigger(struct Data matrix,int col,int wanted)
{
    for (int i = 0; i < matrix._num_of_lines; i++)
    {
        if (matrix._lines_len[i] >= col)
            if (matrix._the_data[i][col] > wanted)
                return false;
    }
    return true;
}

int find(struct Data matrix)
{
    bool bigger = true; // signs if all the cells of a certain row are bigger
                        // than their parallels in the all the other rows
    // itarate all the matrix cells
    for (int i = 0; i < matrix._num_of_lines; i++)
    {
        for (int j = 0; j < matrix._lines_len[i]; j++)
        {
            bigger = is_bigger(matrix,j,matrix._the_data[i][j]);
            if (!bigger)
                break;
        }
        if (bigger)
            return i;
    }
    return -1;
}


void free_matrix(struct Data matrix)
{
    // free each line
    for (int i = 0; i < matrix._num_of_lines ; i++)
    {
        free(matrix._the_data[i]);
    }
    free (matrix._the_data); // free the matrix itself
    free (matrix._lines_len); // free _lines_len array
}

