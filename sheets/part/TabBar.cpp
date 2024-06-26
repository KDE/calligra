/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TabBar.h"

#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QTimer>
#include <QToolButton>

// TODO
// improvement possibilities
// - use offscreen buffer to reduce flicker even more
// - keep track of tabs, only (re)layout when necessary
// - paint all tabs to buffer, show only by shifting
// - customizable button pixmaps
// - use QStyle to paint the tabs & buttons (is it good/possible?)

namespace Calligra
{
namespace Sheets
{

class TabBarPrivate
{
public:
    TabBar *tabbar;

    // scroll buttons
    QToolButton *scrollFirstButton;
    QToolButton *scrollLastButton;
    QToolButton *scrollBackButton;
    QToolButton *scrollForwardButton;

    // read-only: no mouse drag, double-click, right-click
    bool readOnly;

    // list of all tabs, in order of appearance
    QStringList tabs;

    // array of QRect for each visible tabs
    QList<QRect> tabRects;

    // leftmost tab (or rightmost for right-to-left layouts)
    int firstTab;

    // rightmost tab (or leftmost for right-to-left layouts)
    int lastTab;

    // the active tab in the range form 1..n.
    // if this value is 0, that means that no tab is active.
    int activeTab;

    // unusable space on the left, taken by the scroll buttons
    int offset;

    // when the user drag the tab (in order to move it)
    // this is the target position, it's 0 if no tab is dragged
    int targetTab;

    // wheel movement since selected tab was last changed by the
    // mouse wheel
    int wheelDelta;

    // true if autoscroll is active
    bool autoScroll;

    // calculate the bounding rectangle for each visible tab
    void layoutTabs();

    // reposition scroll buttons
    void layoutButtons();

    // find a tab whose bounding rectangle contains the pos
    // return -1 if no such tab is found
    int tabAt(const QPoint &pos);

    // draw a single tab
    void drawTab(QPainter &painter, QRect &rect, const QString &text, bool active);

    // draw a marker to indicate tab moving
    void drawMoveMarker(QPainter &painter, int x, int y);

    // update the enable/disable status of scroll buttons
    void updateButtons();

    // get the font to use on the tabs
    QFont font(bool selected);
};

// built-in pixmap for scroll-first button
static const char *const arrow_leftmost_xpm[] = {"10 10 2 1",
                                                 "  c None",
                                                 ". c #000000",
                                                 "          ",
                                                 "  .    .  ",
                                                 "  .   ..  ",
                                                 "  .  ...  ",
                                                 "  . ....  ",
                                                 "  .  ...  ",
                                                 "  .   ..  ",
                                                 "  .    .  ",
                                                 "          ",
                                                 "          "};

// built-in pixmap for scroll-last button
static const char *const arrow_rightmost_xpm[] = {"10 10 2 1",
                                                  "  c None",
                                                  ". c #000000",
                                                  "          ",
                                                  "  .    .  ",
                                                  "  ..   .  ",
                                                  "  ...  .  ",
                                                  "  .... .  ",
                                                  "  ...  .  ",
                                                  "  ..   .  ",
                                                  "  .    .  ",
                                                  "          ",
                                                  "          "};

// built-in pixmap for scroll-left button
static const char *const arrow_left_xpm[] = {"10 10 2 1",
                                             "  c None",
                                             ". c #000000",
                                             "          ",
                                             "      .   ",
                                             "     ..   ",
                                             "    ...   ",
                                             "   ....   ",
                                             "    ...   ",
                                             "     ..   ",
                                             "      .   ",
                                             "          ",
                                             "          "};

// built-in pixmap for scroll-right button
static const char *const arrow_right_xpm[] = {"10 10 2 1",
                                              "  c None",
                                              ". c #000000",
                                              "          ",
                                              "   .      ",
                                              "   ..     ",
                                              "   ...    ",
                                              "   ....   ",
                                              "   ...    ",
                                              "   ..     ",
                                              "   .      ",
                                              "          ",
                                              "          "};

void TabBarPrivate::layoutTabs()
{
    tabRects.clear();

    QFont f = font(true);
    QFontMetrics fm(f, tabbar);
    if (tabbar->isLeftToRight()) {
        // left to right
        int x = 0;
        for (int c = 0; c < tabs.count(); c++) {
            QRect rect;
            if (c >= firstTab - 1) {
                QString text = tabs[c];
                int tw = fm.horizontalAdvance(text) + 4;
                rect = QRect(x, 0, tw + 20, tabbar->height());
                x = x + tw + 20;
            }
            tabRects.append(rect);
        }

        lastTab = tabRects.count();
        for (int i = 0; i < tabRects.count(); i++)
            if (tabRects[i].right() - 10 + offset > tabbar->width()) {
                lastTab = i;
                break;
            }
    } else {
        // right to left
        int x = tabbar->width() - offset;
        for (int c = 0; c < tabs.count(); c++) {
            QRect rect;
            if (c >= firstTab - 1) {
                QString text = tabs[c];
                int tw = fm.horizontalAdvance(text) + 4;
                rect = QRect(x - tw - 20, 0, tw + 20, tabbar->height());
                x = x - tw - 20;
            }
            tabRects.append(rect);
        }

        lastTab = tabRects.count();
        for (int i = tabRects.count() - 1; i > 0; i--)
            if (tabRects[i].left() > 0) {
                lastTab = i + 1;
                break;
            }
    }
    tabbar->updateGeometry();
}

int TabBarPrivate::tabAt(const QPoint &pos)
{
    for (int i = 0; i < tabRects.count(); i++) {
        QRect rect = tabRects[i];
        if (rect.isNull())
            continue;
        if (rect.contains(pos))
            return i;
    }

    return -1; // not found
}

void TabBarPrivate::drawTab(QPainter &painter, QRect &rect, const QString &text, bool active)
{
    QPolygon polygon;

    if (tabbar->isLeftToRight())
        polygon << QPoint(rect.x(), rect.y()) << QPoint(rect.x(), rect.bottom() - 3) << QPoint(rect.x() + 2, rect.bottom())
                << QPoint(rect.right() - 4, rect.bottom()) << QPoint(rect.right() - 2, rect.bottom() - 2) << QPoint(rect.right() + 5, rect.top());
    else
        polygon << QPoint(rect.right(), rect.top()) << QPoint(rect.right(), rect.bottom() - 3) << QPoint(rect.right() - 2, rect.bottom())
                << QPoint(rect.x() + 4, rect.bottom()) << QPoint(rect.x() + 2, rect.bottom() - 2) << QPoint(rect.x() - 5, rect.top());

    painter.save();

    // fill it first
    QBrush bg = tabbar->palette().window();
    if (active)
        bg = tabbar->palette().base();
    painter.setBrush(bg);
    painter.setPen(QPen(Qt::NoPen));
    painter.drawPolygon(polygon);

    // draw the lines
    painter.setPen(QPen(tabbar->palette().color(QPalette::Dark), 0));
    painter.setRenderHint(QPainter::Antialiasing);
    if (!active) {
        const bool reverseLayout = tabbar->isRightToLeft();
        painter.drawLine(rect.x() - (reverseLayout ? 5 : 0), rect.y(), rect.right() + (reverseLayout ? 0 : 5), rect.top());
    }

    painter.drawPolyline(polygon);

    painter.setPen(tabbar->palette().color(QPalette::ButtonText));
    QFont f = font(active);
    painter.setFont(f);
    QFontMetrics fm = painter.fontMetrics();
    int tx = rect.x() + (rect.width() - fm.horizontalAdvance(text)) / 2;
    int ty = rect.y() + (rect.height() - fm.height()) / 2 + fm.ascent();
    painter.drawText(tx, ty, text);

    painter.restore();
}

void TabBarPrivate::drawMoveMarker(QPainter &painter, int x, int y)
{
    QPolygon movmark;

    movmark << QPoint(x, y) << QPoint(x + 7, y) << QPoint(x + 4, y + 6);
    QBrush oldBrush = painter.brush();
    painter.setBrush(Qt::black);
    painter.drawPolygon(movmark);
    painter.setBrush(oldBrush);
}

void TabBarPrivate::layoutButtons()
{
    int bw = tabbar->height();
    int w = tabbar->width();
    offset = bw * 4;

    if (tabbar->isLeftToRight()) {
        scrollFirstButton->setGeometry(0, 0, bw, bw);
        scrollFirstButton->setIcon(QIcon(QPixmap(arrow_leftmost_xpm)));
        scrollBackButton->setGeometry(bw, 0, bw, bw);
        scrollBackButton->setIcon(QIcon(QPixmap(arrow_left_xpm)));
        scrollForwardButton->setGeometry(bw * 2, 0, bw, bw);
        scrollForwardButton->setIcon(QIcon(QPixmap(arrow_right_xpm)));
        scrollLastButton->setGeometry(bw * 3, 0, bw, bw);
        scrollLastButton->setIcon(QIcon(QPixmap(arrow_rightmost_xpm)));
    } else {
        scrollFirstButton->setGeometry(w - bw, 0, bw, bw);
        scrollFirstButton->setIcon(QIcon(QPixmap(arrow_rightmost_xpm)));
        scrollBackButton->setGeometry(w - 2 * bw, 0, bw, bw);
        scrollBackButton->setIcon(QIcon(QPixmap(arrow_right_xpm)));
        scrollForwardButton->setGeometry(w - 3 * bw, 0, bw, bw);
        scrollForwardButton->setIcon(QIcon(QPixmap(arrow_left_xpm)));
        scrollLastButton->setGeometry(w - 4 * bw, 0, bw, bw);
        scrollLastButton->setIcon(QIcon(QPixmap(arrow_leftmost_xpm)));
    }
}

void TabBarPrivate::updateButtons()
{
    scrollFirstButton->setEnabled(tabbar->canScrollBack());
    scrollBackButton->setEnabled(tabbar->canScrollBack());
    scrollForwardButton->setEnabled(tabbar->canScrollForward());
    scrollLastButton->setEnabled(tabbar->canScrollForward());
}

QFont TabBarPrivate::font(bool selected)
{
    QFont f = QFontDatabase::systemFont(QFontDatabase::TitleFont);
    if (selected)
        f.setBold(true);
    return f;
}

// creates a new tabbar
TabBar::TabBar(QWidget *parent, const char * /*name*/)
    : QWidget(parent)
    , d(new TabBarPrivate)
{
    d->tabbar = this;
    d->readOnly = false;
    d->firstTab = 1;
    d->lastTab = 0;
    d->activeTab = 0;
    d->targetTab = 0;
    d->wheelDelta = 0;
    d->autoScroll = false;
    d->offset = 64;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // initialize the scroll buttons
    d->scrollFirstButton = new QToolButton(this);
    connect(d->scrollFirstButton, &QAbstractButton::clicked, this, &TabBar::scrollFirst);
    d->scrollLastButton = new QToolButton(this);
    connect(d->scrollLastButton, &QAbstractButton::clicked, this, &TabBar::scrollLast);
    d->scrollBackButton = new QToolButton(this);
    connect(d->scrollBackButton, &QAbstractButton::clicked, this, &TabBar::scrollBack);
    d->scrollForwardButton = new QToolButton(this);
    connect(d->scrollForwardButton, &QAbstractButton::clicked, this, &TabBar::scrollForward);
    d->layoutButtons();
    d->updateButtons();
}

// destroys the tabbar
TabBar::~TabBar()
{
    delete d;
}

// adds a new visible tab
void TabBar::addTab(const QString &text)
{
    d->tabs.append(text);

    update();
}

// removes a tab
void TabBar::removeTab(const QString &text)
{
    int i = d->tabs.indexOf(text);
    if (i == -1)
        return;

    if (d->activeTab == i + 1)
        d->activeTab = 0;

    d->tabs.removeAll(text);

    update();
}

// removes all tabs
void TabBar::clear()
{
    d->tabs.clear();
    d->activeTab = 0;
    d->firstTab = 1;

    update();
}

bool TabBar::readOnly() const
{
    return d->readOnly;
}

void TabBar::setReadOnly(bool ro)
{
    d->readOnly = ro;
}

void TabBar::setTabs(const QStringList &list)
{
    QString left, active;

    if (d->activeTab > 0)
        active = d->tabs[d->activeTab - 1];
    if (d->firstTab > 0 && d->firstTab <= d->tabs.size())
        left = d->tabs[d->firstTab - 1];

    d->tabs = list;

    if (!left.isNull()) {
        d->firstTab = d->tabs.indexOf(left) + 1;
        if (d->firstTab > (int)d->tabs.count())
            d->firstTab = 1;
        if (d->firstTab <= 0)
            d->firstTab = 1;
    }

    d->activeTab = 0;
    if (!active.isNull())
        setActiveTab(active);

    update();
}

QStringList TabBar::tabs() const
{
    return d->tabs;
}

unsigned TabBar::count() const
{
    return d->tabs.count();
}

bool TabBar::canScrollBack() const
{
    if (d->tabs.count() == 0)
        return false;

    return d->firstTab > 1;
}

bool TabBar::canScrollForward() const
{
    if (d->tabs.count() == 0)
        return false;

    return d->lastTab < (int)d->tabs.count();
}

void TabBar::scrollBack()
{
    if (!canScrollBack())
        return;

    d->firstTab--;
    if (d->firstTab < 1)
        d->firstTab = 1;

    d->layoutTabs();
    d->updateButtons();
    update();
}

void TabBar::scrollForward()
{
    if (!canScrollForward())
        return;

    d->firstTab++;
    if (d->firstTab > (int)d->tabs.count())
        d->firstTab = d->tabs.count();

    d->layoutTabs();
    d->updateButtons();
    update();
}

void TabBar::scrollFirst()
{
    if (!canScrollBack())
        return;

    d->firstTab = 1;
    d->layoutTabs();
    d->updateButtons();
    update();
}

void TabBar::scrollLast()
{
    if (!canScrollForward())
        return;

    d->layoutTabs();

    if (!isRightToLeft()) {
        int fullWidth = d->tabRects[d->tabRects.count() - 1].right();
        int delta = fullWidth - width() + d->offset;
        for (int i = 0; i < d->tabRects.count(); i++)
            if (d->tabRects[i].x() > delta) {
                d->firstTab = i + 1;
                break;
            }
    } else {
        // FIXME optimize this, perhaps without loop
        for (; d->firstTab <= (int)d->tabRects.count();) {
            int x = d->tabRects[d->tabRects.count() - 1].x();
            if (x > 0)
                break;
            d->firstTab++;
            d->layoutTabs();
        }
    }

    d->layoutTabs();
    d->updateButtons();
    update();
}

void TabBar::ensureVisible(const QString &tab)
{
    int i = d->tabs.indexOf(tab);
    if (i == -1)
        return;
    i++;

    // already visible, then do nothing
    if ((i >= d->firstTab) && (i <= d->lastTab))
        return;

    if (i < d->firstTab)
        while (i < d->firstTab)
            scrollBack();

    if (i > d->lastTab)
        while (i > d->lastTab)
            scrollForward();
}

void TabBar::moveTab(int tab, int target)
{
    QString tabName = d->tabs.takeAt(tab);

    if (target > tab)
        target--;

    if (target >= d->tabs.count())
        d->tabs.append(tabName);
    else
        d->tabs.insert(target, tabName);

    if (d->activeTab == tab + 1)
        d->activeTab = target + 1;

    update();
}

void TabBar::setActiveTab(const QString &text)
{
    int i = d->tabs.indexOf(text);
    if (i == -1)
        return;

    if (i + 1 == d->activeTab)
        return;

    d->activeTab = i + 1;
    d->updateButtons();
    update();

    Q_EMIT tabChanged(text);
}

void TabBar::autoScrollBack()
{
    if (!d->autoScroll)
        return;

    scrollBack();

    if (!canScrollBack())
        d->autoScroll = false;
    else
        QTimer::singleShot(400, this, &TabBar::autoScrollBack);
}

void TabBar::autoScrollForward()
{
    if (!d->autoScroll)
        return;

    scrollForward();

    if (!canScrollForward())
        d->autoScroll = false;
    else
        QTimer::singleShot(400, this, &TabBar::autoScrollForward);
}

void TabBar::paintEvent(QPaintEvent *)
{
    if (d->tabs.count() == 0) {
        update();
        return;
    }

    d->layoutTabs();
    d->updateButtons();

    QPainter painter(this);
    if (!isRightToLeft())
        painter.translate(d->offset, 0);

    if (!isRightToLeft())
        painter.translate(5, 0);

    // draw first all non-active, visible tabs
    for (int c = d->tabRects.count() - 1; c >= 0; c--) {
        QRect rect = d->tabRects[c];
        if (rect.isNull())
            continue;
        QString text = d->tabs[c];
        d->drawTab(painter, rect, text, false);
    }

    // draw the active tab
    if (d->activeTab > 0) {
        QRect rect = d->tabRects[d->activeTab - 1];
        if (!rect.isNull()) {
            QString text = d->tabs[d->activeTab - 1];
            d->drawTab(painter, rect, text, true);
        }
    }

    // draw the move marker
    if (d->targetTab > 0) {
        int p = qMin(d->targetTab, (int)d->tabRects.count());
        QRect rect = d->tabRects[p - 1];
        if (!rect.isNull()) {
            int x = !isRightToLeft() ? rect.x() : rect.right() - 7;
            if (d->targetTab > (int)d->tabRects.count())
                x = !isRightToLeft() ? rect.right() - 7 : rect.x() - 3;
            d->drawMoveMarker(painter, x, rect.y());
        }
    }
}

void TabBar::resizeEvent(QResizeEvent *)
{
    d->layoutButtons();
    d->updateButtons();
    update();
}

QSize TabBar::sizeHint() const
{
    int h = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    int w = 5 * h; // we have 4 buttons, the 5 is to give some free space too
    if (d->tabRects.size())
        w += d->tabRects[d->tabRects.size() - 1].right();

    return QSize(w, h);
}

void TabBar::renameTab(const QString &old_name, const QString &new_name)
{
    d->tabs.replace(d->tabs.indexOf(old_name), new_name);

    update();
}

QString TabBar::activeTab() const
{
    if (d->activeTab == 0)
        return QString();
    else
        return d->tabs[d->activeTab - 1];
}

void TabBar::mousePressEvent(QMouseEvent *ev)
{
    if (d->tabs.count() == 0) {
        update();
        return;
    }

    d->layoutTabs();

    QPoint pos = ev->pos();
    if (!isRightToLeft())
        pos = pos - QPoint(d->offset, 0);

    int tab = d->tabAt(pos) + 1;
    if ((tab > 0) && (tab != d->activeTab)) {
        d->activeTab = tab;
        update();

        Q_EMIT tabChanged(d->tabs[d->activeTab - 1]);

        // scroll if partially visible
        if (d->tabRects[tab - 1].right() > width() - d->offset)
            scrollForward();
    }

    if (ev->button() == Qt::RightButton)
        if (!d->readOnly)
            Q_EMIT contextMenu(ev->globalPos());
}

void TabBar::mouseReleaseEvent(QMouseEvent *ev)
{
    if (d->readOnly)
        return;

    d->autoScroll = false;

    if (ev->button() == Qt::LeftButton && d->targetTab != 0) {
        Q_EMIT tabMoved(d->activeTab - 1, d->targetTab - 1);
        d->targetTab = 0;
    }
}

void TabBar::mouseMoveEvent(QMouseEvent *ev)
{
    if (d->readOnly)
        return;

    QPoint pos = ev->pos();
    if (!isRightToLeft())
        pos = pos - QPoint(d->offset, 0);

    // check if user drags a tab to move it
    int i = d->tabAt(pos) + 1;
    if ((i > 0) && (i != d->targetTab)) {
        if (i == d->activeTab)
            i = 0;
        if (i == d->activeTab + 1)
            i = 0;

        if (i != d->targetTab) {
            d->targetTab = i;
            d->autoScroll = false;
            update();
        }
    }

    // drag past the very latest visible tab
    // e.g move a tab to the last ordering position
    QRect r = d->tabRects[d->tabRects.count() - 1];
    bool moveToLast = false;
    if (r.isValid()) {
        if (!isRightToLeft())
            if (pos.x() > r.right())
                if (pos.x() < width())
                    moveToLast = true;
        if (isRightToLeft())
            if (pos.x() < r.x())
                if (pos.x() > 0)
                    moveToLast = true;
    }
    if (moveToLast)
        if (d->targetTab != (int)d->tabRects.count() + 1) {
            d->targetTab = d->tabRects.count() + 1;
            d->autoScroll = false;
            update();
        }

    // outside far too left ? activate autoscroll...
    if (pos.x() < 0 && !d->autoScroll) {
        d->autoScroll = true;
        autoScrollBack();
    }

    // outside far too right ? activate autoscroll...
    int w = width() - d->offset;
    if (pos.x() > w && !d->autoScroll) {
        d->autoScroll = true;
        autoScrollForward();
    }
}

void TabBar::mouseDoubleClickEvent(QMouseEvent *ev)
{
    int offset = isRightToLeft() ? 0 : d->offset;
    if (ev->pos().x() > offset)
        if (!d->readOnly)
            Q_EMIT doubleClicked();
}

void TabBar::wheelEvent(QWheelEvent *e)
{
    if (d->tabs.count() == 0) {
        update();
        return;
    }

    // Currently one wheel movement is a delta of 120.
    // The 'unused' delta is stored for devices that allow
    // a higher scrolling resolution.
    // The delta required to move one tab is one wheel movement:
    const int deltaRequired = 120;

    d->wheelDelta += e->angleDelta().y();
    int tabDelta = -(d->wheelDelta / deltaRequired);
    d->wheelDelta = d->wheelDelta % deltaRequired;
    int numTabs = d->tabs.size();

    if (d->activeTab + tabDelta > numTabs) {
        // Would take us past the last tab
        d->activeTab = numTabs;
    } else if (d->activeTab + tabDelta < 1) {
        // Would take us before the first tab
        d->activeTab = 1;
    } else {
        d->activeTab = d->activeTab + tabDelta;
    }

    // Find the left and right edge of the new tab.  If we're
    // going forward, and the right of the new tab isn't visible
    // then scroll forward.  Likewise, if going back, and the
    // left of the new tab isn't visible, then scroll back.
    int activeTabRight = d->tabRects[d->activeTab - 1].right();
    int activeTabLeft = d->tabRects[d->activeTab - 1].left();
    if (tabDelta > 0 && activeTabRight > width() - d->offset) {
        scrollForward();
    } else if (tabDelta < 0 && activeTabLeft < width() - d->offset) {
        scrollBack();
    }

    update();
    Q_EMIT tabChanged(d->tabs[d->activeTab - 1]);
}

} // namespace Sheets
} // namespace Calligra
