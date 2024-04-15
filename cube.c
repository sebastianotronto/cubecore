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

#include "constants.h"

_static_inline cube_t invertco(cube_t);
_static int permsign(uint8_t *, int);
_static uint8_t readco(const char *);
_static uint8_t readcp(const char *);
_static uint8_t readeo(const char *);
_static uint8_t readep(const char *);
_static cube_t read_H48(const char *);
_static uint8_t readpiece_LST(const char **);
_static cube_t read_LST(const char *);
_static int writepiece_LST(uint8_t, char *);
_static void write_H48(cube_t, char *);
_static void write_LST(cube_t, char *);
_static uint8_t readmove(char);
_static uint8_t readmodifier(char);

_static uint8_t
readco(const char *str)
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
readcp(const char *str)
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
readeo(const char *str)
{
	if (*str == '0')
		return 0;
	if (*str == '1')
		return _eflip;

	DBG_LOG("Error reading EO\n");
	return _error;
}

_static uint8_t
readep(const char *str)
{
	uint8_t e;

	for (e = 0; e < 12; e++)
		if (!strncmp(str, edgestr[e], 2))
			return e;

	DBG_LOG("Error reading EP\n");
	return _error;
}

_static cube_t
read_H48(const char *buf)
{
	int i;
	uint8_t piece, orient;
	cube_t ret = {0};
	const char *b;
	
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
readpiece_LST(const char **b)
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
read_LST(const char *buf)
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
write_H48(cube_t cube, char *buf)
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
write_LST(cube_t cube, char *buf)
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

_static_inline cube_t
invertco(cube_t c)
{
	uint8_t i, piece, orien;
	cube_t ret;

	ret = c;
	for (i = 0; i < 8; i++) {
		piece = c.corner[i];
		orien = ((piece << 1) | (piece >> 1)) & _cobits2;
		ret.corner[i] = (piece & _pbits) | orien;
	}

	return ret;
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
	cube_t ret;
	uint8_t i, piece1, piece2, p, orien, aux, auy;

	DBG_ASSERT(cube_consistent(c1) && cube_consistent(c2),
	    zero, "cube_compose error: inconsistent cube\n")

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
cube_move(cube_t c, move_t m)
{
	return cube_compose(c, move_table[m]);
}

cube_t
cube_transform(cube_t c, trans_t t)
{
	cube_t tcube, tinv;

	tcube = trans_table[t][NORMAL];
	tinv = trans_table[t][INVERSE];

	return t < 24 ?
	    cube_compose(cube_compose(tcube, c), tinv) :
	    invertco(cube_compose(cube_compose(tcube, c), tinv));
}

int64_t
cube_coord_co(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 0, p = 1; i < 7; i++, p *= 3)
		ret += p * (c.corner[i] >> _coshift);

	return ret;
}

int64_t
cube_coord_eo(cube_t c)
{
	int i, p;
	int64_t ret;

	for (ret = 0, i = 1, p = 1; i < 12; i++, p *= 2)
		ret += p * (c.edge[i] >> _eoshift);

	return ret;
}

cube_t
cube_read(const char *format, const char *buf)
{
	cube_t cube;

	if (!strcmp(format, "H48")) {
		cube = read_H48(buf);
	} else if (!strcmp(format, "LST")) {
		cube = read_LST(buf);
	} else {
		DBG_LOG("Cannot read cube in the given format\n");
		cube = zero;
	}

	return cube;
}

void
cube_write(const char *format, cube_t cube, char *buf)
{
	char *errormsg;
	size_t len;

	if (!cube_consistent(cube)) {
		errormsg = "ERROR: cannot write inconsistent cube";
		goto write_error;
	}

	if (!strcmp(format, "H48")) {
		write_H48(cube, buf);
	} else if (!strcmp(format, "LST")) {
		write_LST(cube, buf);
	} else {
		errormsg = "ERROR: cannot write cube in the given format";
		goto write_error;
	}

	return;

write_error:
	DBG_LOG("cube_write error, see stdout for details\n");
	len = strlen(errormsg);
	memcpy(buf, errormsg, len);
	buf[len] = '\n';
	buf[len+1] = '\0';
}

int
cube_readmoves(const char *buf, move_t *ret)
{
	int n;
	move_t r, m;
	const char *b;

	for (n = 0, b = buf; *b != '\0'; b++) {
		while (*b == ' ' || *b == '\t' || *b == '\n')
			b++;
		if (*b == '\0')
			goto applymoves_finish;
		if ((r = readmove(*b)) == _error)
			goto applymoves_error;
		if ((m = readmodifier(*(b+1))) != 0)
			b++;
		ret[n++] = m + r;
	}

applymoves_finish:
	return n;

applymoves_error:
	DBG_LOG("applymoves error\n");
	return -1;
}

trans_t
cube_readtrans(const char *buf)
{
	trans_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	return -1;
}

char *
cube_movestr(move_t m)
{
	return movestr[m];
}

char *
cube_transstr(trans_t t)
{
	return transstr[t];
}

move_t
cube_inversemove(move_t m)
{
	return m - 2*(m%3) + 2;
}

trans_t
cube_inversetrans(trans_t t)
{
	return inverse_trans_table[t];
}
