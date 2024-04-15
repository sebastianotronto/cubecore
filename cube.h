typedef enum {
	U, U2, U3, D, D2, D3,
	R, R2, R3, L, L2, L3,
	F, F2, F3, B, B2, B3
} move_t;

typedef enum {
	UFr, ULr, UBr, URr, DFr, DLr, DBr, DRr,
	RUr, RFr, RDr, RBr, LUr, LFr, LDr, LBr,
	FUr, FRr, FDr, FLr, BUr, BRr, BDr, BLr,

	UFm, ULm, UBm, URm, DFm, DLm, DBm, DRm,
	RUm, RFm, RDm, RBm, LUm, LFm, LDm, LBm,
	FUm, FRm, FDm, FLm, BUm, BRm, BDm, BLm
} trans_t;

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
cube_t cube_move(cube_t, move_t);
cube_t cube_transform(cube_t, trans_t);

int64_t cube_coord_co(cube_t);
int64_t cube_coord_eo(cube_t);

cube_t cube_read(const char *format, const char *buf);
void cube_write(const char *format, cube_t cube, char *buf);
int cube_readmoves(const char *, move_t *);
char *cube_movestr(move_t);
trans_t cube_readtrans(const char *);
char *cube_transstr(trans_t);
move_t cube_inversemove(move_t);
trans_t cube_inversetrans(trans_t);
