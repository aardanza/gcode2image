#ifndef GCODEFILELOADER_H
#define GCODEFILELOADER_H

#include <gcode_program.h>

#include "gcodelayer.h"


class GcodeFileLoader
{
public:

	struct LayerTableRow{
		int layerNum;
		std::string extruder;
		std::string zVal;
	};
	struct CommandTableRow{
		int rowNum;
		int id;
		std::string state;
		std::string response;
		std::string command;
	};

	struct DataBuffer{
		float progress;
		std::vector<LayerTableRow> layerTableRows;
		std::vector<CommandTableRow> commandTableRows;
	};

	GcodeFileLoader();
	~GcodeFileLoader();

	std::vector<GcodeLayer>	 load(std::string file);

	gpr::gcode_program* getGcodeProgram() { return &_gcode; }

	std::vector<GcodeLayer> getLayers();

protected:

	gpr::gcode_program				_gcode;
	std::vector<GcodeLayer>			_layers;
};

#endif // GCODEFILELOADER_H
