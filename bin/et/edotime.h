/*
  edotime.h
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#ifndef __EDOTIME_H__
#define __EDOTIME_H__ 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */


/* define of bits  */

enum {
  ET_SIX = 0,  // mutsu
  ET_FIVE,     // itsutsu
  ET_FOUR,     // yotsu
  ET_NINE,     // kokonotsu
  ET_EIGHT,    // yatsu
  ET_SEVEN     // nanatsu
};

enum {
  ET_QUARTER_FIRST    = 0x10,
  ET_QUARTER_SECOND   = (ET_QUARTER_FIRST << 1u),
  ET_QUARTER_THIRD    = (ET_QUARTER_FIRST << 2u),
  ET_QUARTER_FOURTH   = (ET_QUARTER_FIRST << 3u)
};

enum {
  ET_DAYMASK      = 0x0,
  ET_TIMEMASK     = 0x7,
  ET_NIGHTMASK    = 0x8,

  ET_JUSTHALFMASK = (ET_QUARTER_SECOND|ET_QUARTER_THIRD),
  ET_HALFMASK     = (ET_QUARTER_THIRD|ET_QUARTER_FOURTH),
  ET_QUARTERMASK  = 0xF0,
};

/* Naming by he Twelve Zodiac Signs in Chinese astrology */
enum {
  ET_TZS_RABBIT    = ET_DAYMASK|ET_SIX,        // u
  ET_TZS_DRAGON    = ET_DAYMASK|ET_FIVE,       // tatsu
  ET_TZS_SNAKE     = ET_DAYMASK|ET_FOUR,       // mi
  ET_TZS_HORSE     = ET_DAYMASK|ET_NINE,       // uma
  ET_TZS_SHEEP     = ET_DAYMASK|ET_EIGHT,      // hitsuji
  ET_TZS_MONKEY    = ET_DAYMASK|ET_SEVEN,      // saru
  ET_TZS_COCK      = ET_NIGHTMASK|ET_SIX,      // tori
  ET_TZS_DOG       = ET_NIGHTMASK|ET_FIVE,     // inu
  ET_TZS_BOAR      = ET_NIGHTMASK|ET_FOUR,     // i
  ET_TZS_RAT       = ET_NIGHTMASK|ET_NINE,     // ne
  ET_TZS_OX        = ET_NIGHTMASK|ET_EIGHT,    // ushi
  ET_TZS_TIGER     = ET_NIGHTMASK|ET_SEVEN     // tora
};

static inline unsigned int
edotime_is_justhalftime( unsigned char et ) {
  return ((et & ET_QUARTERMASK) ^ ET_JUSTHALFMASK) == 0 ? 1 : 0;
}

typedef struct _edotime_t *edotime_t;

extern edotime_t
edotime_create(    int year,
                   int month,
                   int day, 
                   double offset_utc,
                   double latitude,
                   double longitude );

extern unsigned char
edotime_get_time( edotime_t t,
                  double time );

extern double
edotime_get_contemporarytime( edotime_t t,
                              unsigned char time );

extern void
edotime_release( edotime_t t );

#ifdef __cplusplus
}
#endif    /* __cplusplus  */
#endif    /* __EDOTIME_H__  */
