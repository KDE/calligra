/* Global variables holding parameter data (for now)  
 *****************************************************/

#include "engine.h"
#include <qcolor.h>

/*** Common options ***/
GDC_HOLD_IMAGE_T	GDC_hold_img = GDC_DESTROY_IMAGE;
void				*GDC_image = (void*)NULL;	/* in/out */

/*** Stuff ***/
short GDC_xlabel_spacing	= 5;			/* pixels  MAXSHORT means force all */
char GDC_yval_style		= TRUE;
char *GDC_thumblabel;
float GDC_thumbval		= -MAXFLOAT;
char *GDC_BGImage;
char *GDCPIE_title = 0;		/* NLs ok here */
int	GDCPIE_label_dist = 1+8/2;		/* 1+GDC_fontc[GDCPIE_label_size].h/2 */
unsigned char GDCPIE_label_line = false;		/* from label to slice */
int	*GDCPIE_explode = 0;	/* [num_points] */
unsigned char *GDCPIE_missing = 0;	/* TRUE/FALSE */

/*** Colors ***/

#ifdef PROBABLY_NOT_NEEDED
QColor* GDC_BGColor;	 /* black */
QColor* GDC_GridColor;	 /* gray */
QColor* GDC_LineColor;
QColor* GDC_PlotColor;
QColor* GDC_VolColor;	 /* lgtblue1 */
QColor* GDC_TitleColor; /* "opposite" of BG */
QColor* GDC_XTitleColor;
QColor* GDC_YTitleColor;
QColor* GDC_YTitle2Color;
QColor* GDC_XLabelColor;
QColor* GDC_YLabelColor;
QColor* GDC_YLabel2Color;
/* supercedes VolColor	ulong_color[num_points] */
QColor *GDC_ExtVolColor( 0 );
/* supercedes LineColor	ulong_color[num_sets] */
QColor *GDC_SetColor( 0 );
/* supercedes SetColor	ulong_color[num_sets][num_points] */
QColor *GDC_ExtColor( 0 );
QColor* GDCPIE_BGColor;	/* black */
QColor* GDCPIE_PlotColor;	/* gray */
QColor* GDCPIE_LineColor;
QColor* GDCPIE_EdgeColor; /* edging on/off */
/* [num_points] supercedes GDCPIE_PlotColor */
QColor *GDCPIE_Color( 0 );
#endif // probably not needed

#ifdef WE_MIGHT_NEED_THIS_LATER_BETTER_NOT_DELETE_IT_NOW
/* ------------------------------------------------------------------ *\
 * gd out a string with '\n's
\* ------------------------------------------------------------------ */
void
GDCImageStringNL( gdImagePtr		im,
				  QFont f,
				  int				x,
				  int				y,
				  char				*str,
				  int				clr,
				  KChartJustifyType	justify )
{
    int		i;
    int		len;
    int     max_len;
    short   strs_num = cnt_nl( str, &max_len );
    char    sub_str[max_len+1];

    len      = -1;
    strs_num = -1;
    i = -1;
    do {
		++i;
		++len;
		sub_str[len] = *(str+i);
		if( *(str+i) == '\n' ||
			*(str+i) == '\0' ) {
			int	xpos;
	
			sub_str[len] = '\0';
			++strs_num;
			switch( justify ) {
			case KCHARTJUSTIFYLEFT:	
				xpos = x;					
				break;
			case KCHARTJUSTIFYRIGHT:	
				xpos = x+f->w*(max_len-len);
				break;
			case KCHARTJUSTIFYCENTER:
			default:					
				xpos = x+f->w*(max_len-len)/2;
			}
			gdImageString( im,
						   f->f,
						   xpos,
						   y + (f->h-1)*strs_num,
						   sub_str,
						   clr );
			len = -1;
		}
    }
    while( *(str+i) );
}
#endif


#ifdef THUMB_VALS
// -------------------------------------------------------------------
// draw an arrow at (x,y)-upper left in arrwclr to the size of SmallFont
// could, with just a little difficulty, be made to accept a font size
// -------------------------------------------------------------------
void
smallarrow( QPainter* p,
			int	x,
			int	y,
			char up,
			int	arrwclr )
{
    debug( "+++smallarrow" );
	QFont f( "Helvetica", 10 );
	QFontMetrics fm( f );
	int h = fm.height();
    p->drawLine( x+2, y, x+2, y+h );
    p->drawLine( x+3, y, x+3, y+h );
    if( up ) {	/*   oo   */
		/*  uoou  */
		p->setPen( arrwclr );
		p->drawPoint( x, y+2 );   /* uuoouu */
		p->drawPoint( x+1, y+2 ); /*   oo   */
		p->drawPoint( x+4, y+2 );	/*   oo   */
		p->drawPoint( x+5, y+2 );	/*   oo   */
		p->drawPoint( x+1, y+1 ); /*   oo   */
		p->drawPoint( x+4, y+1 );	/*   oo   */
		/*   oo   */
    } else { /* ddoodd */
		/*  dood  */
		p->setPen( arrwclr );
		p->drawPoint( x,   y+(h-2) );	/*   oo   */
		p->drawPoint( x+1, y+(h-2) );
		p->drawPoint( x+4, y+(h-2) );
		p->drawPoint( x+5, y+(h-2) );
		p->drawPoint( x+1, y+(h-1) );
		p->drawPoint( x+4, y+(h-1) );
    }
}
#endif
