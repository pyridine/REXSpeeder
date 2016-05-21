/*For version 1.02 of REXPaint*/

#ifndef REXSPEEDER_H
#define REXSPEEDER_H
#include <iostream>

//There is a maximum of four layers in an .xp file
#define REXPAINT_MAX_NUM_LAYERS 4

namespace xp {
	
	//This struct matches the order and width of data in .xp tiles.
	struct  RexTile {
		//I don't know why a CP437 character should be 4 bytes wide, but thus spoke the manual.
		unsigned int  character;  
		unsigned char fore_red;
		unsigned char fore_green;
		unsigned char fore_blue;
		unsigned char back_red;
		unsigned char back_green;
		unsigned char back_blue;
	};

	//REXpaint identifies transparent tiles by setting their background color to 255,0,255 as of v1.02.
	//You may want to check this before converting a RexFile to your own image representation format.
	//(By default, no tile in the first layer is transaprent).
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
		//Save this RexFile into a valid .xp file that RexPaint can load (if the ".xp" suffix is present).
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

		//Replaces the data for a tile. Not super necessary, but might save you a couple lines.
		inline void setTile(int layer, int x, int y, RexTile& val) { *getTile(layer, x, y) = val; };

		//Replaces the data for a tile. Not super necessary, but might save you a couple lines.
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

#endif //REXSPEEDER_H
