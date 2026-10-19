/**
 *This is free and unencumbered software released into the public domain.
 *
 *Anyone is free to copy, modify, publish, use, compile, sell, or
 *distribute this software, either in source code form or as a compiled
 *binary, for any purpose, commercial or non-commercial, and by any
 *means.
 *
 *In jurisdictions that recognize copyright laws, the author or authors
 *of this software dedicate any and all copyright interest in the
 *software to the public domain. We make this dedication for the benefit
 *of the public at large and to the detriment of our heirs and
 *successors. We intend this dedication to be an overt act of
 *relinquishment in perpetuity of all present and future rights to this
 *software under copyright law.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *OTHER DEALINGS IN THE SOFTWARE.
 *
 *For more information, please refer to <https://unlicense.org>
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef unsigned char uchar_t;

typedef struct {
	uchar_t *data;
	size_t len;
} file_info_t;

// caller must free ret.data
file_info_t read_lock_file(void) {
	file_info_t ret;

	FILE *f = fopen("lock.levels", "r");
	if (!f) {
		fprintf(stderr, "couldn't open lock.levels file\n");
		return (file_info_t){NULL};
	}

	fseek(f, 0, SEEK_END);
	ret.len = ftell(f);
	fseek(f, 0, SEEK_SET);

	ret.data = malloc(ret.len);
	if (!ret.data) {
		fprintf(stderr, "libc: can't allocate\n");
		return (file_info_t){NULL};
	}

	fread(ret.data, ret.len, 1, f);

	return ret;
}

ssize_t count_leading_spaces(uchar_t *line) {
	size_t spaces = 0;
	while (line[spaces] && line[spaces] == ' ') {
		if (line[spaces] == '\t') return -1;
		spaces++;
	}
	if (line[spaces] == '\t') return -1;
	return spaces;
}

void write_indent_of(size_t indent) {
	for (size_t i = 0; i < indent; i++) {
		fprintf(stdout, "    ");
	}
}

// true on success, false on error
bool handle_line(uchar_t *line) {
	// comment or empty
	if (!line[0] || line[0] == '#') return true;

	ssize_t spaces = count_leading_spaces(line);
	if (spaces < 0) {
		fprintf(stderr, "tab used for indent, spaces expected\n");
		return false;
	}
	if (spaces % 4) {
		fprintf(stderr, "indents must be 4 spaces wide\n");
		return false;
	}
	size_t level = spaces / 4; // 4 spaces per indent
	uchar_t *label = &line[spaces];

	write_indent_of(level + 1);
	fprintf(stdout, "%s=%zu,\n", label, level);
	return true;
}

int main(void) {
	file_info_t file = read_lock_file();
	if (!file.data) return 1;

	fprintf(stdout, "typedef enum {\n");
	// iterate over each line
	for (size_t line_start = 0; line_start < file.len;) {
		// get the end of the line...
		size_t line_end = line_start;
		while (file.data[line_end] != '\n' && line_end < file.len)
			line_end++;
		// ...null terminate this line...
		file.data[line_end] = 0;

		// ...handle this line...
		if (!handle_line(&file.data[line_start])) return 1;

		// ...and bring us to the next line
		line_start = line_end + 1;
	}
	fprintf(stdout, "} LockLevels;\n");

	free(file.data);
	return 0;
}
