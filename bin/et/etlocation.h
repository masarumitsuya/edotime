/*
  etlocation.h
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#ifndef __ETLOCATION_H__
#define __ETLOCATION_H__ 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */


struct et_location_t {
  double latitude;
  double longitude;
};

typedef struct et_location_t et_location_t;

const et_location_t ET_LOCATION_NIHONBASHI = { 35.689387, 139.772658 };

const double ET_UTC_OFFSET_JAPAN  = 9;

#ifdef __cplusplus
}
#endif	/* __cplusplus */
#endif	/* __ETLOCATION_H__ */

