#include <iostream>

#include <librealsense2/rs.hpp>

using namespace rs2;

void main()
{
    // Create a Pipeline, which serves as a top-level API for streaming and processing frames
    pipeline p;

    // Configure and start the pipeline
    p.start();

    while (true)
    {
        // Block program until frames arrive
        frameset frames = p.wait_for_frames();

        // Try to get a frame of a depth image
        depth_frame depth = frames.get_depth_frame();
        // The frameset might not contain a depth frame, if so continue until it does
        if (!depth) continue;

        // Get the depth frame's dimensions
        int width  = depth.get_width();
        int height = depth.get_height();

        // Query the distance from the camera to the object in the center of the image
        float dist_to_center = depth.get_distance(width / 2, height / 2);

        // Print the distance 
        std::cout << "The camera is facing an object " << dist_to_center << " meters away \r";
    }
}