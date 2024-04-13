#!/bin/sh

gcc -DDEBUG h48_to_lst.c ../cube.c -o h48_to_lst
gcc -DDEBUG invert.c ../cube.c -o invert

lineavx() { printf '#define _trans_cube_%s ' "$1"; }
linesrc() { printf '_static cube_fast_t _trans_cube_%s = ' "$1"; }
sedavx() { sed '1,2s/$/ \\/ ; 3s/$/)/ ; 3q'; }
sedsrc() { sed '3s/$/ };/ ; 3q'; }

gen() {
	for f in transform_??_???.txt; do
		trans="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		printf '[%s] = {\n' "$trans"

		printf '\t[NORMAL] = {\n'
		lst="$(./h48_to_lst <"$f")"
		c="$(echo $lst | cut -d ' ' -f -8 | rev | cut -c 2- | rev)"
		e="$(echo $lst | cut -d ' ' -f 9-)"
		printf '\t\t.corner = {%s},\n' "$c"
		printf '\t\t.edge = {%s}\n' "$e"
		printf '\t},\n'

		printf '\t[INVERSE] = {\n'
		inv="$(./invert <"$f" | ./h48_to_lst)"
		c="$(echo $inv | cut -d ' ' -f -8 | rev | cut -c 2- | rev)"
		e="$(echo $inv | cut -d ' ' -f 9-)"
		printf '\t\t.corner = {%s},\n' "$c"
		printf '\t\t.edge = {%s}\n' "$e"
		printf '\t}\n'

		printf '},\n'
	done
}

gen
rm -f h48_to_lst invert
