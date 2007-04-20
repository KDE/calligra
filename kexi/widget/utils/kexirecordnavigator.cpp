/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qtoolbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qtooltip.h>
#include <q3scrollview.h>
//Added by qt3to4:
#include <QPixmap>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QEvent>
#include <Q3Frame>
#include <Q3HBoxLayout>

#include <klocale.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kguiitem.h>
#include <kstaticdeleter.h>

#include "kexirecordnavigator.h"
#include "kexirecordmarker.h"

//! @internal
class KexiRecordNavigatorPrivate
{
	public:
		KexiRecordNavigatorPrivate()
		 : handler(0)
		 , editingIndicatorLabel(0)
		 , editingIndicatorEnabled(false)
		 , editingIndicatorVisible(false)
		{
		}
		KexiRecordNavigatorHandler *handler;
		Q3HBoxLayout *lyr;

		QLabel *editingIndicatorLabel;
		bool editingIndicatorEnabled : 1;
		bool editingIndicatorVisible : 1;
};

//--------------------------------------------------

KexiRecordNavigatorHandler::KexiRecordNavigatorHandler()
{
}

KexiRecordNavigatorHandler::~KexiRecordNavigatorHandler()
{
}

//--------------------------------------------------

KexiRecordNavigator::KexiRecordNavigator(QWidget *parent, int leftMargin, const char *name)
 : Q3Frame(parent, name)
 , m_view(0)
 , m_isInsertingEnabled(true)
 , d( new KexiRecordNavigatorPrivate() )
{
	if (parent->inherits("QScrollView"))
		setParentView( dynamic_cast<Q3ScrollView*>(parent) );
	setFrameStyle(Q3Frame::NoFrame);
	d->lyr = new Q3HBoxLayout(this,0,0,"nav_lyr");

	m_textLabel = new QLabel(this);
	d->lyr->addWidget( m_textLabel  );
	setLabelText(i18n("Row:"));
		
	int bw = 6+SmallIcon("navigator_first").width(); //qMin( horizontalScrollBar()->height(), 20);
	QFont f = font();
	f.setPixelSize((bw > 12) ? 12 : bw);
	QFontMetrics fm(f);
	m_nav1DigitWidth = fm.width("8");

	d->lyr->addWidget( m_navBtnFirst = new QToolButton(this) );
	m_navBtnFirst->setFixedWidth(bw);
	m_navBtnFirst->setFocusPolicy(Qt::NoFocus);
	m_navBtnFirst->setIcon( KIcon("navigator_first") );
	m_navBtnFirst->setToolTip( i18n("First row"));
	
	d->lyr->addWidget( m_navBtnPrev = new QToolButton(this) );
	m_navBtnPrev->setFixedWidth(bw);
	m_navBtnPrev->setFocusPolicy(Qt::NoFocus);
	m_navBtnPrev->setIcon( KIcon("navigator_prev") );
	m_navBtnPrev->setAutoRepeat(true);
	m_navBtnPrev->setToolTip( i18n("Previous row"));
	
	d->lyr->addSpacing( 6 );
	
	d->lyr->addWidget( m_navRecordNumber = new KLineEdit(this) );
	m_navRecordNumber->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_navRecordNumber->setFocusPolicy(Qt::ClickFocus);
	m_navRecordNumber->installEventFilter(this);
//	m_navRowNumber->setFixedWidth(fw);
	m_navRecordNumberValidator = new QIntValidator(1, INT_MAX, this);
	m_navRecordNumber->setValidator(m_navRecordNumberValidator);
	m_navRecordNumber->installEventFilter(this);
	m_navRecordNumber->setToolTip( i18n("Current row number"));
	
	KLineEdit *lbl_of = new KLineEdit(i18n("of"), this);
	lbl_of->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	lbl_of->setMaximumWidth(fm.width(lbl_of->text())+8);
	lbl_of->setReadOnly(true);
	//lbl_of->setLineWidth(0); // ### Qt4 TODO
	lbl_of->setFocusPolicy(Qt::NoFocus);
	lbl_of->setAlignment(Qt::AlignCenter);
	d->lyr->addWidget( lbl_of );
	
	d->lyr->addWidget( m_navRecordCount = new KLineEdit(this) );
	m_navRecordCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	m_navRecordCount->setReadOnly(true);
	//m_navRecordCount->setLineWidth(0); // ### Qt4 TODO
	m_navRecordCount->setFocusPolicy(Qt::NoFocus);
	m_navRecordCount->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_navRecordCount->setToolTip( i18n("Number of rows"));

	lbl_of->setFont(f);
	m_navRecordNumber->setFont(f);
	m_navRecordCount->setFont(f);
	setFont(f);

	d->lyr->addWidget( m_navBtnNext = new QToolButton(this) );
	m_navBtnNext->setFixedWidth(bw);
	m_navBtnNext->setFocusPolicy(Qt::NoFocus);
	m_navBtnNext->setIcon( KIcon("navigator_next") );
	m_navBtnNext->setAutoRepeat(true);
	m_navBtnNext->setToolTip( i18n("Next row"));
	
	d->lyr->addWidget( m_navBtnLast = new QToolButton(this) );
	m_navBtnLast->setFixedWidth(bw);
	m_navBtnLast->setFocusPolicy(Qt::NoFocus);
	m_navBtnLast->setIcon( KIcon("navigator_last") );
	m_navBtnLast->setToolTip( i18n("Last row"));
	
	d->lyr->addSpacing( 6 );
	d->lyr->addWidget( m_navBtnNew = new QToolButton(this) );
	m_navBtnNew->setFixedWidth(bw);
	m_navBtnNew->setFocusPolicy(Qt::NoFocus);
	m_navBtnNew->setIcon( KIcon("navigator_new") );
	m_navBtnNew->setToolTip( i18n("New row"));
	m_navBtnNext->setEnabled(isInsertingEnabled());
	
	d->lyr->addSpacing( 6 );
	d->lyr->addStretch(10);

	connect(m_navBtnPrev,SIGNAL(clicked()),this,SLOT(slotPrevButtonClicked()));
	connect(m_navBtnNext,SIGNAL(clicked()),this,SLOT(slotNextButtonClicked()));
	connect(m_navBtnLast,SIGNAL(clicked()),this,SLOT(slotLastButtonClicked()));
	connect(m_navBtnFirst,SIGNAL(clicked()),this,SLOT(slotFirstButtonClicked()));
	connect(m_navBtnNew,SIGNAL(clicked()),this,SLOT(slotNewButtonClicked()));

	setRecordCount(0);
	setCurrentRecordNumber(0);

	updateGeometry(leftMargin);
}

KexiRecordNavigator::~KexiRecordNavigator()
{
	delete d;
}

void KexiRecordNavigator::setInsertingEnabled(bool set)
{
	if (m_isInsertingEnabled==set)
		return;
	m_isInsertingEnabled = set;
	if (isEnabled())
		m_navBtnNew->setEnabled( m_isInsertingEnabled );
}

void KexiRecordNavigator::setEnabled( bool set )
{
	Q3Frame::setEnabled(set);
	if (set && !m_isInsertingEnabled)
		m_navBtnNew->setEnabled( false );
}

bool KexiRecordNavigator::eventFilter( QObject *o, QEvent *e )
{
	if (o==m_navRecordNumber) {
		bool recordEntered = false;
		bool ret;
		if (e->type()==QEvent::KeyPress) {
			QKeyEvent *ke = static_cast<QKeyEvent*>(e);
			switch (ke->key()) {
			case Qt::Key_Escape: {
				ke->accept();
				m_navRecordNumber->undo();
				if (m_view)
					m_view->setFocus();
				return true;
			}
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Tab:
			case Qt::Key_Backtab: 
			{
				recordEntered=true;
				ke->accept(); //to avoid pressing Enter later
				ret = true;
			}
			default:;
			}
		}
		else if (e->type()==QEvent::FocusOut) {
			if (static_cast<QFocusEvent*>(e)->reason()!=Qt::TabFocusReason
				&& static_cast<QFocusEvent*>(e)->reason()!=Qt::BacktabFocusReason
				&& static_cast<QFocusEvent*>(e)->reason()!=Qt::OtherFocusReason)
				recordEntered=true;
			ret = false;
		}

		if (recordEntered) {
			bool ok=true;
			uint r = m_navRecordNumber->text().toUInt(&ok);
			if (!ok || r<1)
				r = (recordCount()>0)?1:0;
			if (m_view && (hasFocus() || e->type()==QEvent::KeyPress))
				m_view->setFocus();
			setCurrentRecordNumber(r);
			emit recordNumberEntered(r);
			if (d->handler)
				d->handler->moveToRecordRequested(r-1);
			return ret;
		}
	}
/*
	bool ok=true;
	int r = text.toInt(&ok);
	if (!ok || r<1)
		r = 1;
	emit recordNumberEntered(r);*/
	return false;
}

void KexiRecordNavigator::setCurrentRecordNumber(uint r)
{
	uint recCnt = recordCount();
	if (r>(recCnt+(m_isInsertingEnabled?1:0)))
		r = recCnt+(m_isInsertingEnabled?1:0);
	QString n;
	if (r>0)
		n = QString::number(r);
	else
		n = " ";
//	if (d->navRecordNumber->text().length() != n.length()) {//resize
//		d->navRecordNumber->setFixedWidth(
//			d->nav1DigitWidth*qMax( qMax(n.length(),2)+1,d->navRecordCount->text().length()+1)+6 
//		);
//	}

	m_navRecordNumber->setText(n);
	m_navRecordCount->deselect();
	updateButtons(recCnt);
}

void KexiRecordNavigator::updateButtons(uint recCnt)
{
	const uint r = currentRecordNumber();
	if (isEnabled()) {
		m_navBtnPrev->setEnabled(r > 1);
		m_navBtnFirst->setEnabled(r > 1);
		m_navBtnNext->setEnabled(r > 0 
			&& r < (recCnt +(m_isInsertingEnabled?(1+d->editingIndicatorVisible/*if we're editing, next btn is avail.*/):0) ) );
		m_navBtnLast->setEnabled(r!=(recCnt+(m_isInsertingEnabled?1:0)) && (m_isInsertingEnabled || recCnt>0));
	}
}

void KexiRecordNavigator::setRecordCount(uint count)
{
	const QString & n = QString::number(count);
	if (m_isInsertingEnabled && currentRecordNumber()==0) {
		setCurrentRecordNumber(1);
	}
	if (m_navRecordCount->text().length() != n.length()) {//resize
		m_navRecordCount->setFixedWidth(m_nav1DigitWidth*n.length()+6);
		
		if (m_view && m_view->horizontalScrollBar()->isVisible()) {
			//+width of the delta
			resize(width()+(n.length()-m_navRecordCount->text().length())*m_nav1DigitWidth, height());
//			horizontalScrollBar()->move(d->navPanel->x()+d->navPanel->width()+20,horizontalScrollBar()->y());
		}
	}
	//update row number widget's width
	const int w = m_nav1DigitWidth*qMax( qMax(n.length(),2)+1,m_navRecordNumber->text().length()+1)+6;
	if (m_navRecordNumber->width()!=w) //resize
		m_navRecordNumber->setFixedWidth(w);

	m_navRecordCount->setText(n);
	m_navRecordCount->deselect();
	if (m_view)
		m_view->updateScrollBars();
	updateButtons(recordCount());
}

uint KexiRecordNavigator::currentRecordNumber() const
{
	bool ok=true;
	int r = m_navRecordNumber->text().toInt(&ok);
	if (!ok || r<1)
		r = 0;
	return r;
}

uint KexiRecordNavigator::recordCount() const
{
	bool ok=true;
	int r = m_navRecordCount->text().toInt(&ok);
	if (!ok || r<1)
		r = 0;
	return r;
}

void KexiRecordNavigator::setParentView(Q3ScrollView *view)
{
	m_view = view;
}

void KexiRecordNavigator::updateGeometry(int leftMargin)
{
	Q3Frame::updateGeometry();
	if (m_view) {
		int navWidth;
		if (m_view->horizontalScrollBar()->isVisible()) {
			navWidth = sizeHint().width();
		}
		else {
			navWidth = leftMargin + m_view->clipper()->width();
		}
		
		setGeometry(
			m_view->frameWidth(),
			m_view->height() - m_view->horizontalScrollBar()->sizeHint().height()-m_view->frameWidth(),
			navWidth,
			m_view->horizontalScrollBar()->sizeHint().height()
		);

		m_view->updateScrollBars();
	}
}

void KexiRecordNavigator::setHBarGeometry( QScrollBar & hbar, int x, int y, int w, int h )
{
	hbar.setGeometry( x + width(), y, w - width(), h );
}

void KexiRecordNavigator::setLabelText(const QString& text)
{
	m_textLabel->setText(text.isEmpty() ? QString::null : (QString::fromLatin1(" ")+text+" "));
}

void KexiRecordNavigator::setInsertingButtonVisible(bool set)
{
	if (set)
		m_navBtnNew->show();
	else
		m_navBtnNew->hide();
}

void KexiRecordNavigator::slotPrevButtonClicked()
{
	emit prevButtonClicked();
	if (d->handler)
		d->handler->moveToPreviousRecordRequested();
}

void KexiRecordNavigator::slotNextButtonClicked()
{
	emit nextButtonClicked();
	if (d->handler)
		d->handler->moveToNextRecordRequested();
}

void KexiRecordNavigator::slotLastButtonClicked()
{
	emit lastButtonClicked();
	if (d->handler)
		d->handler->moveToLastRecordRequested();
}

void KexiRecordNavigator::slotFirstButtonClicked()
{
	emit firstButtonClicked();
	if (d->handler)
		d->handler->moveToFirstRecordRequested();
}

void KexiRecordNavigator::slotNewButtonClicked()
{
	emit newButtonClicked();
	if (d->handler)
		d->handler->addNewRecordRequested();
}


void KexiRecordNavigator::setRecordHandler(KexiRecordNavigatorHandler *handler)
{
	d->handler = handler;
}

bool KexiRecordNavigator::editingIndicatorVisible() const
{
	return d->editingIndicatorVisible;
}

bool KexiRecordNavigator::editingIndicatorEnabled() const
{
	return d->editingIndicatorEnabled;
}

void KexiRecordNavigator::setEditingIndicatorEnabled(bool set)
{
	d->editingIndicatorEnabled = set;
	if (d->editingIndicatorEnabled) {
		if (!d->editingIndicatorLabel) {
			d->editingIndicatorLabel = new QLabel(this);
			d->editingIndicatorLabel->setAlignment(Qt::AlignCenter);
			QPixmap pix;
			pix.fromImage( *KexiRecordMarker::penImage() );
			d->editingIndicatorLabel->setFixedWidth( pix.width() + 2*2 );
			d->lyr->insertWidget( 0, d->editingIndicatorLabel );
		}
		d->editingIndicatorLabel->show();
	}
	else {
		if (d->editingIndicatorLabel) {
			d->editingIndicatorLabel->hide();
		}
	}
}

void KexiRecordNavigator::showEditingIndicator(bool show)
{
	d->editingIndicatorVisible = show;
	updateButtons(recordCount()); //this will refresh 'next btn'
	if (!d->editingIndicatorEnabled)
		return;
	if (d->editingIndicatorVisible) {
		QPixmap pix;
		pix.convertFromImage( *KexiRecordMarker::penImage() );
		d->editingIndicatorLabel->setPixmap( pix );
		d->editingIndicatorLabel->setToolTip( i18n("Editing indicator") );
	}
	else {
		d->editingIndicatorLabel->setPixmap( QPixmap() );
		d->editingIndicatorLabel->setToolTip( QString() );
	}
}

//------------------------------------------------

//! @internal
class KexiRecordNavigatorActionsInternal {
	public:
		KexiRecordNavigatorActionsInternal()
		 : moveToFirstRecord(i18n("First row"), "navigator_first", i18n("Go to first row"))
		 , moveToPreviousRecord(i18n("Previous row"), "navigator_prev", i18n("Go to previous row"))
		 , moveToNextRecord(i18n("Next row"), "navigator_next", i18n("Go to next row"))
		 , moveToLastRecord(i18n("Last row"), "navigator_last", i18n("Go to last row"))
		 , moveToNewRecord(i18n("New row"), "navigator_new", i18n("Go to new row"))
		{
		}
		static void init();
		KGuiItem moveToFirstRecord;
		KGuiItem moveToPreviousRecord;
		KGuiItem moveToNextRecord;
		KGuiItem moveToLastRecord;
		KGuiItem moveToNewRecord;
};

static KStaticDeleter<KexiRecordNavigatorActionsInternal> KexiRecordNavigatorActions_deleter;
KexiRecordNavigatorActionsInternal* KexiRecordNavigatorActions_internal = 0;

void KexiRecordNavigatorActionsInternal::init()
{
	if (!KexiRecordNavigatorActions_internal)
		KexiRecordNavigatorActions_deleter.setObject(KexiRecordNavigatorActions_internal, 
			new KexiRecordNavigatorActionsInternal());
}

const KGuiItem& KexiRecordNavigator::Actions::moveToFirstRecord()
{ KexiRecordNavigatorActionsInternal::init(); return KexiRecordNavigatorActions_internal->moveToFirstRecord; }

const KGuiItem& KexiRecordNavigator::Actions::moveToPreviousRecord()
{ KexiRecordNavigatorActionsInternal::init(); return KexiRecordNavigatorActions_internal->moveToPreviousRecord; }

const KGuiItem& KexiRecordNavigator::Actions::moveToNextRecord()
{ KexiRecordNavigatorActionsInternal::init(); return KexiRecordNavigatorActions_internal->moveToNextRecord; }

const KGuiItem& KexiRecordNavigator::Actions::moveToLastRecord()
{ KexiRecordNavigatorActionsInternal::init(); return KexiRecordNavigatorActions_internal->moveToLastRecord; }

const KGuiItem& KexiRecordNavigator::Actions::moveToNewRecord()
{ KexiRecordNavigatorActionsInternal::init(); return KexiRecordNavigatorActions_internal->moveToNewRecord; }

#include "kexirecordnavigator.moc"
