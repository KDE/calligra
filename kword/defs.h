#ifndef defs_h
#define defs_h

// #define ZOOM(vx) ((vx)*ZoomZ/ZoomN)
#define ZOOM(vx) (vx)
// #define UNZOOM(px) ((px)*ZoomN/ZoomZ)
#define UNZOOM(px) (px)

// #define MM_TO_PX(mm) (int((float)mm*0.283465))	// empirisch ermittelte Werte
// #define PX_TO_MM(px) (int((float)px)/0.283465))
#define POINT_TO_MM(px) (int((float)px)/2.83465))
#define MM_TO_POINT(mm) (int((float)mm*2.83465))	// empirisch ermittelte Werte

#define A4_WIDTH 210.0
#define A4_HEIGHT 297.0

#define A5_WIDTH 155.0
#define A5_HEIGHT 290.0

#define LETTER_WIDTH 216.0
#define LETTER_HEIGHT 280.0

#define LEGAL_WIDTH 216.0
#define LEGAL_HEIGHT 355.0

#define EXECUTIVE_WIDTH 184.0
#define EXECUTIVE_HEIGHT 266.0

#define DEFAULT_PAPER_WIDTH  210.0
#define DEFAULT_PAPER_HEIGHT 297.0

#define DEFAULT_TOP_BORDER 15.0
#define DEFAULT_BOTTOM_BORDER 15.0
#define DEFAULT_LEFT_BORDER 10.0
#define DEFAULT_RIGHT_BORDER 10.0

#define STANDARD_COLUMNS 2
#define STANDARD_COLUMN_SPACING 3

#endif
