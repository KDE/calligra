/**
 * Parameters for new KChart engine
 *
 * $Id:$
 */

#include <qcolor.h>
#include <gdc.h>
#include <gdchart.h>
#include <gdcpie.h>

/*** Common options ***/
char				GDC_generate_gif = TRUE;
GDC_HOLD_IMAGE_T	GDC_hold_img = GDC_DESTROY_IMAGE;
void				*GDC_image = (void*)NULL;	/* in/out */

/*** Stuff ***/
char *GDC_ytitle;
char *GDC_xtitle;
char *GDC_ytitle2;		/* ostesibly: volume label */
char *GDC_title;			/* NLs ok here */
enum GDC_font_size	GDC_title_size		= GDC_MEDBOLD;
enum GDC_font_size	GDC_ytitle_size		= GDC_MEDBOLD;
enum GDC_font_size	GDC_xtitle_size		= GDC_MEDBOLD;
enum GDC_font_size	GDC_yaxisfont_size	= GDC_SMALL;
enum GDC_font_size	GDC_xaxisfont_size	= GDC_SMALL;
char *GDC_ylabel_fmt		= NULL;		/* printf fmt'ing, e.g.: "%.2f" */
char *GDC_ylabel2_fmt	= NULL;		/* default: "%.0f" future: fractions */
short GDC_xlabel_spacing	= 5;			/* pixels  MAXSHORT means force all */
char GDC_ylabel_density	= 80;			/* % */
float GDC_requested_ymin	= GDC_NOVALUE;
float GDC_requested_ymax	= GDC_NOVALUE;
float GDC_requested_yinterval	= GDC_NOVALUE;
char GDC_0Shelf			= TRUE;		/* if applicable */
char GDC_grid			= TRUE;
char GDC_xaxis			= TRUE;
char GDC_yaxis			= TRUE;
char GDC_yaxis2			= TRUE;
char GDC_yval_style		= TRUE;
GDC_STACK_T	GDC_stack_type		= GDC_STACK_DEPTH;
float GDC_3d_depth		= 5.0;		/* % gif size */
unsigned char GDC_3d_angle		= 45;			/* 1-89 */
unsigned char GDC_bar_width		= 75;			/* % (1-100) */
GDC_HLC_STYLE_T	GDC_HLC_style		= GDC_HLC_CLOSE_CONNECTED;
unsigned char GDC_HLC_cap_width	= 25;			/* % (1-100) */
GDC_ANNOTATION_T *GDC_annotation		= (GDC_ANNOTATION_T*)NULL;
enum GDC_font_size GDC_annotation_font	= GDC_SMALL;
int	GDC_num_scatter_pts	= 0;
GDC_SCATTER_T *GDC_scatter		= (GDC_SCATTER_T*)NULL;
char GDC_thumbnail		= FALSE;
char *GDC_thumblabel;
float GDC_thumbval		= -MAXFLOAT;
char GDC_border			= TRUE;
char GDC_transparent_bg;
char *GDC_BGImage;
char GDC_hard_size;
int	GDC_hard_xorig;				/* in/out */
int	GDC_hard_graphwidth;				/* in/out */
int	GDC_hard_yorig;				/* in/out */
int	GDC_hard_grapheight;				/* in/out */
char GDCPIE_other_threshold = -1;
unsigned short GDCPIE_3d_angle = 45;			/* 0-360 */
unsigned short GDCPIE_3d_depth = 10;			/* % gif width */
char *GDCPIE_title = 0;		/* NLs ok here */
enum GDC_font_size GDCPIE_title_size = GDC_MEDBOLD;
enum GDC_font_size GDCPIE_label_size = GDC_SMALL;
int	GDCPIE_label_dist = 1+8/2;		/* 1+GDC_fontc[GDCPIE_label_size].h/2 */
unsigned char GDCPIE_label_line = false;		/* from label to slice */
int	*GDCPIE_explode = 0;	/* [num_points] */
unsigned char *GDCPIE_missing = 0;	/* TRUE/FALSE */
GDCPIE_PCT_TYPE	GDCPIE_percent_labels = GDCPIE_PCT_NONE;

/*** Colors ***/
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
QColor GDCPIE_BGColor;	/* black */
QColor GDCPIE_PlotColor;	/* gray */
QColor GDCPIE_LineColor;
QColor GDCPIE_EdgeColor; /* edging on/off */
/* [num_points] supercedes GDCPIE_PlotColor */
QColor *GDCPIE_Color( 0 );



/*** Defaultfonts ***/
QFont* theKChartTinyFont;
QFont* theKChartSmallFont;
QFont* theKChartMediumFont;
QFont* theKChartLargeFont;
QFont* theKChartGiantFont;
struct	GDC_FONT_T*	GDC_fontc;
