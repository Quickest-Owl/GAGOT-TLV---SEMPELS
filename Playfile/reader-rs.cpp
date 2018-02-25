/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-rs.cpp
 Purpose     :  Realsense Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky, (c) Quickest-Owl Ltd
\**********************************************************************/

#include "def.h"
#include "reader-rs.h"

using namespace rs2;

//static
cchar* PlayerRealsense::FormatNames [RS2_FORMAT_COUNT] = 
{
    "ANY"           ,
    "Z16"           ,
    "DISPARITY16"   ,
    "XYZ32F"        ,
    "YUYV"          ,
    "RGB8"          ,
    "BGR8"          ,
    "RGBA8"         ,
    "BGRA8"         ,
    "Y8"            ,
    "Y16"           ,
    "RAW10"         ,
    "RAW16"         ,
    "RAW8"          ,
    "UYVY"          ,
    "MOTION_RAW"    ,
    "MOTION_XYZ32F" ,
    "GPIO_RAW"      ,
    "6DOF"          ,
    "DISPARITY32"   ,
};


void PlayerRealsense::Construct (int64 jump, cchar* file)
{
	PlayerName = "Quickest Owl : REALSENSE : ";
	if (file)
		PlayerName += file;

	pipeline_profile profile;

	if (file) {
		FromFile = true;
		rs2::config	 cfg;
		cfg.enable_device_from_file (file);
		//cfg.enable_stream(RS2_STREAM_ANY, RS2_FORMAT_BGR8);
		profile = Pipe.start (cfg);
	}
	else {
		FromFile = false;
		profile = Pipe.start ();
	}

	printf ("Device: %s\n\n"
	        "Sensors:\n", (cchar*)PlayerName);

	int i = 0;
	for (auto&& s : profile.get_device().query_sensors()) {
		cchar* sname;
		if (s.is<depth_sensor>())
			sname = "depth_sensor";
		else if (s.is<depth_stereo_sensor>())
			sname = "depth_stereo_sensor";
		else if (s.is<roi_sensor>())
			sname = "roi_sensor";
		else
			sname = "unknown";

		printf ("%4d. %s, FRAMES_QUEUE_SIZE = %d\n", ++i, sname, int(s.get_option(RS2_OPTION_FRAMES_QUEUE_SIZE)));
		//printf ("%4d. %s, FRAMES_QUEUE_SIZE = %d, MIN_DISTANCE = %d, MAX_DISTANCE = %d\n", ++i, s.get_info(RS2_CAMERA_INFO_FIRMWARE_VERSION), int(s.get_option(RS2_OPTION_FRAMES_QUEUE_SIZE)), int(s.get_option(RS2_OPTION_MIN_DISTANCE)), int(s.get_option(RS2_OPTION_MAX_DISTANCE)));
	}

	printf ("\nContained streams:\n");
	i = 0;
	for (auto&& s : profile.get_streams()) {
		printf ("%4d. %s, %s, FPS = %d\n", ++i, s.stream_name().c_str(), FormatNames[s.format()], s.fps());
	}
	
	video_stream_profile&&  colorstream = profile.get_stream(RS2_STREAM_COLOR).as<video_stream_profile>();
	assert (colorstream.format() == RS2_FORMAT_RGB8);

	video_stream_profile&&  depthstream = profile.get_stream(RS2_STREAM_DEPTH).as<video_stream_profile>();
	assert (depthstream.format() == RS2_FORMAT_Z16);

	Nframes = -1;  // didn't find how to get RS BAG file amount of frames
	Iframe0 = 0;

	FrameSize = { colorstream.width(), colorstream.height() };

	ImageDataBuf = new char[FrameSize.width * FrameSize.height * 3];
	assert (ImageDataBuf);

	DepthDataBuf = new uint16[FrameSize.width * FrameSize.height];
	assert (DepthDataBuf);

	Frame = cv::Mat (FrameSize.height, FrameSize.width, CV_8UC4, ImageDataBuf);
    
	printf ("\nCommon parameters:\n"
	        "  Video size: %d * %d\n"
			"  Depth scale: %.6f\n\n", FrameSize.width, FrameSize.height, profile.get_device().first<depth_sensor>().get_depth_scale());

	PlayerB::Construct (jump, file);
}


int64 PlayerRealsense::GetNextFrame ()
{
	bool		newdata;
	frameset	curset;

	if (FromFile) {
		newdata = Pipe.poll_for_frames (&curset);
		if (Iframe < 0  &&  !newdata)
			return Iframe;	// 1st frame still not appeared
	}
	else {
		newdata = true;
		curset = Pipe.wait_for_frames();
	}

	rs2::frame& imgframe = curset.get_color_frame();

	if (Iframe < 0)
		Iframe0 = imgframe.get_frame_number();

	if (newdata) {
		Iframe = imgframe.get_frame_number() - Iframe0;
		cv::Mat mat(FrameSize, CV_8UC3, (void*)imgframe.get_data());	// create OpenCV matrix of w*h size from the RGB data
		cv::cvtColor(mat, Frame, CV_RGB2BGR);							// convert to OpenCV's BGR

		rs2::frame& dthframe = curset.get_depth_frame();
		//assert (dthframe.as<video_frame>().get_bits_per_pixel() == sizeof(uint16)*8);
		//it's already checcked by format == RS2_FORMAT_Z16
		memcpy (DepthDataBuf, dthframe.get_data(), FrameSize.width * FrameSize.height * sizeof(uint16));
	}

	return Iframe;
}

