/* GDCHART 0.94b  GDC.C  12 Nov 1998 */

/**
 * Version hacked by Kalle Dalheimer (kalle@kde.org) to use QPainter instead
 * of libgd.
 *
 * $Id$
 */

#include "gdc.h"

#include <qfont.h>
#include <qcolor.h>
#include <qpainter.h>



/* ------------------------------------------------------------------ *\ 
 * count (natural) substrings (new line sep)
\* ------------------------------------------------------------------ */
short
cnt_nl( char	*nstr,
		int		*len )			/* strlen - max seg */
{
	short	c           = 1;
	short	max_seg_len = 0;
	short	tmplen      = 0;

	if( !nstr )
		{
		if( len )
			*len = 0;
		return 0;
		}

	while( *nstr )
		{
		if( *nstr == '\n' )
			{
			++c;
			max_seg_len = MAX( tmplen, max_seg_len );
			tmplen = 0;
			}
		else
			++tmplen;
		++nstr;
		}

	if( len )
		*len = MAX( tmplen, max_seg_len );		/* don't forget last seg */
	return c;
}

#ifdef WE_MIGHT_NEED_THIS_LATER_BETTER_NOT_DELETE_IT_NOW
/* ------------------------------------------------------------------ *\ 
 * gd out a string with '\n's
\* ------------------------------------------------------------------ */
void
GDCImageStringNL( gdImagePtr		im,
		  struct GDC_FONT_T	*f,
		  int				x,
		  int				y,
		  char				*str,
		  int				clr,
		  GDC_justify_t		justify )
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
	    case GDC_JUSTIFY_LEFT:	
		xpos = x;					 
		break;
	    case GDC_JUSTIFY_RIGHT:	
		xpos = x+f->w*(max_len-len); 
		break;
	    case GDC_JUSTIFY_CENTER:
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

/* ------------------------------------------------------------------------ */
void
GDC_destroy_image(void *im)
{
	debug( "Calling GDC_destroy_image is not necessary" );
}

/* ------------------------------------------------------------------------ */
void
out_err( QPainter* p,
		 int			GIFWIDTH,
		 int			GIFHEIGHT,
		 QColor	BGColor,
		 QColor	LineColor,
		 char			*err_str )
{
	QColor			lineclr;
	QColor			bgclr;


	// PENDING(kalle) Implement reuse of images
	if( GDC_hold_img & GDC_REUSE_IMAGE )
		debug( "Sorry, not implemented: Reusing existing images" );
// 	if( (GDC_hold_img & GDC_REUSE_IMAGE) &&
// 		GDC_image != (void*)NULL )
// 		im = GDC_image;
// 	else
// 		im = gdImageCreate( GIFWIDTH, GIFHEIGHT );

	bgclr = BGColor;
	lineclr = LineColor;

	p->setPen( lineclr );
	p->setFont( *theKChartMediumFont );
	p->drawText( GIFWIDTH/2 - GDC_fontc[GDC_MEDBOLD].w*strlen(err_str)/2,
				 GIFHEIGHT/3, err_str );
}
