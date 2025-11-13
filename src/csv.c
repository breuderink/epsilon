#include "csv.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

typedef enum { START, IN_FIELD, IN_QUOTE } state_t;

// Parse a single CSV field into buf; returns pointer after field.
const char *csv_parse_field(const char *r, char *buf, size_t buf_size) {
	state_t state = START;
	size_t w = 0; // write position in buf

	while (*r) {
		char c = *r++;
		switch (state) {
		case START:
			if (c == '"')
				state = IN_QUOTE;
			else if (c == ',' || c == '\n' || c == '\r')
				goto done; // empty field
			else {
				if (w < buf_size - 1)
					buf[w++] = c;
				state = IN_FIELD;
			}
			break;

		case IN_FIELD:
			if (c == ',' || c == '\n' || c == '\r')
				goto done;
			if (w < buf_size - 1)
				buf[w++] = c;
			break;

		case IN_QUOTE:
			if (c == '"') {
				if (*r == '"') {
					if (w < buf_size - 1)
						buf[w++] = '"';
				} else {
					state = IN_FIELD;
					continue;
				}
			} else {
				if (w < buf_size - 1)
					buf[w++] = c;
			}
			break;
		}
	}

done:
	if (w < buf_size)
		buf[w] = '\0';
	else
		buf[buf_size - 1] = '\0';
	return r;
}

// Parse a CSV row into row->fields; returns number of fields
size_t csv_parse_row(const char *r, csv_row_t *row) {
	row->n_fields = 0;
	char *w = row->buf; // write pointer into buffer
	size_t buf_remaining = CSV_BUF_SIZE;

	while (*r && row->n_fields < CSV_MAX_FIELDS) {
		row->fields[row->n_fields++] = w;

		r = csv_parse_field(r, w, buf_remaining);

		// advance write pointer past written field
		size_t field_len = strlen(w) + 1;
		w += field_len;
		if (field_len >= buf_remaining)
			buf_remaining = 0;
		else
			buf_remaining -= field_len;

		// move to next field or end of row
		if (*r == ',')
			r++;
		else if (*r == '\r' && r[1] == '\n') {
			r += 2;
			break;
		} else if (*r == '\n' || *r == '\r') {
			r++;
			break;
		} else
			break; // end of buffer
	}

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

	csv_parse_row(r->buf, row);
	return true;
}

// Close CSV reader
void csv_reader_close(csv_reader_t *r) {
	if (r->f)
		fclose(r->f);
}
