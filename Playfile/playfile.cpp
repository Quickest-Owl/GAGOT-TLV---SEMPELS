/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  playfile.cpp
 Purpose     :  Main file for ZED/Realsense CMD player 
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky
\**********************************************************************/

#include <conio.h>

#include "def.h"
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

void PlayerB::Construct (const char* file)
{
	cv::namedWindow ((cchar*)PlayerName, cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback ((cchar*)PlayerName, ::onMouse, this);
	Iframe = 0;
}


void PlayerB::onMouse (int event, int x, int y, int flags)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		LastX = x;
		LastY = y;
		int d = GetDepthCoordinate (x, y);

		OverlapText.Print ("[%4d:%4d] D = %5d", x, y, d);
		puts ((cchar*)OverlapText);
	}
}


void PlayerB::Loop ()
{
	if (!GetNextFrame())
		return;	// still no data

	cv::Mat img = Frame.clone ();
	cv::putText (img, (cchar*)OverlapText, cv::Point (5,25), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB (0,255,0), 2);
	cv::circle (img, cv::Point (LastX,LastY), 5, CV_RGB (0,255,0), -1, 8);
	cv::imshow ((cchar*)PlayerName, img);

	printf ("Frame #%d\n", Iframe);
}


/*--------------------------------------------------------------------------------------*\
											Main 
\*--------------------------------------------------------------------------------------*/


int main(int argc, char * argv[]) try
{
	if (argc != 2 && argc != 3) {
		usage:
		puts ("\nUsage:\n  playfile -{zed|rs} [file-path]\n");
		goto end;
	}

	if (strcmp (argv[1], "-zed") == 0) {
		Player = new PlayerZed();
	}
	else if (strcmp (argv[1], "-rs") == 0) {
		Player = new PlayerRealsense ();
	}
	else {
		goto usage;
	}

	Player->Construct(argv[2]);

	while (cv::waitKey(1) < 0 && Player->GetWindowHandle())
    {
		Player->Loop();
    }

	end:
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
