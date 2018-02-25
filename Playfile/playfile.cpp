/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  playfile.cpp
 Purpose     :  Main file for ZED/Realsense CMD player 
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky, (c) Quickest-Owl Ltd
\**********************************************************************/

#include <conio.h>

#include "def.h"
#include "str.h"
#include "playfile.h"
#include "reader-rs.h"
#include "reader-zed.h"

/*--------------------------------------------------------------------------------------*\
									Global data & funcs
\*--------------------------------------------------------------------------------------*/

PlayerB*  Player;

void onMouse (int event, int x, int y, int flags, void* param)
{
	((PlayerB*)param)->onMouse (event, x, y, flags);
}



/*--------------------------------------------------------------------------------------*\
									PlayerB class
\*--------------------------------------------------------------------------------------*/

void PlayerB::Construct (int64 jump, cchar* file)
{
	cv::namedWindow ((cchar*)PlayerName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback ((cchar*)PlayerName, ::onMouse, this);
	OverlapText.Clear();
	Iframe = -1;
	Ijump  = jump;
	Paused = false;
}


void PlayerB::onMouse (int event, int x, int y, int flags)
{
	if (Ijump >= 0 && Iframe < Ijump) {
		// we are looking forward to frame #Ijump, any user actions are not active
		return;
	}

	if (event == CV_EVENT_LBUTTONDOWN || event == CV_EVENT_RBUTTONDOWN)
	{
		LastX = x;
		LastY = y;
		unsigned d = GetDepthCoordinate (x, y);

		OverlapText.Print ("[%2d:%2d] D = %3d", x, y, d);
		puts ((cchar*)OverlapText);

		switch (event) {
			case CV_EVENT_LBUTTONDOWN:	Paused = false;  break;
			case CV_EVENT_RBUTTONDOWN:	Paused = true;	 break;
		}
	}
}


void PlayerB::Loop ()
{
	if (!Paused) {
		// Do not call GetNextFrame() in a pause
		if (GetNextFrame() < 0)
			return;	// still no data
	}

	cv::Mat  img = Frame.clone ();

	const cv::Scalar TEXTCOLOR = CV_RGB(0,255,0);
	STR<100> str;

	if (Ijump >= 0 && Iframe < Ijump) {
		str = "Looking forward to frame #";
		str += Ijump;
		cv::putText (img, (cchar*)str, cv::Point(3,25), cv::FONT_HERSHEY_DUPLEX, 1.0, TEXTCOLOR, 2);
	}
	else {
		if (Ijump >= 0) {
			// just switched from the condition above, pause the video
			Paused = true;
			Ijump = -1;
		}
		str = '#';
		str += Iframe;
		if (Nframes >= 0) {
			str += " / ";
			str += Nframes;
		}
		cv::putText (img, (cchar*)str,		   cv::Point(3,25), cv::FONT_HERSHEY_DUPLEX, 1.0, TEXTCOLOR, 2);
		cv::putText (img, (cchar*)OverlapText, cv::Point(3,65), cv::FONT_HERSHEY_DUPLEX, 1.0, TEXTCOLOR, 2);
		if (LastX || LastY)
			cv::circle (img, cv::Point (LastX, LastY), 5, TEXTCOLOR, -1, 8);
	}

	cv::imshow ((cchar*)PlayerName, img);
}


/*--------------------------------------------------------------------------------------*\
										 Main 
\*--------------------------------------------------------------------------------------*/

int main (int argc, char * argv[]) try
{
	// 1. Check arguments 
	if (argc != 2 && argc != 3 && argc != 4) {
		usage:
		puts ("\nUsage:\n  playfile -{zed|rs} [-j=<JumpToFrameNum>] [file-path]\n");
		goto end;
	}

	// 2. Parse command line args
	if (STRB::strequ(argv[1], "-zed")) {
		Player = new PlayerZed();
	}
	else if (STRB::strequ (argv[1], "-rs")) {
		Player = new PlayerRealsense();
	}
	else {
		goto usage;
	}

	cchar*  file = nullptr;
	int		jump = -1;

	if (argc > 2 && STRB::strequ(argv[2], "-j=", 3)) {
		jump = STRB::atoi32(argv[2] + 3);
		printf ("\nJumping to frame #%d...\n", jump);
		if (argc == 4)
			file = argv[3];
	}
	else {
		if (argc == 3)
			file = argv[2];
	}

	// 3. Construct the necessary Player object
	Player->Construct(jump, file);

	// 3. Endless loop calling PlayerB::Loop() func, will be ended after a user closes OpenCV Window
	while (cv::waitKey(1) < 0 && Player->GetWindowHandle())
    {
		Player->Loop();
    }

	end:
	// 4. Finalize
	delete Player;
	_getch ();
	return 0;
}


catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
	_getch();
    return 1;
}

catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
	_getch ();
    return 2;
}
