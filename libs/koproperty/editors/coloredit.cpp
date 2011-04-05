/* This file is part of the KDE project
   Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>

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

#include "coloredit.h"
#include "koproperty/Utils_p.h"

#include <QVariant>
#include <QLayout>
#include <QColor>
#include <QPainter>

#include <slp/KGlobal>

/** @WARNING (DK) KColorCombo has been changed by QComboBox ! 
 * Since KColorCombo extends QComboBox it is very likely that some funcionality has been lost!. 
 */

/** @TODO (DK) Provide class that extends QComboBox and provide completion feature into widgets and use it instead of QComboBox.
 */
#include <QComboBox>

#warning (DK) KColorCombo has been changed by QComboBox !!
#ifdef __KDE4_LIBS__
  #include <KColorCombo>
#endif

#include <KColorCollection>

using namespace KoProperty;

K_GLOBAL_STATIC_WITH_ARGS(KColorCollection, g_oxygenColors, ("Oxygen.colors"))

#warning (DK) KColorCombo has been changed by QComboBox !!
ColorCombo::ColorCombo(QWidget *parent)
        : QComboBox(parent) /** @WARNING (DK) KColorCombo has been changed by QComboBox ! */
{
    connect(this, SIGNAL(activated(QColor)), this, SLOT(slotValueChanged(QColor)));

    QList< QColor > colors;
    const int oxygenColorsCount = g_oxygenColors->count();
    for (int i = 0; i < oxygenColorsCount; i++) {
        colors += g_oxygenColors->color(i);
    }
    setColors(colors);
}

ColorCombo::~ColorCombo()
{
}

QVariant ColorCombo::value() const
{
    return color();
}

void ColorCombo::setValue(const QVariant &value)
{
    setColor(value.value<QColor>());
}

void ColorCombo::slotValueChanged(const QColor&)
{
    emit commitData(this);
}

QWidget * ColorComboDelegate::createEditor( int type, QWidget *parent, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new ColorCombo(parent);
}

void ColorComboDelegate::paint( QPainter * painter, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
    const QBrush b(index.data(Qt::EditRole).value<QColor>());
    painter->setBrush(b);
    painter->setPen(QPen(Qt::NoPen));
    painter->drawRect(option.rect);
    painter->setBrush(KoProperty::contrastColor(b.color()));
    painter->setPen(KoProperty::contrastColor(b.color()));
    QFont f(option.font);
    f.setFamily("courier");
    painter->setFont(f);
    painter->drawText(option.rect, Qt::AlignCenter, b.color().name());
    painter->restore();
}

#include "coloredit.moc"
