/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-rs.h
 Purpose     :  Realsense Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky, (c) Quickest-Owl Ltd
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
	PlayerRealsense() : ImageDataBuf(nullptr), DepthDataBuf(nullptr)
	{}

	virtual ~PlayerRealsense ()
	{
		if (ImageDataBuf)
			delete ImageDataBuf;
		if (DepthDataBuf)
			delete DepthDataBuf;
	}

  private:
	virtual void		Construct (int64 jump = -1, cchar* file = nullptr);
	virtual int64		GetNextFrame ();
	virtual unsigned	GetDepthCoordinate (int x, int y);

  private:
	static cchar* FormatNames[RS2_FORMAT_COUNT];

  private:
	rs2::pipeline	Pipe;
	bool			FromFile;
	int64			Iframe0;
	char*			ImageDataBuf;
	uint16*			DepthDataBuf;
};


inline unsigned PlayerRealsense::GetDepthCoordinate (int x, int y)
{
	return DepthDataBuf [FrameSize.width * y + x];
}


#endif // _PLAYFILE_RS_H
