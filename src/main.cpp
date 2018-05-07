
#include <iostream>
#include <fstream>
#include <list>

#include "gcodeFileLoader.h"

#include "version.h"

#define PRINTER_SIZE_X  260                             // print X axis size mm
#define PRINTER_SIZE_Y  220                             // print Y axis size mm
#define IMAGE_SCALE     20								// print area size and layer image scale factor
#define LAYER_WIDTH     PRINTER_SIZE_X *  IMAGE_SCALE   // layer paint image X size
#define LAYER_HEIGHT    PRINTER_SIZE_Y *  IMAGE_SCALE   // layer paint image Y size

#define ONLY_DRAW_LAYER 0

const char* GCODE2IMAGES_CMD_MSG_HELP =
" verbs:\n"
" \tv/version - Display gcode2images version\n"
" \th/help - Display gcode2images help\n"
" \t2IMAGES - Export input gcode to image files (layer num of images)\n"
;

const char* USAGE_MSG_HELP = "usage: gcode2images [--verbose] input [output]\n";

void printver()
{
	std::cout << "gcode2images v" << GCODE2IMAGES_VERSION_MAJOR << "." << GCODE2IMAGES_VERSION_MINOR << "\n";
	std::cout << "(c) 2018, aardanza, Vicomtech\n" 	<< std::endl;
}

void printhelp()
{
	printf("%s", USAGE_MSG_HELP);
	printf("%s", GCODE2IMAGES_CMD_MSG_HELP);
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		printhelp();
		return 0;
	}

	bool _2images = false;
	
	int nextarg = 1;
	while (nextarg < argc && argv[nextarg][0] == '-') {
		if (!strcmp(argv[nextarg], "--help") || !strcmp(argv[nextarg], "--h")) {
			printhelp();
			return 0;
		}
		else if (!strcmp(argv[nextarg], "--version") || !strcmp(argv[nextarg], "--v")) {
			printver();
			return 0;
		}
		else if (!strcmp(argv[nextarg], "--2images") || !strcmp(argv[nextarg], "-2IMAGES")) {
			_2images = true;
		}
		else{
			printhelp();
			return 0;
		}

		++nextarg;
	}

	const char* in = argv[nextarg], *out = (argc < nextarg + 2 ? NULL : argv[nextarg + 1]);

	//read imput file format
	std::string inFile = in;
	std::string inputfileFormat = inFile.substr(inFile.find_last_of(".") + 1);

	if (_2images)
	{
		GcodeFileLoader gcodeLoader;
		std::vector<GcodeLayer> layers = gcodeLoader.load(inFile);

		cv::Scalar e0col = cv::Scalar(50.0, 50.0, 50.0);
		cv::Scalar e1col = cv::Scalar(255.0, 255.0, 255.0);

		for (unsigned int i = 0; i < layers.size();i++)
		{
			GcodeLayerImage layer;
			layers[i].scaleLines(IMAGE_SCALE, IMAGE_SCALE);
#ifdef ONLY_DRAW_LAYER
			//only paint selected layer
			layer = layers[i].paint(cv::Size(LAYER_WIDTH, LAYER_HEIGHT), e0col, e1col);
			
#else
			//create image painting before layers
			layer = layers[0].paint(cv::Size(LAYER_WIDTH, LAYER_HEIGHT), e0col, e1col);
			for (unsigned int j = 1; j <= i; j++){
				GcodeLayerImage l = layers[j].paint(cv::Size(LAYER_WIDTH, LAYER_HEIGHT), e0col, e1col);
				cv::add(layer.image, l.image, layer.image);
				cv::add(layer.mask, l.mask, layer.mask);
			}
#endif

			if (!layer.image.empty())
			{
				std::string outFilePath, extension, filename;
				outFilePath = out;
				filename = out;

				// Remove directory if present.
				// Do this before extension removal incase directory has a period character.
				const size_t last_slash_idx = outFilePath.find_last_of("\\/");
				if (std::string::npos != last_slash_idx)
				{
					outFilePath.erase(last_slash_idx + 1);
					filename.erase(0, last_slash_idx + 1);
				}

				// Remove extension if present.
				extension = filename;
				const size_t period_idx = filename.rfind('.');
				if (std::string::npos != period_idx)
				{
					extension.erase(0, period_idx);
					filename.erase(period_idx);
				}
				else{
					extension = ".jpg";
				}

				filename += std::to_string(i);
				filename += extension;

				outFilePath += filename;

				cv::imwrite(outFilePath.c_str(), layer.image);

				printf("image exported to: %s\n", outFilePath.c_str());
			}
		}
	}

	return 0;
}
