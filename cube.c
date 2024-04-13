#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "cube.h"

#ifdef DEBUG

#include <stdio.h>
#define _static
#define _static_inline
#define DBG_LOG(...) fprintf(stderr, __VA_ARGS__)
#define DBG_WARN(condition, ...) if (!(condition)) DBG_LOG(__VA_ARGS__);
#define DBG_ASSERT(condition, retval, ...) \
	if (!(condition)) {                \
		DBG_LOG(__VA_ARGS__);      \
		return retval;             \
	}

#else

#define _static static
#define _static_inline static inline
#define DBG_LOG(...)
#define DBG_WARN(condition, ...)
#define DBG_ASSERT(condition, retval, ...)

#endif

#define _2p11  2048U
#define _2p12  4096U
#define _3p7   2187U
#define _3p8   6561U
#define _12c4  495U
#define _8c4   70U

#define _c_ufr      0U
#define _c_ubl      1U
#define _c_dfl      2U
#define _c_dbr      3U
#define _c_ufl      4U
#define _c_ubr      5U
#define _c_dfr      6U
#define _c_dbl      7U

#define _e_uf       0U
#define _e_ub       1U
#define _e_db       2U
#define _e_df       3U
#define _e_ur       4U
#define _e_ul       5U
#define _e_dl       6U
#define _e_dr       7U
#define _e_fr       8U
#define _e_fl       9U
#define _e_bl       10U
#define _e_br       11U

#define _eoshift    4U
#define _coshift    5U

#define _pbits      0xFU
#define _esepbit1   0x4U
#define _esepbit2   0x8U
#define _csepbit    0x4U
#define _eobit      0x10U
#define _cobits     0xF0U
#define _cobits2    0x60U
#define _ctwist_cw  0x20U
#define _ctwist_ccw 0x40U
#define _eflip      0x10U
#define _error      0xFFU

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

_static cube_t zero = { .corner = {0}, .edge = {0} };
_static cube_t solved = {
	.corner = {0, 1, 2, 3, 4, 5, 6, 7},
	.edge = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
};

#include "tables.h"

/******************************************************************************
Section: portable fast methods

This section contains performance-critical methods that do not use
advanced CPU instructions. They are used as an alternative to the ones
in the previous section(s) for unsupported architectures.
******************************************************************************/

typedef cube_t cube_fast_t;

_static cube_fast_t cubetofast(cube_t);
_static cube_t fasttocube(cube_fast_t);
_static_inline bool equal_fast(cube_fast_t, cube_fast_t);
_static_inline bool issolved_fast(cube_fast_t);
_static_inline cube_fast_t invertco_fast(cube_fast_t);
_static_inline cube_fast_t compose_fast(cube_fast_t, cube_fast_t);

_static_inline int64_t coord_fast_co(cube_fast_t);
_static_inline int64_t coord_fast_eo(cube_fast_t);

_static cube_fast_t
cubetofast(cube_t cube)
{
	cube_fast_t fast;
	memcpy(&fast, &cube, sizeof(cube_fast_t));
	return fast;
}

_static cube_t
fasttocube(cube_fast_t fast)
{
	cube_t cube;
	memcpy(&cube, &fast, sizeof(cube_fast_t));
	return cube;
}

_static_inline bool
equal_fast(cube_fast_t c1, cube_fast_t c2)
{
	uint8_t i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.corner[i] == c2.corner[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.edge[i] == c2.edge[i];

	return ret;
}

_static_inline bool
issolved_fast(cube_fast_t cube)
{
	return equal_fast(cube, solved);
}

_static_inline cube_fast_t
invertco_fast(cube_fast_t c)
{
	uint8_t i, piece, orien;
	cube_fast_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = c.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[i] = (piece & _pbits) | orien;
	}

	return ret;
}

_static_inline cube_fast_t
compose_fast(cube_fast_t c1, cube_fast_t c2)
{
	cube_fast_t ret;
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	ret = zero;

	for (i = 0; i < 12; i++) {
		piece2 = c2.edge[i];
		p = piece2 & _pbits;
		piece1 = c1.edge[p];
		orien = (piece2 ^ piece1) & _eobit;
		ret.edge[i] = (piece1 & _pbits) | orien;
	}

	for (i = 0; i < 8; i++) {
		piece2 = c2.corner[i];
		p = piece2 & _pbits;
		piece1 = c1.corner[p];
		aux = (piece2 & _cobits) + (piece1 & _cobits);
		auy = (aux + _ctwist_cw) >> 2U;
		orien = (aux + auy) & _cobits2;
		ret.corner[i] = (piece1 & _pbits) | orien;
	}

	return ret;
}

_static_inline int64_t
coord_fast_co(cube_fast_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 3)
		ret += p * (c.corner[i] >> _coshift);

	return ret;
}

_static_inline int64_t
coord_fast_eo(cube_fast_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 1, p = 1; i < 12; i++, p *= 2)
		ret += p * (c.edge[i] >> _eoshift);

	return ret;
}

/******************************************************************************
Section: generic methods

This section contains generic functionality, including the public functions.
Some of these routines depend on the efficient functions implemented in the
previous sections, while some other operate directly on the cube.
******************************************************************************/

_static int permsign(uint8_t *, int);
_static uint8_t readco(char *);
_static uint8_t readcp(char *);
_static uint8_t readeo(char *);
_static uint8_t readep(char *);
_static cube_t readcube_H48(char *);
_static uint8_t readpiece_LST(char **);
_static cube_t readcube_LST(char *);
_static int writepiece_LST(uint8_t, char *);
_static void writecube_H48(cube_t, char *);
_static void writecube_LST(cube_t, char *);
_static uint8_t readmove(char);
_static uint8_t readmodifier(char);
_static uint8_t readtrans(char *);
_static int writemoves(uint8_t *, int, char *);
_static void writetrans(uint8_t, char *);
_static cube_fast_t move(cube_fast_t, move_t);
_static cube_fast_t transform(cube_fast_t, trans_t);

cube_t
cube_new(void)
{
	return solved;
}

cube_t
cube_clone(cube_t c)
{
	cube_t ret;

	memcpy(&ret, &c, sizeof(cube_t));

	return ret;
}

bool
cube_consistent(cube_t cube)
{
	uint8_t i, p, e, piece;
	bool found[12];

	for (i = 0; i < 12; i++)
		found[i] = false;
	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		p = piece & _pbits;
		e = piece & _eobit;
		if (p >= 12)
			goto inconsistent_ep;
		if (e != 0 && e != _eobit)
			goto inconsistent_eo;
		found[p] = true;
	}
	for (i = 0; i < 12; i++)
		if (!found[i])
			goto inconsistent_ep;

	for (i = 0; i < 8; i++)
		found[i] = false;
	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		p = piece & _pbits;
		e = piece & _cobits;
		if (p >= 8)
			goto inconsistent_cp;
		if (e != 0 && e != _ctwist_cw && e != _ctwist_ccw)
			goto inconsistent_co;
		found[p] = true;
	}
	for (i = 0; i < 8; i++)
		if (!found[i])
			goto inconsistent_co;

	return true;

inconsistent_ep:
	DBG_LOG("Inconsistent EP\n");
	return false;
inconsistent_cp:
	DBG_LOG("Inconsistent CP\n");
	return false;
inconsistent_eo:
	DBG_LOG("Inconsistent EO\n");
	return false;
inconsistent_co:
	DBG_LOG("Inconsistent CO\n");
	return false;
}

bool
cube_solvable(cube_t cube)
{
	uint8_t i, eo, co, piece, edges[12], corners[8];

	DBG_ASSERT(cube_consistent(cube), false,
	    "cube_solvable: cube is inconsistent\n");

	for (i = 0; i < 12; i++)
		edges[i] = cube.edge[i] & _pbits;
	for (i = 0; i < 8; i++)
		corners[i] = cube.corner[i] & _pbits;

	if (permsign(edges, 12) != permsign(corners, 8))
		goto solvable_parity;

	eo = 0;
	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		eo += (piece & _eobit) >> _eoshift;
	}
	if (eo % 2 != 0)
		goto solvable_eo;

	co = 0;
	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		co += (piece & _cobits) >> _coshift;
	}
	if (co % 3 != 0)
		goto solvable_co;

	return true;

solvable_parity:
	DBG_LOG("EP and CP parities are different\n");
	return false;
solvable_eo:
	DBG_LOG("Odd number of flipped edges\n");
	return false;
solvable_co:
	DBG_LOG("Sum of corner orientation is not multiple of 3\n");
	return false;
}

bool
cube_solved(cube_t cube)
{
	return cube_equal(cube, solved);
}

bool
cube_equal(cube_t c1, cube_t c2)
{
	int i;
	bool ret;

	ret = true;
	for (i = 0; i < 8; i++)
		ret = ret && c1.corner[i] == c2.corner[i];
	for (i = 0; i < 12; i++)
		ret = ret && c1.edge[i] == c2.edge[i];

	return ret;
}

bool
cube_error(cube_t cube)
{
	return cube_equal(cube, zero);
}

cube_t
cube_compose(cube_t c1, cube_t c2)
{
	DBG_ASSERT(cube_consistent(c1) && cube_consistent(c2),
	    zero, "cube_compose error: inconsistent cube\n")

	return fasttocube(compose_fast(cubetofast(c1), cubetofast(c2)));
}

cube_t
cube_inverse(cube_t cube)
{
	cube_t ret;
	uint8_t i, piece, orien;

	DBG_ASSERT(cube_consistent(cube), zero,
	    "cube_inverse error: inconsistent cube\n");

	ret = zero;

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		orien = piece & _eobit;
		ret.edge[piece & _pbits] = i | orien;
	}

	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[piece & _pbits] = i | orien;
	}

	return ret;
}

cube_t
applymoves(cube_t cube, char *buf)
{
	cube_fast_t fast;
	uint8_t r, m;
	char *b;

	DBG_ASSERT(cube_consistent(cube), zero,
	    "move error: inconsistent cube\n");

	fast = cubetofast(cube);

	for (b = buf; *b != '\0'; b++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if (*b == '\0')
			goto applymoves_finish;
		if ((r = readmove(*b)) == _error)
			goto applymoves_error;
		if ((m = readmodifier(*(b+1))) != 0)
			b++;
		fast = move(fast, r + m);
	}

applymoves_finish:
	return fasttocube(fast);

applymoves_error:
	DBG_LOG("applymoves error\n");
	return zero;
}

cube_t
applytrans(cube_t cube, char *buf)
{
	cube_fast_t fast;
	uint8_t t;

	DBG_ASSERT(cube_consistent(cube), zero,
	    "transformation error: inconsistent cube\n");

	t = readtrans(buf);
	fast = cubetofast(cube);
	fast = transform(fast, t);

	return fasttocube(fast);
}

cube_t
readcube(char *format, char *buf)
{
	cube_t cube;

	if (!strcmp(format, "H48")) {
		cube = readcube_H48(buf);
	} else if (!strcmp(format, "LST")) {
		cube = readcube_LST(buf);
	} else {
		DBG_LOG("Cannot read cube in the given format\n");
		cube = zero;
	}

	return cube;
}

void
writecube(char *format, cube_t cube, char *buf)
{
	char *errormsg;
	size_t len;

	if (!cube_consistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto writecube_error;
	}

	if (!strcmp(format, "H48")) {
		writecube_H48(cube, buf);
	} else if (!strcmp(format, "LST")) {
		writecube_LST(cube, buf);
	} else {
		errormsg = "ERROR: cannot write cube in the given format";
		goto writecube_error;
	}

	return;

writecube_error:
	DBG_LOG("writecube error, see stdout for details\n");
	len = strlen(errormsg);
	memcpy(buf, errormsg, len);
	buf[len] = '\n';
	buf[len+1] = '\0';
}

_static int
permsign(uint8_t *a, int n)
{
	int i, j;
	uint8_t ret = 0;

	for (i = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

_static uint8_t
readco(char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _ctwist_cw;
	if (*str == '2')
		return _ctwist_ccw;

	DBG_LOG("Error reading CO\n");
	return _error;
}

_static uint8_t
readcp(char *str)
{
	uint8_t c;

	for (c = 0; c < 8; c++)
		if (!strncmp(str, cornerstr[c], 3) ||
		    !strncmp(str, cornerstralt[c], 3))
			return c;

	DBG_LOG("Error reading CP\n");
	return _error;
}

_static uint8_t
readeo(char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _eflip;

	DBG_LOG("Error reading EO\n");
	return _error;
}

_static uint8_t
readep(char *str)
{
	uint8_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

	DBG_LOG("Error reading EP\n");
	return _error;
}

_static cube_t
readcube_H48(char *buf)
{
	int i;
	uint8_t piece, orient;
	cube_t ret = {0};
	char *b;
	
	b = buf;

	for (i = 0; i < 12; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readep(b)) == _error)
			return zero;
		b += 2;
		if ((orient = readeo(b)) == _error)
			return zero;
		b++;
		ret.edge[i] = piece | orient;
	}
	for (i = 0; i < 8; i++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if ((piece = readcp(b)) == _error)
			return zero;
		b += 3;
		if ((orient = readco(b)) == _error)
			return zero;
		b++;
		ret.corner[i] = piece | orient;
	}

	return ret;
}

_static uint8_t
readpiece_LST(char **b)
{
	uint8_t ret;
	bool read;

	while (**b == ',' || **b == ' ' || **b == '\t' || **b == '\n')
		(*b)++;

	for (ret = 0, read = false; **b >= '0' && **b <= '9'; (*b)++) {
		read = true;
		ret = ret * 10 + (**b) - '0';
	}

	return read ? ret : _error;
}

_static cube_t
readcube_LST(char *buf)
{
	int i;
	cube_t ret = {0};

	for (i = 0; i < 8; i++)
		ret.corner[i] = readpiece_LST(&buf);

	for (i = 0; i < 12; i++)
		ret.edge[i] = readpiece_LST(&buf);

	return ret;
}

_static int
writepiece_LST(uint8_t piece, char *buf)
{
	char digits[3];
	int i, len = 0;

	while (piece != 0) {
		digits[len++] = (piece % 10) + '0';
		piece /= 10;
	}

	if (len == 0)
		digits[len++] = '0';

	for (i = 0; i < len; i++)
		buf[i] = digits[len-i-1];

	buf[len] = ',';
	buf[len+1] = ' ';

	return len+2;
}

_static void
writecube_H48(cube_t cube, char *buf)
{
	uint8_t piece, perm, orient;
	int i;

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		perm = piece & _pbits;
		orient = (piece & _eobit) >> _eoshift;
		buf[4*i    ] = edgestr[perm][0];
		buf[4*i + 1] = edgestr[perm][1];
		buf[4*i + 2] = orient + '0';
		buf[4*i + 3] = ' ';
	}
	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		perm = piece & _pbits;
		orient = (piece & _cobits) >> _coshift;
		buf[48 + 5*i    ] = cornerstr[perm][0];
		buf[48 + 5*i + 1] = cornerstr[perm][1];
		buf[48 + 5*i + 2] = cornerstr[perm][2];
		buf[48 + 5*i + 3] = orient + '0';
		buf[48 + 5*i + 4] = ' ';
	}

	buf[48+39] = '\0';
}

_static void
writecube_LST(cube_t cube, char *buf)
{
	int i, ptr;
	uint8_t piece;

	ptr = 0;

	for (i = 0; i < 8; i++) {
		piece = cube.corner[i];
		ptr += writepiece_LST(piece, buf + ptr);
	}

	for (i = 0; i < 12; i++) {
		piece = cube.edge[i];
		ptr += writepiece_LST(piece, buf + ptr);
	}

	*(buf+ptr-2) = 0;
}

_static uint8_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return U;
	case 'D':
		return D;
	case 'R':
		return R;
	case 'L':
		return L;
	case 'F':
		return F;
	case 'B':
		return B;
	default:
		return _error;
	}
}

_static uint8_t
readmodifier(char c)
{
	switch (c) {
	case '1': /* Fallthrough */
	case '2': /* Fallthrough */
	case '3':
		return c - '0' - 1;
	case '\'':
		return 2;
	default:
		return 0;
	}
}

_static uint8_t
readtrans(char *buf)
{
	uint8_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	DBG_LOG("readtrans error\n");
	return _error;
}

_static int
writemoves(uint8_t *m, int n, char *buf)
{
	int i;
	size_t len;
	char *b, *s;

	for (i = 0, b = buf; i < n; i++, b++) {
		s = movestr[m[i]];
		len = strlen(s);
		memcpy(b, s, len);
		b += len;	
		*b = ' ';
	}

	if (b != buf)
		b--; /* Remove last space */
	*b = '\0';

	return b - buf;
}

_static void
writetrans(uint8_t t, char *buf)
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
}

_static cube_fast_t
move(cube_fast_t c, move_t m)
{
	return compose_fast(c, move_table[m]);
}

_static cube_fast_t
transform(cube_fast_t c, trans_t t)
{
	cube_t tcube, tinv;

	tcube = trans_table[t][NORMAL];
	tinv = trans_table[t][INVERSE];

	return t < 24 ?
	    compose_fast(compose_fast(tcube, c), tinv) :
	    invertco_fast(compose_fast(compose_fast(tcube, c), tinv));
}

_static_inline uint8_t inverse_trans(uint8_t);
_static_inline uint8_t movebase(uint8_t);
_static_inline uint8_t moveaxis(uint8_t);

_static_inline uint8_t
inverse_trans(uint8_t t)
{
	return inverse_trans_table[t];
}

_static_inline uint8_t
movebase(uint8_t move)
{
	return move / 3;
}

_static_inline uint8_t
moveaxis(uint8_t move)
{
	return move / 6;
}
