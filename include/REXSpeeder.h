/*For version 1.02 of REXPaint*/

#ifndef REX_PAINT_H
#define REX_PAINT_H
#include <iostream>

//There is a maximum of four layers in an .xp file
#define REXPAINT_MAX_NUM_LAYERS 4

namespace xp {
	typedef unsigned char uchar;
	typedef unsigned int uint;

	//This struct's layout matches that of an .xp file.
	struct  RexTile {
		//I don't know why a supposedly CP437 character should be 4 bytes wide, but thus sayeth the manual.
		uint  character;  
		uchar fore_red;
		uchar fore_green;
		uchar fore_blue;
		uchar back_red;
		uchar back_green;
		uchar back_blue;
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
			tiles = nullptr;
		}
	};

	class RexFile {
	public:
		//Load an .xp file into a new RexFile.
		RexFile(std::string const& filename);
		//Save this RexFile into a vavlid .xp file that RexPaint can load (if the ".xp" suffix is present).
		void save(std::string const& filename);
		//Create a blank RexFile with the specified attributes.
		RexFile(int _version, int _width, int _height, int _num_layers); 
		~RexFile();

		//Combines all the layers of the image into one layer.
		//Respects transparency.
		void flatten();

		//Image attributes
		inline int getVersion() { return version; };
		inline int getWidth() { return width; };
		inline int getHeight() { return height; };
		inline int getNumLayers() { return num_layers; };

		//Returns a pointer to a single tile specified by layer, x coordinate, y coordinate.
		//0,0 is the top-left corner.
		inline RexTile* getTile(int layer, int x, int y) { return &layers[layer]->tiles[y + (x * height)]; };

		//Returns a pointer to a single tile specified by layer and the actual index into the array.
		//Useful for iterating through a whole layer in one go for coordinate-nonspecific tasks.
		inline RexTile* getTile(int layer, int index) { return &layers[layer]->tiles[index]; };

		//Replaces the data for a tile. Not super necessary, but might save you some syntax.
		inline void setTile(int layer, int x, int y, RexTile& val) { *getTile(layer, x, y) = val; };

		//Replaces the data for a tile. Not super necessary, but might save you some syntax.
		inline void setTile(int layer, int i, RexTile& val) { *getTile(layer, i) = val; };

	private:
		//Image properties
		int version;
		int width, height, num_layers;
		RexLayer* layers[REXPAINT_MAX_NUM_LAYERS];

		//Forbid default construction.
		RexFile(); 
	};

}

#endif //REX_PAINT_H
