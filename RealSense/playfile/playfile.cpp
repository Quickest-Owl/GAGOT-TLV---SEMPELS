// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <opencv2/opencv.hpp>   // Include OpenCV API
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <librealsense2/hpp/rs_context.hpp>
#include "example.hpp"          // Include short list of convenience functions for rendering


int main(int argc, char * argv[]) try
{
	rs2::context	ctx;
	rs2::playback	playback = ctx.load_device (argv[1]);

	texture			depth_image, color_image;
    rs2::colorizer	color_map;

    rs2::pipeline pipe(ctx);		// Declare RealSense pipeline, encapsulating the actual device and sensors
    pipe.start();					// Start streaming with default recommended configuration

	const auto window_name = "Quickest Owl Playfile";
	cv::namedWindow (window_name, cv::WINDOW_AUTOSIZE);

	while(true)
    {
        rs2::frameset	data = pipe.wait_for_frames();				// Wait for next set of frames from the camera
        rs2::frame		depth = color_map(data.get_depth_frame());	// Find and colorize the depth data
        rs2::frame		color = data.get_color_frame();				// Find the color data

        // Render depth on to the first half of the screen and color on to the second
        depth_image.render(depth, { 0,        0, 1280 / 2, 720 });
        color_image.render(color, { 1280 / 2, 0, 1280 / 2, 720 });

		// Query frame size (width and height)
		const int w = depth.as<rs2::video_frame> ().get_width ();
		const int h = depth.as<rs2::video_frame> ().get_height ();

		// Create OpenCV matrix of size (w,h) from the colorized depth data
		cv::Mat image (cv::Size (w, h), CV_8UC3, (void*)depth.get_data(), cv::Mat::AUTO_STEP);

		// Update the window with new data
		cv::imshow (window_name, image);
    }

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
