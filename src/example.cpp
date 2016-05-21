#include "REXSpeeder.h"

int main(){
	xp::RexFile nyan("..\\layered.xp");

	/*Flatten all layers into one, respecting transparency.*/
	nyan.flatten();

	/*Flip around the R,G,B values.*/
	for (int layer = 0; layer < nyan.getNumLayers(); ++layer)
		for (int x = 0; x < nyan.getWidth(); x++) {
			for (int y = 0; y < nyan.getHeight(); y++) {
				xp::RexTile original = *nyan.getTile(layer, x, y);
				nyan.getTile(layer, x, y)->fore_red = original.fore_blue;
				nyan.getTile(layer, x, y)->fore_blue = original.fore_green;
				nyan.getTile(layer, x, y)->fore_green = original.fore_red;
			}
		}

	nyan.save("..\\lay.xp");

	return EXIT_SUCCESS;
}
