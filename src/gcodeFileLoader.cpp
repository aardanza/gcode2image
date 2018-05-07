#include "gcodeFileLoader.h"

#include <iostream>
#include <fstream>

#include <parser.h>


void parseGLine(gpr::block& b, cv::Point3d& actPos, unsigned int &extruderId)
{
	gpr::chunk_type tp = b.get_chunk(0).tp();
	char cmdLetter = b.get_chunk(0).get_word();
	
	if (tp != 2 && cmdLetter == 'G')
	{
		int cmdVal = b.get_chunk(0).get_address().int_value();
		switch (cmdVal)
		{
		case 2:
			for (size_t i = 1; i < b.size(); ++i)
			{
				if (b.get_chunk(i).tp() != gpr::CHUNK_TYPE_WORD_ADDRESS)
					continue;

				char letter = b.get_chunk(i).get_word();
				double val = b.get_chunk(i).get_address().float_value();
				switch (letter)
				{
				case 'X':
                    actPos.x =  val;
					continue;
				case 'Y':
                    actPos.y =  val;
					continue;
				case 'I':
                    actPos.x +=  val;
					continue;
				case 'J':
                    actPos.y +=  val;
					continue;
				case 'E':
                    actPos.z =  val;
					continue;
				};
			}
			break;

		case 92:{
			actPos.z = 0.f;
			break;
		}
		case 0:
		case 1:
			for (int i = 1; i < (int)b.size(); ++i)
			{
				if (b.get_chunk(i).tp() != gpr::CHUNK_TYPE_WORD_ADDRESS)
					continue;

				char letter = b.get_chunk(i).get_word();
				double val = b.get_chunk(i).get_address().float_value();
				switch (letter)
				{
				case 'X':
                    actPos.x =  val;
					continue;
				case 'Y':
                    actPos.y =  val;
					continue;
				case 'E':
                    actPos.z =  val;
					continue;
				};
			}
			break;
		}
	}

	if (tp != 2 && cmdLetter == 'T')
	{
		int cmdVal = b.get_chunk(0).get_address().int_value();
		extruderId = cmdVal;
	}
}

void parseComment(std::string line, std::vector<std::string> &elems)
{
	elems.clear();

	std::istringstream iss(line);
	std::string item;
    while (std::getline(iss, item, ' '))
		elems.push_back(item);

	std::vector<std::string> finalelems;
	for (size_t i = 0; i < elems.size(); ++i)
	{
        if(elems[i].empty())
            elems.erase(elems.begin() + i);
      //  else
      //      std::replace(elems[i].begin(), elems[i].end(), ',', ' ');
    /*	std::istringstream iss(elems[i]);
		while (std::getline(iss, item, ','))
            finalelems.push_back(item);*/
	}

    //elems.clear();
    //elems = finalelems;
}

GcodeFileLoader::GcodeFileLoader()
{
}

GcodeFileLoader::~GcodeFileLoader()
{
}

std::vector<GcodeLayer>	 GcodeFileLoader::load(std::string filePath)
{
	_gcode = gpr::parse_gcode_file(filePath);

	_layers.clear();

	std::string lineType;
	bool calibration = false;
	bool init = false;
	unsigned int layerCount = 0;
	unsigned int commandCount = 0;
	unsigned int extruderId = 0;

	cv::Point3d prevPos, actPos, offset0, offset1;
	prevPos = cv::Point3d(0.f, 0.f, 0.f);
	actPos = cv::Point3d(0.f, 0.f, 0.f);

	for (int i = 0; i < _gcode.num_blocks(); i++)
	{		
		gpr::block block = _gcode.get_block(i);
		unsigned int j = 0;
		if(block.size())
		{
			if (block.get_chunk(j).tp() == gpr::CHUNK_TYPE_COMMENT)
			{
				std::string comment = block.get_chunk(j).get_comment_text()/*.getComment()*/;

				if (comment.size() != 0)
				{
					if (comment.find("layer") != -1 && comment.find("Z=") != -1&& comment.find("end") == -1 )
					{
						init = true;
						int layerNum = std::stoi(comment.substr(comment.find("layer")+5, comment.find(",")));

						GcodeLayer layer(layerNum, std::stof(comment.substr(comment.find("Z=")+2)));
						layer.setExtruder(extruderId);
						_layers.push_back(layer);
					
						layerCount++;
					}
					else if (comment.find("tool")!=-1)
					{
					}
					else
						lineType = comment;
				}
			}
			else if (block.get_chunk(j).tp() == gpr::CHUNK_TYPE_WORD_ADDRESS)
			{

				parseGLine(block, actPos, extruderId);

				if (init && actPos.z > prevPos.z && actPos.z > 0)
				{
					_layers[_layers.size() - 1].addLine(cv::Point2f(prevPos.x, prevPos.y), 
						cv::Point2f(actPos.x, actPos.y), lineType, extruderId);
				}
				prevPos = actPos;				
				
				commandCount++;
			}
		}
	}

	return _layers;
}

std::vector<GcodeLayer> GcodeFileLoader::getLayers()
{
	return _layers;
}
