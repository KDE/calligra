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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qtoolbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qtooltip.h>
#include <qscrollview.h>

#include <klocale.h>
#include <kiconloader.h>
#include <klineedit.h>

#include "kexirecordnavigator.h"

class KexiRecordNavigatorPrivate
{
	public:
		KexiRecordNavigatorPrivate() {}
};

KexiRecordNavigator::KexiRecordNavigator(QWidget *parent, int leftMargin, const char *name)
 : QFrame(parent, name)
 , m_view(0)
 , m_isInsertingEnabled(true)
 , d( 0 )
{
	if (parent->inherits("QScrollView"))
		setParentView( dynamic_cast<QScrollView*>(parent) );
	setFrameStyle(QFrame::NoFrame);
	QHBoxLayout *navPanelLyr = new QHBoxLayout(this,0,0,"nav_lyr");

	m_textLabel = new QLabel(this);
	navPanelLyr->addWidget( m_textLabel  );
	setLabelText(i18n("Row:"));
		
	int bw = 6+SmallIcon("navigator_first").width(); //QMIN( horizontalScrollBar()->height(), 20);
	QFont f = font();
	f.setPixelSize((bw > 12) ? 12 : bw);
	QFontMetrics fm(f);
	m_nav1DigitWidth = fm.width("8");

	navPanelLyr->addWidget( m_navBtnFirst = new QToolButton(this) );
	m_navBtnFirst->setFixedWidth(bw);
	m_navBtnFirst->setFocusPolicy(NoFocus);
	m_navBtnFirst->setIconSet( SmallIconSet("navigator_first") );
	QToolTip::add(m_navBtnFirst, i18n("First row"));
	
	navPanelLyr->addWidget( m_navBtnPrev = new QToolButton(this) );
	m_navBtnPrev->setFixedWidth(bw);
	m_navBtnPrev->setFocusPolicy(NoFocus);
	m_navBtnPrev->setIconSet( SmallIconSet("navigator_prev") );
	QToolTip::add(m_navBtnPrev, i18n("Previous row"));
	
	navPanelLyr->addSpacing( 6 );
	
	navPanelLyr->addWidget( m_navRecordNumber = new KLineEdit(this) );
	m_navRecordNumber->setAlignment(AlignRight | AlignVCenter);
	m_navRecordNumber->setFocusPolicy(ClickFocus);
	m_navRecordNumber->installEventFilter(this);
//	m_navRowNumber->setFixedWidth(fw);
	m_navRecordNumberValidator = new QIntValidator(1, INT_MAX, this);
	m_navRecordNumber->setValidator(m_navRecordNumberValidator);
	m_navRecordNumber->installEventFilter(this);
	QToolTip::add(m_navRecordNumber, i18n("Current row number"));
	
	KLineEdit *lbl_of = new KLineEdit(i18n("of"), this);
	lbl_of->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	lbl_of->setMaximumWidth(fm.width(lbl_of->text())+8);
	lbl_of->setReadOnly(true);
	lbl_of->setLineWidth(0);
	lbl_of->setFocusPolicy(NoFocus);
	lbl_of->setAlignment(AlignCenter);
	navPanelLyr->addWidget( lbl_of );
	
	navPanelLyr->addWidget( m_navRecordCount = new KLineEdit(this) );
	m_navRecordCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	m_navRecordCount->setReadOnly(true);
	m_navRecordCount->setLineWidth(0);
	m_navRecordCount->setFocusPolicy(NoFocus);
	m_navRecordCount->setAlignment(AlignLeft | AlignVCenter);
	QToolTip::add(m_navRecordCount, i18n("Number of rows"));

	lbl_of->setFont(f);
	m_navRecordNumber->setFont(f);
	m_navRecordCount->setFont(f);
	setFont(f);

	navPanelLyr->addWidget( m_navBtnNext = new QToolButton(this) );
	m_navBtnNext->setFixedWidth(bw);
	m_navBtnNext->setFocusPolicy(NoFocus);
	m_navBtnNext->setIconSet( SmallIconSet("navigator_next") );
	QToolTip::add(m_navBtnNext, i18n("Next row"));
	
	navPanelLyr->addWidget( m_navBtnLast = new QToolButton(this) );
	m_navBtnLast->setFixedWidth(bw);
	m_navBtnLast->setFocusPolicy(NoFocus);
	m_navBtnLast->setIconSet( SmallIconSet("navigator_last") );
	QToolTip::add(m_navBtnLast, i18n("Last row"));
	
	navPanelLyr->addSpacing( 6 );
	navPanelLyr->addWidget( m_navBtnNew = new QToolButton(this) );
	m_navBtnNew->setFixedWidth(bw);
	m_navBtnNew->setFocusPolicy(NoFocus);
	m_navBtnNew->setIconSet( SmallIconSet("navigator_new") );
	QToolTip::add(m_navBtnNew, i18n("New row"));
	m_navBtnNext->setEnabled(isInsertingEnabled());
	
	navPanelLyr->addSpacing( 6 );
	navPanelLyr->addStretch(10);

	connect(m_navBtnPrev,SIGNAL(clicked()),this,SIGNAL(prevButtonClicked()));
	connect(m_navBtnNext,SIGNAL(clicked()),this,SIGNAL(nextButtonClicked()));
	connect(m_navBtnLast,SIGNAL(clicked()),this,SIGNAL(lastButtonClicked()));
	connect(m_navBtnFirst,SIGNAL(clicked()),this,SIGNAL(firstButtonClicked()));
	connect(m_navBtnNew,SIGNAL(clicked()),this,SIGNAL(newButtonClicked()));

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
	QFrame::setEnabled(set);
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
			case Qt::Key_BackTab: {
				recordEntered=true;
				ke->accept(); //to avoid pressing Enter later
				ret = true;
			}
			default:;
			}
		}
		else if (e->type()==QEvent::FocusOut) {
			recordEntered=true;
			ret = false;
		}

		if (recordEntered) {
			bool ok=true;
			uint r = m_navRecordNumber->text().toUInt(&ok);
			if (!ok || r<1)
				r = (recordCount()>0)?1:0;
			if (m_view)
				m_view->setFocus();
			setCurrentRecordNumber(r);
			emit recordNumberEntered(r);
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
//			d->nav1DigitWidth*QMAX( QMAX(n.length(),2)+1,d->navRecordCount->text().length()+1)+6 
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
		m_navBtnNext->setEnabled(r > 0 && r < (recCnt +(m_isInsertingEnabled?1:0)));
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
	const int w = m_nav1DigitWidth*QMAX( QMAX(n.length(),2)+1,m_navRecordNumber->text().length()+1)+6;
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

void KexiRecordNavigator::setParentView(QScrollView *view)
{
	m_view = view;
}

void KexiRecordNavigator::updateGeometry(int leftMargin)
{
	QFrame::updateGeometry();
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

#include "kexirecordnavigator.moc"
