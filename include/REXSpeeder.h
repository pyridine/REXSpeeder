#ifndef REX_PAINT_H
#define REX_PAINT_H
#include <iostream>

//As of v1.02, there is a maximum of four layers in an .xp file
#define REXPAINT_MAX_NUM_LAYERS 4

namespace xp {
	typedef unsigned char uchar;
	typedef unsigned int uint;

	struct  RexTile {
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
				   instead of 10. Or something. Anyway, don't count on sizeof == 10.*/
	};

	//REXpaint identifies transparent tiles by setting their background color to 255,0,255 as of v1.02.
	//You may want to check this before converting a RexFile to your own image representation format.
	//(By default, the background of layer 1 is not transparent).
	static bool isTransparent(RexTile* tile) {
		return (tile->back_red == 255 && tile->back_green == 0 && tile->back_blue == 255);
	}

	struct RexLayer {
		RexTile* tiles;

		RexLayer(int width, int height)
			:tiles(new RexTile[width*height]) {}

		~RexLayer() {
			delete[] tiles;
			tiles = NULL;
		}
	};

	class RexFile {
	private:
		int version;
		int width, height, num_layers;
	public:
		RexFile(std::string const& filename); //Load an .xp file
		RexFile(int _version, int _width, int _height, int _num_layers); //Create a blank RexFile with the specified properties
		~RexFile();
		void save(std::string const& filename); //Save this object into a valid .xp file that RexPaint can load.

		//Combines all the layers of the image into one layer.
		//Respects transparency.
		inline void flatten();

		inline int getVersion() { return version; };
		inline int getWidth() { return width; };
		inline int getHeight() { return height; };
		inline int getNumLayers() { return num_layers; };

		RexLayer* layers[REXPAINT_MAX_NUM_LAYERS];
		inline RexTile* getTile(int layer, int x, int y) { return &layers[layer]->tiles[x + (y * width)]; };
		inline RexTile* getTile(int layer, int index) { return &layers[layer]->tiles[index]; };
		inline void setTile(int layer, int x, int y, RexTile& val) { *getTile(layer, x, y) = val; };

	private:
		RexFile(); //No default constructor.
	};

}

#endif //REX_PAINT_H
