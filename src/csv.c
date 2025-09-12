#include "csv.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parses a CSV row in-place. Returns number of fields.
static size_t parse_csv_row(char *buf, csv_row_t *row) {
	row->n_fields = 0;
	char *p = buf;
	char *field_start = buf;
	bool in_quotes = false;

	while (*p) {
		if (*p == '"') {
			in_quotes = !in_quotes;
		} else if (*p == ',' && !in_quotes) {
			*p = 0;
			row->fields[row->n_fields++] = field_start;
			field_start = p + 1;
		} else if ((*p == '\n' || *p == '\r') && !in_quotes) {
			*p = 0;
			row->fields[row->n_fields++] = field_start;
			break;
		}
		p++;
		if (row->n_fields >= CSV_MAX_FIELDS)
			break;
	}
	// Handle case where last field is empty but no newline
	if (*field_start != 0 && row->n_fields < CSV_MAX_FIELDS) {
		row->fields[row->n_fields++] = field_start;
	}
	printf("Parsed %d fields\n", (int)row->n_fields);
	return row->n_fields;
}

// Initialize CSV reader
int csv_reader_open(csv_reader_t *r, const char *path) {
	r->f = fopen(path, "r");
	return r->f != NULL ? 0 : -1;
}

// Read next row; returns 1 on success, 0 on EOF
int csv_reader_next(csv_reader_t *r, csv_row_t *row) {
	if (!r->f)
		return 0;
	char *p = r->buf;
	size_t len = 0;
	int in_quotes = 0;

	while (fgets(p, CSV_BUF_SIZE - len, r->f)) {
		len += strlen(p);
		// Count quotes to see if row is complete
		for (char *q = p; *q; q++) {
			if (*q == '"')
				in_quotes = !in_quotes;
		}
		if (!in_quotes)
			break;        // complete row
		p = r->buf + len; // append next fgets
		if (len >= CSV_BUF_SIZE - 1)
			break; // prevent overflow
	}

	if (len == 0)
		return false; // EOF

	parse_csv_row(r->buf, row);
	return true;
}

// Close CSV reader
void csv_reader_close(csv_reader_t *r) {
	if (r->f)
		fclose(r->f);
}
