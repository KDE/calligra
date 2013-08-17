/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 2007 Pino Toscano (pino@kde.org)
    Copyright (c) 2007 David Jarvie (software@astrojar.org.uk)

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
    Boston, MA 02110-1301, USA.
*/

#include "kcolorcombo.h"

#include <QAbstractItemDelegate>
#include <QApplication>
#include <QColorDialog>
#include <QStylePainter>

class KColorComboDelegate : public QAbstractItemDelegate
{
public:
    enum ItemRoles {
        ColorRole = Qt::UserRole + 1
    };

    enum LayoutMetrics {
        FrameMargin = 3
    };

    KColorComboDelegate(QObject *parent = 0);
    virtual ~KColorComboDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

static QBrush k_colorcombodelegate_brush(const QModelIndex &index, int role)
{
    QBrush brush;
    QVariant v = index.data(role);
    if (v.type() == QVariant::Brush) {
        brush = v.value<QBrush>();
    } else if (v.type() == QVariant::Color) {
        brush = QBrush(v.value<QColor>());
    }
    return brush;
}

KColorComboDelegate::KColorComboDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

KColorComboDelegate::~KColorComboDelegate()
{
}

void KColorComboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // background
    QColor innercolor(Qt::white);
    bool isSelected = (option.state & QStyle::State_Selected);
    bool paletteBrush = (k_colorcombodelegate_brush(index, Qt::BackgroundRole).style() == Qt::NoBrush);
    if (isSelected) {
        innercolor = option.palette.color(QPalette::Highlight);
    } else {
        innercolor = option.palette.color(QPalette::Base);
    }
    // highlight selected item
    QStyleOptionViewItemV4 opt(option);
    opt.showDecorationSelected = true;
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    QRect innerrect = option.rect.adjusted(FrameMargin, FrameMargin, -FrameMargin, -FrameMargin);
    // inner color
    QVariant cv = index.data(ColorRole);
    if (cv.type() == QVariant::Color) {
        QColor tmpcolor = cv.value<QColor>();
        if (tmpcolor.isValid()) {
            innercolor = tmpcolor;
            paletteBrush = false;
            painter->setPen(Qt::transparent);
            painter->setBrush(innercolor);
            QPainter::RenderHints tmpHint = painter->renderHints();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->drawRoundedRect(innerrect, 2, 2);
            painter->setRenderHints(tmpHint);
            painter->setBrush(Qt::NoBrush);
        }
    }
    // text
    QVariant tv = index.data(Qt::DisplayRole);
    if (tv.type() == QVariant::String) {
        QString text = tv.toString();
        QColor textColor;
        if (paletteBrush) {
            if (isSelected) {
                textColor = option.palette.color(QPalette::HighlightedText);
            } else {
                textColor = option.palette.color(QPalette::Text);
            }
        } else {
            int unused, v;
            innercolor.getHsv(&unused, &unused, &v);
            if (v > 128) {
                textColor = Qt::black;
            } else {
                textColor = Qt::white;
            }
        }
        painter->setPen(textColor);
        painter->drawText(innerrect.adjusted(1, 1, -1, -1), text);
    }
}

QSize KColorComboDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    // the width does not matter, as the view will always use the maximum width available
    return QSize(100, option.fontMetrics.height() + 2 * FrameMargin);
}


static const uchar standardPalette[][4] = {
    { 255, 255, 255 }, // white
    { 192, 192, 192 }, // light gray
    { 160, 160, 160 }, // gray
    { 128, 128, 128 }, // dark gray
    { 0, 0, 0 }, // black

    { 255, 128, 128 }, //light red
    { 255, 192, 128 }, //light orange
    { 255, 255, 128 }, //light yellow
    { 128, 255, 128 }, //light green
    { 128, 255, 255 }, //cyan blue
    { 128, 128, 255 }, //light blue
    { 255, 128, 255 }, //light violet
    { 255, 0, 0 }, //red
    { 255, 128, 0 }, //orange
    { 255, 255, 0 }, //yellow
    { 0, 255, 0 }, //green
    { 0, 255, 255 }, //light blue
    { 0, 0, 255 }, //blue
    { 255, 0, 255 }, //violet
    { 128, 0, 0 }, //dark red
    { 128, 64, 0 }, //dark orange
    { 128, 128, 0 }, //dark yellow
    { 0, 128, 0 }, //dark green
    { 0, 128, 128 }, //dark light blue
    { 0, 0, 128 }, //dark blue
    { 128, 0, 128 } //dark violet
};

#define STANDARD_PALETTE_SIZE (int(sizeof(standardPalette) / sizeof(*standardPalette)))

static inline QColor standardColor(int i)
{
    const uchar *entry = standardPalette[i];
    return QColor(entry[0], entry[1], entry[2]);
}

class KColorComboPrivate
{
public:
    KColorComboPrivate(KColorCombo *qq);

    void addColors();
    void setCustomColor(const QColor &color, bool lookupInPresets = true);

    // slots
    void _k_slotActivated(int index);
    void _k_slotHighlighted(int index);

    KColorCombo *q;
    QList<QColor> colorList;
    QColor customColor;
    QColor internalcolor;
};

KColorComboPrivate::KColorComboPrivate(KColorCombo *qq)
    : q(qq), customColor(Qt::white)
{
}

void KColorComboPrivate::setCustomColor(const QColor &color, bool lookupInPresets)
{
    if (lookupInPresets) {
        if (colorList.isEmpty()) {
            for (int i = 0; i < STANDARD_PALETTE_SIZE; ++i) {
                if (standardColor(i) == color) {
                    q->setCurrentIndex(i + 1);
                    internalcolor = color;
                    return;
                }
            }
        } else {
            int i = colorList.indexOf(color);
            if (i >= 0) {
                q->setCurrentIndex(i + 1);
                internalcolor = color;
                return;
            }
        }
    }

    internalcolor = color;
    customColor = color;
    q->setItemData(0, customColor, KColorComboDelegate::ColorRole);
}


KColorCombo::KColorCombo( QWidget *parent )
    : QComboBox(parent), d(new KColorComboPrivate(this))
{
    setItemDelegate(new KColorComboDelegate(this));
    d->addColors();

    connect(this, SIGNAL(activated(int)), SLOT(_k_slotActivated(int)));
    connect(this, SIGNAL(highlighted(int)), SLOT(_k_slotHighlighted(int)));

    // select the white color
    setCurrentIndex(1);
    d->_k_slotActivated(1);
    
    setMaxVisibleItems(13);
}


KColorCombo::~KColorCombo()
{
    delete d;
}

void KColorCombo::setColors( const QList<QColor> &colors )
{
    clear();
    d->colorList = colors;
    d->addColors();
}

QList<QColor> KColorCombo::colors() const
{
    if (d->colorList.isEmpty()) {
        QList<QColor> list;
        for (int i = 0; i < STANDARD_PALETTE_SIZE; ++i) {
            list += standardColor(i);
    }
        return list;
    } else {
        return d->colorList;
    }
}

void KColorCombo::setColor( const QColor &col )
{
    if (!col.isValid()) {
        return;
    }

    if (count() == 0) {
        d->addColors();
    }

    d->setCustomColor(col, true);
}

QColor KColorCombo::color() const
{
  return d->internalcolor;
}

bool KColorCombo::isCustomColor() const
{
    return d->internalcolor == d->customColor;
}

void KColorCombo::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    QRect frame = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::transparent);
    painter.setBrush(QBrush(d->internalcolor));
    painter.drawRoundedRect(frame.adjusted(1, 1, -1, -1), 2, 2);
}

void KColorCombo::showEmptyList()
{
    clear();
}

void KColorComboPrivate::_k_slotActivated(int index)
{
    if (index == 0) {
        QColor c = QColorDialog::getColor(customColor, q);
        if (c.isValid()) {
            customColor = c;
            setCustomColor(customColor, false);
        }
    } else if (colorList.isEmpty()) {
        internalcolor = standardColor(index - 1);
    } else {
        internalcolor = colorList[index - 1];
    }

    emit q->activated(internalcolor);
}

void KColorComboPrivate::_k_slotHighlighted(int index)
{
    if (index == 0) {
        internalcolor = customColor;
    } else if (colorList.isEmpty()) {
        internalcolor = standardColor(index - 1);
    } else {
        internalcolor = colorList[index - 1];
    }

    emit q->highlighted(internalcolor);
}

void KColorComboPrivate::addColors()
{
    q->addItem(QObject::tr("Custom...", "Custom color"));

    if (colorList.isEmpty()) {
        for (int i = 0; i < STANDARD_PALETTE_SIZE; ++i) {
            q->addItem(QString());
            q->setItemData(i + 1, standardColor(i), KColorComboDelegate::ColorRole);
        }
    } else {
        for (int i = 0, count = colorList.count(); i < count; ++i) {
            q->addItem(QString());
            q->setItemData(i + 1, colorList[i], KColorComboDelegate::ColorRole);
        }
    }
}

#include "moc_kcolorcombo.cpp"
