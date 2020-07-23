/*
 * This is a sloppy process for creating random input of lines that
 * are smaller than a given size from stdin.
 * 
 * Read stdin line by line and write it to stdout.  Make sure all lines 
 * are not longer than STR_BUFFER_SZ.  If the string contains a null 
 * character, then the string will be shorter and the latter part of the 
 * line will be ignored.  If there are newlines, the line will produce 
 * multiple lines.
 */

#include <stdio.h>

// This is the maximum size I am allowing for a line.
#define STR_BUFFER_SZ 300

int main(int argc, char **argv) {
	char b[STR_BUFFER_SZ];

	while (fgets(b, STR_BUFFER_SZ-1, stdin)) {
		printf("%s\n", b);
	}
	return 0;
}

