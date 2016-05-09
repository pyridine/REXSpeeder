#ifndef REX_PAINT_H
#define REX_PAINT_H
#include <iostream>
#include "zlib.h"
#include "zconf.h"

namespace xp {

	typedef unsigned char uchar;
	typedef unsigned int uint;

	struct RexTile {
		/*This layout matches that of the .xp file*/
		uint  character;  //Why this is an unsigned int I have no idea
		uchar fore_red;
		uchar fore_green;
		uchar fore_blue;
		uchar back_red;
		uchar back_green;
		uchar back_blue;
		/*WARNING: Despite this layout matching the RexPaint representation,
		           your compiler will probably pad this out to 12 bytes (4*3)
				   instead of 10. Anyway, don't count on sizeof == 10.*/
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
		int version; //Only needed for saving files.
		int width, height, num_layers;
		//As of v1.02, there is a maximum of four layers to a .xp file
		RexLayer* layers[4];

		RexFile(int _version,  int _width,  int _height,  int _num_layers)
			:version(_version), width(_width), height(_height), num_layers(_num_layers) {
			for ( int i = 0; i < num_layers; i++) {
				layers[i] = new RexLayer(width, height);
			}
		}

		~RexFile() {
			for (int i = 0; i < num_layers; i++) {
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
