/***************************************************************************
                          kdbpickdate.cpp  -  description                              
                             -------------------                                         
    begin                : Fri Jun 25 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include <iostream.h>

#include <qframe.h>
#include <kdbpickdate.h>

extern "C" {

#include <langinfo.h>
#include <time.h>

};

#define LI_WDAY(n)         nl_langinfo((nl_item)(ABDAY_1 + n))
#define LI_MDAY(n)         nl_langinfo((nl_item)(ABMON_1 + n))
#define LI_WEEKDAY(n)      nl_langinfo((nl_item)(DAY_1 + n))
#define LI_MONTH(n)        nl_langinfo((nl_item)(MON_1 + n))

#define LI_DATE_FMT        nl_langinfo(D_FMT)
#define LI_TIME_FMT        nl_langinfo(T_FMT)
#define LI_DATE_TIME       nl_langinfo(D_T_FMT)

kdbPickDate::kdbPickDate(QWidget *p_par, const char *p_nam, WFlags p_f)
 : QWidget(p_par, p_nam, p_f), _small("helvetica",8), _headr("helvetica",10)
{
	QFontMetrics sf(_small);
	QFontMetrics hf(_headr);
		
	_headr.setBold(true);
	_date = QDate::currentDate();
	_rpushed = _lpushed = false;
	setFocusPolicy( QWidget::StrongFocus );
	setMinimumSize( 28*sf.width('x'), 2*hf.height()+8*sf.height() );
	setBackgroundMode( PaletteBase );
	resize( minimumSize() );
}

kdbPickDate::kdbPickDate(const QDate& p_date)
 : QWidget(0,0,0), _small("helvetica",8), _headr("helvetica",10)
{
	QFontMetrics sf(_small);
	QFontMetrics hf(_headr);
		
	_headr.setBold(true);
	_date = p_date;
	_rpushed = _lpushed = false;
	setFocusPolicy( QWidget::StrongFocus );
	setMinimumSize( 28*sf.width('x'), 2*hf.height()+8*sf.height() );
	setBackgroundMode( PaletteBase );
	resize( minimumSize() );
}

kdbPickDate::~kdbPickDate()
{
}

QDate
kdbPickDate::getDate() const
{
	return _date;
}


void
kdbPickDate::setDate(const QDate& p_date)
{
	if (p_date.isValid())
		_date = p_date;
}

void
kdbPickDate::setDate(uint p_year, uint p_month, uint p_day)
{
	QDate tmp(p_year, p_month, p_day);
	
	setDate(tmp);
}

QString
kdbPickDate::format()
{
	QString str = LI_DATE_FMT;
	
	return format(str);
}

QString
kdbPickDate::format(const QString& p_fmt)
{
	QString str = p_fmt;
	int pos;
	uint off;
	QChar kind;
	
	while((pos = str.find('%')) >= 0) {
		kind = str[pos+1];
		str.remove( pos,2 );
		switch(kind) {
			case 'y':
				str.insert(pos, QString("%1").arg(_date.year()%100));
				break;
			case 'Y':
				str.insert(pos, QString("%1").arg(_date.year()));
				break;
			case 'a':
				str.insert(pos, QString("%1").arg(LI_WDAY(_date.dayOfWeek()%7)));
				break;
			case 'A':
				str.insert(pos, QString("%1").arg(LI_WEEKDAY(_date.dayOfWeek()%7)));
				break;
			case 'b':
			case 'h':
				str.insert(pos, QString("%1").arg(LI_MDAY(_date.month()-1)));
				break;
			case 'B':
				str.insert(pos, QString("%1").arg(LI_MONTH(_date.month()-1)));
				break;
			case 'e':
				str.insert(pos, QString("%1").arg(_date.day()));
				break;
			case 'd':
				str.insert(pos, QString("").sprintf("%02d",_date.day()));
				break;
			case 'm':
				str.insert(pos, QString("").sprintf("%02d", _date.month()));
				break;
			case 'j':
				str.insert(pos, QString("%1").arg(_date.dayOfYear()));
				break;
			case 'W':
				off = QDate(_date.year(),1,1).dayOfWeek()-1;
				str.insert(pos, QString("%1").arg((off+_date.dayOfYear())/7));
				break;
			case 'U':
				off = QDate(_date.year(),1,1).dayOfWeek()%7;
				str.insert(pos, QString("%1").arg((off+_date.dayOfYear())/7));
				break;
			case 'w':
				str.insert(pos, QString("%1").arg(_date.dayOfWeek()%7));
				break;
			default:
				cerr << "Unknown format character " << kind << "\n";
		}
	}
	return str;
}

void
kdbPickDate::setText(const QString& p_format, const QString& p_txt)
{
	uint day = _date.day(), month = _date.month(), year = _date.year();
	uint pos, tpos;
	
	for( tpos=pos=0;pos<p_format.length();pos++,tpos++ ) {
		if ( tpos>=p_txt.length() )
			return;
		if ( p_format[pos] == '%' && pos+1 < p_format.length() ) {
			switch( p_format[++pos] ) {
				case 'd':
					day = p_txt.mid( tpos++,2 ).toInt();
					break;
				case 'e':
					if ( p_txt[tpos+1].isDigit() )
						day = p_txt.mid( tpos++,2 ).toInt();
					else
						day = p_txt.mid( tpos,1 ).toInt();
					break;
				case 'y':
					year = p_txt.mid( tpos++,2 ).toInt();
					break;
				case 'Y':
					year = p_txt.mid( tpos,4 ).toInt();
					tpos += 3;
					break;
				case 'm':
					month = p_txt.mid( tpos++,2 ).toInt();
					break;
			}
		}
	}
	setDate( year,month,day );
}

QSize
kdbPickDate::sizeHint() const
{
	return QSize(width(),height());
}

QSizePolicy
kdbPickDate::sizePolicy() const
{
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void
kdbPickDate::paintEvent(QPaintEvent *p_ev)
{
	QString str;
	QPainter paint;
	uint i,w,off,frameWidth;
	QBrush fill = colorGroup().button();
		
	QFontMetrics fm(_small);
	QFontMetrics fmh(_headr);
	paint.begin(this);
	paint.setPen(colorGroup().text());
	paint.setFont(_headr);
	str = QString("%1 %2").arg(LI_MONTH(_date.month()-1)).arg(_date.year());
	frameWidth = style().defaultFrameWidth();
	style().drawRect(&paint,0,fmh.height()+10,width(),height()-fmh.height()-10,colorGroup().midlight());
	style().drawPanel(&paint,0,0,width(),fmh.height()+10,colorGroup(),false,1,&fill);
	style().drawArrow(&paint,Qt::RightArrow,_rpushed,_r.left(),_r.top(),_r.width(),_r.height(),colorGroup(),true);
	style().drawArrow(&paint,Qt::LeftArrow,_lpushed,_l.left(),_l.top(),_l.width(),_l.height(),colorGroup(),true);
	paint.drawText(_l.right(),frameWidth,_r.left()-_l.right(),fmh.height()+10-2*frameWidth, AlignVCenter|AlignCenter, str);
	paint.setFont(_small);
	paint.setPen(colorGroup().brightText());
	w = _contents.width()/7;
	for(i=0;i<7;i++)
		paint.drawText(_contents.x()+(i*w), _contents.y()-fm.height()-4, w, fm.height(), AlignVCenter|AlignRight, LI_WDAY(i));
	paint.setPen(colorGroup().text());
	off = QDate(_date.year(),_date.month(),1).dayOfWeek()%7;
	for(i=0;i<(uint)_date.daysInMonth();i++)
		drawCell(paint, i+1, off, false);
	paint.end();
}

void
kdbPickDate::drawCell(QPainter& p_paint, uint p_day, uint p_off, bool p_halo)
{
	QFontMetrics hf(_headr);
	QFontMetrics fm = p_paint.fontMetrics();
	QString str;
	QRect r;
	uint x, y, w;
	
	w = _contents.width()/7;
	x = ((p_day+p_off-1)%7)*w;
	y = ((p_day+p_off-1)/7)*fm.height();
	str.sprintf("%d", p_day);
	r = p_paint.boundingRect(_contents.x()+x,_contents.y()+y,w,fm.height(),AlignVCenter|AlignRight,str);
	p_paint.setPen(colorGroup().text());
	p_paint.eraseRect(r);
	p_paint.drawText(r,AlignLeft,str);
	p_paint.setRasterOp(Qt::XorROP);
	if (p_day == (uint)_date.day())
		p_paint.fillRect(r,colorGroup().brightText());
	p_paint.setRasterOp(Qt::CopyROP);
	if (p_halo)
		p_paint.drawRect(r);
}

void
kdbPickDate::drawArrows(Qt::ArrowType p_arrow, bool p_state)
{
	QPainter paint;
	
	paint.begin(this);
	if (p_arrow == Qt::RightArrow)
		style().drawArrow(&paint,p_arrow,_rpushed=p_state,_r.left(),_r.top(),_r.width(),_r.height(),colorGroup(),true);
	else
		style().drawArrow(&paint,p_arrow,_lpushed=p_state,_l.left(),_l.top(),_l.width(),_l.height(),colorGroup(),true);
	paint.end();
}

void
kdbPickDate::markPos(QPoint p_pos, bool p_draw)
{
	QFontMetrics sf(_small);
	int i,off;
	
	i   = p_pos.y()*7 + p_pos.x();
	off = QDate(_date.year(),_date.month(),1).dayOfWeek()%7;
	if (i-off >= 0 && i-off < _date.daysInMonth()) {
		QPainter paint;
		paint.begin(this);
		paint.setFont(_small);
		drawCell(paint, i-off+1, off, p_draw);
		paint.end();
	}
}

void
kdbPickDate::mousePressEvent(QMouseEvent *p_mev)
{
	if (_r.contains(p_mev->pos())) {
		if (_date.month() < 12)
			_date.setYMD(_date.year(),_date.month()+1,_date.day());
		else
			_date.setYMD(_date.year()+1,1,_date.day());
		_rpushed = true;
	} else if (_l.contains(p_mev->pos())) {
		if (_date.month() > 1)
			_date.setYMD(_date.year(),_date.month()-1,_date.day());
		else
			_date.setYMD(_date.year()-1,12,_date.day());
		_lpushed = true;
	} else if (_contents.contains(p_mev->pos())) {
		uint day = (_pos.y()*7)+_pos.x() - QDate(_date.year(),_date.month(),1).dayOfWeek()%7;
		if (day+1 > 0 && day < (uint)_date.daysInMonth()) {
			_date.setYMD( _date.year(),_date.month(), day+1 );
			emit dateSet( _date );
		}
	} else
		return;
	repaint();
}

void
kdbPickDate::mouseReleaseEvent(QMouseEvent *p_mev)
{
	if (_r.contains(p_mev->pos()))
		drawArrows(Qt::RightArrow,false);
	else if (_l.contains(p_mev->pos()))
		drawArrows(Qt::LeftArrow,false);
}

void
kdbPickDate::mouseMoveEvent(QMouseEvent *p_ev)
{
	QPoint pos;
	uint x,y;
	
	if (_contents.contains(p_ev->pos())) {
		x = (p_ev->pos().x()-_contents.x())/(_contents.width()/7);
		y = (p_ev->pos().y()-_contents.y())/QFontMetrics(_small).height();
		pos = QPoint(x,y);
		if (_pos != pos) {
			markPos(_pos, false);
			markPos(_pos = pos, true);
		}
	}
	if (_rpushed && !_r.contains(p_ev->pos()))
		drawArrows(Qt::RightArrow, false);
	if (_lpushed && !_l.contains(p_ev->pos()))
		drawArrows(Qt::LeftArrow, false);
}

void
kdbPickDate::focusInEvent(QFocusEvent *p_ev)
{
	setMouseTracking(true);
	repaint();
}

void
kdbPickDate::resizeEvent(QResizeEvent *p_ev)
{
	QFontMetrics sf(_small);
	QFontMetrics hf(_headr);
	uint frameWidth = style().defaultFrameWidth();
	uint arrowSize,arrowYOffset;
	uint w,y;
	
	w = width()/8;
	y = 2*hf.height()+sf.height()+10;
	arrowSize    = hf.height()+10 - 2*frameWidth;
	arrowYOffset = arrowSize/4 + 1;
	arrowSize    = arrowSize - arrowYOffset;
	_l = QRect(2+frameWidth,frameWidth+arrowYOffset/2,arrowSize,arrowSize);
	_r = QRect(width()-_l.width()-2,_l.y(),_l.width(),_l.height());
	_contents = QRect(w/2,y,7*w,5*sf.height());
}

void
kdbPickDate::focusOutEvent(QFocusEvent *p_ev)
{
	setMouseTracking(false);
	repaint();
}

#include "kdbpickdate.moc"

