// helper stuff for the engine

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <qpoint.h>
#include "engine.h"
#include "engine2.h"
#include "enginehelper.h"


/* ------------------------------------------------------------------ *\
 * count (natural) substrings (new line sep)
\* ------------------------------------------------------------------ */
short cnt_nl( const char *nstr,int *len )/* strlen - max seg */
{
	short c = 1;
	short max_seg_len = 0;
	short tmplen = 0;

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
		        max_seg_len = QMAX( tmplen, max_seg_len );
		        tmplen = 0;
		        }
		else
		        ++tmplen;
		++nstr;
		}

	if( len )
		*len = QMAX( tmplen, max_seg_len );/* don't forget last seg */
	return c;
}



int qcmpr( const void *a, const void *b ) {
    if( ((struct YS*)a)->y2 < ((struct YS*)b)->y2 ) return 1;
    if( ((struct YS*)a)->y2 > ((struct YS*)b)->y2 ) return -1;
    return 0;
}


void setrect( QPointArray& gdp, int x1, int x2, int y1, int y2 )
{
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x2, y1 );
    gdp.setPoint( 2, x2, y2 );
    gdp.setPoint( 3, x1, y2 );
}



 void set3dpoly( QPointArray& gdp, int x1, int x2, int y1, int y2,
				  int xoff, int yoff )
{
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x1 + xoff, y1 - yoff );
    gdp.setPoint( 2, x2 + xoff, y2 - yoff );
    gdp.setPoint( 3, x2, y2 );
}

 void set3dbar( QPointArray& gdp, int x1, int x2, int y1, int y2,
				 int xoff, int yoff )
{
    gdp.setPoint( 0, x1, y1 );
    gdp.setPoint( 1, x1 + xoff, y1 - yoff );
    gdp.setPoint( 2, x2 + xoff, y2 - yoff );
    gdp.setPoint( 3, x2, y2 );
}

int barcmpr( const void *a, const void *b )
{
  if( ((struct BS*)a)->y2 < ((struct BS*)b)->y2 ) return -1;
  if( ((struct BS*)a)->y2 > ((struct BS*)b)->y2 ) return 1;
  return 0;
}

double func_px( int cx, int x, double rad, double pscl )
{
    return( cx + (int)( ((float)rad)*sin(pscl*(double)(x)) ) );
}


double func_py( int cy, int x, double rad, double pscl )
{
    return( cy - (int)( ((float)rad)*cos(pscl*(double)(x)) ) );
}


char*
price_to_str( float price,
                int *numorator,
                int *demoninator,
                int *decimal,
                const char	*fltfmt )// printf fmt'ing str
{
    static char rtn[64];
	int whole = (int)price;
	float dec = GET_DEC( price );
        float numr=0;
	/* float		pow( double, double ); */

	// caller doesn't want fractions
	if( fltfmt )
		{
		sprintf( rtn, fltfmt, price );
		*numorator = *demoninator = *decimal = 0;
		return rtn;
		}

	numr = dec * 256;
	/* check if we have a perfect fration in 256ths */
	{
		float	rdec = GET_DEC( numr );

		if( rdec < EPSILON )
			;							/* close enough to frac */
		else if( (1-rdec) < EPSILON )	/* just over but close enough */
			++numr;
		else							/* no frac match */
		{
			sprintf( rtn, "%f", price );
			*numorator = *demoninator = *decimal = 0;
			return rtn;
		}
	}

	/* now have numr 256ths */
	/* resolve down */
	if( numr != 0 )
		{
		int	cnt = 8;

		while( (float)(numr)/2.0 == (float)(int)(numr/2) )
			{
			numr /= 2;
			--cnt;
			}

		/* don't want both whole AND numerator to be - */
		if( whole<0 && numr<0.0 )
			numr = -numr;
		*numorator = (int)numr;
		*demoninator = (int)pow((float)2.0, (float)cnt);
		*decimal = whole;
		sprintf( rtn, "%d %d/%d", whole,(int)numr,*demoninator );
		}
	else
		{
		*numorator = *demoninator = 0;
		*decimal = whole;
		sprintf( rtn, "%d", whole );
		}

    return rtn;
}

