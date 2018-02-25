/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  playfile.h
 Purpose     :  Main file for ZED/Realsense CMD player 
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky, (c) Quickest-Owl Ltd
\**********************************************************************/

#ifndef _PLAYFILE_H
#define _PLAYFILE_H

#include <opencv2/opencv.hpp>   // OpenCV API
#include "str.h"


/*
 ******************************************************************************
  Basic interface class for all Cameras Players.
  All functions may throw exceptions.
 ******************************************************************************
*/
class PlayerB
{
  public:
	virtual void		Construct (int64 jump = -1, cchar* file = nullptr);	// Construct the object. If file param is supplied then use it as input stream. Otherwise to work with correspondent Camera HW
	virtual int64		GetNextFrame () = 0;								// Called from PlayerB::Loop(), returns frame number (IFrame), should fill PlayerB::Frame. Returning -1 means a 1st frame still was not appeared
	virtual unsigned	GetDepthCoordinate (int x, int y) = 0;				// Get Depth value for the X,Y pixel in Frame

	void  onMouse			(int event, int x, int y, int flags);			// OpenCV onMouse callback for a specific Player object
	void* GetWindowHandle() { return cvGetWindowHandle(PlayerName); }		// Gets current player OpenCV window handle
	void  Loop();															// Endless loop body function to show the video, called from main() while loop

  protected:
	STRING			PlayerName;												// Player OpenCV Window name
	STRING			OverlapText;											// Overlapped text formated by PlayerB::onMouse() func
	cv::Mat			Frame;													// Current frame RGB data filled by GetNextFrame()
	cv::Size		FrameSize;												// Frame size 
	int				LastX, LastY;											// X,Y coordinates in the Frame after last mouse press
	int64			Nframes;												// Keeps amount of frames or -1 when it's unknown. Actually it should contain last frame number.
	int64			Iframe;													// Frame sequence number, initialized in PlayerB::Construct as -1, in order to detect first interations with no data 
	int64			Ijump;													// When != 0, the playing starts from Iframe == Ijump
	bool			Paused;													// When true, the player should be paused on a current frame
};


#endif // _PLAYFILE_H
