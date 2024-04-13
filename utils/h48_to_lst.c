#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../cube.h"

#define STRLENMAX 1000

int main(void) {
	char str[STRLENMAX];
	cube_t cube;

	fgets(str, STRLENMAX, stdin);
	cube = cube_read("H48", str);
	write_cube("LST", cube, str);
	fputs(str, stdout);

	return 0;
}
