/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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
#include <kexi_version.h>

#include <QLayout>
#include <q3accel.h>
#include <QTimer>
#include <QLabel>
//Added by qt3to4:
#include <QPixmap>
#include <QPaintEvent>
#include <QEvent>
#include <QKeyEvent>
#include <Q3Frame>
#include <QResizeEvent>
#include <Q3VBoxLayout>

#include <kdialog.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kapplication.h>

KexiSimplePrintPreviewView::KexiSimplePrintPreviewView(
    QWidget *parent, KexiSimplePrintPreviewWindow *window)
        : QWidget(parent, "KexiSimplePrintPreviewView", Qt::WA_StaticContents)//|WNoAutoErase)
        , m_window(window)
{
    enablePainting = false;
//   resize(300,400);
//   resizeContents(200, 400);
}

void KexiSimplePrintPreviewView::paintEvent(QPaintEvent *pe)
{
    Q_UNUSED(pe);
    if (!enablePainting)
        return;
    QPixmap pm(size()); //dbl buffered
    QPainter p;
    p.begin(&pm, this);
//! @todo only for screen!
    p.fillRect(QRect(QPoint(0, 0), pm.size()), QBrush(Qt::white));//pe->rect(), QBrush(white));
    if (m_window->currentPage() >= 0)
        m_window->m_engine.paintPage(m_window->currentPage(), p);
//  emit m_window->paintingPageRequested(m_window->currentPage(), p);
    p.end();
    bitBlt(this, 0, 0, &pm);
}

//--------------------------

#define KexiSimplePrintPreviewScrollView_MARGIN KDialog::marginHint()

KexiSimplePrintPreviewScrollView::KexiSimplePrintPreviewScrollView(
    KexiSimplePrintPreviewWindow *window)
        : Q3ScrollView(window, "scrollview", Qt::WA_StaticContents /*Qt 4-not needed |Qt::WNoAutoErase*/)
        , m_window(window)
{
//   this->settings = settings;
    widget = new KexiSimplePrintPreviewView(viewport(), m_window);

    /*   int widthMM = KoPageFormat::width(
        settings.pageLayout.format, settings.pageLayout.orientation);
      int heightMM = KoPageFormat::height(
        settings.pageLayout.format, settings.pageLayout.orientation);
    //   int constantHeight = 400;
    //   widget->resize(constantHeight * widthMM / heightMM, constantHeight ); //keep aspect
    */
    addChild(widget);
}

void KexiSimplePrintPreviewScrollView::resizeEvent(QResizeEvent *re)
{
    Q3ScrollView::resizeEvent(re);
// kDebug() << re->size().width() << " " << re->size().height();
// kDebug() << contentsWidth() << " " << contentsHeight();
// kDebug() << widget->width() << " " << widget->height();
    setUpdatesEnabled(false);
    if (re->size().width() > (widget->width() + 2*KexiSimplePrintPreviewScrollView_MARGIN)
            || re->size().height() > (widget->height() + 2*KexiSimplePrintPreviewScrollView_MARGIN)) {
        resizeContents(
            qMax(re->size().width(), widget->width() + 2*KexiSimplePrintPreviewScrollView_MARGIN),
            qMax(re->size().height(), widget->height() + 2*KexiSimplePrintPreviewScrollView_MARGIN));
        int vscrbarWidth = verticalScrollBar()->isVisible() ? verticalScrollBar()->width() : 0;
        int newContentsWidth
        = qMax(re->size().width(), widget->width() + 2 * KexiSimplePrintPreviewScrollView_MARGIN);
        int newContentsHeight
        = qMax(re->size().height(), widget->height() + 2 * KexiSimplePrintPreviewScrollView_MARGIN);
        moveChild(widget, (newContentsWidth - vscrbarWidth - widget->width()) / 2,
                  (newContentsHeight - widget->height()) / 2);
        resizeContents(newContentsWidth, newContentsHeight);
    }
    setUpdatesEnabled(true);
}

void KexiSimplePrintPreviewScrollView::setFullWidth()
{
    viewport()->setUpdatesEnabled(false);
    double widthMM = KoPageFormat::width(
                         m_window->settings().pageLayout.format,
                         m_window->settings().pageLayout.orientation);
    double heightMM = KoPageFormat::height(
                          m_window->settings().pageLayout.format, m_window->settings().pageLayout.orientation);
// int constantWidth = m_window->width()- KexiSimplePrintPreviewScrollView_MARGIN*6;
    double constantWidth = width() - KexiSimplePrintPreviewScrollView_MARGIN * 6;
    double heightForWidth = constantWidth * heightMM / widthMM;
// heightForWidth = qMin(kapp->desktop()->height()*4/5, heightForWidth);
    kDebug() << "1: " << heightForWidth;
#if 0 //todo we can use this if we want to fix the height to width of the page
    heightForWidth = qMin(height(), heightForWidth);
    kDebug() << "2: " << heightForWidth;
#endif
    constantWidth = heightForWidth * widthMM / heightMM;
    widget->resize((int)constantWidth, (int)heightForWidth); //keep aspect
    resizeContents(int(widget->width() + 2*KexiSimplePrintPreviewScrollView_MARGIN),
                   int(widget->height() + 2*KexiSimplePrintPreviewScrollView_MARGIN));
    moveChild(widget, (contentsWidth() - widget->width()) / 2,
              (contentsHeight() - widget->height()) / 2);
    viewport()->setUpdatesEnabled(true);
    resize(size() + QSize(1, 1)); //to update pos.
    widget->enablePainting = true;
    widget->repaint();
}

void KexiSimplePrintPreviewScrollView::setContentsPos(int x, int y)
{
// kDebug() << "############" << x << " " << y << " " << contentsX()<< " " <<contentsY();
    if (x < 0 || y < 0) //to avoid endless loop on Linux
        return;
    Q3ScrollView::setContentsPos(x, y);
}

//------------------

KexiSimplePrintPreviewWindow::KexiSimplePrintPreviewWindow(
    KexiSimplePrintingEngine &engine, const QString& previewName,
    QWidget *parent, Qt::WFlags f)
        : QWidget(parent, "KexiSimplePrintPreviewWindow", f)
        , m_engine(engine)
        , m_settings(*m_engine.settings())
        , m_pageNumber(-1)
        , m_pagesCount(-1)
{
    setWindowTitle(i18n("%1 - Print Preview - %2", previewName, KEXI_APP_NAME));
    setIcon(DesktopIcon("document-print-preview"));
    Q3VBoxLayout *lyr = new Q3VBoxLayout(this, 6);

    int id;
    m_toolbar = new KToolBar(0, this);
    m_toolbar->setLineWidth(0);
    m_toolbar->setFrameStyle(Q3Frame::NoFrame);
    m_toolbar->setIconText(KToolBar::IconTextRight);
    lyr->addWidget(m_toolbar);

    id = m_toolbar->insertWidget(-1, 0, new KPushButton(KStandardGuiItem::print(), m_toolbar));
    m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotPrintClicked()));
    static_cast<KPushButton*>(m_toolbar->getWidget(id))->setAccel(Qt::CTRL | Qt::Key_P);
    m_toolbar->insertSeparator();

    id = m_toolbar->insertWidget(-1, 0, new KPushButton(i18n("Page Set&up..."), m_toolbar));
    m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotPageSetup()));
    m_toolbar->insertSeparator();


#ifndef KEXI_NO_UNFINISHED
//! @todo unfinished
    id = m_toolbar->insertWidget(-1, 0, new KPushButton(KIcon("zoom-in"), i18n("Zoom In"), m_toolbar));
    m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotZoomInClicked()));
    m_toolbar->insertSeparator();

    id = m_toolbar->insertWidget(-1, 0, new KPushButton(KIcon("zoom-out"), i18n("Zoom Out"), m_toolbar));
    m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(slotZoomOutClicked()));
    m_toolbar->insertSeparator();
#endif

    id = m_toolbar->insertWidget(-1, 0, new KPushButton(KStandardGuiItem::close(), m_toolbar));
    m_toolbar->addConnection(id, SIGNAL(clicked()), this, SLOT(close()));
    m_toolbar->alignItemRight(id);

    m_scrollView = new KexiSimplePrintPreviewScrollView(this);
    m_scrollView->setUpdatesEnabled(false);
    m_view = m_scrollView->widget;
    m_scrollView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lyr->addWidget(m_scrollView);

    QWidget* navToolbarWidget = new QWidget(this); //widget used to center the navigator toolbar
    navToolbarWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    Q3HBoxLayout *navToolbarLyr = new Q3HBoxLayout(navToolbarWidget);
    lyr->addWidget(navToolbarWidget);

    m_navToolbar = new KToolBar(0, navToolbarWidget);
    navToolbarLyr->addStretch(1);
    navToolbarLyr->addWidget(m_navToolbar);
    navToolbarLyr->addStretch(1);
// m_navToolbar->setFullWidth(true);
    m_navToolbar->setLineWidth(0);
    m_navToolbar->setFrameStyle(Q3Frame::NoFrame);
    m_navToolbar->setIconText(KToolBar::IconTextRight);

    m_idFirst = m_navToolbar->insertWidget(-1, 0, new KPushButton(KIcon("go-first"), i18n("First Page"), m_navToolbar));
    m_navToolbar->addConnection(m_idFirst, SIGNAL(clicked()), this, SLOT(slotFirstClicked()));
    m_navToolbar->insertSeparator();

    m_idPrevious = m_navToolbar->insertWidget(-1, 0, new KPushButton(KIcon("go-previous"), i18n("Previous Page"), m_navToolbar));
    m_navToolbar->addConnection(m_idPrevious, SIGNAL(clicked()), this, SLOT(slotPreviousClicked()));
    m_navToolbar->insertSeparator();

    m_idPageNumberLabel = m_navToolbar->insertWidget(-1, 0, new QLabel(m_navToolbar));
    m_navToolbar->insertSeparator();

    m_idNext = m_navToolbar->insertWidget(-1, 0, new KPushButton(KIcon("go-next"), i18n("Next Page"), m_navToolbar));
    m_navToolbar->addConnection(m_idNext, SIGNAL(clicked()), this, SLOT(slotNextClicked()));
    m_navToolbar->insertSeparator();

    m_idLast = m_navToolbar->insertWidget(-1, 0, new KPushButton(KIcon("go-last"), i18n("Last Page"), m_navToolbar));
    m_navToolbar->addConnection(m_idLast, SIGNAL(clicked()), this, SLOT(slotLastClicked()));
    m_navToolbar->insertSeparator();

    resize(width(), kapp->desktop()->height()*4 / 5);

//! @todo progress bar...

    QTimer::singleShot(50, this, SLOT(initLater()));
}

void KexiSimplePrintPreviewWindow::initLater()
{
    setFullWidth();
    updatePagesCount();
    goToPage(0);
}

KexiSimplePrintPreviewWindow::~KexiSimplePrintPreviewWindow()
{
}

/*void KexiSimplePrintPreviewWindow::setPagesCount(int pagesCount)
{
  m_pagesCount = pagesCount;
  goToPage(0);
}*/

void KexiSimplePrintPreviewWindow::slotPrintClicked()
{
    hide();
    emit printRequested();
    show();
    raise();
}

void KexiSimplePrintPreviewWindow::slotPageSetup()
{
    lower();
    emit pageSetupRequested();
}

void KexiSimplePrintPreviewWindow::slotZoomInClicked()
{
    //! @todo
}

void KexiSimplePrintPreviewWindow::slotZoomOutClicked()
{
    //! @todo
}

void KexiSimplePrintPreviewWindow::slotFirstClicked()
{
    goToPage(0);
}

void KexiSimplePrintPreviewWindow::slotPreviousClicked()
{
    goToPage(m_pageNumber - 1);
}

void KexiSimplePrintPreviewWindow::slotNextClicked()
{
    goToPage(m_pageNumber + 1);
}

void KexiSimplePrintPreviewWindow::slotLastClicked()
{
    goToPage(m_engine.pagesCount() - 1);
}

void KexiSimplePrintPreviewWindow::goToPage(int pageNumber)
{
    if ((pageNumber == m_pageNumber && m_pagesCount == (int)m_engine.pagesCount())
            || pageNumber < 0 || pageNumber > ((int)m_engine.pagesCount() - 1))
        return;
    m_pageNumber = pageNumber;
    m_pagesCount = m_engine.pagesCount();

    m_view->repaint(); //this will automatically paint a new page

    m_navToolbar->setItemEnabled(m_idNext, pageNumber < ((int)m_engine.pagesCount() - 1));
    m_navToolbar->setItemEnabled(m_idLast, pageNumber < ((int)m_engine.pagesCount() - 1));
    m_navToolbar->setItemEnabled(m_idPrevious, pageNumber > 0);
    m_navToolbar->setItemEnabled(m_idFirst, pageNumber > 0);
    static_cast<QLabel*>(m_navToolbar->getWidget(m_idPageNumberLabel))->setText(
        i18nc(
            "Page (number) of (total)", "Page %1 of %2", m_pageNumber + 1, m_engine.pagesCount()));
}

void KexiSimplePrintPreviewWindow::setFullWidth()
{
    m_scrollView->setFullWidth();
}

void KexiSimplePrintPreviewWindow::updatePagesCount()
{
    QPixmap pm(m_view->size()); //dbl buffered
    QPainter p(m_view);
    //p.begin(&pm, this);
////! @todo only for screen!
// p.fillRect(pe->rect(), QBrush(white));
    m_engine.calculatePagesCount(p);
    p.end();
}

bool KexiSimplePrintPreviewWindow::event(QEvent * e)
{
    QEvent::Type t = e->type();
    if (t == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        const int k = ke->key();
        bool ok = true;
        if (k == Qt::Key_Equal || k == Qt::Key_Plus)
            slotZoomInClicked();
        else if (k == Qt::Key_Minus)
            slotZoomOutClicked();
        else if (k == Qt::Key_Home)
            slotFirstClicked();
        else if (k == Qt::Key_End)
            slotLastClicked();
        else
            ok = false;

        if (ok) {
            ke->accept();
            return true;
        }
    } else if (t == QEvent::ShortcutOverride) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        const int k = ke->key();
        bool ok = true;
        if (k == Qt::Key_PageUp)
            slotPreviousClicked();
        else if (k == Qt::Key_PageDown)
            slotNextClicked();
        else
            ok = false;

        if (ok) {
            ke->accept();
            return true;
        }
    }
    return QWidget::event(e);
}


#include "kexisimpleprintpreviewwindow.moc"
#include "kexisimpleprintpreviewwindow_p.moc"
