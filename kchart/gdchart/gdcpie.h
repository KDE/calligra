/* GDCHART 0.94b  GDCPIE.H  12 Nov 1998 */

#ifndef _GDCPIE_H
#define _GDCPIE_H

#ifndef _GDC_H
#include "gdc.h"
#endif

class QPainter;

extern struct GDC_FONT_T*	GDC_fontc;

typedef enum {
             GDC_3DPIE,
             GDC_2DPIE
             } GDCPIE_TYPE;

typedef enum {
             GDCPIE_PCT_NONE,
             GDCPIE_PCT_ABOVE,		/* relative to label, if any */
             GDCPIE_PCT_BELOW,
             GDCPIE_PCT_RIGHT,
             GDCPIE_PCT_LEFT
             } GDCPIE_PCT_TYPE;


/**************************************************/
/**** USER DEFINABLE PIE OPTIONS  w/ defaults *****/
/**************************************************/
extern QColor* GDCPIE_BGColor;	/* black */
extern QColor* GDCPIE_PlotColor;	/* gray */
extern QColor* GDCPIE_LineColor;
extern QColor* GDCPIE_EdgeColor; /* edging on/off */

extern char	GDCPIE_other_threshold;
extern unsigned short GDCPIE_3d_angle;			/* 0-360 */
extern unsigned short GDCPIE_3d_depth;			/* % gif width */
extern char *GDCPIE_title;		/* NLs ok here */
extern enum GDC_font_size GDCPIE_title_size;
extern enum GDC_font_size GDCPIE_label_size;
extern int GDCPIE_label_dist;		/* 1+GDC_fontc[GDCPIE_label_size].h/2 */
extern unsigned char GDCPIE_label_line;		/* from label to slice */

extern int *GDCPIE_explode;	/* [num_points] */
															/* [num_points] supercedes GDCPIE_PlotColor */
extern QColor *GDCPIE_Color;
extern unsigned char *GDCPIE_missing;	/* TRUE/FALSE */

extern GDCPIE_PCT_TYPE		GDCPIE_percent_labels;
/**** COMMON OPTIONS ******************************/
/* NOTE:  common options copy here for reference only! */
/*        they live in gdc.h                           */
#ifndef _GDC_COMMON_OPTIONS
#define _GDC_COMMON_OPTIONS
EXTERND char				GDC_generate_gif	DEFAULTO( TRUE );

EXTERND GDC_HOLD_IMAGE_T	GDC_hold_img		DEFAULTO( GDC_DESTROY_IMAGE );
EXTERND void				*GDC_image			DEFAULTO( (void*)NULL );	/* in/out */
#endif
/**************************************************/

void pie_gif( short			width,
			  short			height,
			  QPainter*,						/* open file pointer, can be stdout */
			  GDCPIE_TYPE,
			  int			num_points,
			  char			*labels[],		/* slice labels */
			  float			data[] );

#endif /*!_GDCPIE_H*/
