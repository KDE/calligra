/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <stdlib.h>

#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qstyle.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kconfig.h>
#include <kglobalsettings.h>

#include "kexidb/tableschema.h"
#include "kexidragobjects.h"
#include "kexirelationviewtable.h"
#include "kexirelationview.h"

#if defined(Q_WS_WIN) || KDE_IS_VERSION(3,2,90)
# define USE_KMDI_getCaptionColors //we'are using KMDI feature from KDE 3.3
# include <kmdichildarea.h>
#endif

KexiRelationViewTableContainer::KexiRelationViewTableContainer(
	KexiRelationView *parent, KexiDB::TableSchema *t)
 : QFrame(parent,"KexiRelationViewTableContainer" )
 , m_table(t)
 , m_parent(parent)
//	, m_mousePressed(false)
{

//	setFixedSize(100, 150);
//js:	resize(100, 150);
	//setMouseTracking(true);

	setFrameStyle( QFrame::WinPanel | QFrame::Raised );

	QVBoxLayout *lyr = new QVBoxLayout(this,4,1); //js: using Q*BoxLayout is a good idea

	m_tableHeader = new KexiRelationViewTableContainerHeader(m_table->name(), this);

	m_tableHeader->unsetFocus();
	m_tableHeader->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
	lyr->addWidget(m_tableHeader);
	connect(m_tableHeader,SIGNAL(moved()),this,SLOT(moved()));
	connect(m_tableHeader, SIGNAL(endDrag()), this, SIGNAL(endDrag()));

	m_tableView = new KexiRelationViewTable(this, parent, m_table, "KexiRelationViewTable");
	//m_tableHeader->setFocusProxy( m_tableView );
	m_tableView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum));
	m_tableView->setMaximumSize( m_tableView->sizeHint() );
//	m_tableView->resize( m_tableView->sizeHint() );
	lyr->addWidget(m_tableView, 0);
	connect(m_tableView, SIGNAL(tableScrolling()), this, SLOT(moved()));
	connect(m_tableView, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
		this, SLOT(slotContextMenu(KListView*, QListViewItem*, const QPoint&)));
}

KexiRelationViewTableContainer::~KexiRelationViewTableContainer()
{
}

KexiDB::TableSchema *KexiRelationViewTableContainer::table()
{
	return m_table;
}

void KexiRelationViewTableContainer::slotContextMenu(KListView *, QListViewItem *, const QPoint &p)
{
//	m_parent->executePopup(p);
	emit contextMenuRequest( p );
}

void KexiRelationViewTableContainer::moved() {
//	kdDebug()<<"finally emitting moved"<<endl;
	emit moved(this);
}

int KexiRelationViewTableContainer::globalY(const QString &field)
{
//	kdDebug() << "KexiRelationViewTableContainer::globalY()" << endl;
//	QPoint o = mapFromGlobal(QPoint(0, (m_tableView->globalY(field))/*+m_parent->contentsY()*/));

	QPoint o(0, (m_tableView->globalY(field)) + m_parent->contentsY());
//	kdDebug() << "KexiRelationViewTableContainer::globalY() db2" << endl;
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
	kdDebug() << "SET FOCUS" << endl;
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
	kdDebug() << "UNSET FOCUS" << endl;
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

//for compat. with KDE <= 3.2
#ifndef USE_KMDI_getCaptionColors
void getCaptionColors( const QPalette &pal, 
    QColor &activeBG, QColor &activeFG, QColor &inactiveBG, QColor &inactiveFG )
{
    QColor def_activeBG = pal.active().highlight();
    QColor def_activeFG = pal.active().highlightedText();
    QColor def_inactiveBG = pal.inactive().dark();
    QColor def_inactiveFG = pal.inactive().brightText();

    if ( QApplication::desktopSettingsAware() ) {
        //get colors from WM
        KConfig *cfg = KGlobal::config();
        cfg->setGroup( "WM" );
        activeBG = cfg->readColorEntry("activeBackground", &def_activeBG);
        activeFG = cfg->readColorEntry("activeForeground", &def_activeFG);
        inactiveBG = cfg->readColorEntry("inactiveBackground", &def_inactiveBG);
        inactiveFG = cfg->readColorEntry("inactiveForeground", &def_inactiveFG);
    }
    else {
        activeBG = def_activeBG;
        activeFG = def_activeFG;
        inactiveBG = def_inactiveBG;
        inactiveFG = def_inactiveFG;
    }
}
#endif


KexiRelationViewTableContainerHeader::KexiRelationViewTableContainerHeader(
	const QString& text,QWidget *parent)
	:QLabel(text,parent),m_dragging(false) 
{
	setMargin(1);

#ifdef USE_KMDI_getCaptionColors
	KMdiChildArea::getCaptionColors( 
#else
	getCaptionColors( 
#endif
		palette(), m_activeBG, m_activeFG, m_inactiveBG, m_inactiveFG );

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
		if (m_dragging && static_cast<QMouseEvent*>(ev)->state()==Qt::LeftButton) {
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
	parentWidget()->setFocus();
	ev->accept();
	if (ev->button()==Qt::LeftButton) {
		m_dragging=true;
		m_grabX=ev->globalPos().x();
		m_grabY=ev->globalPos().y();
		m_offsetX=0;
		m_offsetY=0;
		return;
	}
	if (ev->button()==Qt::RightButton) {
		emit static_cast<KexiRelationViewTableContainer*>(parentWidget())
			->contextMenuRequest(ev->globalPos());
	}
//	QLabel::mousePressEvent(ev);
}

void KexiRelationViewTableContainerHeader::mouseReleaseEvent(QMouseEvent *ev) {
	kdDebug()<<"KexiRelationViewTableContainerHeader::Mouse Release Event"<<endl;
	if (m_dragging && ev->button() & Qt::LeftButton) {
		m_dragging=false;
		emit endDrag();
	}
	ev->accept();
}

//END KexiRelatoinViewTableContainerHeader


//=====================================================================================

KexiRelationViewTable::KexiRelationViewTable(QWidget *parent, KexiRelationView *view, KexiDB::TableSchema *t, const char *name)
 : KListView(parent, name)
 , m_table(t)
{
//	m_fieldList = t.;
//	m_table = table;
	m_view = view;
//	m_table = t->name();
//	m_parent = parent;

	m_keyIcon = SmallIcon("key");
	m_noIcon = QPixmap(m_keyIcon.size());
	QBitmap bmp(m_noIcon.size());
	bmp.fill(Qt::color0);
	m_noIcon.setMask(bmp);

	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setDropVisualizer(false);
	setDropHighlighter(true);
	setAllColumnsShowFocus(true);
	addColumn("", 0);
	addColumn("fields");
	setResizeMode(QListView::LastColumn);
//	setResizeMode(QListView::AllColumns);
	header()->hide();

	setSorting(-1, true); // disable sorting
	setDragEnabled(true);

	int order=0;

	bool hasPKeys = true; //t->hasPrimaryKeys();
	KListViewItem *item = 0;
	for(int i=-1; i < (int)t->fieldCount(); i++)
	{
		KexiDB::Field *f = 0;
		if (i==-1) {
			item = new KexiRelationViewTableItem(this, item, QString::number(order), "*");
		}
		else {
			f = t->field(i);
			item = new KexiRelationViewTableItem(this, item, QString::number(order), f->name());
		}
//		item = item ? new KexiRelationViewTableItem(this, item, QString::number(order), f.name())
//			: new KexiRelationViewTableItem(this, QString::number(order), f.name());
		if(f && (f->isPrimaryKey() || f->isUniqueKey()))
			item->setPixmap(1, m_keyIcon);
		else if (hasPKeys) {
			item->setPixmap(1, m_noIcon);
		}

//js: We use KListView::setDragEnabled() instead.
//js		item->setDragEnabled(true);
//js		item->setDropEnabled(true);
		order++;
	}

//	setDragEnabled
	connect(this, SIGNAL(dropped(QDropEvent *, QListViewItem *)), this, SLOT(slotDropped(QDropEvent *)));
	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotContentsMoving(int,int)));
	connect(this, SIGNAL(doubleClicked(QListViewItem*,const QPoint&,int)),
		this, SLOT(slotItemDoubleClicked(QListViewItem*,const QPoint&,int)));
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}

QSize KexiRelationViewTable::sizeHint()
{
	QFontMetrics fm(font());

	kdDebug() << m_table->name() << " cw=" << columnWidth(1) + fm.width("i") << ", " << fm.width(m_table->name()+"  ") << endl; 

	QSize s( 
		QMAX( columnWidth(1) + fm.width("i"), fm.width(m_table->name()+"  ")), 
		childCount()*firstChild()->totalHeight() + 4 );
//	QSize s( columnWidth(1), childCount()*firstChild()->totalHeight() + 3*firstChild()->totalHeight()/10);
	return s;
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
	if(selectedItem())
	{
//		QString f = selectedItem()->text(1).stripWhiteSpace();
//		if (f!="*") {
			KexiFieldDrag *drag = new KexiFieldDrag("kexi/table",m_table->name(),selectedItem()->text(1), this, "metaDrag");
			return drag;
//		}
	}

	return 0;
}

bool
KexiRelationViewTable::acceptDrag(QDropEvent *ev) const
{
//	kdDebug() << "KexiRelationViewTable::acceptDrag()" << endl;
	QListViewItem *receiver = itemAt(ev->pos());
	if (!receiver)
		return false;
	QString srcTable;
	QString dummy;
	QString srcField;
	KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
	QString f = receiver->text(1).stripWhiteSpace();
	if (srcField.stripWhiteSpace()!="*" && f!="*" && ev->provides("kexi/field") && ev->source() != (QWidget*)this)
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
		s.masterTable = srcTable;
		s.detailsTable = m_table->name();
		s.masterField = srcField;
		s.detailsField = rcvField;

//		m_parent->addConnection(s);
		m_view->addConnection(s); //, false);

		kdDebug() << "KexiRelationViewTable::slotDropped() " << srcTable << ":" << srcField << " " 
			<< m_table->name() << ":" << rcvField << endl;
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

void KexiRelationViewTable::contentsMousePressEvent(QMouseEvent *ev)
{
	parentWidget()->setFocus();
	setFocus();
	KListView::contentsMousePressEvent(ev);
//	if (ev->button()==Qt::RightButton)
//		static_cast<KexiRelationView*>(parentWidget())->executePopup(ev->pos());
}

QRect KexiRelationViewTable::drawItemHighlighter(QPainter *painter, QListViewItem *item)
{
	if (painter) {
		style().drawPrimitive(QStyle::PE_FocusRect, painter, itemRect(item), colorGroup(),
			QStyle::Style_FocusAtBorder);
	}
	return itemRect(item);
}

void KexiRelationViewTable::slotItemDoubleClicked( QListViewItem *i, const QPoint &, int )
{
}


//=====================================================================================

KexiRelationViewTableItem::KexiRelationViewTableItem(
	QListView *parent, QListViewItem *after, QString key, QString field)
	: KListViewItem(parent, after, key, field)
{
}

void KexiRelationViewTableItem::paintFocus (QPainter * , const QColorGroup &, const QRect &)
{
}

//=====================================================================================


#include "kexirelationviewtable.moc"
