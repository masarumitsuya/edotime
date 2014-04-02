/*
  edotime.c
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include <stdio.h>
#include <stdlib.h>
#include "edotime.h"
#include "suntime.h"

#define __EDOTIME_DEBUG_LOG__    0

#if (__EDOTIME_DEBUG_LOG__)
#define _edotime_log(s,...)    printf(s, __VA_ARGS__)
#else
#define _edotime_log(s,...)
#endif     // __EDOTIME_DEBUG_LOG__

typedef struct _edotime_t 
{
  double latitude;
  double longitude;
  double offset_utc;
  double sunrise;
  double sunset;
  double segtime_day;
  double segtime_night;
  int year;
  int month;
  int day;
} _edotime_t;

static inline
double _edotime_normalize_time( double t ) {
  if (t < 0)       t += 24;
  else if (24 < t) t -= 24;
  return t;
}

static inline
double _edotime_offset_time(    double t,
                                double offset,
                                int subtract) {
  double result;

  if (subtract)
	result = t - offset;
  else
	result = t + offset;

  result = _edotime_normalize_time(result);

  return result;
}

edotime_t
edotime_create(    int year,
                   int month,
                   int day, 
                   double offset_utc,
                   double latitude,
                   double longitude )
{
  edotime_t edotime;

  edotime = malloc(sizeof(_edotime_t));
  if (edotime == NULL)
	return NULL;

  edotime->year = year;
  edotime->month = month;
  edotime->day = day;
  edotime->offset_utc = offset_utc;
  edotime->latitude = latitude;
  edotime->longitude = longitude;

  double sunrise, sunset;
  // kure/ake is offset +-36min from sunrise/set.
  const double adjuster = 0.6;

  sunrise = suntime_get_sunrise_utc( latitude,
                                     longitude,
                                     SUNTIME_ZENITH,
                                     year,
                                     month,
                                     day );

  sunset = suntime_get_sunset_utc( latitude,
                                   longitude,
                                   SUNTIME_ZENITH,
                                   year,
                                   month,
                                   day );

  sunrise -= adjuster;
  sunset += adjuster;

  edotime->sunrise = sunrise;
  edotime->sunset = sunset;

  double daytime, nighttime;

  daytime = suntime_get_daytime( latitude,
                                 longitude,
                                 SUNTIME_ZENITH,
                                 year,
                                 month,
                                 day );

  nighttime = suntime_get_nighttime( latitude,
                                     longitude,
                                     SUNTIME_ZENITH,
                                     year,
                                     month,
                                     day );

  daytime += (adjuster * 2);
  nighttime -= (adjuster * 2);

  double segtime_day, segtime_night;

  segtime_day = daytime / 6;
  segtime_night = nighttime / 6;

  edotime->segtime_day = segtime_day;
  edotime->segtime_night = segtime_night;

  return edotime;
}

unsigned char
edotime_get_time( edotime_t t,
                  double time )
{
  if (!t) return 0;

  unsigned char rtime = ET_SIX;
  double sunset_local, sunrise_local;
  int is_day = 0;

  sunrise_local = _edotime_offset_time( t->sunrise, t->offset_utc, 0 );
  sunset_local  = _edotime_offset_time( t->sunset, t->offset_utc, 0 );

  if (sunrise_local < time && time < sunset_local)
	is_day = 1;

  _edotime_log("sunrise_local:%f\n", sunrise_local);
  _edotime_log("sunset_local:%f\n", sunset_local);
  _edotime_log("is_day:%d\n", is_day);

  double base_time;
  double interval;

  if (is_day) {
	base_time = sunrise_local;
	interval = t->segtime_day;
  } else {
	rtime += ET_NIGHTMASK;
	base_time = sunset_local;
	interval = t->segtime_night;
    // 0:00 ~ sunrise time calculates as 24:00 ~
	if (time < sunrise_local)
	  time += 24;
  }

  _edotime_log("interval:%f\n", interval);

  double interval_time;

  for (int i = 0; i < 6; i ++) {
    interval_time = base_time + interval;

    if (base_time <= time && time <= interval_time) {
      double interval_quarter;
	  unsigned char quarter;

	  interval_quarter = interval / 4.0;
	  quarter = ET_QUARTER_FIRST;

	  for (int ii = 0; ii < 4; ii ++) {
		//rtime |= quarter;
		interval_time = base_time + interval_quarter;
		if (base_time <= time && time <= interval_time) {
          rtime |= quarter;
		  if (quarter == ET_QUARTER_THIRD &&
			  (time < (base_time + 0.0167))) {
			/* just half time (before 1 minute after) -- add mask also ET_QUARTER_SECOND */
			rtime |= ET_QUARTER_SECOND;
		  }
		  break;
		}

		base_time = interval_time;
		quarter <<= 1u;
	  }
	  break;
	}

	rtime ++;
    base_time = interval_time;
  }

  return rtime;
}

double
edotime_get_hour_day( edotime_t t )
{
  if (t == NULL) return 0;

  return t->segtime_day;
}

double
edotime_get_hour_night( edotime_t t )
{
  if (t == NULL) return 0;

  return t->segtime_night;
}


double
edotime_get_contemporarytime( edotime_t t,
                              unsigned char time )
{
  double rtime = 0;
  unsigned char major_time;
  unsigned char quarter_time;
  unsigned int is_night;
  double segtime;

  major_time = time & ET_TIMEMASK;
  quarter_time = time & ET_QUARTERMASK;
  is_night = (time & ET_NIGHTMASK) ? 1 : 0;

  segtime = is_night ? t->segtime_night : t->segtime_day;

  rtime = is_night ? t->sunset : t->sunrise;
  rtime = _edotime_offset_time(rtime, t->offset_utc, 0);

  rtime += segtime * major_time;
  if (quarter_time) {
	double seg_quarter;

	seg_quarter = segtime / 4;

    if (quarter_time & ET_QUARTER_SECOND)
	  rtime += seg_quarter;
	else if (quarter_time & ET_QUARTER_THIRD)
	  rtime += seg_quarter * 2;
	else if (quarter_time & ET_QUARTER_FOURTH)
	  rtime += seg_quarter * 3;
  }

  rtime = _edotime_normalize_time(rtime);

  return rtime;
}

void
edotime_release( edotime_t t )
{
  if (!t) return;
  free(t);
}

