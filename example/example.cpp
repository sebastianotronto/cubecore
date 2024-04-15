#include <cinttypes>
#include <iostream>
#include <vector>

extern "C" {
#include "../cube.h"
}

cube_t apply_alg(cube_t cube, std::vector<move_t> *moves) {
	auto ret = cube;

	for (auto m : *moves)
		ret = cube_move(ret, m);

	return ret;
}

int main() {
	auto cube = cube_new();
	auto moves = new std::vector<move_t> { R, U, R3, U3 };
	char cstr[500];

	std::cout << "The solved cube looks like this in H48 notation:\n";
	cube_write("H48", cube, cstr);
	std::string solvedstr(cstr);
	std::cout << solvedstr << "\n";
	std::cout << "After a sexy move it looks like this:\n";
	cube = apply_alg(cube, moves);
	cube_write("H48", cube, cstr);
	std::string sexystr(cstr);
	std::cout << sexystr << "\n";
	
	return 0;
}
