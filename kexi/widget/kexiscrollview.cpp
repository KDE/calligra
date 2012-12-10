/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "kexiscrollview.h"

#include <QPalette>
#include <QCursor>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>

#include <kdebug.h>
#include <klocale.h>

#include <utils/kexirecordnavigator.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

//disabled #define KEXI_SHOW_OUTERAREA_TEXT

#ifdef KEXI_SHOW_OUTERAREA_TEXT
//! @internal

class KexiScrollViewData
{
public:
    QPixmap horizontalOuterAreaPixmapBuffer;
    QPixmap verticalOuterAreaPixmapBuffer;
};

// @todo warning: not reentrant!
K_GLOBAL_STATIC(KexiScrollViewData, KexiScrollView_data)
#endif


class KexiScrollView::Private
{
public:
    Private(bool preview_)
	: widget(0)
        , preview(preview_)
        , scrollViewNavPanel(0)
    {
    }

    bool resizing;
    bool enableResizing;
    QWidget *widget;

    int gridSize;
    QFont helpFont;
    QColor helpColor;
    QTimer delayedResize;
    //! for refreshContentsSizeLater()
    Q3ScrollView::ScrollBarMode vsmode, hsmode;
    bool snapToGrid;
    bool preview;
    bool scrollbarModeSet;
    bool outerAreaVisible;
    KexiRecordNavigator* scrollViewNavPanel;
};


KexiScrollView::KexiScrollView(QWidget *parent, bool preview)
        : Q3ScrollView(parent)
        , d(new Private(preview))
{

    setObjectName("kexiscrollview");
    setAttribute(Qt::WA_StaticContents, true);
    setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
    QPalette pal(viewport()->palette());
    pal.setBrush(viewport()->backgroundRole(), pal.brush(QPalette::Mid));
    viewport()->setPalette(pal);
    const QColor fc = palette().color(QPalette::WindowText);
    const QColor bc = viewport()->palette().color(QPalette::Window);
    d->helpColor = KexiUtils::blendedColors(fc, bc, 1, 2);
// d->helpColor = QColor((fc.red()+bc.red()*2)/3, (fc.green()+bc.green()*2)/3,
//  (fc.blue()+bc.blue()*2)/3);
    d->helpFont = font();
    d->helpFont.setPointSize(d->helpFont.pointSize() * 3);

    setFocusPolicy(Qt::WheelFocus);

    //initial resize mode is always manual;
    //will be changed on show(), if needed
    setResizePolicy(Manual);

    viewport()->setMouseTracking(true);
    d->resizing = false;
    d->enableResizing = true;
    d->snapToGrid = false;
    d->gridSize = 0;
    d->outerAreaVisible = true;

    d->delayedResize.setSingleShot(true);
    connect(&(d->delayedResize), SIGNAL(timeout()), this, SLOT(refreshContentsSize()));
    d->scrollbarModeSet = false;
    if (d->preview) {
        refreshContentsSizeLater(true, true);
//! @todo allow to hide navigator
        updateScrollBars();
        d->scrollViewNavPanel = new KexiRecordNavigator(this, this, leftMargin());
        d->scrollViewNavPanel->setObjectName("nav");
        d->scrollViewNavPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    }
}

KexiScrollView::~KexiScrollView()
{
    delete d;
}

void
KexiScrollView::setWidget(QWidget *w)
{
    if (!w)
        return;
    addChild(w);
    d->widget = w;
}

void
KexiScrollView::setRecordNavigatorVisible(bool visible)
{
    if (/*d->scrollViewNavPanel->isVisible() &&*/ !visible) {
        if (d->scrollViewNavPanel)
            d->scrollViewNavPanel->hide();
    } else if (visible)  {
        if (d->scrollViewNavPanel)
            d->scrollViewNavPanel->show();
        updateNavPanelGeometry();
    }
}
void
KexiScrollView::setOuterAreaIndicatorVisible(bool visible)  {
    d->outerAreaVisible = visible;
}


void
KexiScrollView::setSnapToGrid(bool enable, int gridSize)
{
    d->snapToGrid = enable;
    if (enable) {
        d->gridSize = gridSize;
    }
}

void
KexiScrollView::setResizingEnabled(bool enabled)
{
    d->enableResizing = enabled;
}

void
KexiScrollView::refreshContentsSizeLater(bool horizontal, bool vertical)
{
    Q_UNUSED(horizontal);
    Q_UNUSED(vertical);

    if (!d->scrollbarModeSet) {
        d->scrollbarModeSet = true;
        d->vsmode = vScrollBarMode();
        d->hsmode = hScrollBarMode();
    }
// if (vertical)
    setVScrollBarMode(Q3ScrollView::AlwaysOff);
    //if (horizontal)
    setHScrollBarMode(Q3ScrollView::AlwaysOff);
    updateScrollBars();
    d->delayedResize.start(100);
}

void
KexiScrollView::refreshContentsSize()
{
    if (!d->widget)
        return;
    if (d->preview) {
        resizeContents(d->widget->width(), d->widget->height());
//  kDebug() << "KexiScrollView::refreshContentsSize(): ( "
        //  << d->widget->width() <<", "<< d->widget->height();
        setVScrollBarMode(d->vsmode);
        setHScrollBarMode(d->hsmode);
        d->scrollbarModeSet = false;
        updateScrollBars();
    } else {
        // Ensure there is always space to resize Form
        int w = contentsWidth(), h = contentsHeight();
        bool change = false;
#ifdef KEXI_SHOW_OUTERAREA_TEXT
        const int delta_x = qMax(KexiScrollView_data->verticalOuterAreaPixmapBuffer.width(), 300);
        const int delta_y = qMax(KexiScrollView_data->horizontalOuterAreaPixmapBuffer.height(), 300);
#else
        const int delta_x = 300;
        const int delta_y = 300;
#endif
        if ((d->widget->width() + delta_x * 2 / 3) > w) {
            w = d->widget->width() + delta_x;
            change = true;
        } else if ((w - d->widget->width()) > delta_x) {
            w = d->widget->width() + delta_x;
            change = true;
        }
        if ((d->widget->height() + delta_y * 2 / 3) > h) {
            h = d->widget->height() + delta_y;
            change = true;
        } else if ((h - d->widget->height()) > delta_y) {
            h = d->widget->height() + delta_y;
            change = true;
        }
        if (change) {
            repaint();
            viewport()->repaint();
            repaintContents();
            updateContents(0, 0, 2000, 2000);
            clipper()->repaint();

            resizeContents(w, h);
        }
//  kDebug() << "KexiScrollView::refreshContentsSize(): ( "
        //  << contentsWidth() <<", "<< contentsHeight();
        updateScrollBars();
        setVScrollBarMode(Auto);
        setHScrollBarMode(Auto);
    }
    updateContents();
    updateScrollBars();
}

void
KexiScrollView::updateNavPanelGeometry()
{
    if (d->scrollViewNavPanel)
        d->scrollViewNavPanel->updateGeometry(leftMargin());
}

void
KexiScrollView::contentsMousePressEvent(QMouseEvent *ev)
{
    if (!d->widget)
        return;

    QRect r3(0, 0, d->widget->width() + 4, d->widget->height() + 4);
    if (!r3.contains(ev->pos())) // clicked outside form
        //d->form->resetSelection();
        emit outerAreaClicked();

    if (!d->enableResizing)
        return;

    QRect r(d->widget->width(),  0, 4, d->widget->height() + 4); // right limit
    QRect r2(0, d->widget->height(), d->widget->width() + 4, 4); // bottom limit
    if (r.contains(ev->pos()) || r2.contains(ev->pos())) {
        d->resizing = true;
        emit resizingStarted();
    }
}

void
KexiScrollView::contentsMouseReleaseEvent(QMouseEvent *)
{
    if (d->resizing) {
        d->resizing = false;
        emit resizingEnded();
    }

    unsetCursor();
}

void
KexiScrollView::contentsMouseMoveEvent(QMouseEvent *ev)
{
    if (!d->widget || !d->enableResizing)
        return;

    if (d->resizing) { // resize widget
        int tmpx = ev->x(), tmpy = ev->y();
        const int exceeds_x = (tmpx - contentsX() + 5) - clipper()->width();
        const int exceeds_y = (tmpy - contentsY() + 5) - clipper()->height();
        if (exceeds_x > 0)
            tmpx -= exceeds_x;
        if (exceeds_y > 0)
            tmpy -= exceeds_y;
        if ((tmpx - contentsX()) < 0)
            tmpx = contentsX();
        if ((tmpy - contentsY()) < 0)
            tmpy = contentsY();

        // we look for the max widget right() (or bottom()), which would be the limit for form resizing (not to hide widgets)
        const QList<QWidget*> list(d->widget->findChildren<QWidget*>());   /* not recursive*/
        foreach(QWidget *w, list) {
            tmpx = qMax(tmpx, (w->geometry().right() + 10));
            tmpy = qMax(tmpy, (w->geometry().bottom() + 10));
        }

        int neww = -1, newh;
        if (cursor().shape() == Qt::SizeHorCursor) {
            if (d->snapToGrid)
                neww = int(float(tmpx) / float(d->gridSize) + 0.5) * d->gridSize;
            else
                neww = tmpx;
            newh = d->widget->height();
        } else if (cursor().shape() == Qt::SizeVerCursor) {
            neww = d->widget->width();
            if (d->snapToGrid)
                newh = int(float(tmpy) / float(d->gridSize) + 0.5) * d->gridSize;
            else
                newh = tmpy;
        } else if (cursor().shape() == Qt::SizeFDiagCursor) {
            if (d->snapToGrid) {
                neww = int(float(tmpx) / float(d->gridSize) + 0.5) * d->gridSize;
                newh = int(float(tmpy) / float(d->gridSize) + 0.5) * d->gridSize;
            } else {
                neww = tmpx;
                newh = tmpy;
            }
        }
        //needs update?
        if (neww != -1 && d->widget->size() != QSize(neww, newh)) {
            d->widget->resize(neww, newh);
            refreshContentsSize();
            updateContents();
        }
    } else { // update mouse cursor
        QPoint p = ev->pos();
        QRect r(d->widget->width(),  0, 4, d->widget->height()); // right
        QRect r2(0, d->widget->height(), d->widget->width(), 4); // bottom
        QRect r3(d->widget->width(), d->widget->height(), 4, 4); // bottom-right corner

        if (r.contains(p))
            setCursor(Qt::SizeHorCursor);
        else if (r2.contains(p))
            setCursor(Qt::SizeVerCursor);
        else if (r3.contains(p))
            setCursor(Qt::SizeFDiagCursor);
        else
            unsetCursor();
    }
}

void
KexiScrollView::setupPixmapBuffer(QPixmap& pixmap, const QString& text, int lines)
{
    Q_UNUSED(lines);

    QFontMetrics fm(d->helpFont);
    const int flags = Qt::AlignCenter | Qt::AlignTop;
    QRect rect(fm.boundingRect(0, 0, 1000, 1000, flags, text));
    const int txtw = rect.width(), txth = rect.height();//fm.width(text), txth = fm.height()*lines;
    pixmap = QPixmap(txtw, txth);
    if (!pixmap.isNull()) {
        //create pixmap once
        pixmap.fill(viewport()->palette().color(QPalette::Window));
        QPainter pb(&pixmap);
        pb.initFrom(this);
        pb.setPen(d->helpColor);
        pb.setFont(d->helpFont);
        pb.drawText(0, 0, txtw, txth, Qt::AlignCenter | Qt::AlignTop, text);
    }
}

void
KexiScrollView::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
    Q3ScrollView::drawContents(p, clipx, clipy, clipw, cliph);
    if (d->widget) {
        if (d->preview || !d->outerAreaVisible)
            return;

        //draw right and bottom borders
        const int wx = childX(d->widget);
        const int wy = childY(d->widget);
        p->setPen(palette().color(QPalette::Active, QPalette::Foreground));
        p->drawLine(wx + d->widget->width(), wy, wx + d->widget->width(), wy + d->widget->height());
        p->drawLine(wx, wy + d->widget->height(), wx + d->widget->width(), wy + d->widget->height());
//kDebug() << "KexiScrollView::drawContents() " << wy+d->widget->height();

#ifdef KEXI_SHOW_OUTERAREA_TEXT
        if (KexiScrollView_data->horizontalOuterAreaPixmapBuffer.isNull()) {
            //create flicker-less buffer
            setupPixmapBuffer(KexiScrollView_data->horizontalOuterAreaPixmapBuffer, i18n("Outer Area"), 1);
            setupPixmapBuffer(KexiScrollView_data->verticalOuterAreaPixmapBuffer, i18n("Outer\nArea"), 2);
        }
        if (!KexiScrollView_data->horizontalOuterAreaPixmapBuffer.isNull()
                && !KexiScrollView_data->verticalOuterAreaPixmapBuffer.isNull()
                && !d->delayedResize.isActive() /* only draw text if there's not pending delayed resize*/) {
            if (d->widget->height() > (KexiScrollView_data->verticalOuterAreaPixmapBuffer.height() + 20)) {
                p->drawPixmap(
                    qMax(d->widget->width(), KexiScrollView_data->verticalOuterAreaPixmapBuffer.width() + 20) + 20,
                    qMax((d->widget->height() - KexiScrollView_data->verticalOuterAreaPixmapBuffer.height()) / 2, 20),
                    KexiScrollView_data->verticalOuterAreaPixmapBuffer
                );
            }
            p->drawPixmap(
                qMax((d->widget->width() - KexiScrollView_data->horizontalOuterAreaPixmapBuffer.width()) / 2, 20),
                qMax(d->widget->height(), KexiScrollView_data->horizontalOuterAreaPixmapBuffer.height() + 20) + 20,
                KexiScrollView_data->horizontalOuterAreaPixmapBuffer
            );
        }
#endif
    }
}

void
KexiScrollView::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    d->widget->update(); //update form elements on too fast mouse move
}

void
KexiScrollView::setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h)
{
    /*todo*/
// kDebug()<<"KexiScrollView::setHBarGeometry";
    if (d->scrollViewNavPanel && d->scrollViewNavPanel->isVisible()) {
        d->scrollViewNavPanel->setHBarGeometry(hbar, x, y, w, h);
    } else {
        hbar.setGeometry(x, y, w, h);
    }
}

KexiRecordNavigator*
KexiScrollView::recordNavigator() const
{
    return d->scrollViewNavPanel;
}


bool KexiScrollView::isPreviewing() const
{
    return d->preview;
}

const QTimer *KexiScrollView::delayedResizeTimer() const
{
  return &(d->delayedResize);
}

QWidget * KexiScrollView::widget() const
{
    return d->widget;
}

#include "kexiscrollview.moc"


