#include "kivio_stencil.h"
#include "KIvioStencilIface.h"

 KivioStencilIface::KivioStencilIface(  KivioStencil *s ) :
 	DCOPObject(), stencil(s)
 {

 }
 DCOPRef  KivioStencilIface::ref()
 {
 	return DCOPRef(this);
 }
// DCOPRef duplicate();

 bool  KivioStencilIface::connected()
 {

 }
 void  KivioStencilIface::setConnected(bool c)
 {

 }

 double  KivioStencilIface::x()
 {

 }
 void  KivioStencilIface::setX( double f )
 {

 }

 double  KivioStencilIface::y()
 {

 }
 void  KivioStencilIface::setY( double f )
 {

 }

 double  KivioStencilIface::w()
 {

 }
 void  KivioStencilIface::setW( double f )
 {

 }

 double  KivioStencilIface::h()
 {

 }
 void  KivioStencilIface::setH( double f )
 {

 }

 QRect  KivioStencilIface::rect()
 {

 }

 void  KivioStencilIface::setPosition( double f1, double f2 )
 {

 }
 void  KivioStencilIface::setDimensions( double f1, double f2 )
 {

 }

 QColor  KivioStencilIface::fgColor()
 {

 }
 void  KivioStencilIface::setFGColor( QColor )
 {

 }

 void  KivioStencilIface::setBGColor( QColor )
 {

 }
 QColor  KivioStencilIface::bgColor()
 {

 }

 void  KivioStencilIface::setLineWidth( double )
 {

 }
 double  KivioStencilIface::lineWidth()
 {

 }

// FOnt stuff
 QColor  KivioStencilIface::textColor()
 {

 }
 void  KivioStencilIface::setTextColor( QColor )
 {

 }

 QFont  KivioStencilIface::textFont()
 {

 }
 void  KivioStencilIface::setTextFont( const QFont & )
 {

 }

 int  KivioStencilIface::hTextAlign()
 {

 }
 int  KivioStencilIface::vTextAlign()
 {

 }

 void  KivioStencilIface::setHTextAlign(int)
 {

 }
 void  KivioStencilIface::setVTextAlign(int)
 {

 }

 void  KivioStencilIface::setText( const QString & )
 {

 }
 QString  KivioStencilIface::text()
 {

 }

 bool  KivioStencilIface::isSelected()
 {

 }
 void  KivioStencilIface::select()
 {

 }
 void  KivioStencilIface::unselect()
 {

 }
 void  KivioStencilIface::subSelect( const double &, const double & )
 {

 }