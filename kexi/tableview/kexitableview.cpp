/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (c) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include <qpainter.h>
#include <qkeycode.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwmatrix.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qstyle.h>

#include <config.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapp.h>
#include <kiconloader.h>

#ifndef KEXI_NO_PRINT
# include <kprinter.h>
# include <kdeprint/driver.h>
#endif

#include "kexitablerm.h"
#include "kexitableview.h"

#ifdef USE_KDE
#include "kexidatetableedit.h"
#endif

#include "kexitableedit.h"
#include "kexiinputtableedit.h"
#include "kexicomboboxtableedit.h"
#include "kexiblobtableedit.h"

#include "kexitableview_p.h"

KexiTableView::KexiTableView(QWidget *parent, const char *name, KexiTableList *contents)
:QScrollView(parent, name, /*Qt::WRepaintNoErase | */Qt::WStaticContents /*| Qt::WResizeNoErase*/)
//:QScrollView(parent, name, Qt::WRepaintNoErase | Qt::WStaticContents | Qt::WResizeNoErase)
	,m_contents(0)
	,d( new KexiTableViewPrivate() )
{
	setResizePolicy(Manual);
	viewport()->setBackgroundMode(NoBackground);
	viewport()->setFocusPolicy(QWidget::StrongFocus);

	//setup colors defaults
	setBackgroundMode(PaletteBackground);
	setEmptyAreaColor(palette().active().color(QColorGroup::Base));

	setLineWidth(1);
	horizontalScrollBar()->raise();
	verticalScrollBar()->raise();

	//context menu
	d->pContextMenu = new QPopupMenu(this);
	d->menu_id_addRecord = d->pContextMenu->insertItem(i18n("Add Record"), this, SLOT(addRecord()), CTRL+Key_Insert);
	d->menu_id_removeRecord = d->pContextMenu->insertItem(kapp->iconLoader()->loadIcon("button_cancel", KIcon::Small), i18n("Remove Record"), this, SLOT(removeRecord()), CTRL+Key_Delete);

#ifdef Q_WS_WIN
	d->rowHeight = fontMetrics().lineSpacing() + 4;
#else
	d->rowHeight = fontMetrics().lineSpacing() + 1;
#endif

	if(d->rowHeight < 17)
		d->rowHeight = 17;

	d->pUpdateTimer = new QTimer(this);

//	d->pColumnTypes = new QMemArray<QVariant::Type>;
//	d->pColumnModes = new QMemArray<int>;
//	d->pColumnDefaults = new QPtrList<QVariant>;
//	deletionPolicy = NoDelete;
	//m_additionPolicy = NoAdd;
	setAdditionPolicy( NoAdd );

	setMargins(14, fontMetrics().height() + 4, 0, 0);

	// Create headers
	d->pTopHeader = new QHeader(0, this);
//	d->pTopHeader = new QHeader(d->numCols, this);
	d->pTopHeader->setOrientation(Horizontal);
	d->pTopHeader->setTracking(false);
	d->pTopHeader->setMovingEnabled(false);

	d->pVerticalHeader = new KexiTableRM(this);
	d->pVerticalHeader->setCellHeight(d->rowHeight);
	d->pVerticalHeader->setCurrentRow(-1);

/*	d->pVerticalHeader = new KexiTableHeader(this);
	
	d->pVerticalHeader->setOrientation(Vertical);
	d->pVerticalHeader->setTracking(false);
	d->pVerticalHeader->setMovingEnabled(false);
	d->pVerticalHeader->setStretchEnabled(false);
	d->pVerticalHeader->setResizeEnabled(false);
	d->pVerticalHeader->setCurrentRow(false);
*/

//	enableClipper(true);
	if(!contents)
	{
		m_contents = new KexiTableList();
	}
	else
	{
		kdDebug() << "KexiTableView::KexiTableView(): using shared contents" << endl;
		m_contents = contents;
		d->numRows = contents->count();
		triggerUpdate();
		KexiTableItem *it;
		for(it = m_contents->first(); it; it = m_contents->next())
		{
			d->pVerticalHeader->addLabel();
/*			if(!it->isInsertItem())
				d->pVerticalHeader->addLabel("",  d->rowHeight);
			else
				d->pVerticalHeader->addLabel("*",  d->rowHeight);*/
		}
	}
	m_contents->setAutoDelete(true);

	d->scrollTimer = new QTimer(this);
	connect(d->scrollTimer, SIGNAL(timeout()), this, SLOT(slotAutoScroll()));

	setBackgroundAltering(true);

	// Connect header, table and scrollbars
	connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), d->pTopHeader, SLOT(setOffset(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)),	d->pVerticalHeader, SLOT(setOffset(int)));
	connect(d->pTopHeader, SIGNAL(sizeChange(int, int, int)), this, SLOT(columnWidthChanged(int, int, int)));
	connect(d->pTopHeader, SIGNAL(clicked(int)), this, SLOT(columnSort(int)));

	connect(d->pUpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
}

void KexiTableView::addDropFilter(const QString &filter)
{
	d->dropFilters.append(filter);
}

void KexiTableView::addColumn(QString name, QVariant::Type type, bool editable, QVariant defaultValue,
 int width, bool autoinc)
{
	d->numCols++;
	d->pColumnTypes.resize(d->numCols);
	d->pColumnModes.resize(d->numCols);

	d->pColumnTypes.at(d->numCols-1) = type;

	if(editable && autoinc)
	{
		d->pColumnModes.at(d->numCols-1) = ColumnEditable | ColumnAutoIncrement;
	}
	else if(editable)
	{
		d->pColumnModes.at(d->numCols-1) = ColumnEditable;
	}
	else
	{
		d->pColumnModes.at(d->numCols-1) = ColumnReadOnly;
	}

	d->pColumnDefaults.append(new QVariant(defaultValue));
	d->pTopHeader->addLabel(name, width);
	d->pTopHeader->setUpdatesEnabled(true);
}


void KexiTableView::setFont(const QFont &f)
{
	QWidget::setFont(f);
#ifdef Q_WS_WIN
  d->rowHeight = fontMetrics().lineSpacing() + 4;
#else
  d->rowHeight = fontMetrics().lineSpacing() + 2;
#endif
	if(d->rowHeight < 22)
		d->rowHeight = 22;
	setMargins(14, d->rowHeight, 0, 0);
	d->pVerticalHeader->setCellHeight(d->rowHeight);
}

void KexiTableView::remove(KexiTableItem *item, bool moveCursor/*=true*/)
{
	if(m_contents->removeRef(item))
	{
		d->pVerticalHeader->removeLabel();
		d->numRows--;
		if(moveCursor)
			setCursor(d->curRow);
//		selectPrev();
		d->pUpdateTimer->start(1,true);
	}
}

void KexiTableView::removeRecord()
{
	if (d->deletionPolicy == NoDelete)
    return;
  if (d->deletionPolicy == ImmediateDelete && d->pCurrentItem) {
    remove(d->pCurrentItem);
  } else if (d->deletionPolicy == AskDelete) {
    //TODO(js)
  } else if (d->deletionPolicy == SignalDelete) {
  	emit itemRemoveRequest(d->pCurrentItem);
	  emit currentItemRemoveRequest();
  }
}

void KexiTableView::addRecord()
{
	emit addRecordRequest();
}

void KexiTableView::clear()
{
	for(int i=0; i < rows(); i++)
	{
		d->pVerticalHeader->removeLabel();
	}

	editorCancel();
	m_contents->clear();

	d->clearVariables();

	d->pVerticalHeader->setCurrentRow(-1);
//	d->pUpdateTimer->start(1,true);
	viewport()->repaint();
}

void KexiTableView::clearAll()
{
	for(int i=0; i < rows(); i++)
	{
		d->pVerticalHeader->removeLabel();
	}

	editorCancel();
	m_contents->clear();

	d->clearVariables();
	d->numCols = 0;

	while(d->pTopHeader->count())
		d->pTopHeader->removeLabel(0);

	d->pVerticalHeader->setCurrentRow(-1);

	viewport()->repaint();

	d->pColumnTypes.resize(0);
	d->pColumnModes.resize(0);
	d->pColumnDefaults.clear();
}

int KexiTableView::findString(const QString &string)
{
	int row = 0;
	int col = sorting();
	if(col == -1)
		return -1;
	if(string.isEmpty())
	{
		setCursor(0, col);
		return 0;
	}

	QPtrListIterator<KexiTableItem> it(*m_contents);

	if(string.at(0) != QChar('*'))
	{
		switch(columnType(col))
		{
			case QVariant::String:
			{
				QString str2 = string.lower();
				for(; it.current(); ++it)
				{
					if(it.current()->getText(col).left(string.length()).lower().compare(str2)==0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;
			}
			case QVariant::Int:
			case QVariant::Bool:
				for(; it.current(); ++it)
				{
					if(QString::number(it.current()->getInt(col)).left(string.length()).compare(string)==0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;

			default:
				break;
		}
	}
	else
	{
		QString str2 = string.mid(1);
		switch(columnType(col))
		{
			case QVariant::String:
				for(; it.current(); ++it)
				{
					if(it.current()->getText(col).find(str2,0,false) >= 0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;
			case QVariant::Int:
			case QVariant::Bool:
				for(; it.current(); ++it)
				{
					if(QString::number(it.current()->getInt(col)).find(str2,0,true) >= 0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;

			default:
				break;
		}
	}
	return -1;
}


KexiTableView::~KexiTableView()
{
	editorCancel();

//	delete d->pColumnTypes;
//	delete d->pColumnModes;
//	delete d->pColumnDefaults;
	delete d->pBufferPm;
}
/*
void KexiTableView::setColumn(int col, QString name, ColumnTyFpe type, bool changeable=false)
{
	(d->pColumnTypes->at(col)) = type;
	d->pTopHeader->setLabel(col, name);
	(d->pColumnModes->at(col)) = changeable;
}
*/
void KexiTableView::setSorting(int col, bool ascending/*=true*/)
{
//	d->sortOrder = ascending;
	d->sortedColumn = col;
	d->pTopHeader->setSortIndicator(col, d->sortOrder);
	m_contents->setSorting(col, d->sortOrder, columnType(col));
//	sort();
	columnSort(col);
}

void KexiTableView::slotUpdate()
{
//	static int count=1;
	// qDebug("%s: slotUpdate() - %04d", name(), count++);
	QSize s(tableSize());
//	viewport()->setUpdatesEnabled(false);
	resizeContents(s.width(), s.height());
//	viewport()->setUpdatesEnabled(true);
	updateContents(0, 0, viewport()->width(), contentsHeight());
	updateGeometries();
}

int KexiTableView::sorting()
{
	return d->sortedColumn;
}

void KexiTableView::sort()
{
	if(rows() < 2)
	{
		return;
	}


	m_contents->sort();

	d->curRow = m_contents->findRef(d->pCurrentItem);

	d->pCurrentItem = m_contents->at(d->curRow);

	int cw = columnWidth(d->curCol);
	int rh = rowHeight();

//	d->pVerticalHeader->setCurrentRow(d->curRow);
	center(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + rh / 2);
//	updateCell(oldRow, d->curCol);
//	updateCell(d->curRow, d->curCol);
	d->pVerticalHeader->setCurrentRow(d->curRow);
//	slotUpdate();
	d->pUpdateTimer->start(1,true);
}

QSizePolicy KexiTableView::sizePolicy() const
{
	// this widget is expandable
	return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize KexiTableView::sizeHint() const
{
	return QSize(tableSize().width(),
		QMAX(d->rowHeight*d->numRows + d->pTopHeader->height(), minimumSizeHint().height()) );
}

QSize KexiTableView::minimumSizeHint() const
{
	return QSize(columnWidth(1), d->rowHeight*3 + d->pTopHeader->height());
}

void KexiTableView::createBuffer(int width, int height)
{
	if(!d->pBufferPm)
		d->pBufferPm = new QPixmap(width, height);
	else
		if(d->pBufferPm->width() < width || d->pBufferPm->height() < height)
			d->pBufferPm->resize(width, height);
//	d->pBufferPm->fill();
}

void KexiTableView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
	int colfirst = columnAt(cx);
	int rowfirst = rowAt(cy);
	int collast = columnAt(cx + cw-1);
	int rowlast = rowAt(cy + ch-1);

 	if (rowlast == -1)
		rowlast = rows() - 1;
	if ( collast == -1 )
		collast = cols() - 1;

// 	qDebug("cx:%3d cy:%3d w:%3d h:%3d col:%2d..%2d row:%2d..%2d tsize:%4d,%4d", 
//    cx, cy, cw, ch, colfirst, collast, rowfirst, rowlast, tableSize().width(), tableSize().height());

	if (rowfirst == -1 || colfirst == -1)
	{
//	updateContents(0,0,1000,1000);
		paintEmptyArea(p, cx, cy, cw, ch);
		return;
	}

	createBuffer(cw, ch);
	if(d->pBufferPm->isNull())
		return;
	QPainter *pb = new QPainter(d->pBufferPm, this);

	int r;
//	pb->fillRect(0, 0, cw, ch, colorGroup().base());

	QPtrListIterator<KexiTableItem> it(*m_contents);
	it += rowfirst;

//	int maxwc = QMIN(cw, (columnPos(d->numCols - 1) + columnWidth(d->numCols - 1)));
	int maxwc = columnPos(cols() - 1) + columnWidth(cols() - 1);
//	kdDebug() << "KexiTableView::drawContents(): maxwc: " << maxwc << endl;

// 	pb->fillRect(maxwc, 0, cw - maxwc, ch, colorGroup().base());
// 	pb->fillRect(0, rowPos(rowlast) + d->rowHeight, cw, ch, colorGroup().base());
	pb->fillRect(cx, cy, cw, ch, colorGroup().base());
	for(r = rowfirst; r <= rowlast; r++, ++it)
	{
		// get row position and height
		int rowp = rowPos(r);

		// Go through the columns in the row r
		// if we know from where to where, go through [colfirst, collast],
		// else go through all of them
		int colp, colw;
		colp = 0;
		int transly = rowp-cy;
		int c;

		if(d->bgAltering && r%2 != 0)
			pb->fillRect(0, transly, maxwc, d->rowHeight - 1, KGlobalSettings::alternateBackgroundColor());
		else
			pb->fillRect(0, transly, maxwc, d->rowHeight - 1, colorGroup().base());

		for(c = colfirst; c <= collast; c++)
		{
			// get position and width of column c
			colp = columnPos(c);
			colw = columnWidth(c);
			int translx = colp-cx;

			// Translate painter and draw the cell
			pb->saveWorldMatrix();
			pb->translate(translx, transly);
//	    	paintCell( pb, r, c, QRect(columnPos(c), rowPos(r), colw, d->rowHeight));
		    	paintCell( pb, it.current(), c, QRect(columnPos(c), rowPos(r), colw, d->rowHeight));
			pb->restoreWorldMatrix();
		}

	}

	//draw current cell
/*	int colp, colw, rowp;
	rowp = rowPos(d->curRow);
	colp = columnPos(d->curCol);
	colw = columnWidth(d->curCol);
	int translx = colp-cx;
	int transly = rowp-cy;
	// Translate painter and draw the cell
	pb->saveWorldMatrix();
	pb->translate(translx, transly);
		paintCell( pb, d->pCurrentItem, d->curCol, QRect(colp, rowp, colw, d->rowHeight));
	pb->restoreWorldMatrix();
*/

	delete pb;

	p->drawPixmap(cx,cy,*d->pBufferPm, 0,0,cw,ch);

  //(js)
	paintEmptyArea(p, cx, cy, cw, ch);
}

void KexiTableView::paintCell(QPainter* p, KexiTableItem *item, int col, const QRect &cr, bool print)
{
	int w = cr.width();
	int h = cr.height();
	int x2 = w - 1;
	int y2 = h - 1;

//	p->setPen(colorGroup().button());

/*	if(d->bgAltering && !print && d->contents->findRef(item)%2 != 0)
	{
		QPen originalPen(p->pen());
		QBrush originalBrush(p->brush());

		p->setBrush(KGlobalSettings::alternateBackgroundColor());
		p->setPen(KGlobalSettings::alternateBackgroundColor());
		p->drawRect(0, 0, x2, y2);

		p->setPen(originalPen);
		p->setBrush(originalBrush);
	}
*/
	//	Draw our lines
	QPen pen(p->pen());
//	if(!print)
		p->setPen(QColor(200,200,200));
//	else
//		p->setPen(black);

//	p->drawRect( -1, -1, w+1, h+1 );
//	p->drawRect( 0, 0, w, h );

	p->drawLine( x2, 0, x2, y2 );	// right
	p->drawLine( 0, y2, x2, y2 );	// bottom
	p->setPen(pen);

	//	If we are in the focus cell, draw indication
	if(d->pCurrentItem == item && col == d->curCol && !d->recordIndicator)
	{
		if (hasFocus() || viewport()->hasFocus()) {
			p->drawRect(0, 0, x2, y2);
//			p->drawRect(-1, -1, w+1, h+1);
		}
	}

/*	if(d->pCurrentItem == item && d->recordIndicator)
	{
		p->setBrush(colorGroup().highlight());
		p->setPen(colorGroup().highlight());
		p->drawRect(0, 0, x2, y2);
	}
*/

//	int iOffset = 0;
	QPen fg(colorGroup().text());
	p->setPen(fg);
	if(item->isInsertItem())
	{
//		QFont f = p->font();
//		f.setBold(true);
//		p->setFont(f);
//		p->setPen(QPen(colorGroup().mid()));
		p->setPen(QColor(190,190,190));
//		iOffset = 3;
	}
#ifdef Q_WS_WIN
	int x = 1;
	int y_offset = 1;
#else
	int x = 1;
//	int y_offset = 2;
	int y_offset = 1;
#endif

	switch(columnType(col))
	{
		case QVariant::UInt:
		case QVariant::Int:
		{
			int num = item->getValue(col).toInt();
//			if(num < 0)
//				p->setPen(red);
//			p->drawText(x - (x+x) - 2, 2, w, h, AlignRight, QString::number(num));
//			qDebug("KexiTableView::paintCell(): mode: %i", d->pColumnModes->at(col));
#ifdef Q_WS_WIN
//			y_offset = 1;
			x = 1;
#else
			x = 0;
#endif
			if(item->isInsertItem() && d->pColumnModes.at(col) == 3)  //yes that isn't beautiful
			{
				p->drawText(x, y_offset, w - (x+x) - 6, h, AlignRight, "[Auto]");
			}
			else
			{
				p->drawText(x, y_offset, w - (x+x) - 6, h, AlignRight, QString::number(num));
			}
//			p->drawRect(x - 1, 1, w - (x+x) - 1, h + 1);
			break;
		}
		case QVariant::Double:
		{
#ifdef Q_WS_WIN
#else
			x = 0;
#endif
			QString f = KGlobal::locale()->formatNumber(item->getValue(col).toDouble());
			if(item->isInsertItem() && d->pColumnModes.at(col) == 3)  //yes that isn't beautiful
			{
				p->drawText(x, y_offset, w - (x+x) - 6, h, AlignRight, "[Auto]");
			}
			else
			{
				p->drawText(x, y_offset, w - (x+x) - 6, h, AlignRight, f);
			}
			break;
		}
		case QVariant::Bool:
		{
/*			QRect r(w/2 - style().pixelMetric(QStyle::PM_IndicatorWidth)/2 + x-1, 1, style().pixelMetric(QStyle::PM_IndicatorWidth), style().pixelMetric(QStyle::PM_IndicatorHeight));
			QPen pen(p->pen());		// bug in KDE HighColorStyle
			style().drawControl(QStyle::CE_CheckBox, p, this, r, colorGroup(), (item->getInt(col) ? QStyle::Style_On : QStyle::Style_Off) | QStyle::Style_Enabled);
			p->setPen(pen); */
			int s = QMAX(h - 5, 12);
			QRect r(w/2 - s/2 + x, h/2 - s/2, s, s);
			p->setPen(QPen(colorGroup().text(), 1));
			p->drawRect(r);
			if(item->getValue(col).asBool())
			{
				p->drawLine(r.x() + 2, r.y() + 2, r.right() - 1, r.bottom() - 1);
				p->drawLine(r.x() + 2, r.bottom() - 2, r.right() - 1, r.y() + 1);
			}

			break;
		}
		case QVariant::Date:
		{
#ifdef Q_WS_WIN
			x = 5;
			y_offset = 0;
#else
			x = 5;
			y_offset = 0;
#endif
			QString s = "";

			if(item->getValue(col).toDate().isValid())
			{
				#ifdef USE_KDE
				s = KGlobal::locale()->formatDate(item->getValue(col).toDate(), true);
				#else
				s = item->getDate(col).toString(Qt::LocalDate);
				#endif
				p->drawText(x, y_offset, w - (x+x), h, AlignLeft | SingleLine | AlignVCenter, s);
//				p->drawText(x, -1, w - (x+x), h, AlignLeft | SingleLine | AlignVCenter, s);
				break;
			}
			break;
		}
		case QVariant::StringList:
		{
			QStringList sl = d->pColumnDefaults.at(col)->toStringList();
			p->drawText(x, y_offset, w - (x+x), h, AlignLeft | SingleLine | AlignVCenter,
				sl[item->getInt(col)]);
			break;
		}
		case QVariant::String:
		default:
		{
#ifdef Q_WS_WIN
			x = 5;
			y_offset = -1;
#else
			x = 5;
			y_offset = 0;
#endif
			p->drawText(x, y_offset, w - (x+x), h, AlignLeft | SingleLine | AlignVCenter, item->getText(col));
		}
	}
	p->setPen(fg);
	if(item->isInsertItem())
	{
		QFont f = p->font();
		f.setBold(false);
		p->setFont(f);
//		iOffset = 3;
	}
}

QPoint KexiTableView::contentsToViewport2( const QPoint &p )
{
    return QPoint( p.x() - contentsX(),
		   p.y() - contentsY() );
}

void KexiTableView::contentsToViewport2( int x, int y, int& vx, int& vy )
{
    const QPoint v = contentsToViewport2( QPoint( x, y ) );
    vx = v.x();
    vy = v.y();
}

QPoint KexiTableView::viewportToContents2( const QPoint& vp )
{
    return QPoint( vp.x() + contentsX(),
		   vp.y() + contentsY() );
}

void KexiTableView::paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch )
{
//  qDebug("%s: paintEmptyArea(x:%d y:%d w:%d h:%d)", (const char*)parentWidget()->caption(),cx,cy,cw,ch);

    // Regions work with shorts, so avoid an overflow and adjust the
    // table size to the visible size
    QSize ts( tableSize() );
    ts.setWidth( QMIN( ts.width(), visibleWidth() ) );
    ts.setHeight( QMIN( ts.height(), visibleHeight() ) );

    // Region of the rect we should draw, calculated in viewport
    // coordinates, as a region can't handle bigger coordinates
    contentsToViewport2( cx, cy, cx, cy );
    QRegion reg( QRect( cx, cy, cw, ch ) );

    // Subtract the table from it
    reg = reg.subtract( QRect( QPoint( 0, 0 ), ts ) );

    // And draw the rectangles (transformed inc contents coordinates as needed)
    QMemArray<QRect> r = reg.rects();
    for ( int i = 0; i < (int)r.count(); ++i ) {
		QRect rect( viewportToContents2(r[i].topLeft()), r[i].size() );
//		qDebug("- pEA: p->fillRect(x:%d y:%d w:%d h:%d)", 
//			(const char*)parentWidget()->caption(),
//			rect.x(),rect.y(),rect.width(),rect.height());
		p->fillRect( QRect(viewportToContents2(r[i].topLeft()),r[i].size()), 
			d->emptyAreaColor );
//		p->fillRect( QRect(viewportToContents2(r[i].topLeft()),r[i].size()), viewport()->backgroundBrush() );
	}
}

void KexiTableView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
	contentsMousePressEvent(e);

	if(d->pCurrentItem)
	{
		if(d->editOnDoubleClick && columnEditable(d->curCol) && columnType(d->curCol) != QVariant::Bool)
		{
			createEditor(d->curRow, d->curCol, QString::null);
		}

		emit itemDblClicked(d->pCurrentItem, d->curCol);
	}
}

void KexiTableView::contentsMousePressEvent( QMouseEvent* e )
{
	if(d->numRows == 0)
		return;
	if (rowAt(e->pos().y())==-1 || columnAt(e->pos().x())==-1)
		return; //clicked outside a grid

	// get rid of editor
	if (d->pEditor)
		editorOk();

	// remember old focus cell
	int oldRow = d->curRow;
	int oldCol = d->curCol;

	// get new focus cell
	d->curRow = rowAt(e->pos().y());
	d->curCol = columnAt(e->pos().x());

/*	if(d->curRow == -1)
		d->curRow = d->numRows-1;

	if(d->curCol == -1)
		d->curCol = d->numCols-1;
*/
	//	if we have a new focus cell, repaint
	if (( d->curRow != oldRow) || (d->curCol != oldCol ))
	{
		int cw = columnWidth( d->curCol );
		int rh = rowHeight();
		updateCell( d->curRow, d->curCol );
		ensureVisible( columnPos( d->curCol ) + cw / 2, rowPos( d->curRow ) + rh / 2, cw / 2, rh / 2 );
		updateCell( oldRow, oldCol );
		d->pVerticalHeader->setCurrentRow(d->curRow);
		d->pCurrentItem = itemAt(d->curRow);
//		kdDebug()<<"void KexiTableView::contentsMousePressEvent( QMouseEvent* e ) trying to get the current item"<<endl;
		emit itemSelected(d->pCurrentItem);
	}

//	kdDebug()<<"void KexiTableView::contentsMousePressEvent( QMouseEvent* e ) by now the current items should be set, if not -> error + crash"<<endl;
	if(e->button() == RightButton)
	{
		showContextMenu(e->globalPos());
	}
	else if(e->button() == LeftButton)
	{
		if(columnType(d->curCol) == QVariant::Bool && columnEditable(d->curCol))
		{
			boolToggled();
			updateCell( d->curRow, d->curCol );
		}
		else if(columnType(d->curCol) == QVariant::StringList && columnEditable(d->curCol))
		{
			createEditor(d->curRow, d->curCol);
		}
	}
}

/*! Shows context menu at \a pos for selected cell
	if menu is configured,
	else: contextMenuRequested() signal is emmited.
	Method used in contentsMousePressEvent() (for right button)
	and keyPressEvent() for Qt::Key_Menu key.
	If \a pos is QPoint(-1,-1) (the default), menu is positioned below the current cell.
*/
void KexiTableView::showContextMenu(QPoint pos)
{
	if (pos==QPoint(-1,-1)) {
		pos = viewport()->mapToGlobal( QPoint( columnPos(d->curCol), rowPos(d->curRow) + d->rowHeight ) );
	}
	//show own context menu if configured
	if (updateContextMenu()) {
		selectRow(d->curRow);
		d->pContextMenu->exec(pos);
	}
	else {
		//request other context menu
		emit contextMenuRequested(d->pCurrentItem, d->curCol, pos);
	}
}

void KexiTableView::contentsMouseMoveEvent( QMouseEvent *e )
{
	// do the same as in mouse press
	int x,y;
	contentsToViewport(e->x(), e->y(), x, y);

	if(y > visibleHeight())
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollDown;
	}
	else if(y < 0)
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollUp;
	}
	else if(x > visibleWidth())
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollRight;
	}
	else if(x < 0)
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollLeft;
	}
	else
	{
		d->needAutoScroll = false;
		d->scrollTimer->stop();
		contentsMousePressEvent(e);
	}

}

void KexiTableView::contentsMouseReleaseEvent(QMouseEvent *e)
{
	if(d->needAutoScroll)
	{
		d->scrollTimer->stop();
	}
}

void KexiTableView::keyPressEvent(QKeyEvent* e)
{
	kdDebug() << "KexiTableView::KeyPressEvent()" << endl;

	if(d->pCurrentItem == 0 && d->numRows > 0)
	{
		d->curCol = d->curRow = 0;
		int cw = columnWidth(d->curCol);

		ensureVisible(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + d->rowHeight / 2, cw / 2, d->rowHeight / 2);
		updateCell(d->curRow, d->curCol);
		d->pVerticalHeader->setCurrentRow(d->curRow);
		d->pCurrentItem = itemAt(d->curRow);
		emit itemSelected(d->pCurrentItem);
	}
	else if(d->numRows == 0)
	{
		return;
	}

	// if a cell is just editing, do some special stuff
	if(d->pEditor)
	{
		if (e->key() == Key_Escape)
			editorCancel();
		else if (e->key() == Key_Return || e->key() == Key_Enter)
			editorOk();
		return;
	}

	if(e->key() == Key_Return || e->key() == Key_Enter)
	{
		emit itemReturnPressed(d->pCurrentItem, d->curCol);
	}

	int oldRow = d->curRow;
	int oldCol = d->curCol;

	// navigate in the header...
    switch (e->key())
    {
	case Key_Delete:
		if (e->state()==Qt::ControlButton) {//remove current row
			removeRecord();
		}
		else {//remove all chars in the current cell
			if(columnType(d->curCol) != QVariant::Bool && columnEditable(d->curCol))
				createEditor(d->curRow, d->curCol, QString::null, false);
			if (d->pEditor && d->pEditor->isA("KexiInputTableEdit")) {
				static_cast<KexiInputTableEdit*>(d->pEditor)->clear();
			}
		}
		break;

	case Key_Shift:
	case Key_Alt:
	case Key_Control:
	case Key_Meta:
		e->ignore();
		break;

	case Key_Left:
		d->curCol = QMAX(0, d->curCol - 1);
		break;
	case Key_Right:
	case Key_Tab:
		d->curCol = QMIN(cols() - 1, d->curCol + 1);
		break;
    case Key_Up:
		d->curRow = QMAX(0, d->curRow - 1);
		break;
    case Key_Down:
		d->curRow = QMIN(rows() - 1, d->curRow + 1);
		break;
    case Key_Prior:
		d->curRow -= visibleHeight() / d->rowHeight;
		d->curRow = QMAX(0, d->curRow);
		break;
    case Key_Next:
		d->curRow += visibleHeight() / d->rowHeight;
		d->curRow = QMIN(rows()-1, d->curRow);
		break;
    case Key_Home:
		d->curRow = 0;
		break;
    case Key_End:
		d->curRow = d->numRows-1;
		break;

#ifndef _WIN32
	#warning this needs work!
#endif
	case Key_Enter:
	case Key_Return:
	case Key_F2:
		if(columnType(d->curCol) != QVariant::Bool && columnEditable(d->curCol))
			createEditor(d->curRow, d->curCol, QString::null, false);
		break;

	case Key_Backspace:
		if(columnType(d->curCol) != QVariant::Bool && columnEditable(d->curCol))
			createEditor(d->curRow, d->curCol, QString::null, true);
		break;
	case Key_Space:
		if(columnType(d->curCol) == QVariant::Bool && columnEditable(d->curCol))
		{
			boolToggled();
			break;
		}
	default:
		//others:
		if (e->key()==KGlobalSettings::contextMenuKey()) { //Key_Menu:
			showContextMenu();
		}
		else {
			qDebug("KexiTableView::KeyPressEvent(): default");
			if (e->text().isEmpty() || !e->text().isEmpty() && !e->text()[0].isPrint() ) {
				kdDebug() << "NOT PRINTABLE: 0x0" << QString("%1").arg(e->key(),0,16) <<endl;
				e->ignore();
				return;
			}
			if(columnType(d->curCol) != QVariant::Bool && columnEditable(d->curCol))
			{
				qDebug("KexiTableView::KeyPressEvent(): ev pressed");
	//			if (e->text()[0].isPrint())
				createEditor(d->curRow, d->curCol, e->text(), false);
			}
		}
	}
	// if focus cell changes, repaint
	if ( d->curRow != oldRow || d->curCol != oldCol )
	{
		int cw = columnWidth(d->curCol);

		ensureVisible(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + d->rowHeight / 2, cw / 2, d->rowHeight / 2);
		updateCell(oldRow, oldCol);
		updateCell(d->curRow, d->curCol);
		d->pVerticalHeader->setCurrentRow(d->curRow);
		d->pCurrentItem = itemAt(d->curRow);
		emit itemSelected(d->pCurrentItem);
	}
}

void KexiTableView::emitSelected()
{
	if(d->pCurrentItem)
		emit itemSelected(d->pCurrentItem);
}

void KexiTableView::boolToggled()
{
	int s = d->pCurrentItem->getInt(d->curCol);
	QVariant oldValue=d->pCurrentItem->getValue(d->curCol);
	d->pCurrentItem->setInt(d->curCol, (s ? 0 : 1));
	updateCell(d->curRow, d->curCol);
	emit itemChanged(d->pCurrentItem, d->curCol,oldValue);
	emit itemChanged(d->pCurrentItem, d->curCol);
}

void KexiTableView::selectNext()
{
//	int oldRow = d->curRow;
//	d->curRow = QMIN( rows() - 1, d->curRow + 1 );
	selectRow( QMIN( rows() - 1, d->curRow + 1 ) );
/*js
	if(d->curRow != oldRow)
	{
		int rh = rowHeight(d->curRow);

		ensureVisible(0, rowPos(d->curRow) + rh / 2, 0, rh / 2);
		if (hasFocus() || viewport()->hasFocus())
		{
			updateCell(oldRow, d->curCol);
			updateCell(d->curRow, d->curCol);
		}
		d->pVerticalHeader->setCurrentRow(d->curRow);
		d->pCurrentItem = itemAt(d->curRow);
		emit itemSelected(d->pCurrentItem);
	}*/
}

void KexiTableView::selectRow(int row)
{
	int oldRow = d->curRow;

	if(d->curRow != row)
	{
		d->curRow = row;
		int rh = rowHeight();

		ensureVisible(0, rowPos(d->curRow) + rh / 2, 0, rh / 2);
		if (hasFocus() || viewport()->hasFocus())
		{
			updateCell(oldRow, d->curCol);
			updateCell(d->curRow, d->curCol);
		}
		d->pVerticalHeader->setCurrentRow(d->curRow);
		d->pCurrentItem = itemAt(d->curRow);
		emit itemSelected(d->pCurrentItem);
	}
}

void KexiTableView::selectPrev()
{
//	int oldRow = d->curRow;

//	d->curRow = QMAX( 0, d->curRow - 1 );
	selectRow( QMAX( 0, d->curRow - 1 ) );
	/*js
	if(d->curRow != oldRow)
	{
		int rh = rowHeight(d->curRow);

		ensureVisible(0, rowPos(d->curRow) + rh / 2, 0, rh / 2);
		if (hasFocus() || viewport()->hasFocus())
		{
			updateCell(oldRow, d->curCol);
			updateCell(d->curRow, d->curCol);
		}
		d->pVerticalHeader->setCurrentRow(d->curRow);
		d->pCurrentItem = itemAt(d->curRow);
		emit itemSelected(d->pCurrentItem);
	}*/
}

void KexiTableView::gotoNext()
{
	int oldCol = d->curCol;
	d->curCol = QMIN( cols() - 1, d->curCol + 1 );

	if(d->curCol != oldCol)
	{
		if (hasFocus() || viewport()->hasFocus())
		{
			updateCell(d->curRow, oldCol);
			updateCell(d->curRow, d->curCol);
		}
	}
	else
	{
		selectNext();
	}
}

void KexiTableView::createEditor(int row, int col, QString addText/* = QString::null*/, bool backspace/* = false*/)
{
	if(d->pColumnModes.at(d->numCols-1) & ColumnReadOnly)
		return;

	
//	QString val;
	QVariant val;
/*	switch(columnType(col))
	{
		case QVariant::Date:
			#ifdef USE_KDE
//			val = KGlobal::locale()->formatDate(d->pCurrentItem->getDate(col), true);

			#else
//			val = d->pCurrentItem->getDate(col).toString(Qt::LocalDate);
			#endif
			break;

		default:
//			val = d->pCurrentItem->getText(d->curCol);
			val = d->pCurrentItem->getValue(d->curCol);

			break;
	}*/

	val = d->pCurrentItem->getValue(d->curCol);

	//it's getting ugly :)

	switch(columnType(col))
	{
		case QVariant::ByteArray:
			d->pEditor = new KexiBlobTableEdit(val.toByteArray(), viewport(), "inPlaceEd");
			d->pEditor->resize(columnWidth(d->curCol)-1, 150);
			moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow));
			d->pEditor->show();
			d->pEditor->setFocus();
			return;
		case QVariant::StringList:
			d->pEditor = new KexiComboBoxTableEdit(static_cast<KexiDBField::ColumnType>(val.toInt()),
				d->pColumnDefaults.at(col)->toStringList(), viewport(), "inPlaceEd");
			break;
		case QVariant::Date:
			d->pEditor = new KexiDateTableEdit(val, viewport(), "inPlaceEd");
			kdDebug() << "date editor created..." << endl;
			break;
		default:
			d->pEditor = new KexiInputTableEdit(val, columnType(col), addText, false, viewport(), "inPlaceEd",
			 d->pColumnDefaults.at(col)->toStringList());
			static_cast<KexiInputTableEdit*>(d->pEditor)->end(false);
			if(backspace)
				static_cast<KexiInputTableEdit*>(d->pEditor)->backspace();

			break;
	}

#ifdef Q_WS_WIN
//TODO
	moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow)-1);
	d->pEditor->resize(columnWidth(d->curCol)-1, rowHeight());
#else
//	if (columnType(col)==QVariant::String) {
		//moveChild(d->pEditor, columnPos(d->curCol)+5, rowPos(d->curRow));
		//d->pEditor->resize(columnWidth(d->curCol)-1, rowHeight(d->curRow)-1);
	//}
	if (columnType(col)==QVariant::Date) {
		moveChild(d->pEditor, columnPos(d->curCol)-0, rowPos(d->curRow)-2);
		d->pEditor->resize(columnWidth(d->curCol)+0, rowHeight()+3);
	}
	else {
		moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow));
		d->pEditor->resize(columnWidth(d->curCol)-2, rowHeight()-1);
	}
#endif
//	moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow));
	d->pEditor->show();
	d->pEditor->setFocus();
}

void KexiTableView::focusInEvent(QFocusEvent*)
{
	updateCell(d->curRow, d->curCol);
}


void KexiTableView::focusOutEvent(QFocusEvent*)
{
	updateCell(d->curRow, d->curCol);
}

bool KexiTableView::focusNextPrevChild(bool next)
{
	if (d->pEditor)
		return true;
	return QScrollView::focusNextPrevChild(next);
}

void KexiTableView::resizeEvent(QResizeEvent *e)
{
	QScrollView::resizeEvent(e);
	updateGeometries();
//	updateContents(0,0,1000,1000);//js
//	erase(); repaint();
}

void KexiTableView::viewportResizeEvent( QResizeEvent *e )
{
    QScrollView::viewportResizeEvent( e );
    updateGeometries();
//	erase(); repaint();
}

void KexiTableView::showEvent(QShowEvent *e)
{
	QScrollView::showEvent(e);
	QRect r(cellGeometry(rows() - 1, cols() - 1 ));
	resizeContents(r.right() + 1, r.bottom() + 1);
	updateGeometries();
}


void KexiTableView::contentsDragMoveEvent(QDragMoveEvent *e)
{
	for(QStringList::Iterator it = d->dropFilters.begin(); it != d->dropFilters.end(); it++)
	{
		if(e->provides((*it).latin1()))
		{
			e->acceptAction(true);
			return;
		}
	}
	e->acceptAction(false);
}

void KexiTableView::contentsDropEvent(QDropEvent *ev)
{
	emit dropped(ev);
}

void KexiTableView::updateCell(int row, int col)
{

	if(!d->recordIndicator)
	{
		updateContents(cellGeometry(row, col));
	}
	else
	{
		for(int i = 0; i < d->numCols; i++)
		{
			updateContents(cellGeometry(row, i));
		}
	}

}

void KexiTableView::columnSort(int col)
{
	bool i = false;
	QVariant hint;
	if (d->pEditor)
		return;
	if(d->pInsertItem)
	{
		i = true;
		hint = d->pInsertItem->getHint();
//		delete d->pInsertItem;
		remove(d->pInsertItem);
		d->pInsertItem = 0;
//		d->pVerticalHeader->removeLabel(rows());
	}

	if(d->sortedColumn == col)
		d->sortOrder = !d->sortOrder;
	else
		d->sortOrder = true;
	d->sortedColumn = col;
	d->pTopHeader->setSortIndicator(col, d->sortOrder);
	m_contents->setSorting(col, d->sortOrder, columnType(col));
	sort();

	if(i)
	{
		KexiTableItem *insert = new KexiTableItem(this);
		insert->setHint(hint);
		insert->setInsertItem(true);
		d->pInsertItem = insert;

	}
//	updateContents( 0, 0, viewport()->width(), viewport()->height());

	emit sortedColumnChanged(col);
}

void KexiTableView::columnWidthChanged( int col, int, int )
{
	QSize s(tableSize());
	int w = contentsWidth();
	viewport()->setUpdatesEnabled(false);
	resizeContents( s.width(), s.height() );
	viewport()->setUpdatesEnabled(true);
	if (contentsWidth() < w)
		updateContents(contentsX(), 0, viewport()->width(), contentsHeight());
//		repaintContents( s.width(), 0, w - s.width() + 1, contentsHeight(), TRUE );
	else
	//	updateContents( columnPos(col), 0, contentsWidth(), contentsHeight() );
		updateContents(contentsX(), 0, viewport()->width(), contentsHeight());
	//	viewport()->repaint();

//	updateContents(0, 0, d->pBufferPm->width(), d->pBufferPm->height());
	if (d->pEditor)
	{
		d->pEditor->resize(columnWidth(d->curCol)-1, rowHeight()-1);
		moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow));
	}
	updateGeometries();
}

void KexiTableView::updateGeometries()
{
	QSize ts = tableSize();
	if (d->pTopHeader->offset() && ts.width() < d->pTopHeader->offset() + d->pTopHeader->width())
		horizontalScrollBar()->setValue(ts.width() - d->pTopHeader->width());

//	d->pVerticalHeader->setGeometry(1, topMargin() + 1, leftMargin(), visibleHeight());
	d->pTopHeader->setGeometry(leftMargin() + 1, 1, visibleWidth(), topMargin());
	d->pVerticalHeader->setGeometry(1, topMargin() + 1, leftMargin(), visibleHeight());
}

int KexiTableView::columnWidth(int col) const
{
    return d->pTopHeader->sectionSize(col);
}

int KexiTableView::rowHeight() const
{
	return d->rowHeight;
}

int KexiTableView::columnPos(int col) const
{
    return d->pTopHeader->sectionPos(col);
}

int KexiTableView::rowPos(int row) const
{
	return d->rowHeight*row;
}

int KexiTableView::columnAt(int pos) const
{
    return d->pTopHeader->sectionAt(pos);
}

int KexiTableView::rowAt(int pos, bool ignoreEnd) const
{
	pos /=d->rowHeight;
	if (pos < 0)
		return 0;
	if ((pos >= d->numRows) && !ignoreEnd)
		return -1;
	return pos;
}

QRect KexiTableView::cellGeometry(int row, int col) const
{
    return QRect(columnPos(col), rowPos(row),
		  columnWidth(col), rowHeight());
}

QSize KexiTableView::tableSize() const
{
	if (rows() > 0 && cols() > 0)
	    return QSize( columnPos( cols() - 1 ) + columnWidth( cols() - 1 ),
			rowPos( rows()-1 ) + rowHeight());
	return QSize(0,0);
}

int KexiTableView::rows() const
{
	return d->numRows;
}

int KexiTableView::cols() const
{
    return d->pTopHeader->count();
}

void KexiTableView::setCursor(int row, int col/*=-1*/)
{
	// get rid of editor
	if (d->pEditor)
		editorOk();
	if(rows() <= 0)
	{
		d->curRow=0;
		d->pCurrentItem=0;
		d->pVerticalHeader->setCurrentRow(-1);
		return;
	}

	int oldRow = d->curRow;
	int oldCol = d->curCol;

	if(col>=0)
	{
		d->curCol = QMAX(0, col);
		d->curCol = QMIN(cols() - 1, d->curCol);
	}
	d->curRow = QMAX( 0, row);
	d->curRow = QMIN( rows() - 1, d->curRow);

	d->pCurrentItem = itemAt(d->curRow);
	if ( d->curRow != oldRow || d->curCol != oldCol )
	{
//		int cw = columnWidth( d->curCol );
		int rh = rowHeight();
//		ensureVisible( columnPos( d->curCol ) + cw / 2, rowPos( d->curRow ) + rh / 2, cw / 2, rh / 2 );
//		center(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + rh / 2, cw / 2, rh / 2);
		ensureVisible(columnPos(d->curCol), rowPos(d->curRow), columnWidth(d->curCol), rh);
		updateCell( oldRow, oldCol );
		updateCell( d->curRow, d->curCol );
		d->pVerticalHeader->setCurrentRow(d->curRow);
		emit itemSelected(d->pCurrentItem);
	}
}

void KexiTableView::editorOk()
{
	if (!d->pEditor)
		return;
//	d->pCurrentItem->setText(d->curCol, d->pEditor->text());
	QVariant oldValue=d->pCurrentItem->getValue(d->curCol);
	d->pCurrentItem->setValue(d->curCol, d->pEditor->value());
	editorCancel();
	emit itemChanged(d->pCurrentItem, d->curCol,oldValue);
	emit itemChanged(d->pCurrentItem, d->curCol);
}

void KexiTableView::editorCancel()
{
	if (!d->pEditor)
		return;

	delete d->pEditor;
	d->pEditor = 0;
	viewport()->setFocus();
}

void KexiTableView::setAdditionPolicy(AdditionPolicy policy)
{
	d->additionPolicy = policy;
//	updateContextMenu();
}

KexiTableView::AdditionPolicy KexiTableView::additionPolicy()
{
	return d->additionPolicy;
}

void KexiTableView::setDeletionPolicy(DeletionPolicy policy)
{
	d->deletionPolicy = policy;
//	updateContextMenu();
}

KexiTableView::DeletionPolicy KexiTableView::deletionPolicy()
{
	return d->deletionPolicy;
}

/*! Updates visibility/accesibility of popup menu items,
  returns false if no items are visible after update. */
bool KexiTableView::updateContextMenu()
{
  // delete d->pContextMenu;
  //  d->pContextMenu = 0L;
//  d->pContextMenu->clear();
//	if(d->pCurrentItem && d->pCurrentItem->isInsertItem())
//    return;

//	if(d->additionPolicy != NoAdd || d->deletionPolicy != NoDelete)
//	{
//		d->pContextMenu = new QPopupMenu(this);
  d->pContextMenu->setItemVisible(d->menu_id_addRecord, d->additionPolicy != NoAdd);
  d->pContextMenu->setItemVisible(d->menu_id_removeRecord, d->deletionPolicy != NoDelete
    && d->pCurrentItem && !d->pCurrentItem->isInsertItem());
  for (int i=0; i<(int)d->pContextMenu->count(); i++) {
    if (d->pContextMenu->isItemVisible( d->pContextMenu->idAt(i) ))
      return true;
  }
  return false;
}

void KexiTableView::slotAutoScroll()
{
	if(d->needAutoScroll)
	{
		switch(d->scrollDirection)
		{
			case ScrollDown:
				setCursor(d->curRow + 1, d->curCol);
				break;

			case ScrollUp:
				setCursor(d->curRow - 1, d->curCol);
				break;
			case ScrollLeft:
				setCursor(d->curRow, d->curCol - 1);
				break;

			case ScrollRight:
				setCursor(d->curRow, d->curCol + 1);
				break;
		}
	}
}

void KexiTableView::inserted()
{
	d->pVerticalHeader->addLabel();
#if 0
	if(itemAt(d->numRows)->isInsertItem())
		d->pVerticalHeader->addLabel("*", d->rowHeight);
	else
		d->pVerticalHeader->addLabel("", d->rowHeight);
#endif
}

#ifndef KEXI_NO_PRINT
void
KexiTableView::print(KPrinter &printer)
{
//	printer.setFullPage(true);

	int leftMargin = printer.margins().width() + 2 + d->rowHeight;
	int topMargin = printer.margins().height() + 2;
//	int bottomMargin = topMargin + ( printer.realPageSize()->height() * printer.resolution() + 36 ) / 72;
	int bottomMargin = 0;
	kdDebug() << "KexiTableView::print: bottom = " << bottomMargin << endl;

	QPainter p(&printer);

	KexiTableItem *i;
	int width = leftMargin;
	for(int col=0; col < cols(); col++)
	{
		p.fillRect(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight, QBrush(gray));
		p.drawRect(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight);
		p.drawText(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight, AlignLeft | AlignVCenter, d->pTopHeader->label(col));
		width = width + columnWidth(col);
	}

	int yOffset = topMargin;
	int row = 0;
	int right = 0;
	for(i = m_contents->first(); i; i = m_contents->next())
	{
		if(!i->isInsertItem())
		{	kdDebug() << "KexiTableView::print: row = " << row << " y = " << yOffset << endl;
			int xOffset = leftMargin;
			for(int col=0; col < cols(); col++)
			{
				kdDebug() << "KexiTableView::print: col = " << col << " x = " << xOffset << endl;
				p.saveWorldMatrix();
				p.translate(xOffset, yOffset);
				paintCell(&p, i, col, QRect(0, 0, columnWidth(col) + 1, d->rowHeight), true);
				p.restoreWorldMatrix();
//			p.drawRect(xOffset, yOffset, columnWidth(col), d->rowHeight);
				xOffset = xOffset + columnWidth(col);
				right = xOffset;
			}

			row++;
			yOffset = topMargin  + row * d->rowHeight;
		}

		if(yOffset > 900)
		{
			p.drawLine(leftMargin, topMargin, leftMargin, yOffset);
			p.drawLine(leftMargin, topMargin, right - 1, topMargin);
			printer.newPage();
			yOffset = topMargin;
			row = 0;
		}
	}
	p.drawLine(leftMargin, topMargin, leftMargin, yOffset);
	p.drawLine(leftMargin, topMargin, right - 1, topMargin);

//	p.drawLine(60,60,120,150);
	p.end();
}
#endif

void
KexiTableView::takeInsertItem()
{
//	delete d->pInsertItem;
	d->pInsertItem = 0;
}

KexiTableRM* KexiTableView::recordMarker()
{
	return d->pVerticalHeader;
}

KexiTableRM* KexiTableView::verticalHeader()
{
	return d->pVerticalHeader; 
}

QString KexiTableView::column(int section)
{
	return d->pTopHeader->label(section);
}

int KexiTableView::currentCol()
{ 
	return d->curCol;
}

void KexiTableView::setBackgroundAltering(bool altering) { d->bgAltering = altering; }
bool KexiTableView::backgroundAltering() { return d->bgAltering; }

void KexiTableView::setRecordIndicator(bool indicator) { d->recordIndicator = indicator; }
bool KexiTableView::recordIndicator() { return d->recordIndicator; }

void KexiTableView::setEditableOnDoubleClick(bool set) { d->editOnDoubleClick = set; }
bool KexiTableView::editableOnDoubleClick() { return d->editOnDoubleClick; }

void KexiTableView::setEmptyAreaColor(QColor c) { d->emptyAreaColor = c; }
QColor KexiTableView::emptyAreaColor() { return d->emptyAreaColor; }

void KexiTableView::setInsertItem(KexiTableItem *i) { d->pInsertItem = i; }
KexiTableItem *KexiTableView::insertItem() { return d->pInsertItem; }

void KexiTableView::triggerUpdate()
{
	if (!d->pUpdateTimer->isActive())
		d->pUpdateTimer->start(1, true);
}

int KexiTableView::currentRow()
{
	return d->curRow;
}

KexiTableItem *KexiTableView::selectedItem()
{
	return d->pCurrentItem;
}

QVariant::Type KexiTableView::columnType(int col)
{
	return d->pColumnTypes.at(col);
}

bool KexiTableView::columnEditable(int col)
{
	return d->pColumnModes.at(col);
//	if(d->pColumnModes.at(col) & ColumnEditable)
//		return true;

//	return false;
}

QVariant KexiTableView::columnDefault(int col)
{
	return *d->pColumnDefaults.at(col);
}


#include "kexitableview.moc"
