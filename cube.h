/******************************************************************************
Cube type definition

Each piece is represented by an (unsigned) 8-bit integer. The 4
least-significant bits determine which piece it is, the other 4 determine
the orientation.

Edges are numbered as follows (see also cube.c):
UF=0 UB=1 DB=2 DF=3 UR=4 UL=5 DL=6 DR=7 FR=8 FL=9 BL=10 BR=11

Corners are numbered as follows:
UFR=0 UBL=1 DFL=2 DBR=3 UFL=4 UBR=5 DFR=6 DBL=7

The orientation of the edges is with respect to F/B, the orientation of
corners is with respect to U/D.

The permutation of the center pieces is not stored. This means that the
cube is assumed to be in a fixed orientation.
******************************************************************************/

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

/******************************************************************************
Read / write utilities

Reading and writing is not done directly via stdin / stdout, but via an
array of char (called buf in the prototypes below).

Multiple representations of the cube as text are supported:

- H48: a human-readable format.
  Each edge is represented by two letters denoting the sides it
  belongs to and one number denoting its orientation (0 oriented, 1
  mis-oriented). Similarly, each corner is represented by three letters and
  a number (0 oriented, 1 twisted clockwise, 2 twisted counter-clockwise).

  The solved cube looks like this:

  UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0
  UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0

  The cube after the moves R'U'F looks like this:

  FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0
  UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0

  Whitespace (including newlines) between pieces is ignored when reading the
  cube. A single whitespace character is added between pieces when writing.

- SRC: format used to generate code for internal use.
  If OUT is the output in SRC format, one can use `cube_t cube = OUT` to
  declare a new cube object.

- LST: a format for internal use and generating code.
  The cube is printed as a comma-separated list of 20 integers, as they appear
  in cube_t. Corners come first, followed by edge (unlike H48).
******************************************************************************/

cube_t readcube(char *format, char *buf);
void writecube(char *format, cube_t cube, char *buf);
