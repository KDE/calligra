/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
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
#include <qwmatrix.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qstyle.h>

#include <config.h>

//#ifdef USE_KDE
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
//#endif

#include "kexitablerm.h"
#include "kexitableview.h"
#include "kexitableheader.h"

#ifdef USE_KDE
#include "kexidatetableedit.h"
#endif

#include "kexiinputtableedit.h"

KexiTableView::KexiTableView(QWidget *parent, const char *name)
:QScrollView(parent, name, Qt::WRepaintNoErase | Qt::WStaticContents | Qt::WResizeNoErase)
{
	setResizePolicy(Manual);
	viewport()->setBackgroundMode(NoBackground);
	viewport()->setFocusPolicy(QWidget::StrongFocus);
	setBackgroundMode(NoBackground);
	setLineWidth(1);
	// Initialize variables
	m_pEditor = 0;
	m_numRows = 0;
	m_numCols = 0;
	m_curRow = 0;
	m_curCol = 0;
	m_pCurrentItem = 0;

	m_rowHeight = fontMetrics().lineSpacing();
	m_pBufferPm = 0;

	m_sortedColumn = -1;
	m_sortOrder = true;
	
	m_pUpdateTimer = new QTimer(this);

	m_pColumnTypes = new QMemArray<QVariant::Type>;

	m_pColumnModes = new QMemArray<int>;
	m_deletionPolicy = NoDelete;
	m_additionPolicy = NoAdd;

	setMargins(14, fontMetrics().height() + 4, 0, 0);

	// Create headers
	m_pTopHeader = new QHeader(m_numCols, this);
	m_pTopHeader->setOrientation(Horizontal);
	m_pTopHeader->setTracking(false);
	m_pTopHeader->setMovingEnabled(false);
	
/*	m_pVerticalHeader = new KexiTableRM(this);
	m_pVerticalHeader->setCellHeight(m_rowHeight);
	m_pVerticalHeader->setCurrentRow(-1);
*/

	m_pVerticalHeader = new KexiTableHeader(this);
	m_pVerticalHeader->setOrientation(Vertical);
	m_pVerticalHeader->setTracking(false);
	m_pVerticalHeader->setMovingEnabled(false);
	m_pVerticalHeader->setStretchEnabled(false);
	m_pVerticalHeader->setResizeEnabled(false);
	m_pVerticalHeader->setCurrentRow(false);
	
//	enableClipper(true);
	setBackgroundMode(PaletteBackground);

	m_pContextMenu = 0;

	m_contents.setAutoDelete(true);

	m_scrollTimer = new QTimer(this);
	connect(m_scrollTimer, SIGNAL(timeout()), this, SLOT(slotAutoScroll()));

	// Connect header, table and scrollbars
	connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), m_pTopHeader, SLOT(setOffset(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)),	m_pVerticalHeader, SLOT(setOffset(int)));
	connect(m_pTopHeader, SIGNAL(sizeChange(int, int, int)), this, SLOT(columnWidthChanged(int, int, int)));
	connect(m_pTopHeader, SIGNAL(clicked(int)), this, SLOT(columnSort(int)));

	connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
}


void KexiTableView::addColumn(QString name, QVariant::Type type, bool editable, int width, bool autoinc)
{
	m_numCols++;
	m_pColumnTypes->resize(m_numCols);
	m_pColumnModes->resize(m_numCols);

	m_pColumnTypes->at(m_numCols-1)		= type;

	if(editable && autoinc)
	{
		m_pColumnModes->at(m_numCols-1) = ColumnEditable | ColumnAutoIncrement;
	}
	else if(editable)
	{
		m_pColumnModes->at(m_numCols-1) = ColumnEditable;
	}
	else
	{
		m_pColumnModes->at(m_numCols-1) = ColumnReadOnly;
	}

	m_pTopHeader->addLabel(name, width);

	m_pTopHeader->setUpdatesEnabled(true);
}


void KexiTableView::setFont(const QFont &f)
{
	QWidget::setFont(f);
	m_rowHeight = fontMetrics().lineSpacing()+2;
	setMargins(14, m_rowHeight, 0, 0);
	m_pVerticalHeader->setCellHeight(m_rowHeight);
}

void KexiTableView::remove(KexiTableItem *item, bool moveCursor/*=true*/)
{
	if(m_contents.removeRef(item))
	{
		m_numRows--;
		if(moveCursor)
			setCursor(m_curRow);
//		selectPrev();
		m_pUpdateTimer->start(1,true);
	}
}

void KexiTableView::removeRecord()
{
	emit itemRemoveRequest(m_pCurrentItem);
}

void KexiTableView::addRecord()
{
	emit addRecordRequest();
}

void KexiTableView::clear()
{
	editorCancel();
	m_contents.clear();
	
	// Initialize variables
	m_pEditor = 0;
	m_numRows = 0;
	m_curRow = 0;
	m_curCol = 0;
	m_pCurrentItem=0;

	m_pVerticalHeader->setCurrentRow(-1);
//	m_pUpdateTimer->start(1,true);
	viewport()->repaint();
}

void KexiTableView::clearAll()
{
	editorCancel();
	m_contents.clear();

	m_pEditor = 0;
	m_numRows = 0;
	m_numCols = 0;
	m_curRow = 0;
	m_curCol = 0;
	m_pCurrentItem=0;

	while(m_pTopHeader->count())
		m_pTopHeader->removeLabel(0);

	m_pVerticalHeader->setCurrentRow(-1);

	viewport()->repaint();

	m_sortedColumn = -1;
	m_sortOrder = true;

	m_pColumnTypes->resize(0);
	m_pColumnModes->resize(0);
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

	QPtrListIterator<KexiTableItem> it(m_contents);

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

	delete m_pColumnTypes;
	delete m_pColumnModes;

	if(m_pBufferPm)
		delete m_pBufferPm;
}
/*
void KexiTableView::setColumn(int col, QString name, ColumnType type, bool changeable=false)
{
	(m_pColumnTypes->at(col)) = type;
	m_pTopHeader->setLabel(col, name);
	(m_pColumnModes->at(col)) = changeable;
}
*/
void KexiTableView::setSorting(int col, bool ascending/*=true*/)
{
	m_sortOrder = ascending;
	m_sortedColumn = col;
	m_pTopHeader->setSortIndicator(col, m_sortOrder);
	m_contents.setSorting(col, m_sortOrder, columnType(col));
	sort();
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
	return m_sortedColumn;
}

void KexiTableView::sort()
{
	if(rows() < 2)
	{
		return;
	}

	m_contents.sort();

	m_curRow = m_contents.findRef(m_pCurrentItem);

	m_pCurrentItem = m_contents.at(m_curRow);

	int cw = columnWidth(m_curCol);
	int rh = rowHeight(m_curRow);

//	m_pVerticalHeader->setCurrentRow(m_curRow);
	center(columnPos(m_curCol) + cw / 2, rowPos(m_curRow) + rh / 2);
//	updateCell(oldRow, m_curCol);
//	updateCell(m_curRow, m_curCol);
	m_pVerticalHeader->setCurrentRow(m_curRow);
//	slotUpdate();
	m_pUpdateTimer->start(1,true);
}

QSizePolicy KexiTableView::sizePolicy() const
{
	// this widget is expandable
	return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize KexiTableView::sizeHint() const
{
	return QSize(tableSize().width(), m_rowHeight*3);
}

QSize KexiTableView::minimumSizeHint() const
{
	return QSize(columnWidth(1), m_rowHeight*2);
}

void KexiTableView::createBuffer(int width, int height)
{
	if(!m_pBufferPm)
		m_pBufferPm = new QPixmap(width, height);
	else
		if(m_pBufferPm->width() < width || m_pBufferPm->height() < height)
			m_pBufferPm->resize(width, height);
//	m_pBufferPm->fill();
}

void KexiTableView::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
	int colfirst = columnAt(cx);
	int rowfirst = rowAt(cy);
	int collast = columnAt(cx + cw-1);
	int rowlast = rowAt(cy + ch-1);

//	qDebug("%3d %3d %3d %3d | %2d, %2d, %2d, %2d [%4d, %4d]", cx, cy, cw, ch,
//													colfirst, rowfirst, collast, rowlast, tableSize().width(), tableSize().height());
	
	if (rowfirst == -1 || colfirst == -1)
	{
		paintEmptyArea(p, cx, cy, cw, ch);
		return;
	}
	
	createBuffer(cw, ch);
	if(m_pBufferPm->isNull())
		return;
	QPainter *pb = new QPainter(m_pBufferPm, this);

	if (rowlast == -1)
		rowlast = rows() - 1;
	if ( collast == -1 )
		collast = cols() - 1;

	int r;
	pb->fillRect(0, 0, cw, ch, colorGroup().base());

	QPtrListIterator<KexiTableItem> it(m_contents);
	it += rowfirst;

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
		
		for(c = colfirst; c <= collast; c++)
		{
	    	// get position and width of column c
	    	colp = columnPos(c);
	    	colw = columnWidth(c);
	    	int translx = colp-cx;

	    	// Translate painter and draw the cell
	    	pb->saveWorldMatrix();
	    	pb->translate(translx, transly);
//	    	paintCell( pb, r, c, QRect(columnPos(c), rowPos(r), colw, m_rowHeight));
	    	paintCell( pb, it.current(), c, QRect(columnPos(c), rowPos(r), colw, m_rowHeight));
		  	pb->restoreWorldMatrix();
		}

	}
	delete pb;	

	p->drawPixmap(cx,cy,*m_pBufferPm, 0,0,cw,ch);
}

void KexiTableView::paintCell(QPainter* p, KexiTableItem *item, int col, const QRect &cr)
{
	int w = cr.width();
	int h = cr.height();
	int x2 = w - 1;
	int y2 = h - 1;

	//	Draw our lines
	QPen pen(p->pen());
	p->setPen(QColor(200,200,200));
//	p->setPen(colorGroup().button());
	p->drawLine( x2, 0, x2, y2 );	// right
	p->drawLine( 0, y2, x2, y2 );	// bottom
	p->setPen(pen);

	//	If we are in the focus cell, draw indication
	if (m_pCurrentItem == item && col == m_curCol)
	{
		if (hasFocus() || viewport()->hasFocus())
			p->drawRect(0, 0, x2, y2);
	}

	int x = 2;

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
	switch(columnType(col))
	{
		case QVariant::UInt:
		case QVariant::Int:
		{
			int num = item->getValue(col).toInt();
//			if(num < 0)
//				p->setPen(red);
//			p->drawText(x - (x+x) - 2, 2, w, h, AlignRight, QString::number(num));
//			qDebug("KexiTableView::paintCell(): mode: %i", m_pColumnModes->at(col));
			if(item->isInsertItem() && m_pColumnModes->at(col) == 3)  //yes that isn't beautiful
			{
				p->drawText(x, 2, w - (x+x) - 2, h, AlignRight, "[Auto]");
			}
			else
			{
				p->drawText(x, 2, w - (x+x) - 2, h, AlignRight, QString::number(num));
			}
//			p->drawRect(x - 1, 1, w - (x+x) - 1, h + 1);
			break;
		}
		case QVariant::Double:
		{
			QString f = KGlobal::_locale->formatNumber(item->getValue(col).toDouble());
			if(item->isInsertItem() && m_pColumnModes->at(col) == 3)  //yes that isn't beautiful
			{
				p->drawText(x, 2, w - (x+x) - 2, h, AlignRight, "[Auto]");
			}
			else
			{
				p->drawText(x, 2, w - (x+x) - 2, h, AlignRight, f);
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
			QString s = "";
			
			if(item->getValue(col).toDate().isValid())
			{
				#ifdef USE_KDE
				s = KGlobal::_locale->formatDate(item->getValue(col).toDate(), true);
				#else
				s = item->getDate(col).toString(Qt::LocalDate);
				#endif
				p->drawText(x, 0, w - (x+x), h, AlignLeft | SingleLine | AlignVCenter, s);
				break;
			}
			break;
		}
		case QVariant::String:
		default:
		{
			p->drawText(x, 0, w - (x+x), h, AlignLeft | SingleLine | AlignVCenter, item->getText(col));
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

void KexiTableView::paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch )
{
	p->fillRect(cx, cy, cw, ch, colorGroup().brush(QColorGroup::Base));
}

void KexiTableView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
	contentsMousePressEvent(e);

	if(m_pCurrentItem)
	{
		if(m_editOnDubleClick && columnEditable(m_curCol))
		{
			createEditor(m_curRow, m_curCol, QString::null);
		}

		emit itemDblClicked(m_pCurrentItem, m_curCol);
	}
}

void KexiTableView::contentsMousePressEvent( QMouseEvent* e )
{
	if(m_numRows == 0)
		return;

	// get rid of editor
	if (m_pEditor)
		editorOk();

	// remember old focus cell
	int oldRow = m_curRow;
	int oldCol = m_curCol;

	// get new focus cell
	m_curRow = rowAt(e->pos().y());
	m_curCol = columnAt(e->pos().x());

	if(m_curRow == -1)
		m_curRow = m_numRows-1;
	
	if(m_curCol == -1)
		m_curCol = m_numCols-1;

	//	if we have a new focus cell, repaint
	if ( m_curRow != oldRow || m_curCol != oldCol )
	{
		int cw = columnWidth( m_curCol );
		int rh = rowHeight( m_curRow );
		updateCell( m_curRow, m_curCol );
		ensureVisible( columnPos( m_curCol ) + cw / 2, rowPos( m_curRow ) + rh / 2, cw / 2, rh / 2 );
		updateCell( oldRow, oldCol );
		m_pVerticalHeader->setCurrentRow(m_curRow);
		m_pCurrentItem = itemAt(m_curRow);
		emit itemSelected(m_pCurrentItem);
	}
	if(m_pContextMenu && e->button() == RightButton)
		m_pContextMenu->exec(QCursor::pos());
}

void KexiTableView::contentsMouseMoveEvent( QMouseEvent *e )
{
	// do the same as in mouse press
	int x,y;
	contentsToViewport(e->x(), e->y(), x, y);

	if(y > visibleHeight())
	{
		m_needAutoScroll = true;
		m_scrollTimer->start(70, false);
		m_scrollDirection = ScrollDown;
	}
	else if(y < 0)
	{
		m_needAutoScroll = true;
		m_scrollTimer->start(70, false);
		m_scrollDirection = ScrollUp;
	}
	else if(x > visibleWidth())
	{
		m_needAutoScroll = true;
		m_scrollTimer->start(70, false);
		m_scrollDirection = ScrollRight;
	}
	else if(x < 0)
	{
		m_needAutoScroll = true;
		m_scrollTimer->start(70, false);
		m_scrollDirection = ScrollLeft;
	}
	else
	{
		m_needAutoScroll = false;
		m_scrollTimer->stop();
		contentsMousePressEvent(e);
	}

}

void KexiTableView::contentsMouseReleaseEvent(QMouseEvent *e)
{
	if(m_needAutoScroll)
	{
		m_scrollTimer->stop();
	}
}

void KexiTableView::keyPressEvent(QKeyEvent* e)
{
	qDebug("KexiTableView::KeyPressEvent()");
	// if a cell is just editing, do some special stuff
	if(m_pEditor)
	{
		if (e->key() == Key_Escape)
			editorCancel();
		else if (e->key() == Key_Return || e->key() == Key_Enter)
			editorOk();
		return;
	}

	if(e->key() == Key_Return || e->key() == Key_Enter)
	{
		emit itemReturnPressed(m_pCurrentItem, m_curCol);
	}

	int oldRow = m_curRow;
	int oldCol = m_curCol;

	// navigate in the header...
    switch (e->key())
    {
    case Key_Left:
		m_curCol = QMAX(0, m_curCol - 1);
		break;
    case Key_Right:
    case Key_Tab:
		m_curCol = QMIN(cols() - 1, m_curCol + 1);
		break;
    case Key_Up:
		m_curRow = QMAX(0, m_curRow - 1);
		break;
    case Key_Down:
		m_curRow = QMIN(rows() - 1, m_curRow + 1);
		break;
    case Key_Prior:
		m_curRow -= visibleHeight() / m_rowHeight;
		m_curRow = QMAX(0, m_curRow);
		break;
    case Key_Next:
		m_curRow += visibleHeight() / m_rowHeight;
		m_curRow = QMIN(rows()-1, m_curRow);
		break;
    case Key_Home:
		m_curRow = 0;
		break;
    case Key_End:
		m_curRow = m_numRows-1;
		break;

    case Key_Shift:
    		e->ignore();
        	break;

	#warning this needs work!
	case Key_Return:
		if(columnType(m_curCol) != QVariant::Bool && columnEditable(m_curCol))
			createEditor(m_curRow, m_curCol, "", false);
		break;

	case Key_Backspace:
		if(columnType(m_curCol) != QVariant::Bool && columnEditable(m_curCol))
			createEditor(m_curRow, m_curCol, QString::null, true);
		break;
	case Key_Space:
		if(columnType(m_curCol) == QVariant::Bool && columnEditable(m_curCol))
		{
			boolToggled();
			break;
		}
    default:
   	 qDebug("KexiTableView::KeyPressEvent(): default");
		if(columnType(m_curCol) != QVariant::Bool && columnEditable(m_curCol))
		{
			qDebug("KexiTableView::KeyPressEvent(): ev pressed");
//			if (e->text()[0].isPrint())
				createEditor(m_curRow, m_curCol, e->text(), false);
		}
	}
	// if focus cell changes, repaint
	if ( m_curRow != oldRow || m_curCol != oldCol )
	{
		int cw = columnWidth(m_curCol);

		ensureVisible(columnPos(m_curCol) + cw / 2, rowPos(m_curRow) + m_rowHeight / 2, cw / 2, m_rowHeight / 2);
		updateCell(oldRow, oldCol);
		updateCell(m_curRow, m_curCol);
		m_pVerticalHeader->setCurrentRow(m_curRow);
		m_pCurrentItem = itemAt(m_curRow);
		emit itemSelected(m_pCurrentItem);
	}
}

void KexiTableView::emitSelected()
{
	if(m_pCurrentItem)
		emit itemSelected(m_pCurrentItem);
}

void KexiTableView::boolToggled()
{
	int s = m_pCurrentItem->getInt(m_curCol);
	m_pCurrentItem->setInt(m_curCol, (s ? 0 : 1));
	updateCell(m_curRow, m_curCol);
	emit itemChanged(m_pCurrentItem, m_curCol);
}

void KexiTableView::selectNext()
{
	int oldRow = m_curRow;
	m_curRow = QMIN( rows() - 1, m_curRow + 1 );

	if(m_curRow != oldRow)
	{
		int rh = rowHeight(m_curRow);

		ensureVisible(0, rowPos(m_curRow) + rh / 2, 0, rh / 2);
		if (hasFocus() || viewport()->hasFocus())
		{
			updateCell(oldRow, m_curCol);
			updateCell(m_curRow, m_curCol);
		}
		m_pVerticalHeader->setCurrentRow(m_curRow);
		m_pCurrentItem = itemAt(m_curRow);
		emit itemSelected(m_pCurrentItem);
	}
}

void KexiTableView::selectPrev()
{
	int oldRow = m_curRow;

	m_curRow = QMAX( 0, m_curRow - 1 );
	if(m_curRow != oldRow)
	{
		int rh = rowHeight(m_curRow);

		ensureVisible(0, rowPos(m_curRow) + rh / 2, 0, rh / 2);
		if (hasFocus() || viewport()->hasFocus())
		{
			updateCell(oldRow, m_curCol);
			updateCell(m_curRow, m_curCol);
		}
		m_pVerticalHeader->setCurrentRow(m_curRow);
		m_pCurrentItem = itemAt(m_curRow);
		emit itemSelected(m_pCurrentItem);
	}
}

void KexiTableView::createEditor(int row, int col, QString addText/* = QString::null*/, bool backspace/* = false*/)
{
//	QString val;
	QVariant val;
	switch(columnType(col))
	{
		case QVariant::Date:
			#ifdef USE_KDE
//			val = KGlobal::_locale->formatDate(m_pCurrentItem->getDate(col), true);

			#else
//			val = m_pCurrentItem->getDate(col).toString(Qt::LocalDate);
			#endif
			break;

		default:
//			val = m_pCurrentItem->getText(m_curCol);
			val = m_pCurrentItem->getValue(m_curCol);

			break;
	}


	//it's getting ugly :)

	switch(columnType(col))
	{
		case QVariant::Date:
			#ifdef USE_KDE
			m_pEditor = new KexiDateTableEdit(val, viewport(), "inPlaceEd");
			qDebug("date editor created...");
			break;
			#endif

		default:
//			m_pEditor = new QLineEdit(val + addText, viewport(), "inPlaceEd");
			m_pEditor = new KexiInputTableEdit(val, columnType(col), addText, false, viewport(), "inPlaceEd");
			m_pEditor->end(false);
			if(backspace)
				m_pEditor->backspace();

			break;
	}

	m_pEditor->resize(columnWidth(m_curCol)-1, rowHeight(m_curRow)-1);
	moveChild(m_pEditor, columnPos(m_curCol), rowPos(m_curRow));
	QPalette p(m_pEditor->palette());
	p.setColor(QColorGroup::Base, QColor(200,200,255));
	m_pEditor->setPalette(p);
	m_pEditor->setFrame(false);
	m_pEditor->show();
	m_pEditor->setFocus();
}

void KexiTableView::focusInEvent(QFocusEvent*)
{
	updateCell(m_curRow, m_curCol);
}


void KexiTableView::focusOutEvent(QFocusEvent*)
{
	updateCell(m_curRow, m_curCol);
}

bool KexiTableView::focusNextPrevChild(bool next)
{
	if (m_pEditor)
		return true;
	return QScrollView::focusNextPrevChild(next);
}

void KexiTableView::resizeEvent(QResizeEvent *e)
{
	QScrollView::resizeEvent(e);
	updateGeometries();
}

void KexiTableView::showEvent(QShowEvent *e)
{
	QScrollView::showEvent(e);
	QRect r(cellGeometry(rows() - 1, cols() - 1 ));
	resizeContents(r.right() + 1, r.bottom() + 1);
	updateGeometries();
}

void KexiTableView::updateCell(int row, int col)
{
	QRect r(cellGeometry(row, col));
	updateContents(r);
}

void KexiTableView::columnSort(int col)
{
	if(m_sortedColumn == col)
		m_sortOrder = !m_sortOrder;
	else
		m_sortOrder = true;
	m_sortedColumn = col;
	m_pTopHeader->setSortIndicator(col, m_sortOrder);
	m_contents.setSorting(col, m_sortOrder, columnType(col));
	sort();
//	updateContents( 0, 0, viewport()->width(), viewport()->height());
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
		updateContents(columnPos(col), 0, viewport()->width(), contentsHeight());
	//	viewport()->repaint();
	if (m_pEditor)
	{
		m_pEditor->resize(columnWidth(m_curCol)-1, rowHeight(m_curRow)-1);
		moveChild(m_pEditor, columnPos(m_curCol), rowPos(m_curRow));
	}
	updateGeometries();
}

void KexiTableView::updateGeometries()
{
	QSize ts = tableSize();
	if (m_pTopHeader->offset() && ts.width() < m_pTopHeader->offset() + m_pTopHeader->width())
		horizontalScrollBar()->setValue(ts.width() - m_pTopHeader->width());

//	m_pVerticalHeader->setGeometry(1, topMargin() + 1, leftMargin(), visibleHeight());
	m_pTopHeader->setGeometry(leftMargin() + 1, 1, visibleWidth(), topMargin());
	m_pVerticalHeader->setGeometry(1, topMargin() + 1, leftMargin(), visibleHeight());
}

int KexiTableView::columnWidth(int col) const
{
    return m_pTopHeader->sectionSize(col);
}

int KexiTableView::rowHeight(int row) const
{
	return m_rowHeight;
}

int KexiTableView::columnPos(int col) const
{
    return m_pTopHeader->sectionPos(col);
}

int KexiTableView::rowPos(int row) const
{
	return m_rowHeight*row;
}

int KexiTableView::columnAt(int pos) const
{
    return m_pTopHeader->sectionAt(pos);
}

int KexiTableView::rowAt(int pos) const
{
	pos /=m_rowHeight;
	if (pos < 0)
		return 0;
	if (pos >= m_numRows)
		return -1;
	return pos;
}

QRect KexiTableView::cellGeometry(int row, int col) const
{
    return QRect(columnPos(col), rowPos(row),
		  columnWidth(col), rowHeight(row));
}

QSize KexiTableView::tableSize() const
{
	if (rows() > 0 && cols() > 0)
	    return QSize( columnPos( cols() - 1 ) + columnWidth( cols() - 1 ),
			rowPos( rows()-1 ) + rowHeight( rows() - 1 ));
	return QSize(0,0);
}

int KexiTableView::rows() const
{
	return m_numRows;
}

int KexiTableView::cols() const
{
    return m_pTopHeader->count();
}

void KexiTableView::setCursor(int row, int col/*=-1*/)
{
	// get rid of editor
	if (m_pEditor)
		editorOk();
	if(rows() <= 0)
	{
		m_curRow=0;
		m_pCurrentItem=0;
		m_pVerticalHeader->setCurrentRow(-1);
		return;
	}

	int oldRow = m_curRow;
	int oldCol = m_curCol;

	if(col>=0)
	{
		m_curCol = QMAX(0, col);
		m_curCol = QMIN(cols() - 1, m_curCol);
	}
	m_curRow = QMAX( 0, row);
	m_curRow = QMIN( rows() - 1, m_curRow);

	m_pCurrentItem = itemAt(m_curRow);
	if ( m_curRow != oldRow || m_curCol != oldCol )
	{
//		int cw = columnWidth( m_curCol );
		int rh = rowHeight( m_curRow );
//		ensureVisible( columnPos( m_curCol ) + cw / 2, rowPos( m_curRow ) + rh / 2, cw / 2, rh / 2 );
//		center(columnPos(m_curCol) + cw / 2, rowPos(m_curRow) + rh / 2, cw / 2, rh / 2);
		ensureVisible(columnPos(m_curCol), rowPos(m_curRow), columnWidth(m_curCol), rh);
		updateCell( oldRow, oldCol );
		updateCell( m_curRow, m_curCol );
		m_pVerticalHeader->setCurrentRow(m_curRow);
		emit itemSelected(m_pCurrentItem);
	}
}

void KexiTableView::editorOk()
{
	if (!m_pEditor)
		return;
//	m_pCurrentItem->setText(m_curCol, m_pEditor->text());
	m_pCurrentItem->setValue(m_curCol, m_pEditor->value());
	editorCancel();
	emit itemChanged(m_pCurrentItem, m_curCol);
}

void KexiTableView::editorCancel()
{
	if (!m_pEditor)
		return;

	delete m_pEditor;
	m_pEditor = 0;
	viewport()->setFocus();
}

void KexiTableView::setAdditionPolicy(AdditionPolicy policy)
{
	m_additionPolicy = policy;
	updateContextMenu();
}

KexiTableView::AdditionPolicy KexiTableView::additionPolicy()
{
	return m_additionPolicy;
}

void KexiTableView::setDeletionPolicy(DeletionPolicy policy)
{
	m_deletionPolicy = policy;
	updateContextMenu();
}

KexiTableView::DeletionPolicy KexiTableView::deletionPolicy()
{
	return m_deletionPolicy;
}

void KexiTableView::updateContextMenu()
{
	if(m_pContextMenu)
		delete m_pContextMenu;
	if(m_additionPolicy != NoAdd || m_deletionPolicy != NoDelete)
	{
		m_pContextMenu = new QPopupMenu(this);
		if(m_additionPolicy != NoAdd)
			m_pContextMenu->insertItem(tr("&neuer datensatz"), this, SLOT(addRecord()), ALT+Key_Insert);
		if(m_deletionPolicy != NoDelete)
			m_pContextMenu->insertItem(tr("datensatz &löschen"), this, SLOT(removeRecord()), ALT+Key_Delete);
	}
}

void KexiTableView::slotAutoScroll()
{
	if(m_needAutoScroll)
	{
		switch(m_scrollDirection)
		{
			case ScrollDown:
				setCursor(m_curRow + 1, m_curCol);
				break;

			case ScrollUp:
				setCursor(m_curRow - 1, m_curCol);
				break;
			case ScrollLeft:
				setCursor(m_curRow, m_curCol - 1);
				break;

			case ScrollRight:
				setCursor(m_curRow, m_curCol + 1);
				break;
		}
	}
}

void KexiTableView::inserted()
{
	if(itemAt(m_numRows)->isInsertItem())
		m_pVerticalHeader->addLabel("*", m_rowHeight);
	else
		m_pVerticalHeader->addLabel("", m_rowHeight);

}

KexiTableHeader* KexiTableView::recordMarker()
{
	return m_pVerticalHeader;
}

#include "kexitableview.moc"
