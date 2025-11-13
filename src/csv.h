#ifndef CSV_H
#define CSV_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define CSV_MAX_FIELDS 512
#define CSV_BUF_SIZE 4096

typedef struct {
	char *fields[CSV_MAX_FIELDS];
	char buf[CSV_BUF_SIZE];
	size_t n_fields;
} csv_row_t;

typedef struct {
	FILE *f;
	char buf[CSV_BUF_SIZE];
} csv_reader_t;

int csv_reader_open(csv_reader_t *r, const char *path);
void csv_reader_close(csv_reader_t *r);
int csv_reader_next(csv_reader_t *r, csv_row_t *row);

#endif // CSV_H
