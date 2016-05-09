#include "RexPaint.h"

/*Each function in this class opens and closes its gzFile, so we can store it statically out here.*/
static gzFile gz;

static int readInt() {
	int value = 0;
	Byte buffer[4];

	/*TODO: why can't I gzread(in,&value,4) ? */
	/*TODO: Careful where you use this and not an unsigned 
	        int reader (could just cast I guess)*/

	gzread(gz, buffer, 4);

	//TODO: WHAT???? IS THIS?
	value = (value << 8) + buffer[3];
	value = (value << 8) + buffer[2];
	value = (value << 8) + buffer[1];
	value = (value << 8) + buffer[0];
	

	return value;
}

xp::RexFile* xp::RexIO::loadFile(std::string const& filename) {
	//difference from rexreader++:
	//does all the reading in one go.

	gz = gzopen(filename.c_str(), "rb"); //Read binary	

	if (gz == NULL) {
		std::cerr << "Failed to open .xp file." << filename.c_str() << std::endl;
		return nullptr;
	}
	try {
		int version = readInt();
		//Note implicit conversion
		unsigned int layers = readInt();
		unsigned int width  = readInt();
		unsigned int height = readInt();

		RexFile* xp = new RexFile(version, width, height, layers);

		const int tileLen = 10;
		Byte buffer[tileLen];

		//We have already read the w and h values, but we can't read them twice for the first layer or we'll be unsynced
		bool firstread = true;

		for (int layer_index = 0; layer_index < layers; layer_index++) {
			if (!firstread) { //this logic is SO FUCKING KLUDGY
				//Read and throw away the repeated layer and width data
				readInt();
				readInt();
			}
			else { firstread = false; }

			xp::RexLayer* layer = xp->layers[layer_index];
			for (unsigned int x = 0; x < width; x++) {
				for (unsigned int y = 0; y < height; y++) {
					xp::RexTile* tile = &layer->tiles[x + (y * width)];
					gzread(gz, tile, tileLen);
				}
			}
		}
		return xp;
	}
	catch (int e) {
		throw("Bad .xp file " + e);
	}

	gzclose(gz);

	return nullptr;
}

bool xp::RexIO::saveFile(RexFile const& xp, std::string const & filename) {
	gzFile gz = gzopen(filename.c_str(),"wb"); //Write binary

	typedef void* vp;

	gzwrite(gz,(vp)&xp.version,4);
	gzwrite(gz,(vp)&xp.layers, 4);

	for (int i = 0; i < xp.num_layers; ++i) {
		gzwrite(gz,(vp)&xp.width,4);
		gzwrite(gz,(vp)&xp.height,4);
		for (int x = 0; x < xp.width; ++x) {
			for (int y = 0; y < xp.height; ++y) {
				xp::RexTile* tile = &xp.layers[i]->tiles[x + (y * xp.width)];
				gzwrite(gz, (vp)&tile->character, 4);

				gzwrite(gz, (vp)&tile->fore_red, 1);
				gzwrite(gz, (vp)&tile->fore_green, 1);
				gzwrite(gz, (vp)&tile->fore_blue, 1);

				gzwrite(gz, (vp)&tile->back_red, 1);
				gzwrite(gz, (vp)&tile->back_green, 1);
				gzwrite(gz, (vp)&tile->back_blue, 1);
			}
		}
	}

	gzflush(gz,Z_FULL_FLUSH);
	gzclose(gz);

	return true;
}
