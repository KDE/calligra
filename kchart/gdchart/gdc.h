/* GDCHART 0.94b  GDC.H  12 Nov 1998 */

/*
General header common to chart (xy[z]) and pie
*/

#ifndef _GDC_H
#define _GDC_H

#include <math.h>
#ifdef GDC_INCL
/* #include "gd.h" */
/* #include "gdfonts.h" */
/* #include "gdfontt.h" */
/* #include "gdfontmb.h" */
/* #include "gdfontg.h" */
/* #include "gdfontl.h" */
#endif

class QFont;

extern QFont* theKChartTinyFont;
extern QFont* theKChartSmallFont;
extern QFont* theKChartMediumFont;
extern QFont* theKChartLargeFont;
extern QFont* theKChartGiantFont;

#include <qfont.h>

#define GDC_NOVALUE			-MAXFLOAT
#define GDC_NULL			GDC_NOVALUE

#define ABS( x )			( (x)<0.0? -(x): (x) )
#define MAX( x, y )			( (x)>(y)?(x):(y) )
#define MIN( x, y )			( (x)<(y)?(x):(y) ) 

#define GDC_NOCOLOR			&Qt::black
#define GDC_DFLTCOLOR		&Qt::black
#define PVRED               0x00FF0000
#define PVGRN               0x0000FF00
#define PVBLU               0x000000FF
#define l2gdcal( c )        ((c)&PVRED)>>16 , ((c)&PVGRN)>>8 , ((c)&0x000000FF)
#define l2gdshd( c )        (((c)&PVRED)>>16)/2 , (((c)&PVGRN)>>8)/2 , (((c)&0x000000FF))/2
static int					_gdccfoo1;
static unsigned long		_gdccfoo2;
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

/* ordered by size */
enum GDC_font_size { GDC_pad = 0, GDC_TINY = 1, GDC_SMALL = 2, GDC_MEDBOLD = 3, 
                     GDC_LARGE = 4, GDC_GIANT = 5, GDC_numfonts= 6 };

/* GDC[PIE]_fontc depends on this */
typedef enum { GDC_DESTROY_IMAGE = 0, /* default */ 
               GDC_EXPOSE_IMAGE  = 1, /* user must call GDC_destroy_image() */ 
               GDC_REUSE_IMAGE   = 2  /* i.e., paint on top of */
} GDC_HOLD_IMAGE_T;			/* EXPOSE & REUSE */

struct GDC_FONT_T {
  QFont*	f;
  char		h;
  char		w;
};

typedef enum { GDC_JUSTIFY_RIGHT,
			   GDC_JUSTIFY_CENTER,
			   GDC_JUSTIFY_LEFT } GDC_justify_t;

void	load_font_conversions();
short	cnt_nl( char*, int* );



extern struct	GDC_FONT_T*	GDC_fontc;
#define EXTERND
#define DEFAULTO(val) = val

/**** COMMON OPTIONS ********************************/
#ifndef _GDC_COMMON_OPTIONS
#define _GDC_COMMON_OPTIONS
extern char				GDC_generate_gif;

extern GDC_HOLD_IMAGE_T	GDC_hold_img;
extern void				*GDC_image;	/* in/out */
#endif
/****************************************************/


void	GDC_destroy_image( void* );
void	out_err( QPainter* p,
				 int			imagewidth,
				 int			imageheight,
				 QColor	BGColor,
				 QColor	LineColor,
				 char			*str );

#endif /*!_GDC_H*/
