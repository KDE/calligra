/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
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
*/

#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapp.h>

#include <kdebug.h>
#include <kiconloader.h>

#include <kexiDB/kexidbtable.h>
#include <kexiDB/kexidbfield.h>
#include "kexirelationviewtable.h"
#include "kexirelationview.h"

#include <kexidragobjects.h>

#include <stdlib.h>

#if defined(Q_WS_WIN)
#include "qt_windows.h"
QRgb qt_colorref2qrgb(COLORREF col)
{
    return qRgb(GetRValue(col),GetGValue(col),GetBValue(col));
}
#endif

KexiRelationViewTableContainer::KexiRelationViewTableContainer(KexiRelationView *parent, QString table, const KexiDBTable *t)
 : QFrame(parent,"tv", QFrame::Panel | QFrame::Raised), m_mousePressed(false)
{
//	setFixedSize(100, 150);
//js:	resize(100, 150);
	//setMouseTracking(true);
	setFrameStyle(QFrame::WinPanel | QFrame::Raised);

	QVBoxLayout *lyr = new QVBoxLayout(this,2,1); //js: using Q*BoxLayout is a good idea
	lyr->setMargin(3);

	m_tableHeader = new KexiRelationViewTableContainerHeader(table, this);
	m_tableHeader->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	lyr->addWidget(m_tableHeader);
	connect(m_tableHeader,SIGNAL(moved()),this,SLOT(moved()));

	m_tableView = new KexiRelationViewTable(this, parent, table, t, "tbl-list");
	lyr->addWidget(m_tableView, 1);
	connect(m_tableView, SIGNAL(tableScrolling()), this, SLOT(moved()));
}

void KexiRelationViewTableContainer::moved() {
//	kdDebug()<<"finally emitting moved"<<endl;
	emit moved(this);
}

int KexiRelationViewTableContainer::globalY(const QString &field)
{
//	kdDebug() << "KexiRelationViewTableContainer::globalY()" << endl;
	QPoint o = mapFromGlobal(QPoint(0, (m_tableView->globalY(field))));
//	kdDebug() << "KexiRelationViewTableContainer::globalY() db2" << endl;
	return mapToParent(o).y();
}

void
KexiRelationViewTableContainer::mousePressEvent(QMouseEvent *ev)
{
	m_mousePressed = true;
	m_bX = ev->x();
	m_bY = ev->y();

	QFrame::mousePressEvent(ev);
}

void
KexiRelationViewTableContainer::mouseMoveEvent(QMouseEvent *ev)
{
	return;
	if(m_mousePressed)
	{
//		move(ev->x() - m_bX, ev->y() - m_bY);
		if(m_bY < m_tableHeader->height())
		{
			QPoint movePoint(ev->x() - m_bX, ev->y() - m_bY);
			move(mapToParent(movePoint));
		}
		else if(ev->x() >= width() - 3)
		{
			if(ev->y() >= height() - 6)
				resize(ev->x(), ev->y());
			else
				resize(ev->x(), height());
		}

		emit moved(this);
	}
	else
	{
		if(ev->x() >= width() - 3)
		{
			if(ev->y() >= height() - 3)
				setCursor(QCursor(SizeFDiagCursor));
			else
				setCursor(QCursor(SizeHorCursor));
		}
		else
		{
			setCursor(QCursor(ArrowCursor));
		}
	}

	QFrame::mouseMoveEvent(ev);
}

void
KexiRelationViewTableContainer::mouseReleaseEvent(QMouseEvent *ev)
{
	m_mousePressed = false;
	QFrame::mouseMoveEvent(ev);
}

const QString
KexiRelationViewTableContainer::table()
{
	return m_tableView->table();
}

KexiRelationViewTableContainer::~KexiRelationViewTableContainer()
{
}


//END KexiRelationViewTableContainer

//BEGIN KexiRelatoinViewTableContainerHeader

KexiRelationViewTableContainerHeader::KexiRelationViewTableContainerHeader(const QString& text,QWidget *parent)
	:QLabel(text,parent),m_dragging(false) {

	QPalette pal = palette();

	bool colorsInitialized = FALSE;

#ifdef Q_WS_WIN // ask system properties on windows
#ifndef SPI_GETGRADIENTCAPTIONS
#define SPI_GETGRADIENTCAPTIONS 0x1008
#endif
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION 27
#endif
#ifndef COLOR_GRADIENTINACTIVECAPTION
#define COLOR_GRADIENTINACTIVECAPTION 28
#endif
	if ( QApplication::desktopSettingsAware() ) {
		//TODO: some day gradient can be added for w98/nt5
		pal.setColor( QPalette::Active, QColorGroup::Background, qt_colorref2qrgb(GetSysColor(COLOR_ACTIVECAPTION)) );
		pal.setColor( QPalette::Inactive, QColorGroup::Background, qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTION)) );
		pal.setColor( QPalette::Active, QColorGroup::Foreground, qt_colorref2qrgb(GetSysColor(COLOR_CAPTIONTEXT)) );
		pal.setColor( QPalette::Inactive, QColorGroup::Foreground, qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTIONTEXT)) );
	}
	else
#endif //Q_WS_WIN
	{
		//TODO: check if this is ok under linux:
		pal.setColor( QPalette::Active, QColorGroup::Background, palette().active().highlight() );
		pal.setColor( QPalette::Active, QColorGroup::Foreground, palette().active().highlightedText() );
		pal.setColor( QPalette::Inactive, QColorGroup::Background, palette().inactive().highlight() );
		pal.setColor( QPalette::Inactive, QColorGroup::Foreground, palette().inactive().highlightedText() );
	}
	setPalette( pal );

	installEventFilter(this);
}

KexiRelationViewTableContainerHeader::~KexiRelationViewTableContainerHeader() {
}

bool KexiRelationViewTableContainerHeader::eventFilter(QObject *obj, QEvent *ev) {
	if (ev->type()==QEvent::MouseMove)
	{
		if (m_dragging) {
			int diffX,diffY;
			diffX=static_cast<QMouseEvent*>(ev)->globalPos().x()-m_grabX;
			diffY=static_cast<QMouseEvent*>(ev)->globalPos().y()-m_grabY;
			if ((abs(diffX)>2) || (abs(diffY)>2))
			{
				QPoint newPos=parentWidget()->pos()+QPoint(diffX,diffY);

//correct the x position
				if (newPos.x()<0) {
					m_offsetX+=newPos.x();
					newPos.setX(0);
				}
				else
				if (m_offsetX<0) {
					m_offsetX+=newPos.x();
					if (m_offsetX>0) {
						newPos.setX(m_offsetX);
						m_offsetX=0;
					}
					else newPos.setX(0);
				}
//correct the y position
				if (newPos.y()<0) {
					m_offsetY+=newPos.y();
					newPos.setY(0);
				}
				else
				if (m_offsetY<0) {
					m_offsetY+=newPos.y();
					if (m_offsetY>0) {
						newPos.setY(m_offsetY);
						m_offsetY=0;
					}
					else newPos.setY(0);
				}
//move and update helpers

				parentWidget()->move(newPos);
				m_grabX=static_cast<QMouseEvent*>(ev)->globalPos().x();
				m_grabY=static_cast<QMouseEvent*>(ev)->globalPos().y();
//				kdDebug()<<"HEADER:emitting moved"<<endl;
				emit moved();
			}
			return true;
		}
	}
	return false;
}

void KexiRelationViewTableContainerHeader::mousePressEvent(QMouseEvent *ev) {
	kdDebug()<<"KexiRelationViewTableContainerHeader::Mouse Press Event"<<endl;
	m_dragging=true;
	m_grabX=ev->globalPos().x();
	m_grabY=ev->globalPos().y();
	m_offsetX=0;
	m_offsetY=0;
}

void KexiRelationViewTableContainerHeader::mouseReleaseEvent(QMouseEvent *ev) {
	kdDebug()<<"KexiRelationViewTableContainerHeader::Mouse Release Event"<<endl;
	m_dragging=false;
}

//END KexiRelatoinViewTableContainerHeader




KexiRelationViewTable::KexiRelationViewTable(QWidget *parent, KexiRelationView *view, QString table,
                                             const KexiDBTable *t, const char *name)
 : KListView(parent)
{
//	m_fieldList = t.;
	m_table = table;
//	m_parent = parent;

	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setDropVisualizer(false);
	setAllColumnsShowFocus(true);

	addColumn("", 0);
	addColumn("fields");
	setResizeMode(QListView::LastColumn);
//	setResizeMode(QListView::AllColumns);
	header()->hide();

	setSorting(-1, true); // disable sorting

	int order=0;

	KListViewItem *item = 0;
	for(uint i=0; i < t->fieldCount(); i++)
	{
		KexiDBField f = t->field(i);
		item = item ? new KListViewItem(this, item, QString::number(order), f.name())
			: new KListViewItem(this, QString::number(order), f.name());
		if(f.primary_key() || f.unique_key())
			item->setPixmap(1, SmallIcon("key"));

		item->setDragEnabled(true);
		item->setDropEnabled(true);
		order++;
	}

//	setDragEnabled
	connect(this, SIGNAL(dropped(QDropEvent *, QListViewItem *)), this, SLOT(slotDropped(QDropEvent *)));
	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotContentsMoving(int,int)));

	m_view = view;
}

void KexiRelationViewTable::setReadOnly(bool b)
{
	setAcceptDrops(!b);
	viewport()->setAcceptDrops(!b);
}

int
KexiRelationViewTable::globalY(const QString &item)
{
	QListViewItem *i = findItem(item, 1);
	if(i)
	{
		int y=itemRect(i).y() + (itemRect(i).height() / 2);
		return mapToGlobal(QPoint(0, y)).y();
	}
	return -1;
}

QDragObject *
KexiRelationViewTable::dragObject()
{
//	qDebug("KexiRelationViewTable::dragObject()");
	if(selectedItem())
	{
		KexiFieldDrag *drag = new KexiFieldDrag(
			"kexi/table",m_table,selectedItem()->text(1), this, "metaDrag");
		return drag;
	}

	return 0;
}

bool
KexiRelationViewTable::acceptDrag(QDropEvent *ev) const
{
//	kdDebug() << "KexiRelationViewTable::acceptDrag()" << endl;
	if(ev->provides("kexi/field") && ev->source() != (QWidget*)this)
		return true;

	return false;
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
//	qDebug("KexiRelationViewTable::slotDropped()");

	QListViewItem *recever = itemAt(ev->pos());
	if(recever)
	{
		QString srcTable;
		QString dummy;
		QString srcField;
		//better check later if the source is really a table
		KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
//		kdDebug() << "KexiRelationViewTable::slotDropped() srcfield: " << srcField << endl;

		QString rcvField = recever->text(1);

		SourceConnection s;
		s.srcTable = srcTable;
		s.rcvTable = m_table;
		s.srcField = srcField;
		s.rcvField = rcvField;

//		m_parent->addConnection(s);
		m_view->addConnection(s, false);

		kdDebug() << "KexiRelationViewTable::slotDropped() " << srcTable << ":" << srcField << " " << m_table << ":" << rcvField << endl;
		ev->accept();
		return;
	}
	ev->ignore();

}

void
KexiRelationViewTable::slotContentsMoving(int,int)
{
	emit tableScrolling();
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}

#include "kexirelationviewtable.moc"
