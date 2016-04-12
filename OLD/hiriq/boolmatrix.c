/*
matrix m x n
where m is rows
and n is cols
*/

/* 
implementation of boolean matrix interface.. or something
*/
typedef struct MATRICES matrix_t;
struct MATRICES {
	size_t rows;
	size_t cols;
	array_t data;
};

typedef ARRAYS* array_t;
struct ARRAYS {
	unsigned char** cell;
}

matrix_t* new_matrix(size_t rows, size_t cols) {
	/* allocate memory for matrix_t */
	matrix_t* new = (matrix_t*) malloc(sizeof(matrix_t));
	
	/* count byte needed for rows */
	size_t rows_bytes = rows / 8;
	if(rows % 8 != 0) {
		rows_bytes += 1;
	}
	/* count byte needed for cols */
	size_t cols_bytes = cols / 8;
	if(cols % 8 != 0) {
		cols_bytes += 1;
	}

	unsigned int m, n;
	new->data->cell = (unsigned char**)
		malloc(sizeof(char*) * rows_bytes);
	for(m = 0; m < row_bytes; m++) {
		new->data->cell[m] = (unsigned char*)
			malloc(sizeof(char) * cols_bytes);
	}

	for(m = 0; m < rows_bytes; m++) {
		for(n = 0; n < cols_bytes; n++) {
			new->data->cell[m][n] == 0;
		}
	}

	return new;
}

/*
to do/implement:
set_value_true(matrix, m, n)
set_value_false(matrix, m, n)
set_
*/