#include "REXSpeeder.h"

int main(){
	xp::RexFile nyan("..\\..\\layered.xp");

	/*
	for(int i = 0; i < nyan.getNumLayers(); ++i)
	for (int x = 0; x < nyan.getWidth(); x++) {
		for (int y = 0; y < nyan.getHeight(); y++) {
			xp::RexTile original = *nyan.getTile(i, x, y);
			nyan.getTile(i, x, y)->fore_red   = original.fore_blue;
			nyan.getTile(i, x, y)->fore_blue  = original.fore_green;
			nyan.getTile(i, x, y)->fore_green = original.fore_red;
		}
	}
	*/

	nyan.save("..\\..\\lay.xp");

	return EXIT_SUCCESS;
}
