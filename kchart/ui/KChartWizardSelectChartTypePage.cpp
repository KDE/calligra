/* $Id: $ */

#include "KChartWizardSelectChartTypePage.h"

#include <qframe.h>

KChartWizardSelectChartTypePage::KChartWizardSelectChartTypePage( QWidget* parent ) :
  QWidget( parent )
{
  frame1 = newFrame( 10, 10 );
  frame2 = newFrame( 92, 10 );
  frame3 = newFrame( 174, 10 );
  frame4 = newFrame( 256, 10 );
  frame5 = newFrame( 338, 10 );
  frame6 = newFrame( 10, 92 );
  frame7 = newFrame( 92, 92 );
  frame8 = newFrame( 174, 92 );    
  frame9 = newFrame( 256, 92 );
  frame10 = newFrame( 338, 92 );
  frame11 = newFrame( 10, 174 );
  frame12 = newFrame( 92, 174 );
  frame13 = newFrame( 174, 174 );    
  frame14 = newFrame( 256, 174 );
  frame15 = newFrame( 338, 174 );
	
  parent->resize( 425, 256 );
}


QFrame* KChartWizardSelectChartTypePage::newFrame( int x, int y )
{
    QFrame *frame;
    frame = new QFrame( this );
    frame->setGeometry( x,y, 80, 80 );
    {
	QColorGroup normal( QColor( QRgb(0) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QColorGroup disabled( QColor( QRgb(8421504) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
	QColorGroup active( QColor( QRgb(0) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
	QPalette palette( normal, disabled, active );
	frame->setPalette( palette );
    }
    frame->setFrameStyle( 50 );
    frame->setLineWidth( 2 );

    return frame;
}
