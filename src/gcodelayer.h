/**
* @file gcodelayer.h
* @author aardanza
* @brief File containing Gcode Layer class reference.
*
*/

#ifndef GCODELAYER_H
#define GCODELAYER_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <opencv2/opencv.hpp>


//! Image layer
struct GcodeLayerImage{
	cv::Mat image;	///< layer image
	cv::Mat mask;	///< layer mask
};


/*! Layer */
/*!
	Class that implement basic opencv video source implementation
*/

class GcodeLayer {

	//! Line type
	enum LineType{
		SKIRT = 0,
		INNER,
		OUTER,
		INFILL,
		SOLID,
		OOZE,
		SUPPORT,
		GAP
	};

	//! Layer line
	struct LayerLine{
		cv::Point2f begin;		///< line begin point
		cv::Point2f end;		///< line end point
		LineType	lineType;	///< line type
	};

public:
	GcodeLayer();
    GcodeLayer(const int id, const float z);
	~GcodeLayer();

	//! Set layer id
	/*!
		\param id Layer ide
	*/
	void setId(int id) { _id = id; };

	//! Set layer Z value
	/*!
		\param z Layer Z value
	*/
	void setZ(float z) { _z = z; };


	//! Set layer extruder
	/*!
		\param extruderId Layer extruder id
	*/
	void setExtruder(float extruderId) { _extruderId = extruderId; };

	//! Get layer id
	/*!
		\return Layer id
	*/
	int getId() { return _id; };

	//! Get layer Z value
	/*!
		\return Layer Z value
	*/
	float getZ() { return _z; };

	//! Get layer extruder
	/*!
		\return Layer extruder
	*/
	int getExtruder() { return _extruderId; };

	//! Paint layer image
	/*!
		\param extruder0Color Extruder 0 color
		\param extruder1Color Extruder 1 color
		\param skirt Skirt
		\param support Support
		\param ooze Ooze
		\return Layer image
	*/
    GcodeLayerImage paint(cv::Size layerSize, cv::Scalar extruder0Color = cv::Scalar(255, 255, 255),
                          cv::Scalar extruder1Color = cv::Scalar(0, 0, 0),
                          bool skirt = false, bool support = false, bool ooze = false);

	//! Add line to image
	/*!
		\param begin Line begin point
		\param end Line end point
		\param lineType Line type
		\param extruderId Line extruder
	*/
	void addLine(cv::Point2f begin, cv::Point2f end, std::string lineType, int extruderId);

    void scaleLines(float scale_x, float scale_y);

private:
	int			_id;			///< layer id
	float		_z;				///< layer z value
	int			_extruderId;	///< layer extruder id
	int			_scale;			///< layer scale value

	cv::Scalar _extruder0Color;	///< extruder 0 color
	cv::Scalar _extruder1Color;	///< extruder 1 color

	typedef std::vector<LayerLine> LayerLines;
	LayerLines _layerLines0;	///< layer lines in extruder 0
	LayerLines _layerLines1;	///< layer lines in extruder 1
};
#endif //GCODELAYER_H
