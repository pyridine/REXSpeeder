#include <REXSpeeder.h>
#include <sstream>
#include <chrono>

void testTime(std::string fname);

int main() {
	try {
		xp::RexImage nyan("files/nyan.xp");

		/*Flatten all layers into one, respecting transparency.*/
		nyan.flatten();

		/*Flip around the R,G,B values.*/
		/*It is also possible to reference a tile by its index into its layer's tile array.*/
		for (int x = 0; x < nyan.getWidth(); x++) {
			for (int y = 0; y < nyan.getHeight(); y++) {
				xp::RexTile original = *nyan.getTile(0, x, y);
				xp::RexTile modified = original;

				modified.fore_red = original.fore_blue;
				modified.fore_blue = original.fore_green;
				modified.fore_green = original.fore_red;

				nyan.setTile(0, x, y, modified);
			}
		}
		nyan.save("cat.xp");
	}
	catch (xp::Rexception& e) {
		std::cerr << "Exception! " << e.what() << " [" << e.code << "]" << std::endl;
	}

	std::cout << "Successfuly saved, modified, and loaded the file." << std::endl;


	/*The following functions test how long it takes to save and load.*/
#define TESTING_TIME 1
#if TESTING_TIME
	/*These files are 10x10, 20x20, etc.*/
	testTime("files\\10");
	testTime("files\\20");
	testTime("files\\40");
	testTime("files\\80");
	testTime("files\\160");
	testTime("files\\320");
	testTime("files\\640");
	testTime("files\\1280");
	testTime("files\\2500"); //Maximum size REXPaint 1.02 allows
#endif
	return EXIT_SUCCESS;
}


/*Tests how much time, in microseconds, it takes to load and save the given file. Overwrites the file with the original file.*/
void testTime(std::string fname)
{
	std::stringstream s;
	s  << fname << ".xp";
	fname = s.str();

	auto start = std::chrono::high_resolution_clock::now();
	xp::RexImage file(fname);
	auto elapsed = std::chrono::high_resolution_clock::now() - start;

	std::cout << fname << " load: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << std::endl;

	auto start2 = std::chrono::high_resolution_clock::now();
	file.save(s.str());
	auto elapsed2 = std::chrono::high_resolution_clock::now() - start;

	std::cout << fname << " save: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count() << std::endl << std::endl;
}

