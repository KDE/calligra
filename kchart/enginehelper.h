#ifndef ENGINEHELPER_H
#define ENGINEHELPER_H

int qcmpr( const void *a, const void *b );
void setrect( QPointArray& gdp, int x1, int x2, int y1, int y2 );
void set3dpoly( QPointArray& gdp, int x1, int x2, int y1, int y2,
		int xoff, int yoff );
void set3dbar( QPointArray& gdp, int x1, int x2, int y1, int y2,
		int xoff, int yoff );
int barcmpr( const void *a, const void *b );

#endif // ENGINEHELPER_H
