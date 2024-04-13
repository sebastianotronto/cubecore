typedef struct {
	uint8_t corner[8];
	uint8_t edge[12];
} cube_t;

cube_t cube_new(void);
cube_t cube_clone(cube_t);
bool cube_consistent(cube_t);
bool cube_solvable(cube_t);
bool cube_solved(cube_t);
bool cube_equal(cube_t, cube_t);
bool cube_error(cube_t);
cube_t cube_compose(cube_t, cube_t);
cube_t cube_inverse(cube_t);

int64_t cube_coord_co(cube_t);
int64_t cube_coord_eo(cube_t);

cube_t cube_read(char *format, char *buf);
void cube_write(char *format, cube_t cube, char *buf);
