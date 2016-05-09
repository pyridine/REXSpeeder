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

		RexFile* xp = new RexFile(width, height, layers);

		//HAVE READ THE TILES
		int offset;

		const int bufferLen = 10;
		Byte buffer[bufferLen];
		
		//We have already read the w and h values, but we can't read them twice for the first layer or we'll be unsynced
		bool firstread = true;

		for (unsigned int layer = 0; layer < layers; layer++) {
			//Read and throw away the repeated layer and width data

			if (!firstread) { //this logic is SO FUCKING KLUDGY
				readInt();
				readInt();
			}
			else { firstread = false; }

			for (unsigned int x = 0; x < width; x++)
				for (unsigned int y = 0; y < height; y++)
				{
					RexTile* tile = &xp->layers[layer]->tiles[x + (y * width)];
					gzread(gz, buffer, bufferLen);
					memcpy(tile, buffer, bufferLen);
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

static void write(void* data, size_t datalen) {
	gzwrite(gz, data, datalen);
	gzseek(gz, datalen, SEEK_CUR);
}

bool xp::RexIO::saveFile(RexFile const& xp, std::string const & filename) {
	gzFile gzfile = gzopen(filename.c_str(),"wb"); //Write binary

	write((void*)&xp.version,4);
	write((void*)&xp.layers, 4);

	for (int i = 0; i < xp.num_layers; ++i) {
		write((void*)&xp.width,4);
		write((void*)&xp.height,4);
		for (int x = 0; x < xp.width; ++x) {
			for (int y = 0; y < xp.height; ++y) {
				write((void*)&xp.layers[i][x + (y*xp.width)],sizeof(RexTile));
			}
		}
	}
	  
	gzflush(gzfile,Z_FULL_FLUSH);
	gzclose(gz);

	return true;
}
