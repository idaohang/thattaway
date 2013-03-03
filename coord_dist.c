/***
Copyright (C) 2012 David DiPaola

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
***/

#include <math.h>
#include "coord_dist.h"

const float EARTH_RADIUS = 6372797.560856; //in meters
const float TO_RAD = M_PI/180;
const float TO_DEG = 180/M_PI;

//converts from degrees, minutes, seconds to decimal degrees
float to_deg(int degrees, int minutes, int seconds){
  return (degrees+(minutes/60.0)+(seconds/3600.0));
}

//calculates distance between two coordinates (lat1,long1) and (lat2,long2)
float get_distance(float lat1, float long1,
                    float lat2, float long2){
  //Haversine implementation stolen from somewhere
  lat1 *= TO_RAD; long1 *= TO_RAD; lat2 *= TO_RAD; long2 *= TO_RAD;

  float latH = sin( (lat2-lat1)/2 );
  latH *= latH;

  float longH = sin( (long2-long1)/2 );
  longH *= longH;

  float arcLength = cos(lat1) * cos(lat2);
  arcLength = 2.0 * asin(sqrt(latH+arcLength*longH));

  return arcLength*EARTH_RADIUS;
}

//calulates the forward azimuth given two points
float get_fwd_azimuth(float lat1, float long1,
                       float lat2, float long2){
  lat1 *= TO_RAD; long1 *= TO_RAD; lat2 *= TO_RAD; long2 *= TO_RAD;

  float y = sin(long2-long1) * cos(lat2);
  float x = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(long2-long1);

  return atan2(y, x)*TO_DEG;
}
