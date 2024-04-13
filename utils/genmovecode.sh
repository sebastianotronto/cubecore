#!/bin/sh

gcc -DDEBUG h48_to_lst.c ../cube.c -o h48_to_lst

gen() {
	for f in move_??_*.txt; do
		move="$(echo $f | sed 's/.*_// ; s/\.txt//')"
		lst="$(./h48_to_lst <$f)"
		c="$(echo $lst | cut -d ' ' -f -8 | rev | cut -c 2- | rev)"
		e="$(echo $lst | cut -d ' ' -f 9-)"
		printf '[%s] = {\n' "$move"
		printf '\t.corner = {%s},\n' "$c"
		printf '\t.edge = {%s}\n' "$e"
		printf '},\n'
	done
}

gen
rm -f h48_to_lst invert
