/* This file is part of the KDE project
   Copyright (C) 2008-2009 Jarosław Staniek <staniek@kde.org>

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

#include "sizepolicyedit.h"

#include <QtGui/QSizePolicy>
#include <QLocale>
#include <slp/KGlobal>

using namespace KoProperty;

class SizePolicyListData : public Property::ListData
{
public:
    SizePolicyListData() : Property::ListData(keysInternal(), stringsInternal())
    {
    }

    QString nameForPolicy(QSizePolicy::Policy p) {
        const int index = keys.indexOf((int)p);
        if (index == -1)
            return names[0];
        return names[index];
    }
private:
    static QList<QVariant> keysInternal() {
        QList<QVariant> keys;
        keys
         << QSizePolicy::Fixed
         << QSizePolicy::Minimum
         << QSizePolicy::Maximum
         << QSizePolicy::Preferred
         << QSizePolicy::Expanding
         << QSizePolicy::MinimumExpanding
         << QSizePolicy::Ignored;
        return keys;
    }

    static QStringList stringsInternal() {
        QStringList strings;
        strings
         << QObject::tr("Fixed","Size Policy")
         << QObject::tr("Minimum","Size Policy")
         << QObject::tr("Maximum","Size Policy")
         << QObject::tr("Preferred","Size Policy")
         << QObject::tr("Expanding","Size Policy")
         << QObject::tr("Minimum Expanding","Size Policy")
         << QObject::tr("Ignored","Size Policy");
        return strings;
    }
};

K_GLOBAL_STATIC(SizePolicyListData, s_sizePolicyListData)

//---------

static const char *SIZEPOLICY_MASK = "%1, %2, %3, %4";

QString SizePolicyDelegate::displayText( const QVariant& value ) const
{
    const QSizePolicy sp(value.value<QSizePolicy>());

    return QString::fromLatin1(SIZEPOLICY_MASK)
        .arg(s_sizePolicyListData->nameForPolicy(sp.horizontalPolicy()))
        .arg(s_sizePolicyListData->nameForPolicy(sp.verticalPolicy()))
        .arg(sp.horizontalStretch())
        .arg(sp.verticalStretch());
}

//static
const Property::ListData& SizePolicyDelegate::listData()
{
    return *s_sizePolicyListData;
}

//------------

SizePolicyComposedProperty::SizePolicyComposedProperty(Property *property)
        : ComposedPropertyInterface(property)
{
    (void)new Property("hor_policy", new SizePolicyListData(),
        QVariant(), QObject::tr("Hor. Policy"), QObject::tr("Horizontal Policy"), ValueFromList, property);
    (void)new Property("vert_policy", new SizePolicyListData(),
        QVariant(), QObject::tr("Vert. Policy"), QObject::tr("Vertical Policy"), ValueFromList, property);
    (void)new Property("hor_sQObject::tretch", QVariant(),
        QObject::tr("Hor. SQObject::tretch"), QObject::tr("Horizontal SQObject::tretch"), UInt, property);
    (void)new Property("vert_sQObject::tretch", QVariant(),
        QObject::tr("Vert. SQObject::tretch"), QObject::tr("Vertical SQObject::tretch"), UInt, property);
}

void SizePolicyComposedProperty::setValue(Property *property,
    const QVariant &value, bool rememberOldValue)
{
    const QSizePolicy sp( value.value<QSizePolicy>() );
    property->child("hor_policy")->setValue(sp.horizontalPolicy(), rememberOldValue, false);
    property->child("vert_policy")->setValue(sp.verticalPolicy(), rememberOldValue, false);
    property->child("hor_stretch")->setValue(sp.horizontalStretch(), rememberOldValue, false);
    property->child("vert_stretch")->setValue(sp.verticalStretch(), rememberOldValue, false);
}

void SizePolicyComposedProperty::childValueChanged(Property *child,
    const QVariant &value, bool rememberOldValue)
{
    Q_UNUSED(rememberOldValue);
    QSizePolicy sp( child->parent()->value().value<QSizePolicy>() );
    if (child->name() == "hor_policy")
        sp.setHorizontalPolicy(static_cast<QSizePolicy::Policy>(value.toInt()));
    else if (child->name() == "vert_policy")
        sp.setVerticalPolicy(static_cast<QSizePolicy::Policy>(value.toInt()));
    else if (child->name() == "hor_stretch")
        sp.setHorizontalStretch(value.toInt());
    else if (child->name() == "vert_stretch")
        sp.setVerticalStretch(value.toInt());

    child->parent()->setValue(sp, true, false);
}
