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
  return false;
}
void  KivioStencilIface::setConnected(bool /*c*/)
{

}

double  KivioStencilIface::x()
{
  return 0.0;
}
void  KivioStencilIface::setX( double /*f*/ )
{

}

double  KivioStencilIface::y()
{
  return 0.0;
}

void  KivioStencilIface::setY( double /*f*/ )
{

}

double  KivioStencilIface::w()
{
  return 0.0;
}

void  KivioStencilIface::setW( double /*f*/ )
{

}

double  KivioStencilIface::h()
{
  return 0.0;
}

void  KivioStencilIface::setH( double /*f*/ )
{

}

QRect  KivioStencilIface::rect()
{
  return QRect();
}

void  KivioStencilIface::setPosition( double /*f1*/, double /*f2*/ )
{

}

void  KivioStencilIface::setDimensions( double /*f1*/, double /*f2*/ )
{

}

QColor  KivioStencilIface::fgColor()
{
  return QColor();
}

void  KivioStencilIface::setFGColor( QColor )
{

}

void  KivioStencilIface::setBGColor( QColor )
{

}

QColor  KivioStencilIface::bgColor()
{
  return QColor();
}

void  KivioStencilIface::setLineWidth( double )
{

}
double  KivioStencilIface::lineWidth()
{
  return 0.0;
}

// FOnt stuff
QColor  KivioStencilIface::textColor()
{
  return QColor();
}

void  KivioStencilIface::setTextColor( QColor )
{

}

QFont  KivioStencilIface::textFont()
{
  return QFont();
}

void  KivioStencilIface::setTextFont( const QFont & )
{

}

int  KivioStencilIface::hTextAlign()
{
  return 0;
}

int  KivioStencilIface::vTextAlign()
{
  return 0;
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
  return QString::null;
}

bool  KivioStencilIface::isSelected()
{
  return false;
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
