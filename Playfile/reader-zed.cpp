/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-zed.cpp
 Purpose     :  ZED Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky
\**********************************************************************/

#include "def.h"
#include "reader-zed.h"

using namespace sl;


void PlayerZed::Construct (const char* file)
{
	PrevImage = nullptr;
	PlayerName = "Quickest Owl : ZED : ";
	if (file)
		PlayerName += file;

	InitParameters  init_params;
	if (file) {
		FromFile = true;
		init_params.svo_input_filename.set (file);
	}
	else {
		FromFile = false;
		init_params.camera_resolution	= sl::RESOLUTION_HD720;
		init_params.depth_mode			= sl::DEPTH_MODE_MEDIUM;
		init_params.coordinate_units	= sl::UNIT_MILLIMETER;
	}

	ERROR_CODE err = Zed.open (init_params);
	if (err)
		throw std::exception(toString(err));

	FrameSize = cv::Size (int(Zed.getResolution().width), int(Zed.getResolution().height));

	new (&SvoImage) sl::Mat (FrameSize.width, FrameSize.height, MAT_TYPE_8U_C4, MEM_CPU);
	new (&SvoDepth) sl::Mat (FrameSize.width, FrameSize.height, MAT_TYPE_8U_C4, MEM_CPU);

	Frame = cv::Mat (int(SvoImage.getHeight()), int(SvoImage.getWidth()), CV_8UC4, SvoImage.getPtr<sl::uchar1>(MEM_CPU)); 
	Depth = cv::Mat (int(SvoImage.getHeight()), int(SvoImage.getWidth()), CV_8UC4, SvoDepth.getPtr<sl::uchar1>(MEM_CPU)); 

	PlayerB::Construct (file);
}


int PlayerZed::GetNextFrame ()
{
	bool newdata;

	if (newdata = (Zed.grab () == SUCCESS)) {
		Zed.retrieveImage (SvoImage, VIEW_LEFT,  MEM_CPU, FrameSize.width, FrameSize.height);
		Zed.retrieveImage (SvoDepth, VIEW_DEPTH, MEM_CPU, FrameSize.width, FrameSize.height);
		++Iframe;
	}
	else if (!Iframe)
		return 0;

	int size_bytes = FrameSize.width * FrameSize.height * 4; // We are RGBA == CV_8UC4

	if (newdata) {
		if (!PrevImage)
			PrevImage = new char[size_bytes];
		memcpy (PrevImage, SvoImage.getPtr<sl::uchar1>(MEM_CPU), size_bytes);
	}
	else {
		assert (PrevImage);
		new(&Frame) cv::Mat (FrameSize, CV_8UC3, (void*)PrevImage);
	}

	return Iframe;
}


int PlayerZed::GetDepthCoordinate (int x, int y)
{
	uint32* pd = (uint32*)Depth.data;
	return (uint8) pd[FrameSize.width * y + x];	// the ZED depth is 3 x same byte values + 0xFF
}
