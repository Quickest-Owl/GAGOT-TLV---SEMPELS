/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-rs.cpp
 Purpose     :  Realsense Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky
\**********************************************************************/

#include "def.h"
#include "reader-rs.h"

using namespace rs2;

void PlayerRealsense::Construct (const char* file)
{
	PrevImage = nullptr;
	PlayerName = "Quickest Owl : REALSENSE : ";
	if (file)
		PlayerName += file;

	if (file) {
		FromFile = true;
		rs2::config	 cfg;
		cfg.enable_device_from_file (file);
		Pipe.start (cfg);
	}
	else {
		FromFile = false;
		Pipe.start ();
	}

	PlayerB::Construct (file);
}


int PlayerRealsense::GetNextFrame ()
{
	bool newdata;

	if (FromFile) {
		if (newdata = Pipe.poll_for_frames (&Cur))
			++Iframe;
		else if (!Iframe)
			return 0;
	}
	else {
		newdata = true;
		Cur = Pipe.wait_for_frames();
	}

	CurDepth = Cur.get_depth_frame();
	CurImage = Cur.get_color_frame();

	//int x = MainDepth.as<video_frame>().get_bits_per_pixel();

	FrameSize = cv::Size (CurImage.as<video_frame>().get_width(),
						  CurImage.as<video_frame>().get_height());

	int size_bytes = FrameSize.width * FrameSize.height * 3; // We are RGB == CV_8UC3

	if (newdata) {
		new(&Frame) cv::Mat (FrameSize, CV_8UC3, (void*)CurImage.get_data());	// create OpenCV matrix of w*h size from the RGB data
		if (!PrevImage)
			PrevImage = new char[size_bytes];
		memcpy (PrevImage, CurImage.get_data(), size_bytes);
	}
	else {
		assert (PrevImage);
		new(&Frame) cv::Mat (FrameSize, CV_8UC3, (void*)PrevImage);
	}

	return Iframe;
}


int PlayerRealsense::GetDepthCoordinate (int x, int y)
{
	uint16* pd = (uint16*)CurDepth.get_data();
	return pd[FrameSize.width * y + x];
}
