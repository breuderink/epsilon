#include "csv.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

char *csv_trim(char *s) {
	char *p = s;
	int l = strlen(p);

	// Trim trailing space
	while (isspace(p[l - 1]))
		p[--l] = 0;

	// Trim leading space
	while (*p && isspace(*p))
		++p;

	return p;
}

char *csv_unquote(char *s) {
	size_t l = strlen(s);
	if (l >= 2 && s[0] == '"' && s[l - 1] == '"') {
		s[l - 1] = '\0';
		s++;
	}
	return s;
}

char *csv_unescape(char *s) {
	char *src = s, *dst = s;

	while (*src) {
		if (*src == '"' && *(src + 1) == '"') {
			*dst++ = '"'; // collapse double quote into one
			src += 2;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';

	return s;
}

char *csv_clean_field(char *field) {
	field = csv_trim(field);
	field = csv_unquote(field);
	field = csv_unescape(field);
	return field;
}

// Returns number of input bytes consumed
size_t csv_tokenize(const char *buf, size_t len, csv_record_t *rec) {
	char *w = rec->buffer;
	char *field_start = w;
	rec->num_fields = 0;
	bool in_quotes = false;

	for (size_t i = 0; i < len; i++) {
		char c = buf[i];

		if (c == '"') {
			in_quotes = !in_quotes;
			*w++ = c;
		} else if (c == ',' && !in_quotes) {
			*w++ = '\0';
			if (rec->num_fields < CSV_FIELDS)
				rec->fields[rec->num_fields++] = field_start;
			field_start = w;
		} else if ((c == '\n' || c == '\r') && !in_quotes) {
			*w++ = '\0';
			if (rec->num_fields < CSV_FIELDS)
				rec->fields[rec->num_fields++] = field_start;

			// Combined CRLF handling
			if (c == '\r' && i + 1 < len && buf[i + 1] == '\n')
				i++;

			return i + 1; // Number of bytes consumed from input
		} else {
			*w++ = c;
		}

		// Safety: avoid buffer overflow
		if (w >= rec->buffer + CSV_RECORD_SIZE)
			break;
	}

	// End of input without newline
	*w = '\0';
	if (rec->num_fields < CSV_FIELDS)
		rec->fields[rec->num_fields++] = field_start;

	return len;
}

size_t csv_parse_record(const char *buf, size_t len, csv_record_t *rec) {
	size_t consumed = csv_tokenize(buf, len, rec);
	for (size_t i = 0; i < rec->num_fields; i++) {
		rec->fields[i] = csv_clean_field(rec->fields[i]);
	}
	return consumed;
}
