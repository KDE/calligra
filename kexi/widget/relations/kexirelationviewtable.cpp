/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include <stdlib.h>

#include <q3header.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qstyle.h>
#include <QEvent>
#include <QDropEvent>
#include <Q3VBoxLayout>
#include <QMouseEvent>
#include <QStyleOptionFocusRect>

#include <kdebug.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kconfig.h>
#include <kglobalsettings.h>

#include <kexidb/tableschema.h>
#include <kexidb/utils.h>
#include <kexidragobjects.h>
#include "kexirelationviewtable.h"
#include "kexirelationview.h"

KexiRelationViewTableContainer::KexiRelationViewTableContainer(
	KexiRelationView *parent, KexiDB::TableOrQuerySchema *schema)
 : QFrame(parent)
// , m_table(t)
 , m_parent(parent)
//	, m_mousePressed(false)
{
	setObjectName("KexiRelationViewTableContainer");

//	setFixedSize(100, 150);
//js:	resize(100, 150);
	//setMouseTracking(true);

	setFrameStyle( QFrame::WinPanel | QFrame::Raised );

	Q3VBoxLayout *lyr = new Q3VBoxLayout(this,4,1); //js: using Q*BoxLayout is a good idea

	m_tableHeader = new KexiRelationViewTableContainerHeader(schema->name(), this);

	m_tableHeader->unsetFocus();
	m_tableHeader->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	lyr->addWidget(m_tableHeader);
	connect(m_tableHeader,SIGNAL(moved()),this,SLOT(moved()));
	connect(m_tableHeader, SIGNAL(endDrag()), this, SIGNAL(endDrag()));

	m_tableView = new KexiRelationViewTable(schema, parent, this);
	m_tableView->setObjectName("KexiRelationViewTable");
	//m_tableHeader->setFocusProxy( m_tableView );
	m_tableView->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	
	m_tableView->setMaximumSize( m_tableView->sizeHint() );
	
//	m_tableView->resize( m_tableView->sizeHint() );
	lyr->addWidget(m_tableView, 0);
	connect(m_tableView, SIGNAL(tableScrolling()), this, SLOT(moved()));
	connect(m_tableView, SIGNAL(contextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)),
		this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)));
}

KexiRelationViewTableContainer::~KexiRelationViewTableContainer()
{
}

KexiDB::TableOrQuerySchema* KexiRelationViewTableContainer::schema() const
{
	return m_tableView->schema();
}

void KexiRelationViewTableContainer::slotContextMenu(K3ListView *, Q3ListViewItem *, 
	const QPoint &p)
{
//	m_parent->executePopup(p);
	emit contextMenuRequest( p );
}

void KexiRelationViewTableContainer::moved() {
//	kDebug()<<"finally emitting moved"<<endl;
	emit moved(this);
}

int KexiRelationViewTableContainer::globalY(const QString &field)
{
//	kDebug() << "KexiRelationViewTableContainer::globalY()" << endl;
//	QPoint o = mapFromGlobal(QPoint(0, (m_tableView->globalY(field))/*+m_parent->contentsY()*/));

	QPoint o(0, (m_tableView->globalY(field)) + m_parent->contentsY());
//	kDebug() << "KexiRelationViewTableContainer::globalY() db2" << endl;
	return m_parent->viewport()->mapFromGlobal(o).y();
}

#if 0//js
QSize KexiRelationViewTableContainer::sizeHint()
{
#ifdef Q_WS_WIN
	QSize s = m_tableView->sizeHint()
		+ QSize(  2 * 5 , m_tableHeader->height() + 2 * 5 );
#else
	QSize s = m_tableView->sizeHint();
	s.setWidth(s.width() + 4);
	s.setHeight(m_tableHeader->height() + s.height());
#endif
	return s;
}
#endif

void KexiRelationViewTableContainer::setFocus()
{
	kDebug() << "SET FOCUS" << endl;
	//select 1st:
	if (m_tableView->firstChild()) {
		if (!m_tableView->selectedItems().first())
			m_tableView->setSelected( m_tableView->firstChild(), true );
	}
	m_tableHeader->setFocus();
	m_tableView->setFocus();
/*	QPalette p = qApp->palette();
	p.setColor( QPalette::Active, QColorGroup::Highlight, KGlobalSettings::highlightColor() );
	p.setColor( QPalette::Active, QColorGroup::HighlightedText, KGlobalSettings::highlightedTextColor() );
	m_tableView->setPalette(p);*/

	raise();
	repaint();
	emit gotFocus();
}

void KexiRelationViewTableContainer::unsetFocus()
{
	kDebug() << "UNSET FOCUS" << endl;
//	if (m_tableView->selectedItem()) //unselect item if was selected
//		m_tableView->setSelected(m_tableView->selectedItem(), false);
//	m_tableView->clearSelection();
	m_tableHeader->unsetFocus();

	m_tableView->clearSelection();

//	m_tableView->unsetPalette();
/*	QPalette p = m_tableView->palette();
//	p.setColor( QPalette::Active, QColorGroup::Highlight, KGlobalSettings::highlightColor() );
//	p.setColor( QPalette::Active, QColorGroup::HighlightedText, KGlobalSettings::highlightedTextColor() );
	p.setColor( QPalette::Active, QColorGroup::Highlight, p.color(QPalette::Active, QColorGroup::Background ) );
//	p.setColor( QPalette::Active, QColorGroup::Highlight, gray );
	p.setColor( QPalette::Active, QColorGroup::HighlightedText, p.color(QPalette::Active, QColorGroup::Foreground ) );
//	p.setColor( QPalette::Active, QColorGroup::Highlight, green );
//	p.setColor( QPalette::Active, QColorGroup::HighlightedText, blue );
	m_tableView->setPalette(p);*/
	
	clearFocus();
	repaint();
}


//END KexiRelationViewTableContainer

//============================================================================
//BEGIN KexiRelatoinViewTableContainerHeader

KexiRelationViewTableContainerHeader::KexiRelationViewTableContainerHeader(
	const QString& text,QWidget *parent)
	:QLabel(text,parent),m_dragging(false) 
{
	setMargin(1);
	m_activeBG = KGlobalSettings::activeTitleColor();
	m_activeFG = KGlobalSettings::activeTextColor();
	m_inactiveBG = KGlobalSettings::inactiveTitleColor();
	m_inactiveFG = KGlobalSettings::inactiveTextColor();

	installEventFilter(this);
}

KexiRelationViewTableContainerHeader::~KexiRelationViewTableContainerHeader() 
{
}

void KexiRelationViewTableContainerHeader::setFocus()
{
	setPaletteBackgroundColor(m_activeBG);
	setPaletteForegroundColor(m_activeFG);
}

void KexiRelationViewTableContainerHeader::unsetFocus()
{
	setPaletteBackgroundColor(m_inactiveBG);
	setPaletteForegroundColor(m_inactiveFG);
}

bool KexiRelationViewTableContainerHeader::eventFilter(QObject *, QEvent *ev) 
{
	if (ev->type()==QEvent::MouseMove)
	{
		if (m_dragging && static_cast<QMouseEvent*>(ev)->modifiers()==Qt::LeftButton) {
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
				else if (m_offsetX<0) {
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
//				kDebug()<<"HEADER:emitting moved"<<endl;
				emit moved();
			}
			return true;
		}
	}
	return false;
}

void KexiRelationViewTableContainerHeader::mousePressEvent(QMouseEvent *ev) {
	kDebug()<<"KexiRelationViewTableContainerHeader::Mouse Press Event"<<endl;
	parentWidget()->setFocus();
	ev->accept();
	if (ev->button()==Qt::LeftButton) {
		m_dragging=true;
		m_grabX=ev->globalPos().x();
		m_grabY=ev->globalPos().y();
		m_offsetX=0;
		m_offsetY=0;
		setCursor(Qt::SizeAllCursor);
		return;
	}
	if (ev->button()==Qt::RightButton) {
		emit static_cast<KexiRelationViewTableContainer*>(parentWidget())
			->contextMenuRequest(ev->globalPos());
	}
//	QLabel::mousePressEvent(ev);
}

void KexiRelationViewTableContainerHeader::mouseReleaseEvent(QMouseEvent *ev) {
	kDebug()<<"KexiRelationViewTableContainerHeader::Mouse Release Event"<<endl;
	if (m_dragging && ev->button() & Qt::LeftButton) {
		setCursor(Qt::ArrowCursor);
		m_dragging=false;
		emit endDrag();
	}
	ev->accept();
}

//END KexiRelatoinViewTableContainerHeader


//=====================================================================================

KexiRelationViewTable::KexiRelationViewTable(KexiDB::TableOrQuerySchema* tableOrQuerySchema, 
	KexiRelationView *view, QWidget *parent)
 : KexiFieldListView(parent, KexiFieldListView::ShowAsterisk)
 , m_view(view)
{
	setSchema(tableOrQuerySchema);
	header()->hide();

	connect(this, SIGNAL(dropped(QDropEvent *, Q3ListViewItem *)),
		this, SLOT(slotDropped(QDropEvent *)));
	connect(this, SIGNAL(contentsMoving(int, int)),
		this, SLOT(slotContentsMoving(int,int)));
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}

QSize KexiRelationViewTable::sizeHint() const
{
	QFontMetrics fm(fontMetrics());

//	kdDebug() << schema()->name() << " cw=" << columnWidth(0) + fm.width("i") 
//		<< ", " << fm.width(schema()->name()+"  ") << endl; 

	int maxWidth = -1;
	const int iconWidth = IconSize(K3Icon::Small) + fm.width("i")+20;
	for (Q3ListViewItem *item = firstChild(); item; item = item->nextSibling())
		maxWidth = qMax(maxWidth, iconWidth + fm.width(item->text(0)));

	const uint rowCount = qMin( 8, childCount() );

	QSize s(
		qMax( maxWidth, fm.width(schema()->name()+" ")), 
		rowCount*firstChild()->totalHeight() + 4 );
	return s;
}

#if 0
void KexiRelationViewTable::setReadOnly(bool b)
{
	setAcceptDrops(!b);
	viewport()->setAcceptDrops(!b);
}
#endif

int
KexiRelationViewTable::globalY(const QString &item)
{
	Q3ListViewItem *i = findItem(item, 0);
	if (!i)
		return -1;
	int y = itemRect(i).y() + (itemRect(i).height() / 2);
	if (contentsY() > itemPos(i))
		y = 0;
	else if (y == 0)
		y = height();
	return mapToGlobal(QPoint(0, y)).y();
}

bool
KexiRelationViewTable::acceptDrag(QDropEvent *ev) const
{
//	kDebug() << "KexiRelationViewTable::acceptDrag()" << endl;
	Q3ListViewItem *receiver = itemAt(ev->pos() - QPoint(0,contentsY()));
	if (!receiver || !KexiFieldDrag::canDecodeSingle(ev))
		return false;
	QString sourceMimeType;
	QString srcTable;
	QString srcField;
	if (!KexiFieldDrag::decodeSingle(ev,sourceMimeType,srcTable,srcField))
		return false;
	if (sourceMimeType!="kexi/table" && sourceMimeType=="kexi/query")
		return false;
	QString f = receiver->text(0).trimmed();
	if (!srcField.trimmed().startsWith("*") && !f.startsWith("*") && ev->source() != (QWidget*)this)
		return true;

	return false;
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
	Q3ListViewItem *recever = itemAt(ev->pos() - QPoint(0,contentsY()));
	if (!recever || !KexiFieldDrag::canDecodeSingle(ev)) {
		ev->ignore();
		return;
	}
	QString sourceMimeType;
	QString srcTable;
	QString srcField;
	if (!KexiFieldDrag::decodeSingle(ev,sourceMimeType,srcTable,srcField))
		return;
	if (sourceMimeType!="kexi/table" && sourceMimeType=="kexi/query")
		return;
//		kDebug() << "KexiRelationViewTable::slotDropped() srcfield: " << srcField << endl;

	QString rcvField = recever->text(0);

	SourceConnection s;
	s.masterTable = srcTable;
	s.detailsTable = schema()->name();
	s.masterField = srcField;
	s.detailsField = rcvField;

	m_view->addConnection(s);

	kDebug() << "KexiRelationViewTable::slotDropped() " << srcTable << ":" << srcField << " " 
		<< schema()->name() << ":" << rcvField << endl;
	ev->accept();
}

void
KexiRelationViewTable::slotContentsMoving(int,int)
{
	emit tableScrolling();
}

void KexiRelationViewTable::contentsMousePressEvent(QMouseEvent *ev)
{
	parentWidget()->setFocus();
	setFocus();
	K3ListView::contentsMousePressEvent(ev);
//	if (ev->button()==Qt::RightButton)
//		static_cast<KexiRelationView*>(parentWidget())->executePopup(ev->pos());
}

QRect KexiRelationViewTable::drawItemHighlighter(QPainter *painter, Q3ListViewItem *item)
{
#ifdef __GNUC__
#warning TODO KexiRelationViewTable::drawItemHighlighter() OK?
#endif
	if (painter) {
		QStyleOptionFocusRect option;
		option.initFrom(this);
		option.rect = itemRect(item);
		option.state |= QStyle::State_FocusAtBorder;
		style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, painter, this);
	}
	return itemRect(item);
}

#include "kexirelationviewtable.moc"
