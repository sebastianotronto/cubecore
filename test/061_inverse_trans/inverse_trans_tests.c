#include "../test.h"

uint8_t readtrans(char *);
uint8_t inverse_trans(uint8_t);
cube_t applymoves(cube_t, char *);
cube_t applytrans(cube_t, char *);
extern char *transstr[];

int main(void) {
	uint8_t t, tinv;
	cube_t cube;

	for (t = 0; t < 48; t++) {
		cube = cube_new();
		cube = applymoves(cube, "R");
		cube = applymoves(cube, "U");
		cube = applymoves(cube, "F");

		cube = applytrans(cube, transstr[t]);
		tinv = inverse_trans(t);
		cube = applytrans(cube, transstr[tinv]);

		if (cube_error(cube)) {
			printf("Error transforming cube\n");
		} else if (!cube_solvable(cube)) {
			printf("Transformed cube is not solvable\n");
		} else {
			cube = applymoves(cube, "F'");
			cube = applymoves(cube, "U'");
			cube = applymoves(cube, "R'");
			if (!cube_solved(cube))
				printf("%s: Error! Got %" PRIu8 "\n",
				    transstr[t], tinv);
		}
	}

	return 0;
}
