/* GDCHART 0.94b  GDC.H  12 Nov 1998 */

/*
 * General header common to chart (xy[z]) and pie
 *
 * KChart port by Kalle Dalheimer <kalle@kde.org>
 *
 * $Id$
 */

#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <qfont.h>
#include <qcolor.h>
#include <limits.h>
#include <kchart_part.h>

class KChartParameters;


#define ABS( x )			( (x)<0.0? -(x): (x) )

short	cnt_nl( const char*, int* );


int out_graph( short gifwidth,
	       short gifheight,
	       QPainter* p, // paint here
	       KChartParameters* params,
	       const KoChart::Data& data );


class QPainter;

/**************************************************/
/**** USER DEFINABLE PIE OPTIONS  w/ defaults *****/
/**************************************************/
/**************************************************/

void pie_gif( short width,
			  short	height,
			  QPainter*,/* open file pointer, can be stdout */
			  KChartParameters* params,
			  int num_points,
			  float	data[],QColor color[],QColor colorShd[] );

#endif /*!_GDCPIE_H*/
