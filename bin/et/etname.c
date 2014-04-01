/*
  etname.c
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "etname.h"
#include "edotime.h"

const char ETNAME_TOKEN[]  = "-";
const char ETNAME_HALF[]   = "half";

void
edotime_get_name(    const unsigned char et,
                     const unsigned char flg,
                     char *outbuf )
{
  if (outbuf == NULL) return;

  char *buf;

  buf = calloc(ETNAME_BUFSIZE, sizeof(char));
  if (buf == NULL) return;

  unsigned int night;

  night = (et & ET_NIGHTMASK) ? 1 : 0;

  if (flg == ETNAME_NUMERIC) {

	switch (et & ET_TIMEMASK) {
      case ET_SIX:
   	    snprintf( buf, 
                  ETNAME_BUFSIZE,
                  "%s%s%s",
                  night ? "dusk" : "dawn",
                  ETNAME_TOKEN,
                  "six" );
        break;

      case ET_FIVE:
   	    snprintf( buf, 
                  ETNAME_BUFSIZE,
                  "%s%s%s",
                  night ? "early evening" : "morning",
                  ETNAME_TOKEN,
                  "five" );
        break;

      case ET_FOUR:
   	    snprintf( buf, 
                  ETNAME_BUFSIZE,
                  "%s%s%s",
                  night ? "night" : "day",
                  ETNAME_TOKEN,
                  "four" );
        break;

      case ET_NINE:
   	    snprintf( buf, 
                  ETNAME_BUFSIZE,
                  "%s%s%s",
                  night ? "midnight" : "noon",
                  ETNAME_TOKEN,
                  "nine" );
        break;

      case ET_EIGHT:
   	    snprintf( buf, 
                  ETNAME_BUFSIZE,
                  "%s%s%s",
                  night ? "night" : "day",
                  ETNAME_TOKEN,
                  "eight" );
        break;

      case ET_SEVEN:
   	    snprintf( buf, 
                  ETNAME_BUFSIZE,
                  "%s%s%s",
                  night ? "nightfall" : "daybreak",
                  ETNAME_TOKEN,
                  "seven" );
        break;

      default:
        break;

	}

    if (strnlen(buf, ETNAME_BUFSIZE) && (et & ET_HALFMASK)) {
	  size_t len;
	  len = strnlen(buf, ETNAME_BUFSIZE);
	  snprintf( &buf[len],
                ETNAME_BUFSIZE - len, 
                "%s%s",
                ETNAME_TOKEN,
                ETNAME_HALF );
    }

  } else if (flg == ETNAME_ZODIAC) {
	switch (et & ET_TIMEMASK) {
      case ET_SIX:
		(void)strncat(buf, night ? "cock" : "rabbit", ETNAME_BUFSIZE);
        break;

      case ET_FIVE:
		(void)strncat(buf, night ? "dog" : "dragon", ETNAME_BUFSIZE);
        break;

      case ET_FOUR:
		(void)strncat(buf, night ? "boar" : "snake", ETNAME_BUFSIZE);
        break;

      case ET_NINE:
		(void)strncat(buf, night ? "rat" : "horse", ETNAME_BUFSIZE);
        break;

      case ET_EIGHT:
		(void)strncat(buf, night ? "ox" : "sheep", ETNAME_BUFSIZE);
        break;

      case ET_SEVEN:
		(void)strncat(buf, night ? "tiger" : "monkey", ETNAME_BUFSIZE);
        break;

      default:
        break;

	}

	size_t len;
	len = strnlen(buf, ETNAME_BUFSIZE);

    if (len) {
	  if (edotime_is_justhalftime(et)) {
		char *buf_clone;
		buf_clone = calloc(ETNAME_BUFSIZE, sizeof(char));
		if (buf_clone) {
		  strncpy(buf_clone, buf, ETNAME_BUFSIZE);
		  snprintf( buf,
					ETNAME_BUFSIZE,
					"%s%s%s",
					"just intermediate",
					ETNAME_TOKEN,
					buf_clone);
		  free(buf_clone);
		}
	  } else {
		switch(et & ET_QUARTERMASK) {
          case ET_QUARTER_FIRST:
			snprintf( &buf[len],
                      ETNAME_BUFSIZE - len,
                      "%s%s",
					  ETNAME_TOKEN,
					  "first");
			break;

          case ET_QUARTER_SECOND:
			snprintf( &buf[len],
                      ETNAME_BUFSIZE - len,
                      "%s%s",
					  ETNAME_TOKEN,
					  "second");
			break;

          case ET_QUARTER_THIRD:
			snprintf( &buf[len],
                      ETNAME_BUFSIZE - len,
                      "%s%s",
					  ETNAME_TOKEN,
					  "third");
			break;

          case ET_QUARTER_FOURTH:
			snprintf( &buf[len],
                      ETNAME_BUFSIZE - len,
                      "%s%s",
					  ETNAME_TOKEN,
					  "fourth");
			break;

          default:
            break;

		}
	  }
    }
  }

  strncpy(outbuf, buf, ETNAME_BUFSIZE);
  free(buf);
}
