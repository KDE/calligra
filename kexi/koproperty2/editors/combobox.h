/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_COMBOBOX_H
#define KPROPERTY_COMBOBOX_H

#include "Factory.h"
#include <KComboBox>

class ComboBox : public KComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    explicit ComboBox(const KoProperty::Property* property, QWidget *parent = 0);

    virtual ~ComboBox();

    virtual QVariant value() const;

    virtual void setProperty(const KoProperty::Property *property);
    
//    virtual void drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value);

public slots:
    virtual void setValue(const QVariant &value);

protected slots:
    void slotValueChanged(int value);

protected:
//    virtual void setReadOnlyInternal(bool readOnly);
    QString keyForValue(const QVariant &value);
    void fillValues();

//    KComboBox *m_edit;
    const KoProperty::Property *m_property;
//    QList<QVariant> keys;
    bool m_setValueEnabled : 1;
    bool m_extraValueAllowed : 1;
};

class ComboBoxDelegate : public KoProperty::EditorCreatorInterface, 
                         public KoProperty::ValueDisplayInterface //ValuePainterInterface
{
public:
    ComboBoxDelegate();
    
    virtual QString displayText( const KoProperty::Property* property ) const;

    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

//    virtual void paint( QPainter * painter, 
//        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif
