/* Global variables holding parameter data (for now)
 *****************************************************/

#include "engine.h"
#include <qcolor.h>



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
    kdDebug(35001) << "+++smallarrow" << endl;
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
