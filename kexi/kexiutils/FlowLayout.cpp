/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2007-2012 Jarosław Staniek <staniek@kde.org>

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

#include "FlowLayout.h"
#include "kexiutils_global.h"

#include <kdebug.h>

#ifdef __GNUC__
#warning vertical KexiFlowLayout ported to Qt4 but not tested
#else
#pragma WARNING( vertical KexiFlowLayout ported to Qt4 but not tested )
#endif


class KexiFlowLayout::Private
{
public:
    Private();
    ~Private();

    QList<QLayoutItem*> list;
    int cached_width;
    int cached_hfw;
    bool justify;
    Qt::Orientation orientation;
    QSize cached_sizeHint;
    QSize cached_minSize;
};

KexiFlowLayout::Private::Private()
    : cached_width(0), justify(false), orientation(Qt::Horizontal)
{

}

KexiFlowLayout::Private::~Private()
{
    qDeleteAll(list);
}

//// The layout itself

KexiFlowLayout::KexiFlowLayout(QWidget *parent, int margin, int spacing)
    : QLayout(parent), d(new Private())
{
    setMargin(margin);
    setSpacing(spacing);
}

KexiFlowLayout::KexiFlowLayout(QLayout* parent, int margin, int spacing)
    : QLayout(), d(new Private())
{
    parent->addItem(this);
    setMargin(margin);
    setSpacing(spacing);
}

KexiFlowLayout::KexiFlowLayout(int margin, int spacing)
    : QLayout(), d(new Private())
{
    setMargin(margin);
    setSpacing(spacing);
}

KexiFlowLayout::~KexiFlowLayout()
{
    delete d;
}

void
KexiFlowLayout::addItem(QLayoutItem *item)
{
    d->list.append(item);
}

void KexiFlowLayout::addSpacing(int size)
{
    if (d->orientation == Qt::Horizontal)
        addItem(new QSpacerItem(size, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    else
        addItem(new QSpacerItem(0, size, QSizePolicy::Minimum, QSizePolicy::Fixed));
}

void KexiFlowLayout::insertWidget(int index, QWidget* widget, int stretch, Qt::Alignment alignment)
{
    Q_UNUSED(stretch);
    QWidgetItem *wi = new QWidgetItem(widget);
    wi->setAlignment(alignment);
    d->list.insert(index, wi);
}


QList<QWidget*>* KexiFlowLayout::widgetList() const
{
    QList<QWidget*> *list = new QList<QWidget*>();
    foreach(QLayoutItem* item, d->list) {
        if (item->widget())
            list->append(item->widget());
    }
    return list;
}

void KexiFlowLayout::invalidate()
{
    QLayout::invalidate();
    d->cached_sizeHint = QSize();
    d->cached_minSize = QSize();
    d->cached_width = 0;
}

int KexiFlowLayout::count() const
{
    return d->list.size();
}

bool KexiFlowLayout::isEmpty() const
{
    return d->list.isEmpty();
}

bool KexiFlowLayout::hasHeightForWidth() const
{
    return (d->orientation == Qt::Horizontal);
}

int KexiFlowLayout::heightForWidth(int w) const
{
    if (d->cached_width != w) {
        // workaround to allow this method to stay 'const'
        KexiFlowLayout *mthis = (KexiFlowLayout*)this;
        int h = mthis->simulateLayout(QRect(0, 0, w, 0));
        mthis->d->cached_hfw = h;
        mthis->d->cached_width = w;
        return h;
    }
    return d->cached_hfw;
}

QSize KexiFlowLayout::sizeHint() const
{
    if (d->cached_sizeHint.isEmpty()) {
        KexiFlowLayout *mthis = (KexiFlowLayout*)this;
        QRect r = QRect(0, 0, 2000, 2000);
        mthis->simulateLayout(r);
    }
    return d->cached_sizeHint;
}

QSize KexiFlowLayout::minimumSize() const
{
//js: do we really need to simulate layout here?
//    I commented this out because it was impossible to stretch layout conveniently.
//    Now, minimum size is computed automatically based on item's minimumSize...
#if 0
    if (d->cached_minSize.isEmpty()) {
        KexiFlowLayout *mthis = (KexiFlowLayout*)this;
        QRect r = QRect(0, 0, 2000, 2000);
        mthis->simulateLayout(r);
    }
#endif
    return d->cached_minSize;
}

/*QSizePolicy::ExpandData
KexiFlowLayout::expanding() const*/
Qt::Orientations KexiFlowLayout::expandingDirections() const
{
    if (d->orientation == Qt::Vertical)
        return Qt::Vertical;
    else
        return Qt::Horizontal;
}

void KexiFlowLayout::setGeometry(const QRect &r)
{
    QLayout::setGeometry(r);
    if (d->orientation == Qt::Horizontal)
        doHorizontalLayout(r);
    else
        doVerticalLayout(r);
}

int KexiFlowLayout::simulateLayout(const QRect &r)
{
    if (d->orientation == Qt::Horizontal)
        return doHorizontalLayout(r, true);
    else
        return doVerticalLayout(r, true);
}

inline void doHorizontalLayoutForLine(const QRect &r, const QList<QLayoutItem*>& currentLine,
                                      int spacing, bool justify, int& y, int& h, int& availableSpace, int& expandingWidgets,
                                      int& sizeHintWidth, int& minSizeWidth, int& lineMinHeight, bool testOnly)
{
    QListIterator<QLayoutItem*> it2(currentLine);
    int wx = r.x();
    sizeHintWidth = 0 - spacing;
    minSizeWidth = 0 - spacing;
    lineMinHeight = 0;
    while (it2.hasNext()) {
        QLayoutItem *item = it2.next();
        QSize itemSizeHint = item->sizeHint(); // we cache these ones because it can take
        QSize itemMinSize = item->minimumSize(); // a while to get them
        QSize s;
        if (justify) {
            if (expandingWidgets != 0) {
                if (item->expandingDirections() & Qt::Horizontal)
                    s = QSize(
                            qMin(itemSizeHint.width() + availableSpace / expandingWidgets, r.width()),
                            itemSizeHint.height()
                        );
                else
                    s = QSize(qMin(itemSizeHint.width(), r.width()), itemSizeHint.height());
            } else
                s = QSize(
                        qMin(itemSizeHint.width() + availableSpace / (int)currentLine.count(), r.width()),
                        itemSizeHint.height()
                    );
        } else
            s = QSize(qMin(itemSizeHint.width(), r.width()), itemSizeHint.height());
        if (!testOnly) {
            // adjust vertical position depending on vertical alignment
            int add_y;
            if (item->alignment() & Qt::AlignBottom)
                add_y = h - s.height() - 1;
            else if (item->alignment() & Qt::AlignVCenter)
                add_y = (h - s.height() - 1) / 2;
            else
                add_y = 0; // Qt::AlignTop
            item->setGeometry(QRect(QPoint(wx, y + add_y), s));
        }
        wx = wx + s.width() + spacing;
        minSizeWidth = minSizeWidth + spacing + itemMinSize.width();
        sizeHintWidth = sizeHintWidth + spacing +  itemSizeHint.width();
        lineMinHeight = qMax(lineMinHeight, itemMinSize.height());
    }
}

int KexiFlowLayout::doHorizontalLayout(const QRect &r, bool testOnly)
{
    int x = r.x();
    int y = r.y();
    int h = 0; // height of this line
    int availableSpace = r.width() + spacing();
    int expandingWidgets = 0; // number of widgets in the line with QSizePolicy == Expanding
    QListIterator<QLayoutItem*> it(d->list);
    QList<QLayoutItem*> currentLine;
    QSize minSize, sizeHint(20, 20);
    int minSizeHeight = 0 - spacing();

    while (it.hasNext()) {
        QLayoutItem *o = it.next();
        if (o->isEmpty()) // do not consider hidden widgets
            continue;

//  kDebug() << "- doHorizontalLayout(): " << o->widget()->className() << " " << o->widget()->name();
        QSize oSizeHint = o->sizeHint(); // we cache these ones because it can take
        // a while to get it (eg for child layouts)
        if ((x + oSizeHint.width()) > r.right() && h > 0) {
            // do the layout of current line
            int sizeHintWidth, minSizeWidth, lineMinHeight;
            doHorizontalLayoutForLine(r, currentLine,
                                      spacing(), d->justify, y, h, availableSpace, expandingWidgets,
                                      sizeHintWidth, minSizeWidth, lineMinHeight, testOnly);

            sizeHint = sizeHint.expandedTo(QSize(sizeHintWidth, 0));
            minSize = minSize.expandedTo(QSize(minSizeWidth, 0));
            minSizeHeight = minSizeHeight + spacing() + lineMinHeight;
            // start a new line
            y = y + spacing() + h;
            h = 0;
            x = r.x();
            currentLine.clear();
            expandingWidgets = 0;
            availableSpace = r.width() + spacing();
        }

        x = x + spacing() + oSizeHint.width();
        h = qMax(h,  oSizeHint.height());
        currentLine.append(o);
        if (o->expandingDirections() & Qt::Horizontal)
            ++expandingWidgets;
        availableSpace = qMax(0, availableSpace - spacing() - oSizeHint.width());
    }

    // don't forget to layout the last line
    int sizeHintWidth, minSizeWidth, lineMinHeight;
    doHorizontalLayoutForLine(r, currentLine,
                              spacing(), d->justify, y, h, availableSpace, expandingWidgets,
                              sizeHintWidth, minSizeWidth, lineMinHeight, testOnly);

    sizeHint = sizeHint.expandedTo(QSize(sizeHintWidth, y + spacing() + h));
    minSizeHeight = minSizeHeight + spacing() + lineMinHeight;
    minSize = minSize.expandedTo(QSize(minSizeWidth, minSizeHeight));

    // store sizeHint() and minimumSize()
    d->cached_sizeHint = sizeHint + QSize(2 * margin(), 2 * margin());
    d->cached_minSize = minSize + QSize(2 * margin() , 2 * margin());
    // return our height
    return y + h - r.y();
}

inline void doVerticalLayoutForLine(const QRect &r, const QList<QLayoutItem*>& currentLine,
                                    int spacing, bool justify, int& x, int& w, int& availableSpace, int& expandingWidgets,
                                    int& sizeHintHeight, int& minSizeHeight, int& colMinWidth, bool testOnly)
{
    QListIterator<QLayoutItem*> it2(currentLine);
    int wy = r.y();
    sizeHintHeight = 0 - spacing;
    minSizeHeight = 0 - spacing;
    colMinWidth = 0;
    while (it2.hasNext()) {
        QLayoutItem *item = it2.next();
        QSize itemSizeHint = item->sizeHint(); // we cache these ones because it can take
        QSize itemMinSize = item->minimumSize(); // a while to get them
        QSize s;
        if (justify) {
            if (expandingWidgets != 0) {
                if (item->expandingDirections() & Qt::Vertical)
                    s = QSize(
                            itemSizeHint.width(),
                            qMin(itemSizeHint.height() + availableSpace / expandingWidgets, r.height())
                        );
                else
                    s = QSize(itemSizeHint.width(), qMin(itemSizeHint.height(), r.height()));
            } else
                s = QSize(
                        itemSizeHint.width(),
                        qMin(itemSizeHint.height() + availableSpace / (int)currentLine.count(), r.height())
                    );
        } else
            s = QSize(itemSizeHint.width(), qMin(itemSizeHint.height(), r.height()));
        if (!testOnly) {
            // adjust horizontal position depending on vertical alignment
            int add_x;
            if (item->alignment() & Qt::AlignRight)
                add_x = w - s.width() - 1;
            else if (item->alignment() & Qt::AlignHCenter)
                add_x = (w - s.width() - 1) / 2;
            else
                add_x = 0; // Qt::AlignLeft
            item->setGeometry(QRect(QPoint(x + add_x, wy), s));
        }
        wy = wy + s.height() + spacing;
        minSizeHeight = minSizeHeight + spacing + itemMinSize.height();
        sizeHintHeight = sizeHintHeight + spacing + itemSizeHint.height();
        colMinWidth = qMax(colMinWidth, itemMinSize.width());
    }
}

int KexiFlowLayout::doVerticalLayout(const QRect &r, bool testOnly)
{
    int x = r.x();
    int y = r.y();
    int w = 0; // width of this line
    int availableSpace = r.height() + spacing();
    int expandingWidgets = 0; // number of widgets in the line with QSizePolicy == Expanding
    QListIterator<QLayoutItem*> it(d->list);
    QList<QLayoutItem*> currentLine;
    QSize minSize, sizeHint(20, 20);
    int minSizeWidth = 0 - spacing();

    while (it.hasNext()) {
        QLayoutItem *o = it.next();
        if (o->isEmpty()) // do not consider hidden widgets
            continue;

        QSize oSizeHint = o->sizeHint(); // we cache these ones because it can take
        // a while to get it (eg for child layouts)
        if (y + oSizeHint.height() > r.bottom() && w > 0) {
            // do the layout of current line
            int sizeHintHeight, minSizeHeight, colMinWidth;
            doVerticalLayoutForLine(r, currentLine,
                                    spacing(), d->justify, y, w, availableSpace, expandingWidgets,
                                    sizeHintHeight, minSizeHeight, colMinWidth, testOnly);

            sizeHint = sizeHint.expandedTo(QSize(0, sizeHintHeight));
            minSize = minSize.expandedTo(QSize(0, minSizeHeight));
            minSizeWidth = minSizeWidth + spacing() + colMinWidth;
            // start a new column
            x = x + spacing() + w;
            w = 0;
            y = r.y();
            currentLine.clear();
            expandingWidgets = 0;
            availableSpace = r.height() + spacing();
        }

        y = y + spacing() + oSizeHint.height();
        w = qMax(w,  oSizeHint.width());
        currentLine.append(o);
        if (o->expandingDirections() & Qt::Vertical)
            ++expandingWidgets;
        availableSpace = qMax(0, availableSpace - spacing() - oSizeHint.height());
    }

    // don't forget to layout the last line
    int sizeHintHeight, minSizeHeight, colMinWidth;
    doVerticalLayoutForLine(r, currentLine,
                            spacing(), d->justify, y, w, availableSpace, expandingWidgets,
                            sizeHintHeight, minSizeHeight, colMinWidth, testOnly);

    sizeHint = sizeHint.expandedTo(QSize(x + spacing() + w, sizeHintHeight));
    minSizeWidth = minSizeWidth + spacing() + colMinWidth;
    minSize = minSize.expandedTo(QSize(minSizeWidth, minSizeHeight));

    // store sizeHint() and minimumSize()
    d->cached_sizeHint = sizeHint + QSize(2 * margin(), 2 * margin());
    d->cached_minSize = minSize + QSize(2 * margin(), 2 * margin());
    // return our width
    return x + w - r.x();
}

QLayoutItem *KexiFlowLayout::itemAt(int index) const
{
    return d->list.value(index);
}

QLayoutItem *KexiFlowLayout::takeAt(int index)
{
    if (index >= 0 && index < d->list.size())
        return d->list.takeAt(index);

    return 0;
}

void KexiFlowLayout::setOrientation(Qt::Orientation orientation)
{
    d->orientation = orientation;
}

Qt::Orientation KexiFlowLayout::orientation() const
{
    return d->orientation;
}

void KexiFlowLayout::setJustified(bool justify)
{
    d->justify = justify;
}

bool KexiFlowLayout::isJustified() const
{
    return d->justify;
}
