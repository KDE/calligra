/* This file is part of the KDE project
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
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

#include "KexiTableScrollAreaHeader.h"
#include "KexiTableScrollArea.h"
#include "KexiTableScrollAreaHeaderModel.h"

#include <QApplication>
#include <QToolTip>
#include <QProxyStyle>
#include <QStyleOptionHeader>
#include <QMouseEvent>
#include <QHelpEvent>
#include <QPainter>
#include <QDebug>

#include <kiconloader.h>
#include <kexiutils/utils.h>
#include <kexiutils/styleproxy.h>

//! @internal A style that allows to temporary change background color while
//!           drawing header section primitive. Used in KexiTableScrollAreaHeader.
class KexiTableScrollAreaHeaderStyle : public QProxyStyle
{
public:
    KexiTableScrollAreaHeaderStyle(QStyle *parentStyle)
            : QProxyStyle(parentStyle)
    {
    }
    virtual ~KexiTableScrollAreaHeaderStyle() {}

    virtual void drawControl(ControlElement ce,
                             const QStyleOption *option, QPainter *painter,
                             const QWidget *widget = 0) const
    {
        const KexiTableScrollAreaHeader *headerWidget
                = qobject_cast<const KexiTableScrollAreaHeader*>(parent());
        if (ce == CE_Header && option) {
            QStyleOptionHeader newOption(*qstyleoption_cast<const QStyleOptionHeader*>(option));
            const int currentSection = headerWidget->orientation() == Qt::Horizontal
                                     ? headerWidget->currentIndex().column()
                                     : headerWidget->currentIndex().row();
//            qDebug() << headerWidget->orientation() << currentSection <<
//                        headerWidget->currentIndex().row() << headerWidget->currentIndex().column();

            if (newOption.section >= 0) {
                int f1 = 0, f2 = 0;
                if (newOption.section == currentSection) {
                    f1 = 34;
                    f2 = 66;
                }
                else if (headerWidget->scrollArea()->appearance().rowMouseOverHighlightingEnabled
                         && headerWidget->orientation() == Qt::Vertical
                         && newOption.section == headerWidget->scrollArea()->highlightedRow())
                {
                    f1 = 10;
                    f2 = 90;
                }
                if (f1 > 0) {
                    newOption.palette.setColor(QPalette::Button,
                        KexiUtils::blendedColors(
                            headerWidget->selectionBackgroundColor(),
                            headerWidget->palette().color(headerWidget->backgroundRole()), f1, f2));

                    //set background color as well (e.g. for thinkeramik)
                    newOption.palette.setColor(QPalette::Window, newOption.palette.color(QPalette::Button));
                }
                if (headerWidget->orientation() == Qt::Vertical) {
                    // For mouse-over styles such as Breeze fill color and animate,
                    // what's in conflict with what we do: disable this.
                    newOption.state &= (0xffffffff ^ QStyle::State_MouseOver);
                    //qDebug() << newOption.rect;
                }
                QProxyStyle::drawControl(ce, &newOption, painter, widget);
                return;
            }
        }
        QProxyStyle::drawControl(ce, option, painter, widget);
    }
};

class KexiTableScrollAreaHeader::Private
{
public:
    Private()
     : selectionBackgroundColor(qApp->palette().highlight().color())
     , styleChangeEnabled(true)
    {
    }

    QColor selectionBackgroundColor;
    QPointer<QStyle> privateStyle;
    bool styleChangeEnabled;
};

KexiTableScrollAreaHeader::KexiTableScrollAreaHeader(Qt::Orientation orientation, KexiTableScrollArea* parent)
        : QHeaderView(orientation, parent)
        , d(new Private)
{
    setMovable(false);
    setSortingEnabled(parent->isSortingEnabled() && orientation == Qt::Horizontal);
    setResizeMode(orientation == Qt::Horizontal ?  QHeaderView::Interactive : QHeaderView::Fixed);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHighlightSections(true);
    setModel(parent->headerModel());
    styleChanged();
}

KexiTableScrollAreaHeader::~KexiTableScrollAreaHeader()
{
    setStyle(0);
    delete d->privateStyle;
    setModel(0); // avoid referencing deleted model
    delete d;
}

void KexiTableScrollAreaHeader::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::StyleChange) {
        styleChanged();
    }
    QHeaderView::changeEvent(e);
}

void KexiTableScrollAreaHeader::styleChanged()
{
    if (!d->styleChangeEnabled)
        return;
    d->styleChangeEnabled = false;
    if (d->privateStyle) {
        setStyle(0);
        delete static_cast<QStyle*>(d->privateStyle);
    }
    setStyle(d->privateStyle = new KexiTableScrollAreaHeaderStyle(style()));
    d->privateStyle->setParent(this);
    d->styleChangeEnabled = true;
}

int KexiTableScrollAreaHeader::preferredSectionSize(int logicalIndex) const
{
    const QString label = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
    int preferredWidth = fontMetrics().width(label) + style()->pixelMetric(QStyle::PM_HeaderMargin) * 2;
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex) {
        preferredWidth += (style()->pixelMetric(QStyle::PM_HeaderMarkSize)
                     + 2 * style()->pixelMetric(QStyle::PM_HeaderMargin));
    }
    QVariant decoration = model()->headerData(logicalIndex, orientation(), Qt::DecorationRole);
    const QIcon icon = decoration.value<QIcon>();
    if (!icon.isNull()) {
        preferredWidth += IconSize(KIconLoader::Small) + style()->pixelMetric(QStyle::PM_HeaderMargin);
    }
    else {
        const QPixmap iconPixmap = decoration.value<QPixmap>();
        if (!iconPixmap.isNull()) {
            preferredWidth += iconPixmap.width() + style()->pixelMetric(QStyle::PM_HeaderMargin);
        }
    }
    return preferredWidth;
}

bool KexiTableScrollAreaHeader::viewportEvent(QEvent *e)
{
    if (   orientation() == Qt::Horizontal
        && e->type() == QEvent::ToolTip)
    {
        QHelpEvent *he = static_cast<QHelpEvent*>(e);
        int section = logicalIndexAt(he->pos());
        // take description by default
        QString tipText = model()->headerData(section, orientation(), Qt::ToolTipRole).toString();
        if (tipText.isEmpty() && preferredSectionSize(section) > sectionSize(section)) {
            // try label only if it's not entirely visible
            tipText = model()->headerData(section, orientation(), Qt::DisplayRole).toString();
        }
        QToolTip::showText(he->globalPos(), tipText,
                           this,
                           orientation() == Qt::Horizontal
                           ? QRect(sectionPosition(section), 0, sectionSize(section), height())
                           : QRect(0, sectionPosition(section), width(), sectionSize(section)));
        return true;
    }
    return QHeaderView::viewportEvent(e);
}

void KexiTableScrollAreaHeader::mousePressEvent(QMouseEvent *e)
{
    if (orientation() == Qt::Vertical && e->button() == Qt::LeftButton) {
        const int section = logicalIndexAt(e->y());
        scrollArea()->setCursorPosition(section);
    }
    QHeaderView::mousePressEvent(e);
}

void KexiTableScrollAreaHeader::mouseMoveEvent(QMouseEvent *e)
{
    if (orientation() == Qt::Vertical) {
        const int section = logicalIndexAt(e->y());
        scrollArea()->setHighlightedRow(section);
    }
    QHeaderView::mouseMoveEvent(e);
}

void KexiTableScrollAreaHeader::leaveEvent(QEvent *e)
{
    if (orientation() == Qt::Vertical) {
        scrollArea()->setHighlightedRow(-1);
    }
    QHeaderView::leaveEvent(e);
}

void KexiTableScrollAreaHeader::setSelectionBackgroundColor(const QColor &color)
{
    d->selectionBackgroundColor = color;
}

QColor KexiTableScrollAreaHeader::selectionBackgroundColor() const
{
    return d->selectionBackgroundColor;
}

int KexiTableScrollAreaHeader::headerWidth() const
{
    int w = 0;
    for (int i = 0; i < count(); i++) {
        w += sectionSize(i);
    }
    return w;
}

KexiTableScrollArea* KexiTableScrollAreaHeader::scrollArea() const
{
    return qobject_cast<KexiTableScrollArea*>(parentWidget());
}

void KexiTableScrollAreaHeader::updateSection(int logicalIndex)
{
    QHeaderView::updateSection(logicalIndex);
}

QSize KexiTableScrollAreaHeader::sectionSizeFromContents(int logicalIndex) const
{
    //QSize s = QHeaderView::sectionSizeFromContents(logicalIndex);

    // otherwise use the contents
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.section = logicalIndex;
    opt.orientation = orientation();
    QVariant var = model()->headerData(logicalIndex, orientation(), Qt::FontRole);
    QFont fnt;
    if (var.isValid() && var.canConvert<QFont>())
        fnt = qvariant_cast<QFont>(var);
    else
        fnt = font();
    fnt.setBold(true);
    opt.fontMetrics = QFontMetrics(fnt);
    opt.text = model()->headerData(logicalIndex, orientation(),
                                    Qt::DisplayRole).toString();
    //qDebug() << opt.text;
    QVariant variant = model()->headerData(logicalIndex, orientation(), Qt::DecorationRole);
    //qDebug() << variant;
//            opt.icon = qvariant_cast<QIcon>(variant);
//            if (opt.icon.isNull())
//                opt.icon = qvariant_cast<QPixmap>(variant);
    QSize size = style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), this);
    //qDebug() << size;
    if (isSortIndicatorShown()) {
        int margin = style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);
        if (orientation() == Qt::Horizontal)
            size.rwidth() += size.height() + margin;
        else
            size.rheight() += size.width() + margin;
        //qDebug() << "margin" << margin;
    }
    //qDebug() << size << "---";
    //qDebug() << orientation() << logicalIndex << s;
    if (orientation() == Qt::Horizontal) {
        KDbTableViewData *data = scrollArea()->data();
        KDbTableViewColumn *col = data->visibleColumn(logicalIndex);
        if (col && col->width() > 0) {
            size.setWidth(col->width());
        }
    }
    return size;
}

QSize KexiTableScrollAreaHeader::sizeHint() const
{
    if (orientation() == Qt::Horizontal) {
        int width = 0;
        if (scrollArea()) {
            width = scrollArea()->viewport()->width() - 1;
        }
        int height = 0;
        for (int i = 0; i < count(); i++) {
            if (isSectionHidden(i))
                continue;
            QSize hint = sectionSizeFromContents(i);
            height = qMax(height, hint.height());
            //qDebug() << "height:" << height;
        }
        return QSize(width, height);
    }
    else { // vertical
        int height = 0;
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        if (scrollArea()) {
            //qDebug() << "scrollArea()->width():" << scrollArea()->width();
            height = scrollArea()->viewport()->height() - 1;
        }
        //qDebug() << scrollArea()->style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this);
        return QSize(scrollArea()->rowHeight()
                       + style()->pixelMetric(QStyle::PM_HeaderMargin, &opt, this),
                     height);
    }
}

void KexiTableScrollAreaHeader::setSortingEnabled(bool set)
{
    setClickable(set);
    setSortIndicatorShown(set);
}

