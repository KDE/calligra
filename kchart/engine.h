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
#include <values.h>
#include <kchart_part.h>

class KChartParameters;


#define ABS( x )			( (x)<0.0? -(x): (x) )

#define PVRED               0x00FF0000
#define PVGRN               0x0000FF00
#define PVBLU               0x000000FF
#define l2gdcal( c )        ((c)&PVRED)>>16 , ((c)&PVGRN)>>8 , ((c)&0x000000FF)
#define l2gdshd( c )        (((c)&PVRED)>>16)/2 , (((c)&PVGRN)>>8)/2 , (((c)&0x000000FF))/2
#define _gdcntrst(bg)		( ((bg)&0x800000?0x000000:0xFF0000)|	\
							  ((bg)&0x008000?0x000000:0x00FF00)|	\
							  ((bg)&0x000080?0x000000:0x0000FF) )

short	cnt_nl( const char*, int* );

#define EXTERND
#define DEFAULTO(val) = val

void	out_err( QPainter* p,
				 int			imagewidth,
				 int			imageheight,
				 QColor	BGColor,
				 QColor	LineColor,
				 char			*str );

/****************************************************/
/********** USER CHART OPTIONS w/ defaults **********/
/****************************************************/
//extern short GDC_xlabel_spacing;			/* pixels  MAXSHORT means force all */
extern char	GDC_yval_style;
extern char	*GDC_thumblabel;
extern float GDC_thumbval;



int out_graph( short		gifwidth,
			   short		gifheight,
			   QPainter*	p, // paint here
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
