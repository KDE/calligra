#include <iostream.h>
#include <stdio.h>
#include <time.h>

#include <qapplication.h>
#include <qkeycode.h>
#include <qevent.h>
#include <qscrbar.h>
#include <qtimer.h>
#include <qframe.h>

#include "kdbcalendar.h"


// Calendar

kdbCalendar::kdbCalendar(const QString& p_df, kdbDataField *p_fld,QWidget* p, const char *n)
:QWidget(p, n)
{
	uint width, height;
	uint frameWidth;
	
	_field      = p_fld;
	_pushed     = false;
	_arrowDown  = false;
  _date       = new kdbPickDate( 0,0, WType_Popup );
  _dateFormat = p_df;
  _date->resize( _date->minimumSize() );
  frameWidth  = style().defaultFrameWidth();
  width       = _date->minimumSize().width();
  height      = fontMetrics().height()+2*frameWidth;
  connect( _date, SIGNAL(dateSet(const QDate&)), SLOT(datePicked(const QDate&)) );
  setMinimumSize( _date->minimumSize().width(), 25 );
  _contents   = style().comboButtonRect( 0,0,width,height );
  _arrow      = QRect( _contents.right(),_contents.top(),width-_contents.right(),_contents.height() );
}

kdbCalendar::~kdbCalendar(void)
{
}

void
kdbCalendar::bindField(kdbDataField *field_p)
{
	_field = field_p;
}

bool
kdbCalendar::event(QEvent *ev_p)
{
	bool rv;
	
	rv = QWidget::event( ev_p );
	if ( ev_p->type() == QEvent::Show && _field != 0 )
		setText( _field->text() );
	return rv;
}

void
kdbCalendar::datePicked(const QDate& p_date)
{
	QString str;
	
	if ( p_date.isValid() && p_date != _date->getDate() )
		_date->setDate( p_date );
	if ( _field )
		_field->set( _date->getDate() );
	popDown();
	repaint( false );
	emit dateChanged( p_date );
}

void
kdbCalendar::setText(const QString& str_p)
{
	if ( str_p == 0 )
		return;
	_date->setText( _dateFormat, str_p );
	repaint();
}

const QString&
kdbCalendar::text()
{
	_str = _date->format( _dateFormat );
  return _str;
}

void
kdbCalendar::paintEvent(QPaintEvent*)
{
  QPainter paint;
  QColorGroup g = colorGroup();
	
  paint.begin( this );

  if ( style() == MotifStyle ) {

  	uint arrowWH, arrowY, arrowX;
  	uint sepH, sepY;
  	
  	if ( height() < 6 ) {
  		arrowWH = height();
  		arrowY  = 0;
  	} else if ( height() < 18 ) {
  		arrowWH = height() - 6;
  		arrowY  = 0;
  	} else {
  		arrowWH = (height()*4)/10;
  		arrowY  = arrowWH/2;
  	}
  	
 		sepH = (arrowWH+3)/4;
		sepY = height() - arrowY - sepH;
		if ( sepH < 3 ) {
	    sepY = sepY+sepH-3;
	    sepH = 3;
		}
		if ( sepY - arrowY - arrowWH > 3 ) {
	    sepY   -= ( sepY-arrowY-arrowWH-3 )/2;
	    arrowY += ( sepY-arrowY-arrowWH-3 )/2;
		}
	  arrowX = width() - 3 - arrowWH;

		if ( arrowX + arrowWH + 2 < width() )
	    arrowX += ( width() - 2 - arrowX - arrowWH ) / 2;

	  style().drawComboButton( &paint,0,0,width(),height(),g,false,true );	
	  style().drawArrow( &paint,Qt::DownArrow,_arrowDown,arrowX,arrowY,arrowWH,arrowWH,g,true );
		
		paint.setPen( g.light() );
		paint.drawLine( arrowX, sepY, arrowX+arrowWH-1, sepY );
		paint.drawLine( arrowX, sepY, arrowX, sepY+sepH-1 );
		paint.setPen( g.dark() );
		paint.drawLine( arrowX+1, sepY+sepH-1, arrowX+arrowWH-1, sepY+sepH-1 );
		paint.drawLine( arrowX+arrowWH-1, sepY+1, arrowX+arrowWH-1, sepY+sepH-1 );

  } else
	  style().drawComboButton( &paint,0,0,width(),height(),g,_arrowDown,true );

  paint.fillRect( _contents,g.midlight() );
  paint.setPen( g.text() );
  paint.drawText( _contents,AlignCenter|AlignVCenter,_date->format() );
  paint.end();
}

void
kdbCalendar::popUp()
{
	if ( _date->isVisible() == false ) {
		QPoint pos = mapToGlobal( QPoint(0, height()) );
		_mouseWasInsidePopup = false;
		_date->installEventFilter( this );
		_date->move( pos.x(), pos.y() );
		_date->raise();
		_date->show();
	}
}

void
kdbCalendar::popDown()
{
	if ( _date->isVisible() ) {
		_date->removeEventFilter( this );
		_date->hide();
	}
}

bool
kdbCalendar::eventFilter( QObject *p_obj, QEvent *p_ev )
{
	if ( !p_ev )
		return true;
	else if ( p_obj == _date ) {
		QMouseEvent *me = (QMouseEvent *)p_ev;
		switch( p_ev->type() ) {
			case QEvent::MouseMove:
				if ( !_mouseWasInsidePopup ) {
					if ( _date->rect().contains(me->pos()) )
						_mouseWasInsidePopup = true;
					if ( _pushed ) {
						QPoint cpos = mapFromGlobal( _date->mapToGlobal(me->pos()) );
						if ( _arrow.contains(cpos) ) {
							_arrowDown = true;
							repaint();
						} else {
							_arrowDown = false;
							repaint();
						}
					}
				}
				break;
			case QEvent::MouseButtonRelease:
				if ( !_date->rect().contains(me->pos()) ) {
					if ( _mouseWasInsidePopup )
						popDown();
					else if ( _pushed ) {
						_pushed = _arrowDown = false;
						repaint( false );
					}
				}
				break;
			case QEvent::MouseButtonPress:
				if ( !_date->rect().contains( me->pos() ) ) {
					QPoint globalPos = _date->mapToGlobal( me->pos() );
					if ( QApplication::widgetAt(globalPos, true) == this )
						_discardPress = true;
					popDown();
					return true;
				}
			default:
				break;
		}
	}
	return false;
}

void
kdbCalendar::mousePressEvent(QMouseEvent* p_me)
{
	if (_discardPress) {
		_discardPress = false;
		return;
	}
	if ( _arrow.contains(p_me->pos()) ) {
		_pushed =_arrowDown = true;
	  if ( _date->isVisible() )
  		popDown();
	  else if ( _arrow.contains(p_me->pos()) )
  		popUp();
  	repaint( false );
  }
}

void
kdbCalendar::mouseReleaseEvent(QMouseEvent*)
{
	if ( _pushed ) {
		_pushed = _arrowDown = false;
		repaint( false );
	}
}

void
kdbCalendar::focusOutEvent(QFocusEvent*)
{
	if ( _date->isVisible() )
		popDown();
}

void
kdbCalendar::resizeEvent(QResizeEvent*)
{
	uint frameWidth;
	
	_contents  = style().comboButtonFocusRect( 0,0,width(),height() );
	frameWidth = style().defaultFrameWidth();
  _arrow     = QRect( _contents.right(),_contents.top(),width()-_contents.right()-frameWidth,_contents.height() );
}

#include "kdbcalendar.moc"

