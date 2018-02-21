/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-rs.h
 Purpose     :  Realsense Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky
\**********************************************************************/

#ifndef _PLAYFILE_RS_H
#define _PLAYFILE_RS_H


#include <opencv2/opencv.hpp>					// OpenCV API
#include <librealsense2/rs.hpp>					// RealSense Cross Platform API
#include <librealsense2/hpp/rs_context.hpp>

#include "playfile.h"


class PlayerRealsense : public PlayerB
{
  public:
	PlayerRealsense() : PrevImage(nullptr)
	{
	}

	virtual ~PlayerRealsense ()
	{
		if (PrevImage)
			delete PrevImage;
	}

  private:
	virtual void	Construct (const char* file = nullptr);
	virtual int		GetNextFrame ();
	virtual int		GetDepthCoordinate (int x, int y);

  private:
	rs2::pipeline	Pipe;
	rs2::frameset	Cur;
	rs2::frame		CurDepth;
	rs2::frame		CurImage;
	bool			FromFile;
	char*			PrevImage;
};


#endif // _PLAYFILE_RS_H
