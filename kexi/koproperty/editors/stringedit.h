/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
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

#ifndef KPROPERTY_STRINGEDIT_H
#define KPROPERTY_STRINGEDIT_H

#include "koproperty/Factory.h"

#include <QtCore/QVariant>
#include <KLineEdit>

namespace KoProperty
{

class KOPROPERTY_EXPORT StringEdit : public KLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue USER true)
public:
    StringEdit(QWidget *parent = 0);
    ~StringEdit();
    virtual QString value() const;
    virtual void setValue(const QString& value);
signals:
    void commitData( QWidget * editor );
private slots:
    void slotTextChanged( const QString & text );
private:
    bool m_slotTextChangedEnabled : 1;
};

class KOPROPERTY_EXPORT StringDelegate : public EditorCreatorInterface
{
public:
    StringDelegate() {}
    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        Q_UNUSED(type);
        Q_UNUSED(option);
        Q_UNUSED(index);
        return new StringEdit(parent);
    }
};

#if 0
class QLineEdit;

namespace KoProperty
{

class KOPROPERTY_EXPORT StringEdit : public Widget
{
    Q_OBJECT

public:
    explicit StringEdit(Property *property, QWidget *parent = 0);
    virtual ~StringEdit();

    virtual QVariant value() const;
    virtual void setValue(const QVariant &value, bool emitChange = true);

protected:
    virtual void setReadOnlyInternal(bool readOnly);

protected slots:
    void slotValueChanged(const QString&);

protected:
    QLineEdit *m_edit;
};

}
#endif

}

#endif
