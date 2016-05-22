#include "REXSpeeder.h"
#include "zlib.h"

//===========================================================================================================//
//    Safe I/O (where "safe" means "will throw errors")                                                      //
//                                                                                                           //
//   These functions will throw an error message from gzerror, and set errno to the error code.              //
//===========================================================================================================//
static void s_gzread(gzFile g, voidp buf, unsigned int len)
{
	if (gzread(g, buf, len) > 0)
		return;

	/*We expect to read past the end of the file after the last layer.*/
	if (gzeof(g))
		return;

	throw gzerror(g,&errno);
}

static void s_gzwrite(gzFile g, voidp buf, unsigned int len)
{
	if (gzwrite(g, buf, len) > 0)
		return;

	throw gzerror(g,&errno);
}

static gzFile s_gzopen(const std::string filename, const char* permissions)
{
	gzFile g = gzopen(filename.c_str(), permissions);

	if (g != Z_NULL)
		return g;

	throw gzerror(g,&errno);
}

namespace xp {
//===========================================================================================================//
//    Loading an xp file                                                                                     //
//===========================================================================================================//
	RexFile::RexFile(std::string const & filename)
	{
		typedef void* vp;
		const int tileLen = 10; //Number of bytes in a tile. Not equal to sizeof(RexTile) due to padding.

		gzFile gz;
		try {
			gz = s_gzopen(filename.c_str(), "rb");

			s_gzread(gz, (vp)&version, sizeof(version));
			s_gzread(gz, (vp)&num_layers, sizeof(num_layers));
			s_gzread(gz, (vp)&width, sizeof(width));
			s_gzread(gz, (vp)&height, sizeof(height));

			for (int i = 0; i < num_layers; i++)
				layers[i] = new RexLayer(width, height);

			for (int layer_index = 0; layer_index < num_layers; layer_index++) {
				for (int i = 0; i < width*height; ++i)
					s_gzread(gz, getTile(layer_index, i), tileLen);

				//The layer and height information is repeated.
				//This is expected to read off the end after the last layer.
				s_gzread(gz, (vp)&width, sizeof(width));
				s_gzread(gz, (vp)&height, sizeof(height));
				//gzseek(gz, SEEK_CUR, sizeof(width) + sizeof(height));
			}
		}
		catch (...) { throw; }

		gzclose(gz);
	}

//===========================================================================================================//
//    Saving an xp file                                                                                      //
//===========================================================================================================//
	void RexFile::save(std::string const & filename)
	{
		typedef void* vp;
		const int color_size = sizeof(RexTile::fore_red);
		const int chara_size = sizeof(RexTile::character);

		try {
			gzFile gz = s_gzopen(filename.c_str(), "wb");

			s_gzwrite(gz, (vp)&version, sizeof(version));
			s_gzwrite(gz, (vp)&num_layers, sizeof(num_layers));

			for (int layer = 0; layer < num_layers; ++layer) {
				s_gzwrite(gz, (vp)&width, sizeof(width));
				s_gzwrite(gz, (vp)&height, sizeof(height));

				for (int i = 0; i < width*height; ++i) {
					RexTile* tile = &layers[layer]->tiles[i];
					//Character
					s_gzwrite(gz, (vp)&tile->character, chara_size);
					//Foreground
					s_gzwrite(gz, (vp)&tile->fore_red, color_size);
					s_gzwrite(gz, (vp)&tile->fore_green, color_size);
					s_gzwrite(gz, (vp)&tile->fore_blue, color_size);
					//Background
					s_gzwrite(gz, (vp)&tile->back_red, color_size);
					s_gzwrite(gz, (vp)&tile->back_green, color_size);
					s_gzwrite(gz, (vp)&tile->back_blue, color_size);
				}
			}

			gzflush(gz, Z_FULL_FLUSH);
			gzclose(gz);
		}
		catch (...) { throw; }
	}

//===========================================================================================================//
//    Constructors / Destructors                                                                             //
//===========================================================================================================//
	RexFile::RexFile(int _version, int _width, int _height, int _num_layers)
		:version(_version), width(_width), height(_height), num_layers(_num_layers)
	{
		for (int i = 0; i < num_layers; i++)
			layers[i] = new RexLayer(width, height);

		//All layers above the first are set transparent.
		for (int l = 1; l < num_layers; l++) {
			for (int i = 0; i < width*height; ++i) {
				RexTile t = transparentTile();
				setTile(l, i, t);
			}
		}
	}

	RexFile::~RexFile() 
	{
		for (int i = 0; i < num_layers; i++) {
			delete(layers[i]);
		}
	}

//===========================================================================================================//
//    Utility Functions                                                                                      //
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


	bool isTransparent(RexTile * tile)
	{
		return (tile->back_red == 255 && tile->back_green == 0 && tile->back_blue == 255);
	}

	RexTile transparentTile()
	{
		RexTile t;
		t.back_red = 255;
		t.back_blue = 255;
		t.back_green = 0;
		return t;
	}

//===========================================================================================================//
//    RexLayer constructor/destructor                                                                        //
//===========================================================================================================//

	RexLayer::RexLayer(int width, int height) :tiles(new RexTile[width*height]) {} 

	RexLayer::~RexLayer()
	{
			delete[] tiles;
			tiles = nullptr;
	}
}
