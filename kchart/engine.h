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

#define GDC_NOVALUE			-MAXFLOAT
#define GDC_NULL			GDC_NOVALUE

#define ABS( x )			( (x)<0.0? -(x): (x) )

#define PVRED               0x00FF0000
#define PVGRN               0x0000FF00
#define PVBLU               0x000000FF
#define l2gdcal( c )        ((c)&PVRED)>>16 , ((c)&PVGRN)>>8 , ((c)&0x000000FF)
#define l2gdshd( c )        (((c)&PVRED)>>16)/2 , (((c)&PVGRN)>>8)/2 , (((c)&0x000000FF))/2
#define _gdcntrst(bg)		( ((bg)&0x800000?0x000000:0xFF0000)|	\
							  ((bg)&0x008000?0x000000:0x00FF00)|	\
							  ((bg)&0x000080?0x000000:0x0000FF) )
#define _clrallocate( im, rawclr, bgc )								   \
	( (_gdccfoo2=rawclr==GDC_DFLTCOLOR? _gdcntrst(bgc): rawclr),	\
     (_gdccfoo1=gdImageColorExact(im,l2gdcal(_gdccfoo2))) != -1?	\
	_gdccfoo1:													\
	gdImageColorsTotal(im) == gdMaxColors?						\
   gdImageColorClosest(im,l2gdcal(_gdccfoo2)):				\
   gdImageColorAllocate(im,l2gdcal(_gdccfoo2)) )

#define _clrshdallocate( im, rawclr, bgc )													\
							( (_gdccfoo2=rawclr==GDC_DFLTCOLOR? _gdcntrst(bgc): rawclr),	\
							  (_gdccfoo1=gdImageColorExact(im,l2gdshd(_gdccfoo2))) != -1?	\
								_gdccfoo1:													\
								gdImageColorsTotal(im) == gdMaxColors?						\
									gdImageColorClosest(im,l2gdshd(_gdccfoo2)):				\
									gdImageColorAllocate(im,l2gdshd(_gdccfoo2)) )

/* GDC[PIE]_fontc depends on this */
typedef enum { GDC_DESTROY_IMAGE = 0, /* default */
               GDC_EXPOSE_IMAGE  = 1, /* user must call GDC_destroy_image() */
               GDC_REUSE_IMAGE   = 2  /* i.e., paint on top of */
} GDC_HOLD_IMAGE_T;			/* EXPOSE & REUSE */

/* struct GDC_FONT_T { */
/* 	QFont*	f; */
/* 	char		h; */
/* 	char		w; */
/* }; */

void	load_font_conversions();
short	cnt_nl( const char*, int* );



#define EXTERND
#define DEFAULTO(val) = val

/**** COMMON OPTIONS ********************************/
#ifndef _GDC_COMMON_OPTIONS
#define _GDC_COMMON_OPTIONS

extern GDC_HOLD_IMAGE_T	GDC_hold_img;
extern void				*GDC_image;	/* in/out */
#endif
/****************************************************/


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
extern QColor* GDC_BGColor;
extern QColor* GDC_GridColor;
extern QColor* GDC_LineColor;
extern QColor* GDC_PlotColor;
extern QColor* GDC_VolColor;
extern QColor* GDC_TitleColor;
extern QColor* GDC_XTitleColor;
extern QColor* GDC_YTitleColor;
extern QColor* GDC_YTitle2Color;
extern QColor* GDC_XLabelColor;
extern QColor* GDC_YLabelColor;
extern QColor* GDC_YLabel2Color;
/* supercedes VolColor	ulong_color[num_points] */
extern QColor *GDC_ExtVolColor;
/* supercedes LineColor	ulong_color[num_sets] */
extern QColor *GDC_SetColor;
/* supercedes SetColor	ulong_color[num_sets][num_points] */
extern QColor *GDC_ExtColor;

/* legends?  separate gif? */
/* auto-size fonts, based on GIF size? */

/* ----- following options are for expert users only ----- */
												/* for alignment of multiple charts */
												/* USE WITH CAUTION! */

/**** COMMON OPTIONS ********************************/
/* NOTE:  common options copy here for reference only! */
/*        they live in gdc.h                           */
#ifndef _GDC_COMMON_OPTIONS
#define _GDC_COMMON_OPTIONS

EXTERND GDC_HOLD_IMAGE_T	GDC_hold_img		DEFAULTO( GDC_DESTROY_IMAGE );
EXTERND void				*GDC_image			DEFAULTO( (void*)NULL );	/* in/out */
#endif
/****************************************************/

#ifdef GDC_LIB
#define clrallocate( im, rawclr )		_clrallocate( im, rawclr, GDC_BGColor )
#define clrshdallocate( im, rawclr )	_clrshdallocate( im, rawclr, GDC_BGColor )
#endif

int out_graph( short		gifwidth,
			   short		gifheight,
			   QPainter*	p, // paint here
			   KChartParameters* params,
                           const KoChart::Data& data );
/* expected params (...) for each chart type:
GDC_LINE
GDC_BAR
GDC_3DBAR
GDC_3DAREA
GDC_AREA			float	vals[], ...
												multiple sets make sense for rest?
GDC_HILOCLOSE		float	high[],
					float	low[],
					float	close[]

GDC_COMBO_LINE_BAR
GDC_COMBO_LINE_AREA	float	val[],
					float   vol[]

GDC_COMBO_HLC_BAR
GDC_COMBO_HLC_AREA	float   high[],
                    float   low[],
                    float   close[],
					float   vol[]

*/

/* Notes:
	GDC_thumbnail = TRUE
	is equivelent to:	GDC_grid = FALSE
						GDC_xaxis = FALSE
						GDC_yaxis = FALSE
*/

class QPainter;

/**************************************************/
/**** USER DEFINABLE PIE OPTIONS  w/ defaults *****/
/**************************************************/
extern QColor* GDCPIE_BGColor;	/* black */
extern QColor* GDCPIE_PlotColor;	/* gray */
extern QColor* GDCPIE_LineColor;
extern QColor* GDCPIE_EdgeColor; /* edging on/off */

extern char	GDCPIE_other_threshold;
extern char *GDCPIE_title;		/* NLs ok here */



extern int *GDCPIE_explode;	/* [num_points] */
/* [num_points] supercedes GDCPIE_PlotColor */
extern QColor *GDCPIE_Color;
extern unsigned char *GDCPIE_missing;	/* TRUE/FALSE */

/**** COMMON OPTIONS ******************************/
/* NOTE:  common options copy here for reference only! */
/*        they live in gdc.h                           */
#ifndef _GDC_COMMON_OPTIONS
#define _GDC_COMMON_OPTIONS

EXTERND GDC_HOLD_IMAGE_T	GDC_hold_img		DEFAULTO( GDC_DESTROY_IMAGE );
EXTERND void				*GDC_image			DEFAULTO( (void*)NULL );	/* in/out */
#endif
/**************************************************/

void pie_gif( short width,
			  short	height,
			  QPainter*,/* open file pointer, can be stdout */
			  KChartParameters* params,
			  int num_points,
			  float	data[],QColor color[],QColor colorShd[] );

#endif /*!_GDCPIE_H*/
