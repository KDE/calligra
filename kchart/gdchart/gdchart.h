/* GDCHART 0.94b  GDCHART.H  12 Nov 1998 */

#ifndef _GDCHART_H
#define _GDCHART_H

#ifndef _GDC_H
#include "gdc.h"
#endif

#include <qcolor.h>
#include <values.h>

class QPainter;

#define MAX_NOTE_LEN		19

typedef enum {
			 GDC_LINE,
			 GDC_AREA,
			 GDC_BAR,
			 GDC_HILOCLOSE,
			 GDC_COMBO_LINE_BAR,			/* aka, VOL[ume] */
			 GDC_COMBO_HLC_BAR,
			 GDC_COMBO_LINE_AREA,
			 GDC_COMBO_HLC_AREA,
			 GDC_3DHILOCLOSE,
			 GDC_3DCOMBO_LINE_BAR,
			 GDC_3DCOMBO_LINE_AREA,
			 GDC_3DCOMBO_HLC_BAR,
			 GDC_3DCOMBO_HLC_AREA,
			 GDC_3DBAR,
			 GDC_3DAREA,
			 GDC_3DLINE
			 } GDC_CHART_T;

typedef enum {
			 GDC_STACK_DEPTH,				/* "behind" (even non-3D) */
			 GDC_STACK_SUM,
			 GDC_STACK_BESIDE,
			 GDC_STACK_LAYER
			 } GDC_STACK_T;					/* applies only to num_lines > 1 */

typedef enum {
			 GDC_HLC_DIAMOND         = 1,
			 GDC_HLC_CLOSE_CONNECTED = 2,	/* can't be used w/ CONNECTING */
			 GDC_HLC_CONNECTING      = 4,	/* can't be used w/ CLOSE_CONNECTED */
			 GDC_HLC_I_CAP           = 8
			 } GDC_HLC_STYLE_T;				/* can be OR'd */

											/* only 1 annotation allowed */
typedef struct
			{
			float			point;			/* 0 <= point < num_points */
			unsigned long	color;
			char			note[MAX_NOTE_LEN+1];	/* NLs ok here */
			} GDC_ANNOTATION_T;

typedef enum {
			 GDC_SCATTER_TRIANGLE_DOWN,
			 GDC_SCATTER_TRIANGLE_UP
			 } GDC_SCATTER_IND_T;
typedef struct {
	float				point;		/* 0 <= point < num_points */
	float				val;
	unsigned short		width;		/* % (1-100) */
	unsigned long		color;
	GDC_SCATTER_IND_T	ind;
} GDC_SCATTER_T;

/****************************************************/
/********** USER CHART OPTIONS w/ defaults **********/
/****************************************************/
extern char	*GDC_ytitle;
extern char	*GDC_xtitle;
extern char	*GDC_ytitle2;		/* ostesibly: volume label */
extern char	*GDC_title;			/* NLs ok here */
extern enum GDC_font_size GDC_title_size;
extern enum GDC_font_size GDC_ytitle_size;
extern enum GDC_font_size GDC_xtitle_size;
extern enum GDC_font_size GDC_yaxisfont_size;
extern enum GDC_font_size GDC_xaxisfont_size;
extern char	*GDC_ylabel_fmt;		/* printf fmt'ing, e.g.: "%.2f" */
extern char	*GDC_ylabel2_fmt;		/* default: "%.0f" future: fractions */
extern short GDC_xlabel_spacing;			/* pixels  MAXSHORT means force all */
extern char	GDC_ylabel_density;			/* % */
extern float GDC_requested_ymin;
extern float GDC_requested_ymax;
extern float GDC_requested_yinterval;
extern char	GDC_0Shelf;		/* if applicable */
extern char	GDC_grid;
extern char	GDC_xaxis;
extern char	GDC_yaxis;
extern char	GDC_yaxis2;
extern char	GDC_yval_style;
extern GDC_STACK_T GDC_stack_type;
extern float GDC_3d_depth;		/* % gif size */
extern unsigned char GDC_3d_angle;			/* 1-89 */
extern unsigned char GDC_bar_width;			/* % (1-100) */
extern GDC_HLC_STYLE_T GDC_HLC_style;
extern unsigned char GDC_HLC_cap_width;			/* % (1-100) */
extern GDC_ANNOTATION_T	*GDC_annotation;
extern enum GDC_font_size GDC_annotation_font;
extern int GDC_num_scatter_pts;
extern GDC_SCATTER_T *GDC_scatter;
extern char	GDC_thumbnail;
extern char	*GDC_thumblabel;
extern float GDC_thumbval;
extern char	GDC_border;
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
extern char	GDC_transparent_bg;
extern char	*GDC_BGImage;

/* legends?  separate gif? */
/* auto-size fonts, based on GIF size? */

/* ----- following options are for expert users only ----- */
												/* for alignment of multiple charts */
												/* USE WITH CAUTION! */
extern char GDC_hard_size;
extern int GDC_hard_xorig;				/* in/out */
extern int GDC_hard_graphwidth;				/* in/out */
extern int GDC_hard_yorig;				/* in/out */
extern int GDC_hard_grapheight;				/* in/out */

/**** COMMON OPTIONS ********************************/
/* NOTE:  common options copy here for reference only! */
/*        they live in gdc.h                           */
#ifndef _GDC_COMMON_OPTIONS
#define _GDC_COMMON_OPTIONS
EXTERND char				GDC_generate_gif	DEFAULTO( TRUE );

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
			   GDC_CHART_T	type,
			   int			num_points,		/* points along x axis (even iterval) */
			   char			*xlbl[],
			   int			num_sets,
								... );
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

#endif /*!_GDCHART_H*/
