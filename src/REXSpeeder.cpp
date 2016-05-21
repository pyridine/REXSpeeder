#include "REXSpeeder.h"
#include "zlib.h"

namespace xp {
//===========================================================================================================//
//    Loading an xp file                                                                                     //
//===========================================================================================================//
	RexFile::RexFile(std::string const & filename) {
		const int tileLen = 10; //Number of bytes in a tile. Not equal to sizeof(RexTile) due to padding.

		gzFile gz = gzopen(filename.c_str(), "rb");

		if (gz == NULL) { std::cerr << "Failed to open .xp file." << filename.c_str() << std::endl; }
		try {
			gzread(gz, (void*)&version, sizeof(version));
			gzread(gz, (void*)&num_layers, sizeof(num_layers));
			gzread(gz, (void*)&width, sizeof(width));
			gzread(gz, (void*)&height, sizeof(height));

			for (int i = 0; i < num_layers; i++)
				layers[i] = new RexLayer(width, height);

			for (int layer_index = 0; layer_index < num_layers; layer_index++) {
				RexLayer* layer = layers[layer_index];
				for (int i = 0; i < width*height; ++i){
					RexTile* tile = &layer->tiles[i];
					gzread(gz, tile, tileLen);
				}

				//The layer and height information is repeated.
				//This is expected to read off the end after the last layer.
				gzread(gz, (void*)&width, sizeof(width));
				gzread(gz, (void*)&height, sizeof(height));
				//gzseek(gz, SEEK_CUR, sizeof(width) + sizeof(height));
			}
		}
		catch (int e) {
			throw("Bad .xp file " + e);
		}
		gzclose(gz);
	}

//===========================================================================================================//
//    Saving an xp file                                                                                      //
//===========================================================================================================//
	void RexFile::save(std::string const & filename) {
		typedef void* vp;
		const int color_size = sizeof(RexTile::fore_red);
		const int chara_size = sizeof(RexTile::character);

		gzFile gz = gzopen(filename.c_str(), "wb");

		gzwrite(gz, (vp)&version, sizeof(version));
		gzwrite(gz, (vp)&num_layers, sizeof(num_layers));

		for (int layer = 0; layer < num_layers; ++layer) {
			gzwrite(gz, (vp)&width, sizeof(width));
			gzwrite(gz, (vp)&height, sizeof(height));

			for (int i = 0; i < width*height; ++i) {
				RexTile* tile = &layers[layer]->tiles[i];
				//Character
				gzwrite(gz, (vp)&tile->character, chara_size);
				//Foreground
				gzwrite(gz, (vp)&tile->fore_red, color_size);
				gzwrite(gz, (vp)&tile->fore_green, color_size);
				gzwrite(gz, (vp)&tile->fore_blue, color_size);
				//Background
				gzwrite(gz, (vp)&tile->back_red, color_size);
				gzwrite(gz, (vp)&tile->back_green, color_size);
				gzwrite(gz, (vp)&tile->back_blue, color_size);
			}
		}

		gzflush(gz, Z_FULL_FLUSH);
		gzclose(gz);
	}

//===========================================================================================================//
//    Constructors / Destructors                                                                             //
//===========================================================================================================//
	RexFile::RexFile(int _version, int _width, int _height, int _num_layers)
		:version(_version), width(_width), height(_height), num_layers(_num_layers) {
		for (int i = 0; i < num_layers; i++) {
			layers[i] = new RexLayer(width, height);
		}
	}

	RexFile::~RexFile() {
		for (int i = 0; i < num_layers; i++) {
			delete(layers[i]);
		}
	}

//===========================================================================================================//
//    Utility functions                                                                                      //
//===========================================================================================================//
	void RexFile::flatten() {
		if (num_layers == 1)
			return;

		//Paint the last layer onto the second-to-last
		for (int i = 0; i < width*height; ++i) {
			RexTile* overlay = getTile(num_layers - 1, i);
			if (!isTransparent(overlay)) {
				*getTile(num_layers - 2, i) = *overlay;
			}
		}

		//Remove the last layer
		delete(layers[num_layers - 1]);
		--num_layers;

		//Recurse
		flatten();
	}
}
