#ifndef EPSILON_CSV_H
#define EPSILON_CSV_H

#include <stddef.h>

#define CSV_RECORD_SIZE 4096
#define CSV_FIELDS 512

typedef struct {
	char buffer[CSV_RECORD_SIZE];
	char *fields[CSV_FIELDS];
	size_t num_fields;
} csv_record_t;

char *csv_clean_field(char *field);
size_t csv_tokenize(const char *buf, size_t len, csv_record_t *rec);
size_t csv_parse_record(const char *buf, size_t len, csv_record_t *rec);

#endif // EPSILON_CSV_H
