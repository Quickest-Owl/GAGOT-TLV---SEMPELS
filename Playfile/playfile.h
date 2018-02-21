/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  playfile.h
 Purpose     :  Main file for ZED/Realsense CMD player 
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky
\**********************************************************************/

#ifndef _PLAYFILE_H
#define _PLAYFILE_H

#include <opencv2/opencv.hpp>   // include OpenCV API
#include "str.h"


class PlayerB
{
  public:
	virtual void	Construct (const char* file);
	virtual int		GetNextFrame () = 0;
	virtual int		GetDepthCoordinate (int x, int y) = 0;

	void  onMouse			(int event, int x, int y, int flags);
	void* GetWindowHandle() { return cvGetWindowHandle(PlayerName); }
	void  Loop();

  protected:
	STRING			PlayerName;
	STRING			OverlapText;
	cv::Mat			Frame;
	cv::Size		FrameSize;
	int				LastX, LastY;
	int				Iframe;
};


#endif // _PLAYFILE_H
