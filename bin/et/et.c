/*
  et.c
 
  Copyright (c) 2014 DoMA
 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "edotime.h"
#include "etlocation.h"
#include "etname.h"
#include "tutil.h"

static int
load_latlong( char *latlong );

static int
load_datetime( char *timearg );
static int
load_current_datetime( void );

static int
exec_et( void );

static int
exec_et_list( void );

struct ___args {
  double time;
  double latitude;
  double longitude;
  double offset_utc;
  int flg_show_table;
  int year;
  int month;
  int day;
  int format;
};
typedef struct ___args __args;

static __args _args;

static edotime_t _etobj = NULL;

int main(int argc, char *argv[], char *envp[])
{
  int ret = 0;
  int has_location = 0;
  int opt;

  static struct option long_options[] = {
	{"table",    no_argument,        NULL, 't'},
	{"zodiac",   no_argument,        NULL, 'z'},
	{"location", required_argument,  NULL, 'l'},
	{"offset",   required_argument,  NULL, 'o'},
	{0, 0, 0, 0}
  };

  memset(&_args, 0, sizeof(__args));

  while (1) {
	opt = getopt_long( argc, argv, "tzl:o:", long_options, NULL );

	if (opt == -1) break;

	switch (opt) {
	case 't':
	  _args.flg_show_table = 1;
	  break;

	case 'z':
      _args.format = ETNAME_ZODIAC;
	  break;

	case 'l':
	  if (load_latlong(optarg) != 0)
		exit(EXIT_FAILURE);
	  has_location = 1;
      break;

	case 'o':
	  _args.offset_utc = atof(optarg);
	  break;

    ////////

	case '?':
	  break;

	case ':':
	  break;

    default:
	  break;
	}
  }

  if (optind < argc) {
	char *arg_time;

	arg_time = argv[optind];
	if (load_datetime(arg_time) != 0)
	  exit(EXIT_FAILURE);

  } else {
	/* load current time */
	load_current_datetime();
  }

  // if no location : set default
  if (has_location == 0) {
	/* JP, JST */
	_args.latitude = ET_LOCATION_NIHONBASHI.latitude;
	_args.longitude = ET_LOCATION_NIHONBASHI.longitude;
	_args.offset_utc = 9;
  }

  _etobj = edotime_create( _args.year,
						   _args.month,
						   _args.day,
						   _args.offset_utc,
						   _args.latitude,
						   _args.longitude );
  if (!_etobj) {
	// dump err
  }

  if (_args.flg_show_table == 0) {
	ret = exec_et();
  } else {
    // if no time : list
	ret = exec_et_list();
  }

  exit(ret);
}

int
load_latlong( char *latlong )
{
  if (latlong == NULL) return 1;

  char *str, *token;
  int i;

  for (i = 0, str = latlong; ; i ++, str = NULL) {
	token = strtok( str, "," );
	if (token == NULL) break;

	if (i == 0)
	  _args.latitude = atof(token);
	else if (i == 1)
	  _args.longitude = atof(token);
    else 
        break;
  }

  return 0;
}

int
load_datetime( char *timearg )
{
  if (timearg == NULL) return 1;

  char *date, *time;
  char *str, *token;
  int i;
  double timeval = 0;

  if ((date = strtok(timearg, " ")) == NULL) return 1;
  time = strtok(NULL, " ");		/* time is option */

  /* load date */
  for (i = 0, str = date; ; i ++, str = NULL) {
	token = strtok(str, ".");
	if (token == NULL) break;

	if (i == 0)
	  _args.month = atoi(token);
	else if (i == 1)
	  _args.day = atoi(token);
	else if (i == 2) {
	  _args.year = atoi(token);
	  break;
	}
  }

  if (i != 2)
    return 1;
  else if (time == NULL) {
	_args.flg_show_table = 1;
	return 0;
  }

  /* load time */
  for (i = 0, str = time; ; i ++, str = NULL) {
	double val = 0;
	token = strtok(str, ":");
	if (token == NULL) break;

	val = atof(token);
	if (i == 0)
	  timeval = val;
	else if (i == 1)
	  timeval += time_reverse_convert_minute(val);
	else if (i == 2)			/* second is option */
	  timeval += time_reverse_convert_second(val);
	else
	  break;
  }

  if (i < 1) return 1;

  _args.time = timeval;

  return 0;
}

int
load_current_datetime( void )
{
  time_t current;
  struct tm *ptm;
  double val, timeval = 0;

  time(&current);
  ptm = localtime(&current);

  timeval = ptm->tm_hour;

  val = time_reverse_convert_minute(ptm->tm_min);
  timeval += val;

  val = time_reverse_convert_second(ptm->tm_sec);
  timeval += val;

  _args.time = timeval;

  return 0;
}

int
exec_et( void )
{
  unsigned char et;
  char description[ETNAME_BUFSIZE];

  et = edotime_get_time( _etobj, _args.time );
  edotime_get_name( et, _args.format, description );

  printf( "%.0f:%02.0f:%02.0f is %s\n",
		  time_convert_hour(_args.time),
		  time_convert_minute(_args.time),
		  time_convert_second(_args.time),
		  description );

  return 0;
}

int
exec_et_list( void )
{
  int ret = 0;
  double t;
  double hour, min, sec;
  unsigned char et = ET_SIX | ET_QUARTER_FIRST;

  while (1) {
	char tname[ETNAME_BUFSIZE];
	char tname_ja[ETNAME_BUFSIZE];
	int is_night;

	t = edotime_get_contemporarytime( _etobj, et );
	hour = time_convert_hour(t);
	min = time_convert_minute(t);
	sec = time_convert_second(t);

	is_night = (et & ET_NIGHTMASK) == ET_NIGHTMASK ? 1 : 0;

	if ((et & ET_TIMEMASK) == ET_SIX) {
	  if (_args.format == ETNAME_NUMERIC) {
		if (is_night) {
		  strncpy(tname, "dusk six", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(kure muttsu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "dawn six", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(ake muttsu)", ETNAME_BUFSIZE);
		}
	  } else if (_args.format == ETNAME_ZODIAC) {
		if (is_night) {
		  strncpy(tname, "cock", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(tori)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "rabbit", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(u)", ETNAME_BUFSIZE);
		}
	  }
	}
	else if ((et & ET_TIMEMASK) == ET_FIVE) {
	  if (_args.format == ETNAME_NUMERIC) {
		if (is_night) {
		  strncpy(tname, "early evening five", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(yoi itsutsu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "morning five", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(asa itsutsu)", ETNAME_BUFSIZE);
		}
	  } else if (_args.format == ETNAME_ZODIAC) {
		if (is_night) {
		  strncpy(tname, "dog", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(inu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "dragon", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(tatsu)", ETNAME_BUFSIZE);
		}
	  }
	}
	else if ((et & ET_TIMEMASK) == ET_FOUR) {
	  if (_args.format == ETNAME_NUMERIC) {
		if (is_night) {
		  strncpy(tname, "night four", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(yoru yottsu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "day four", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(hiru yottsu)", ETNAME_BUFSIZE);
		}
	  } else if (_args.format == ETNAME_ZODIAC) {
		if (is_night) {
		  strncpy(tname, "boar", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(i)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "snake", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(mi)", ETNAME_BUFSIZE);
		}
	  }
	}
	else if ((et & ET_TIMEMASK) == ET_NINE) {
	  if (_args.format == ETNAME_NUMERIC) {
		if (is_night) {
		  strncpy(tname, "midnight nine", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(mayo kokonotsu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "noon nine", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(mahiru kokonotsu)", ETNAME_BUFSIZE);
		}
	  } else if (_args.format == ETNAME_ZODIAC) {
		if (is_night) {
		  strncpy(tname, "rat", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(ne)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "horse", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(uma)", ETNAME_BUFSIZE);
		}
	  }
	}
	else if ((et & ET_TIMEMASK) == ET_EIGHT) {
	  if (_args.format == ETNAME_NUMERIC) {
		if (is_night) {
		  strncpy(tname, "night eight", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(yoru yattsu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "day eight", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(hiru yattsu)", ETNAME_BUFSIZE);
		}
	  } else if (_args.format == ETNAME_ZODIAC) {
		if (is_night) {
		  strncpy(tname, "ox", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(ushi)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "sheep", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(hitsuji)", ETNAME_BUFSIZE);
		}
	  }
	}
	else if ((et & ET_TIMEMASK) == ET_SEVEN) {
	  if (_args.format == ETNAME_NUMERIC) {
		if (is_night) {
		  strncpy(tname, "nightfall seven", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(akatsuki nanatsu)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "daybreak seven", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(yu nanatsu)", ETNAME_BUFSIZE);
		}
	  } else if (_args.format == ETNAME_ZODIAC) {
		if (is_night) {
		  strncpy(tname, "tiger", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(tora)", ETNAME_BUFSIZE);
		} else {
		  strncpy(tname, "monkey", ETNAME_BUFSIZE);
		  strncpy(tname_ja, "(saru)", ETNAME_BUFSIZE);
		}
	  }
	}

	if (_args.format == ETNAME_NUMERIC) {
	  printf( "%-19s%-21s", tname, tname_ja );
	} else if (_args.format == ETNAME_ZODIAC) {
	  printf( "%s %s\n", tname, tname_ja );
	} else {
	  break;    // abort
	}

	while(1) {
	  t = edotime_get_contemporarytime( _etobj, et );
	  hour = time_convert_hour(t);
	  min = time_convert_minute(t);
	  sec = time_convert_second(t);

	  if ((et & ET_QUARTER_FIRST) == ET_QUARTER_FIRST) {
		if (_args.format == ETNAME_ZODIAC) {
		  printf( "  %-7s%-11s", "first", "(hitotsu)");
		}
		printf( "%.0f:%02.0f:%02.0f\n", hour, min, sec );
		et = (et & ~ET_QUARTERMASK) | ET_QUARTER_SECOND;
	  } else if ((et & ET_QUARTER_SECOND) == ET_QUARTER_SECOND) {
		if (_args.format == ETNAME_ZODIAC) {
		  printf( "  %-7s%-11s", "second", "(hutatsu)");
		  printf( "%.0f:%02.0f:%02.0f\n", hour, min, sec );
		}
		et = (et & ~ET_QUARTERMASK) | ET_QUARTER_THIRD;
	  } else if ((et & ET_QUARTER_THIRD) == ET_QUARTER_THIRD) {
		if (_args.format == ETNAME_ZODIAC) {
		  printf( "  %-7s%-11s", "third", "(mittsu)");
		} else if (_args.format == ETNAME_NUMERIC) {
		  printf( "%40s", "half " );
		}
		printf( "%.0f:%02.0f:%02.0f\n", hour, min, sec );
		et = (et & ~ET_QUARTERMASK) | ET_QUARTER_FOURTH;
	  } else if ((et & ET_QUARTER_FOURTH) == ET_QUARTER_FOURTH) {
		if (_args.format == ETNAME_ZODIAC) {
		  printf( "  %-7s%-11s", "fourth", "(yottsu)");
		  printf( "%.0f:%02.0f:%02.0f\n", hour, min, sec );
		}
		et = et & ~ET_QUARTERMASK;
	  }

	  if ((et & ET_QUARTERMASK) == 0) {
		et |= ET_QUARTER_FIRST;
		break;
	  }
	}

	if (is_night && (et & ET_TIMEMASK) == ET_SEVEN) {
	  break;
	} else {
	  if ((et & ET_TIMEMASK) == ET_SEVEN) {
		et = (et & ~ET_TIMEMASK) | ET_NIGHTMASK | ET_SIX;
	  } else {
		et ++;
	  }
	}
  }

  return ret;
}

