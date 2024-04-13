#include "../test.h"

int main(void) {
	uint8_t t, tinv, tt;
	cube_t cube;

	for (t = 0; t < 48; t++) {
		cube = cube_new();
		cube = cube_move(cube, R);
		cube = cube_move(cube, U);
		cube = cube_move(cube, F);

		tt = cube_readtrans(cube_transstr(t));
		cube = cube_transform(cube, tt);
		tinv = cube_inversetrans(t);
		tt = cube_readtrans(cube_transstr(tinv));
		cube = cube_transform(cube, tt);

		if (cube_error(cube)) {
			printf("Error transforming cube\n");
		} else if (!cube_solvable(cube)) {
			printf("Transformed cube is not solvable\n");
		} else {
			cube = cube_move(cube, F3);
			cube = cube_move(cube, U3);
			cube = cube_move(cube, R3);
			if (!cube_solved(cube))
				printf("%s: Error! Got %" PRIu8 "\n",
				    cube_transstr(t), tinv);
		}
	}

	return 0;
}
