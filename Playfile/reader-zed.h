/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-zed.h
 Purpose     :  ZED Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky, (c) Quickest-Owl Ltd
\**********************************************************************/

#ifndef _PLAYFILE_ZED_H
#define _PLAYFILE_ZED_H

#include <opencv2/opencv.hpp>			// OpenCV API
#include <sl_zed/Camera.hpp>			// ZED API
#include "playfile.h"


class PlayerZed : public PlayerB
{
  public:
	PlayerZed ()
	{}

	virtual ~PlayerZed ()
	{}

  private:
	virtual void		Construct (int64 jump = -1, cchar* file = nullptr);
	virtual int64		GetNextFrame ();
	virtual unsigned	GetDepthCoordinate (int x, int y);

  private:
	sl::Camera		Zed;
	sl::Mat			SvoImage;
	sl::Mat			SvoDepth;
	bool			FromFile;
};


#endif // _PLAYFILE_ZED_H
