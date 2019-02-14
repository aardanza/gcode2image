#include "gcodelayer.h"

#define LINE_THICKNESS 4

GcodeLayer::GcodeLayer()
{
	_scale = 20;
}

GcodeLayer::GcodeLayer(const int id, const float z)
{
	_id = id;
	_z = z;
	_scale = 20;

    _extruderId = -1;
}

GcodeLayer::~GcodeLayer() {}

GcodeLayerImage GcodeLayer::paint(cv::Size layerSize, cv::Scalar extruder0Color, cv::Scalar extruder1Color, bool skirt, bool support, bool ooze)
{
	GcodeLayerImage draw;

	//draw.image = cv::Mat(layerSize.height, layerSize.width, CV_8U);
    draw.image = cv::Mat::zeros(layerSize.height, layerSize.width, CV_8UC3);
   // draw.image = cv::Scalar(180, 180, 180);

    //check black color
    if(extruder0Color == cv::Scalar(0,0,0))
        extruder0Color = cv::Scalar(10,10,10);
    if(extruder1Color == cv::Scalar(0,0,0))
        extruder1Color = cv::Scalar(10,10,10);

	for (size_t i = 0; i < _layerLines0.size(); i++)
	{
		switch (_layerLines0[i].lineType){
		case SKIRT:
		case INNER:
		case OUTER:
		case INFILL:
		case SOLID:
		case OOZE:
		case SUPPORT:
		case GAP:
			cv::line(draw.image, _layerLines0[i].begin, _layerLines0[i].end, extruder0Color, LINE_THICKNESS);
			break;
		}	
	}

	for (size_t i = 0; i < _layerLines1.size(); i++)
	{
		switch (_layerLines1[i].lineType){
		case SKIRT:
		case INNER:
		case OUTER:
		case INFILL:
		case SOLID:
		case OOZE:
		case SUPPORT:
		case GAP:
			cv::line(draw.image, _layerLines1[i].begin, _layerLines1[i].end, extruder1Color, LINE_THICKNESS);
			break;
		}
	}

	cv::Mat se = cv::getStructuringElement(1, cv::Size(2, 2), cv::Point(1, 1));

	cv::dilate(draw.image, draw.mask, se);

	if (draw.mask.type() != CV_8U){
		cv::cvtColor(draw.mask, draw.mask, cv::COLOR_BGR2GRAY/*CV_BGR2GRAY*/);
	}

	return draw;
}

void GcodeLayer::addLine(cv::Point2f begin, cv::Point2f end, std::string lineType, int extruderId)
{
	_extruderId = extruderId;

	LayerLine line = { begin, end, SKIRT };
	if (lineType == "skirt")
		line.lineType = SKIRT;
	else if (lineType == "inner")
		line.lineType = INNER;
	else if (lineType == "outer")
		line.lineType = OUTER;
	else if (lineType == "infill")
		line.lineType = INFILL;
	else if (lineType == "solid")
		line.lineType = SOLID;
	else if (lineType == "gap")
		line.lineType = GAP;

	if (extruderId==0)
		_layerLines0.push_back(line);
	else if (extruderId==1)
		_layerLines1.push_back(line);
}

void GcodeLayer::scaleLines(float scale_x, float scale_y)
{
    for(unsigned int i = 0; i<_layerLines0.size();i++)
    {
        _layerLines0[i].begin.x *= scale_x;
        _layerLines0[i].begin.y *= scale_y;

        _layerLines0[i].end.x *= scale_x;
        _layerLines0[i].end.y *= scale_y;
    }

    for(unsigned int i = 0; i<_layerLines1.size();i++)
    {
        _layerLines1[i].begin.x *= scale_x;
        _layerLines1[i].begin.y *= scale_y;

        _layerLines1[i].end.x *= scale_x;
        _layerLines1[i].end.y *= scale_y;
    }
}
