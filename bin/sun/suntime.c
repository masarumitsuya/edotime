/*
  suntime.c
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include <stdio.h>
#include <math.h>
#include "suntime.h"

#define __SUNTIME_DEBUG_LOG__ 0

#if __SUNTIME_DEBUG_LOG__
#define _suntime_log(...) printf(__VA_ARGS__)
#else
#define _suntime_log(...) 
#endif    /* __SUNTIME_DEBUG_LOG__  */

static inline double dtor(double d);
static inline double rtod(double r);

static double
_suntime_get_time(    double latitude,
                      double longitude,
                      double zenith,
                      int year,
                      int month,
					  int day,
                      int sunrise );

double
suntime_get_sunrise_utc(    double latitude,
                            double longitude,
                            double zenith,
                            int year,
                            int month,
                            int day )
{
  return _suntime_get_time( latitude,
                            longitude,
                            zenith,
                            year,
                            month,
                            day,
                            1);
}

double
suntime_get_sunset_utc(    double latitude,
                           double longitude,
                           double zenith,
                           int year,
                           int month,
                           int day )
{
  return _suntime_get_time( latitude,
                            longitude,
                            zenith,
                            year,
                            month,
                            day,
                            0);
}

double
suntime_get_daytime(    double latitude,
                        double longitude,
                        double zenith,
                        int year,
                        int month,
                        int day )
{
  double sunrise, sunset;

  sunrise = suntime_get_sunrise_utc(    latitude,
                                        longitude,
                                        zenith,
                                        year,
                                        month,
                                        day );
  sunset = suntime_get_sunset_utc(    latitude,
                                      longitude,
                                      zenith,
                                      year,
                                      month,
                                      day );

  _suntime_log("sunrise= %f, sunset= %f\n", sunrise, sunset);

  if (sunrise > sunset)
	return (sunset + (24 - sunrise));
  return (sunset - sunrise);
}

double
suntime_get_nighttime(    double latitude,
                          double longitude,
                          double zenith,
                          int year,
                          int month,
                          int day )
{
  double sunrise, sunset;

  sunrise = suntime_get_sunrise_utc(    latitude,
                                        longitude,
                                        zenith,
                                        year,
                                        month,
                                        day );
  sunset = suntime_get_sunset_utc(    latitude,
                                      longitude,
                                      zenith,
                                      year,
                                      month,
                                      day );

  if (sunrise > sunset)
	return (sunrise - sunset);
  return (sunrise + (24 - sunset));
}

inline double dtor(double d) {
  return d * (M_PI / 180.0);
}

inline double rtod(double r) {
  return r * (180.0 / M_PI);
}

double
_suntime_get_time(    double latitude,
                      double longitude,
                      double zenith,
                      int year,
                      int month,
					  int day,
                      int sunrise )
{
  double n1, n2, n3, n;

  // 1. first calculate the day of the year
  n1 = floor(275 * month / 9);
  n2 = floor((month + 9) / 12);
  n3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
  n = n1 - (n2 * n3) + day - 30;
  _suntime_log("n: %.0f\n", n);

  // 2. convert the longitude to hour value an calculate an approximate time
  double longitude_h, t;

  longitude_h = longitude / 15;
  if (sunrise)
	t = n + ((6 - longitude_h) / 24);
  else
	t = n + ((18 - longitude_h) / 24);
  _suntime_log("t: %f\n", t);

  // 3. calculate the Sun's mean anomaly
  double m;

  m = (0.9856 * t) - 3.289;

  // 4. calculate the Sun's true longitude
  // NOTE: l potentially needs to be adjusted into the range [0,360) by adding/subtracting 360
  double l;

  l = m + (1.916 * sin(dtor(m))) + (0.020 * sin(dtor(2 * m))) + 282.634;
  if (l < 0)
	l += 360;
  else if (360 < l)
	l -= 360;
  _suntime_log("l: %f\n", l);

  // 5a. calculate the Sun's right ascension
  // NOTE: ra potentially needs to be adjusted into the range [0,360) by adding/subtracting 360
  double ra;

  ra = rtod(atan(0.91764 * tan(dtor(l))));
  if (ra < 0)
	ra += 360;
  else if (360 < ra)
	ra -= 360;

  // 5b. right ascension value needs to be in the same quadrant as l
  double l_quadrant, ra_quadrant;

  l_quadrant = (floor(l / 90)) * 90;
  ra_quadrant = (floor(ra / 90)) * 90;
  ra = ra + (l_quadrant - ra_quadrant);

  // 5c. right ascension value needs to be converted into hours
  ra = ra / 15;
  _suntime_log("ra: %f\n", ra);

  // 6. calculate the Sun's declination
  double sin_dec, cos_dec;

  sin_dec = 0.39782 * sin(dtor(l));
  cos_dec = cos(asin(sin_dec));

  // 7a. calculate the Sun's local hour angle
  double cos_hour;

  cos_hour = (cos(dtor(zenith)) - (sin_dec * sin(dtor(latitude)))) / (cos_dec * cos(dtor(latitude)));
  if (1 < cos_hour) {
	printf("the sun never rises on this location (on the specified date)\n");
    return -1;
  } else if (cos_hour < -1) {
	printf("the sun never sets on this location (on the specified date)\n");
	return -1;
  }

  // 7b. finish calculating h and convert into hours
  double h;

  if (sunrise)
	h = 360 - rtod(acos(cos_hour));
  else
	h = rtod(acos(cos_hour));
  h = h / 15;

  // 8. calculate local mean time of rising/setting
  double time;

  time = h + ra - (0.06571 * t) - 6.622;
  _suntime_log("time: %f\n", time);

  // 9. adjust back to UTC
  double utc_time;

  utc_time = time - longitude_h;
  if (utc_time < 0)
	utc_time += 24;
  else if (24 < utc_time)
	utc_time -= 24;
  _suntime_log("utc_time:%f\n", utc_time);

  return utc_time;
}
