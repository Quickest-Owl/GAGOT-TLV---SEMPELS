/**********************************************************************\
 Project     :  TA Roofs / player app
 Filename    :  reader-zed.h
 Purpose     :  ZED Player class
 Created	 :  20.2.2018
 Author      :  Sergey Krasnitsky
\**********************************************************************/

#ifndef _PLAYFILE_ZED_H
#define _PLAYFILE_ZED_H

#include <opencv2/opencv.hpp>			// OpenCV API
#include <sl_zed/Camera.hpp>			// ZED API
#include "playfile.h"


class PlayerZed : public PlayerB
{
  public:
	PlayerZed () : PrevImage (nullptr)
	{
	}

	virtual ~PlayerZed ()
	{
		if (PrevImage)
			delete PrevImage;
	}

  private:
	virtual void	Construct (const char* file = nullptr);
	virtual int		GetNextFrame ();
	virtual int		GetDepthCoordinate (int x, int y);

  private:
	sl::Camera		Zed;
	sl::Mat			SvoImage;
	sl::Mat			SvoDepth;
	cv::Mat			Depth;
	bool			FromFile;
	char*			PrevImage;
};


#endif // _PLAYFILE_ZED_H
