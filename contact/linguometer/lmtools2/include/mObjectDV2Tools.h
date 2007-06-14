/*
 *  Copyright (C) 2007 Michele Tavella <michele@liralab.it>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef MOBJECTDV2TOOLS_H
#define MOBJECTDV2TOOLS_H

#ifdef FFMPEG_DISTRO
#	include <ffmpeg/avcodec.h>
#	include <ffmpeg/avformat.h>
#else
#	include "avcodec.h"
#	include "avformat.h"
#endif

namespace mObjectDV2Tools {
	/*
	bool pts2time (int64_t pts) {
		if (pts < 0)
			return false;
		int hours = 0, mins = 0, secs = 0;

		secs = pts / AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		printf ("%.2d:%.2d:%.2d\n", hours, mins, secs);
		return true;
	}
	*/

	bool pts2time (int64_t pts, int &hours, int &mins, int &secs) {
		if (pts < 0)
			return false;

		secs = pts / AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		return true;
	}
	

	bool pts2time (int64_t pts, int &hours, int &mins, int &secs, int &us) {
		if (pts < 0)
			return false;

		secs = pts / AV_TIME_BASE;
		us = pts % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		return true;
	}

	
	bool time2pts (int64_t &pts, int hours, int mins, int secs) {
		if (hours < 0 || mins < 0 || secs < 0)
			return false;

		pts = (60 * (60 * hours + mins) + secs) * AV_TIME_BASE;
		return true;
	}
	
	bool time2pts (int64_t &pts, int hours, int mins, int secs, int us) {
		if (hours < 0 || mins < 0 || secs < 0 || us < 0)
			return false;

		pts = (60 * (60 * hours + mins) + secs) * AV_TIME_BASE + us;
		return true;
	}
	
	bool secs2pts (int64_t &pts, int secs) {
		if (secs < 0)
			return false;

		pts = secs * AV_TIME_BASE;
		return true;
	}
	
	 int64_t secs2pts (int secs) {
		if (secs < 0)
			return -1;

		/* Nasty bug that made me choke.
		 * Bug: return  secs * AV_TIME_BASE;
		 * If secs >= 2400, returns a negative value.
		 */
		return  (int64_t)(secs) * AV_TIME_BASE;
	}
	 
	 int64_t samples2pts (int samples, int sampleRate) {
		if (samples < 0 || sampleRate < 0)
			return -1;
		
		return (int64_t)((double)samples / (double)sampleRate
				* AV_TIME_BASE);
	}
	 
	 int pts2samples (int64_t pts, int sampleRate) {
		if (pts < 0 || sampleRate < 0)
			return -1;
		
		return (pts*sampleRate)/AV_TIME_BASE;
	}

	bool pts2time (int64_t pts) {
		int hours = 0, mins = 0, 
			secs = 0, us = 0;
		if (!pts2time(pts, hours, mins, secs, us))
			return false;
		
		printf("%02d:%02d:%02d.%01d",
				hours, mins, secs, (10 * us) / AV_TIME_BASE);
		return true;
	}
};
#endif
