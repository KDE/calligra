/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexisimpleprintpreviewwindow.h"
#include "kexisimpleprintingengine.h"
#include "kexisimpleprintpreviewwindow_p.h"

#include <qlayout.h>
#include <qaccel.h>
#include <qtimer.h>

#include <kdialogbase.h>
#include <ktoolbarbutton.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kapplication.h>

KexiSimplePrintPreviewView::KexiSimplePrintPreviewView(
	QWidget *parent, KexiSimplePrintPreviewWindow *window)
 : QWidget(parent, "KexiSimplePrintPreviewView", WStaticContents)//|WNoAutoErase)
 , m_window(window)
{
	enablePainting = false;
//			resize(300,400);
//			resizeContents(200, 400);
}

void KexiSimplePrintPreviewView::paintEvent( QPaintEvent *pe )
{
	if (!enablePainting)
		return;
	QPixmap pm(size()); //dbl buffered
	QPainter p;
	p.begin(&pm, this);
//! @todo only for screen!
	p.fillRect(pe->rect(), QBrush(white));
	if (m_window->currentPage()>=0)
		m_window->m_engine.paintPage(m_window->currentPage(), p);
//		emit m_window->paintingPageRequested(m_window->currentPage(), p);
	p.end();
	bitBlt(this, 0, 0, &pm);
}

//--------------------------

#define KexiSimplePrintPreviewScrollView_MARGIN KDialogBase::marginHint()

KexiSimplePrintPreviewScrollView::KexiSimplePrintPreviewScrollView(
	KexiSimplePrintPreviewWindow *window)
 : QScrollView(window, "scrollview", WStaticContents|WNoAutoErase)
 , m_window(window)
{
//			this->settings = settings;
	widget = new KexiSimplePrintPreviewView(viewport(), m_window);

/*			int widthMM = KoPageFormat::width( 
		settings.pageLayout.format, settings.pageLayout.orientation);
	int heightMM = KoPageFormat::height( 
		settings.pageLayout.format, settings.pageLayout.orientation);
//			int constantHeight = 400;
//			widget->resize(constantHeight * widthMM / heightMM, constantHeight ); //keep aspect
*/
	addChild(widget);
}

void KexiSimplePrintPreviewScrollView::resizeEvent( QResizeEvent *re )
{
	QScrollView::resizeEvent(re);
	kdDebug() << re->size().width() << " " << re->size().height() << endl;
	kdDebug() << contentsWidth() << " " << contentsHeight() << endl;
	kdDebug() << widget->width() << " " << widget->height() << endl;
	setUpdatesEnabled(false);
	if (re->size().width() > (widget->width()+2*KexiSimplePrintPreviewScrollView_MARGIN)
		|| re->size().height() > (widget->height()+2*KexiSimplePrintPreviewScrollView_MARGIN)) {
		resizeContents(
			QMAX(re->size().width(), widget->width()+2*KexiSimplePrintPreviewScrollView_MARGIN),
			QMAX(re->size().height(), widget->height()+2*KexiSimplePrintPreviewScrollView_MARGIN));
		int vscrbarWidth = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
		moveChild(widget, (contentsWidth() - vscrbarWidth - widget->width())/2, 
			(contentsHeight() - widget->height())/2);
	}
	setUpdatesEnabled(true);
		
	/*else {
		int newContentsWidth = ;
		resizeContents(
			QMIN(widget->width() re->size().width());
		moveChild(widget, (contentsWidth() - widget->width())/2, 
			(contentsHeight() - widget->height())/2);
		newContentsWidth = re->size().width()+widget->width();
	}*/
}

void KexiSimplePrintPreviewScrollView::setFullWidth()
{
	viewport()->setUpdatesEnabled(false);
	double widthMM = KoPageFormat::width( 
		m_window->settings().pageLayout.format, 
		m_window->settings().pageLayout.orientation);
	double heightMM = KoPageFormat::height( 
		m_window->settings().pageLayout.format, m_window->settings().pageLayout.orientation);
//	int constantWidth = m_window->width()- KexiSimplePrintPreviewScrollView_MARGIN*6;
	double constantWidth = width()- KexiSimplePrintPreviewScrollView_MARGIN*6;
	double heightForWidth = constantWidth * heightMM / widthMM;
//	heightForWidth = QMIN(kapp->desktop()->height()*4/5, heightForWidth);
	heightForWidth = QMIN(height(), heightForWidth);
	constantWidth = heightForWidth * widthMM / heightMM;
	widget->resize((int)constantWidth, (int)heightForWidth); //keep aspect
	resizeContents(int(widget->width() + 2*KexiSimplePrintPreviewScrollView_MARGIN), 
		int(widget->height() + 2*KexiSimplePrintPreviewScrollView_MARGIN));
	moveChild(widget, (contentsWidth()-widget->width())/2, 
		(contentsHeight()-widget->height())/2);
	viewport()->setUpdatesEnabled(true);
	resize(size()+QSize(1,1)); //to update pos.
	widget->enablePainting = true;
	widget->repaint();
}


//------------------

KexiSimplePrintPreviewWindow::KexiSimplePrintPreviewWindow(
	KexiSimplePrintingEngine &engine, const QString& previewName, 
	QWidget *parent, WFlags f)
 : QWidget(parent, "KexiSimplePrintPreviewWindow", f)
 , m_engine(engine)
 , m_settings(*m_engine.settings())
 , m_pageNumber(-1)
{
	m_pagesCount = INT_MAX;

	setCaption(i18n("%1 - Print Preview").arg(previewName));
	setIcon(DesktopIcon("filequickprint"));
	QVBoxLayout *lyr = new QVBoxLayout(this, 6);

	int id;
	QAccel *a;
#ifndef KEXI_NO_UNFINISHED 
//! @todo unfinished
	m_toolbar = new KToolBar(0, this);
	m_toolbar->setLineWidth(0);
	m_toolbar->setFrameStyle(QFrame::NoFrame);
	m_toolbar->setIconText(KToolBar::IconTextRight);

/*	id = m_toolbar->insertButton("fileprint", -1, true, i18n("Print...")+" ");
	m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotPrintClicked()));
	m_toolbar->getButton(id)->setAccel(Qt::CTRL|Qt::Key_P);
	m_toolbar->insertSeparator();*/

	id = m_toolbar->insertButton("viewmag+", -1, true, i18n("Zoom In"));
	m_toolbar->getButton(id)->setAccel(Qt::Key_Plus);
	m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotZoomInClicked()));
	a = new QAccel( m_toolbar->getButton(id) );
	a->connectItem( a->insertItem( Key_Equal ), this, SLOT(slotZoomInClicked()) ); 

	id = m_toolbar->insertButton("viewmag-", -1, true, i18n("Zoom Out"));
	m_toolbar->getButton(id)->setAccel(Qt::Key_Minus);
	m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotZoomOutClicked()));
	lyr->addWidget(m_toolbar);
#endif

//tmp	lyr->addStretch(1);
	m_scrollView = new KexiSimplePrintPreviewScrollView(this);
	m_scrollView->setUpdatesEnabled(false);
	m_view = m_scrollView->widget;
	m_scrollView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	lyr->addWidget(m_scrollView);

	m_navToolbar = new KToolBar(0, this);
//	m_navToolbar->setFullWidth(true);
	m_navToolbar->setLineWidth(0);
	m_navToolbar->setFrameStyle(QFrame::NoFrame);
	m_navToolbar->setIconText(KToolBar::IconTextRight);

	m_idFirst = m_navToolbar->insertButton("start", -1, true, i18n("First Page"));
	m_navToolbar->getButton(m_idFirst)->setAccel(Qt::Key_Home);
	m_navToolbar->addConnection(m_idFirst, SIGNAL(clicked()), this, SLOT(slotFirstClicked()));

	m_idPrevious = m_navToolbar->insertButton("previous", -1, true, i18n("Previous Page"));
	m_navToolbar->getButton(m_idPrevious)->setAccel(Qt::Key_PageUp);
	m_navToolbar->addConnection(m_idPrevious, SIGNAL(clicked()), this, SLOT(slotPreviousClicked()));
	a = new QAccel( m_navToolbar->getButton(m_idPrevious) );
	a->connectItem( a->insertItem( Key_Left ), this, SLOT(slotPreviousClicked()) ); 

	m_idNext = m_navToolbar->insertButton("next", -1, true, i18n("Next Page"));
	m_navToolbar->getButton(m_idNext)->setAccel(Qt::Key_PageDown);
	m_navToolbar->addConnection(m_idNext, SIGNAL(clicked()), this, SLOT(slotNextClicked()));
	a = new QAccel( m_navToolbar->getButton(m_idNext) );
	a->connectItem( a->insertItem( Key_Right ), this, SLOT(slotNextClicked()) ); 

	m_idLast = m_navToolbar->insertButton("finish", -1, true, i18n("Last Page"));
	m_navToolbar->getButton(m_idLast)->setAccel(Qt::Key_End);
	m_navToolbar->addConnection(m_idLast, SIGNAL(clicked()), this, SLOT(slotLastClicked()));
	m_navToolbar->insertSeparator();

	id = m_navToolbar->insertButton("fileclose", -1, true, i18n("Close"));
	m_navToolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(close()));
	m_navToolbar->alignItemRight(id);
	lyr->addWidget(m_navToolbar);

	resize(width(), kapp->desktop()->height()*4/5);
	goToPage(0);

	QTimer::singleShot(50, m_scrollView, SLOT(setFullWidth()));
}

KexiSimplePrintPreviewWindow::~KexiSimplePrintPreviewWindow()
{
}

void KexiSimplePrintPreviewWindow::setPagesCount(int pagesCount)
{
	m_pagesCount = pagesCount;
	goToPage(0);
}

void KexiSimplePrintPreviewWindow::slotPrintClicked()
{
	emit printRequested();
	raise();
}

void KexiSimplePrintPreviewWindow::slotZoomInClicked()
{
	//todo
}

void KexiSimplePrintPreviewWindow::slotZoomOutClicked()
{
	//todo
}

void KexiSimplePrintPreviewWindow::slotFirstClicked()
{
	goToPage(0);
}

void KexiSimplePrintPreviewWindow::slotPreviousClicked()
{
	goToPage(m_pageNumber-1);
}

void KexiSimplePrintPreviewWindow::slotNextClicked()
{
	goToPage(m_pageNumber+1);
}

void KexiSimplePrintPreviewWindow::slotLastClicked()
{
	goToPage(m_pagesCount-1);
}

void KexiSimplePrintPreviewWindow::goToPage(int pageNumber)
{
	if (pageNumber==m_pageNumber || pageNumber < 0 || m_pageNumber > (m_pagesCount-1))
		return;
	m_pageNumber = pageNumber;

	m_view->repaint(); //this will automatically paint a new page
	if (m_engine.eof())
		m_pagesCount = pageNumber+1;

	m_navToolbar->setItemEnabled(m_idNext, pageNumber < (m_pagesCount-1));
	m_navToolbar->setItemEnabled(m_idLast, pageNumber < (m_pagesCount-1));
	m_navToolbar->setItemEnabled(m_idPrevious, pageNumber > 0);
	m_navToolbar->setItemEnabled(m_idFirst, pageNumber > 0);
}

void KexiSimplePrintPreviewWindow::setFullWidth()
{
	m_scrollView->setFullWidth();
}

#include "kexisimpleprintpreviewwindow.moc"
#include "kexisimpleprintpreviewwindow_p.moc"
