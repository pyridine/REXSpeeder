#ifndef REX_PAINT_H
#define REX_PAINT_H
#include <iostream>
#include "zlib.h"
#include "zconf.h"

namespace xp {

	struct RexTile {
		/*This layout matches that of the .xp file*/
		unsigned int  characterCode;  //Why this is an unsigned int I have no idea
		unsigned char foregroundRed;
		unsigned char foregroundGreen;
		unsigned char foregroundBlue;
		unsigned char backgroundRed;
		unsigned char backgroundGreen;
		unsigned char backgroundBlue;
	};

	class RexLayer {
	public:
		RexTile* tiles;

		RexLayer(int width, int height)
			:tiles(new RexTile[width*height]) {}

		~RexLayer() {
			delete[] tiles;
			tiles = NULL;
		}
	};

	class RexFile {
	public:
		unsigned int version; //Only needed for saving files.
		unsigned int width, height, num_layers;
		//As of v1.02, there is a maximum of four layers to a .xp file
		RexLayer* layers[4];

		RexFile(unsigned int _width, unsigned int _height, unsigned int _num_layers)
			:width(_width), height(_height), num_layers(_num_layers) {
			for (unsigned int i = 0; i < num_layers; i++) {
				layers[i] = new RexLayer(width, height);
			}
		}

		~RexFile() {
			for (unsigned int i = 0; i < num_layers; i++) {
				delete(layers[i]);
			}
		}
	};

	class RexIO {
	public:
		RexFile* loadFile(std::string const& filename);
		bool saveFile(RexFile const& xp, std::string const& filename);
	};

}

#endif //REX_PAINT_H
