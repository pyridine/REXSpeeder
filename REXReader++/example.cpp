#include <iostream>
#include <sstream>
#include "RexPaint.h"

using namespace xp;

int main(){

	RexIO t;
	
	RexFile* fupa = t.loadFile("smile.xp");

	std::cout << "layers: " << fupa->num_layers << " width: " << fupa->width << " height: " << fupa->height << std::endl;

	t.saveFile(*fupa, "out.xp");

	return EXIT_SUCCESS;
}
