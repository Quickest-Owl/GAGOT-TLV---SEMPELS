/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-zed.cpp
 Purpose     :  ZED Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky, (c) Quickest-Owl Ltd
\**********************************************************************/

#include "def.h"
#include "reader-zed.h"

using namespace sl;


void PlayerZed::Construct (int64 jump, cchar* file)
{
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

	printf ("Device: %s\n\n"
	        "FW ver: 0x%X:\n"
			"SDK ver: %s\n"
			"Self Calibration State: %d\n\n",
			(cchar*)PlayerName, Zed.getCameraInformation().firmware_version, Zed.getSDKVersion().c_str(), Zed.getSelfCalibrationState());

	Nframes = Zed.getSVONumberOfFrames();
	assert (Nframes != 0); // can be -1 or >0
	if (Nframes > 0)
		Nframes--;  // we will start from 0, 

	FrameSize = { int(Zed.getResolution().width), int(Zed.getResolution().height) };

	new (&SvoImage) sl::Mat (FrameSize.width, FrameSize.height, MAT_TYPE_8U_C4, MEM_CPU);

	Frame = cv::Mat (FrameSize.height, FrameSize.width, CV_8UC4, SvoImage.getPtr<sl::uchar1>(MEM_CPU));

	printf ("\nCommon parameters:\n"
	        "  Video size: %d * %d\n"
			"  FPS: %d\n\n", FrameSize.width, FrameSize.height, int(Zed.getCameraFPS()));


	PlayerB::Construct (jump, file);
}


int64 PlayerZed::GetNextFrame ()
{
	if (Nframes >= 0 && Iframe == Nframes)
		return Iframe;	// for some strange reason Zed.grab() on some SVO files returns SUCCESS after the end, this mechanism guards it

	if (Zed.grab() == SUCCESS) {
		Zed.retrieveImage (SvoImage, VIEW_LEFT);
		//Zed.retrieveImage (SvoDepth, VIEW_DEPTH);
		Zed.retrieveMeasure(SvoDepth, MEASURE_DEPTH);
		Iframe++;
	}
	return Iframe;
}


unsigned PlayerZed::GetDepthCoordinate (int x, int y)
{
	float depth_value = 0;
	SvoDepth.getValue (x, y, &depth_value);
	return unsigned (depth_value);
}
