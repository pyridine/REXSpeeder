#include "REXSpeeder.h"
#include "zlib.h"

namespace xp {
	RexFile::RexFile(std::string const & filename) {
		const int tileLen = 10; //number of bytes in a tile. Probably not equal to sizeof(RexTile) due to padding.

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
				for (unsigned int x = 0; x < width; x++) {
					for (unsigned int y = 0; y < height; y++) {
						RexTile* tile = &layer->tiles[x + (y * width)];
						gzread(gz, tile, tileLen);
					}
				}
				//The layer and height information is repeated.
				//This is expected to read off the end after the last layer.
				gzseek(gz, SEEK_CUR, sizeof(width) + sizeof(height));
			}
		}
		catch (int e) {
			throw("Bad .xp file " + e);
		}
		gzclose(gz);
	}

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

	void RexFile::save(std::string const & filename) {
		typedef void* vp;
		const int color_size = sizeof(RexTile::fore_red);
		const int chara_size = sizeof(RexTile::character);

		gzFile gz = gzopen(filename.c_str(), "wb");

		gzwrite(gz, (vp)&version, sizeof(version));
		gzwrite(gz, (vp)&num_layers, sizeof(num_layers));

		for (int i = 0; i < num_layers; ++i) {
			gzwrite(gz, (vp)&width, 4);
			gzwrite(gz, (vp)&height, 4);
			for (int x = 0; x < width; ++x) {
				for (int y = 0; y < height; ++y) {
					RexTile* tile = getTile(i, x, y);
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
		}

		gzflush(gz, Z_FULL_FLUSH);
		gzclose(gz);
	}
}
