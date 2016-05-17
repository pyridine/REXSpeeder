#include "REXSpeeder.h"

int main(){
	xp::RexFile nyan("..\\..\\cat.xp");

	for (int x = 0; x < nyan.getWidth(); x++) {
		for (int y = 0; y < nyan.getHeight(); y++) {
			xp::RexTile original = *nyan.getTile(0, x, y);
			nyan.getTile(0, x, y)->fore_red   = original.fore_blue;
			nyan.getTile(0, x, y)->fore_blue  = original.fore_green;
			nyan.getTile(0, x, y)->fore_green = original.fore_red;
		}
	}

	nyan.save("..\\..\\cat-edited.xp");

	return EXIT_SUCCESS;
}
