/*
  etname.h
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#ifndef __ETNAME_H__
#define __ETNAME_H__ 1

#ifdef __cplusplus
extern C {
#endif	/* __cplusplus */

#define ETNAME_BUFSIZE 128

/* flags for kind of name */
enum { ETNAME_NUMERIC = 0, ETNAME_ZODIAC };

extern const char ETNAME_TOKEN[];

extern void
edotime_get_name(    const unsigned char et,
                     const unsigned char flg,
                     char *outbuf /* must be obtained ETNAME_BUFSIZE */ );

#ifdef __cplusplus
}
#endif	/* __cplusplus */
#endif	/* __ETNAME_H__ */

