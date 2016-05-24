# REXSpeeder

A cross-platform C++ library for loading and saving [REXPaint](http://www.gridsagegames.com/rexpaint/) files quickly.

Think of REXSpeeder as the glue between REXPaint and your project. 

### Features:
 * Really fast.
 * Simple API.
 * Can export valid .xp files.

## Installation

See `INSTALL` for instructions on how to use REXSpeeder for your project. 

## Usage

Say we have an image in REXPaint, "nyan.xp":

![Before](https://github.com/pyridine/REXSpeeder/raw/master/example/before.png)

In this image, the cat is on layer 2, and the blue background and star are on layer 1.
Then, after `#include <REXSpeeder.h>`, 

````
xp::RexImage nyan("nyan.xp");

nyan.flatten();

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
````

we have a new file, "cat.xp", which can be loaded in REXPaint:

![After](https://github.com/pyridine/REXSpeeder/raw/master/example/after.png)

See `example/` for the source code and a Visual Studio project to build it. If you're using CMake, the example is built along with the library.

REXSpeeder functions are documented in `include/REXSpeeder.h`.

## Acknowledgements

I am technically and inspirationally indebted to GamePopper's REXPaint library, which can be found [here](https://github.com/gamepopper/REXReader-CPlusPlus).
