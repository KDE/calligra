#ifndef ENGINEDRAW_H
#define ENGINEDRAW_H

void draw_3d_bar( QPainter* p,
	     int x1,
	     int x2,
	     int y0,
	     int yhigh,
	     int xdepth,
	     int ydepth,
	     QColor clr,
	     QColor clrshd );

void draw_3d_line( QPainter*	p,
		   int y0,
		   int x1,
		   int x2,
		   int y1[],
		   int y2[],
		   int xdepth,
		   int ydepth,
		   int num_sets,
		   QColor clr[],
		   QColor clrshd[] );

void draw_3d_area( QPainter* p,
			  int x1,
			  int x2,
			  int y0,			// drawn from 0
			  int y1,
			  int y2,
			  int xdepth,
			  int ydepth,
			  QColor clr,
		   QColor clrshd );

#endif // ENGINEDRAW_H
