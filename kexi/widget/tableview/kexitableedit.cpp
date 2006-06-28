/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexitableedit.h"
#include <kexidb/field.h>
#include <kexidb/utils.h>

#include <qpalette.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

//KexiTableEdit::KexiTableEdit(KexiDB::Field &f, QScrollView* parent, const char* name)
KexiTableEdit::KexiTableEdit(KexiTableViewColumn &column, Q3ScrollView* parent, const char* name)
: QWidget(parent->viewport(), name)
, m_column(&column)
// ,m_field(&f)
// ,m_type(f.type()) //copied because the rest of code uses m_type
 ,m_scrollView(parent)
 ,m_view(0)
// ,m_hasFocusableWidget(true)
// ,m_acceptEditorAfterDeleteContents(false)
{
	setPaletteBackgroundColor( palette().color(QPalette::Active, QColorGroup::Base) );
	installEventFilter(this);

	//margins
	if (m_column->field()->isFPNumericType()) {
#ifdef Q_WS_WIN
		m_leftMargin = 0;
#else
		m_leftMargin = 0;
#endif
	}
	else if (m_column->field()->isIntegerType()) {
#ifdef Q_WS_WIN
		m_leftMargin = 1;
#else
		m_leftMargin = 0;
#endif
	}
	else {//default
#ifdef Q_WS_WIN
		m_leftMargin = 5;
#else
		m_leftMargin = 5;
#endif
	}


	m_rightMargin = 0;//TODO
}

KexiTableEdit::~KexiTableEdit()
{
}

void KexiTableEdit::setViewWidget(QWidget *v)
{
	m_view = v;
	m_view->move(0,0);
	m_view->installEventFilter(this);
	setFocusProxy(m_view);
}

void KexiTableEdit::resize(int w, int h)
{
	QWidget::resize(w, h);
	if (m_view) {
		if (!layout()) { //if there is layout (eg. KexiInputTableEdit), resize is automatic
			m_view->move(0,0);
			m_view->resize(w, h);
		}
	}
}

bool
KexiTableEdit::eventFilter(QObject* watched, QEvent* e)
{
/*	if (watched == m_view) {
		if(e->type() == QEvent::KeyPress) {
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);
//			if (ev->key()==Qt::Key_Tab) {

//			}
		}
	}*/

	if(watched == this)
	{
		if(e->type() == QEvent::KeyPress)
		{
			QKeyEvent* ev = static_cast<QKeyEvent*>(e);

			if(ev->key() == Qt::Key_Escape)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
//	return QWidget::eventFilter(watched, e);
}

void KexiTableEdit::paintFocusBorders( QPainter *p, QVariant &, int x, int y, int w, int h )
{
	p->drawRect(x, y, w, h);
}

void KexiTableEdit::setupContents( QPainter * /*p*/, bool /*focused*/, const QVariant& val, 
	QString &txt, int &align, int &/*x*/, int &y_offset, int &w, int &/*h*/  )
{
#ifdef Q_WS_WIN
//	x = 1;
	y_offset = -1;
#else
//	x = 1;
	y_offset = 0;
#endif

//	const int ctype = columnType(col);
//	align = SingleLine | AlignVCenter;
//	QString txt; //text to draw
	if (m_column->field()->isFPNumericType()) {
//js TODO: ADD OPTION to displaying NULL VALUES as e.g. "(null)"
		if (!val.isNull()) {
			txt = KexiDB::formatNumberForVisibleDecimalPlaces(
				val.toDouble(), m_column->field()->visibleDecimalPlaces());
			//txt = KGlobal::locale()->formatNumber(val.toDouble(), 10);
		}
		w -= 6;
		align |= Qt::AlignRight;
	}
	else if (m_column->field()->isIntegerType()) {
		qint64 num = val.toLongLong();
/*#ifdef Q_WS_WIN
		x = 1;
#else
		x = 0;
#endif*/
		w -= 6;
		align |= Qt::AlignRight;
		if (!val.isNull())
			txt = QString::number(num);
	}
/*MOVE TO Date editor!!!!
else if (m_field->type() == KexiDB::Field::Date) { //todo: datetime & time
#ifdef Q_WS_WIN
		x = 5;
#else
		x = 5;
#endif
		if(cell_value.toDate().isValid())
		{
#ifdef USE_KDE
			txt = KGlobal::locale()->formatDate(cell_value.toDate(), true);
#else
			if (!cell_value.isNull())
				txt = cell_value.toDate().toString(Qt::LocalDate);
#endif
		}
		align |= AlignLeft;
	}*/
	else {//default:
/*#ifdef Q_WS_WIN
		x = 5;
//		y_offset = -1;
#else
		x = 5;
//		y_offset = 0;
#endif*/
//		switch (m_column->field()->type()) {
/*moved		case KexiDB::Field::Time:
			//it was QDateTime - a hack needed because QVariant(QTime) has broken isNull()
			if (!val.isNull()) {
				txt = val.toTime().toString(Qt::LocalDate);
			}
			break;
		case KexiDB::Field::Date:
			if (val.toDate().isValid())
				txt = val.toDate().toString(Qt::LocalDate);
			break;
		case KexiDB::Field::DateTime:
			if (!val.toDateTime().isNull())
				txt = val.toDate().toString(Qt::LocalDate) + " " + val.toTime().toString(Qt::LocalDate);
			break;*/
//		default:
		if (!val.isNull()) {
			txt = val.toString();
		}
//		}
		align |= Qt::AlignLeft;
	}
}

void KexiTableEdit::paintSelectionBackground( QPainter *p, bool /*focused*/, 
	const QString& txt, int align, int x, int y_offset, int w, int h, const QColor& fillColor,
	bool readOnly, bool fullRowSelection )
{
	if (!readOnly && !fullRowSelection && !txt.isEmpty()) {
		QRect bound=fontMetrics().boundingRect(x, y_offset, w - (x+x), h, align, txt);
		bound.setY(0);
		bound.setWidth( qMin( bound.width()+2, w - (x+x)+1 ) );
		if (align & Qt::AlignLeft) {
			bound.setX(bound.x()-1);
		}
		else if (align & Qt::AlignRight) {
			bound.moveLeft( w - bound.width() ); //move to left, if too wide
		}
//TODO align center
		bound.setHeight(h-1);
		p->fillRect(bound, fillColor);
	}
	else if (fullRowSelection) {
		p->fillRect(0, 0, w, h, fillColor);
	}
}

int KexiTableEdit::widthForValue( QVariant &val, QFontMetrics &fm )
{
	return fm.width( val.toString() );
}


#include "kexitableedit.moc"
