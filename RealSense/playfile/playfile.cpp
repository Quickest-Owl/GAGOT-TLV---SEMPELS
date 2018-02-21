// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <librealsense2/hpp/rs_context.hpp>
#include "example.hpp"          // Include short list of convenience functions for rendering


const auto		MainWindowName = "Quickest Owl Playfile";
cv::Mat			MainImg;
rs2::frame		MainDepth;
cv::Size		MainFrameSize;
char			MainOverlapText[100];
int				MainLastX, MainLastY;


void onMouse (int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		MainLastX = x;
		MainLastY = y;

		//UINT16 d = MainDepth.at<UINT16>(y,x);
		UINT16* pd = (UINT16*)MainDepth.get_data();

		sprintf (MainOverlapText, "[%4d:%4d] D = %5d", x, y, pd[MainFrameSize.width * y + x]);
		puts (MainOverlapText);
	}
}



int main(int argc, char * argv[]) try
{
	rs2::config cfg;
	cfg.enable_device_from_file (argv[1]);
	rs2::pipeline pipe;					// Declare RealSense pipeline, encapsulating the actual device and sensors
	pipe.start(cfg);					// Start streaming with default recommended configuration

	cv::namedWindow (MainWindowName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback (MainWindowName, onMouse, 0);

	int				iframe = 0;
	cv::Mat*		image = nullptr;
	cv::Vec3b*		previmage = nullptr;
	rs2::frameset	data;
	bool			newdata;

	while (cv::waitKey(1) < 0 && cvGetWindowHandle(MainWindowName))
    {
		//rs2::frameset data = pipe.wait_for_frames();
		if (newdata = pipe.poll_for_frames(&data)) {
			++iframe;
			printf ("Frame #%d\n", iframe);
		}
		
		if (!iframe)
			continue;	// still no data

		//rs2::frame		depth = color_map (data.get_depth_frame ());	// Find and colorize the depth data
		MainDepth		  = data.get_depth_frame();
        rs2::frame	color = data.get_color_frame();				// Find the color data

		rs2::frame&     fr = color;

		//int x = MainDepth.as<rs2::video_frame>().get_bits_per_pixel();

		MainFrameSize = cv::Size(fr.as<rs2::video_frame>().get_width(), fr.as<rs2::video_frame>().get_height());
		int size_bytes = MainFrameSize.width * MainFrameSize.height * sizeof(cv::Vec3b);

		if (newdata) {
			image = new(&MainImg) cv::Mat (MainFrameSize, CV_8UC3, (void*)fr.get_data());	// create OpenCV matrix of w*h size from the RGB data
			if (!previmage) 
				previmage = new cv::Vec3b[MainFrameSize.width * MainFrameSize.height];
			memcpy (previmage, fr.get_data(), size_bytes);
		}
		else {
			assert (previmage);
			image = new(&MainImg) cv::Mat (MainFrameSize, CV_8UC3, (void*)previmage);
		}

		cv::Mat img2 = image->clone ();
		cv::putText (img2, MainOverlapText, cv::Point(5,25), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0,255,0), 2);
		cv::circle  (img2, cv::Point(MainLastX,MainLastY), 5, CV_RGB(0,255,0), -1, 8);
		cv::imshow (MainWindowName, img2);
    }

	if (previmage)
		delete previmage;

	return EXIT_SUCCESS;
}


catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}

catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
