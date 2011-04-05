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

/* WARNING (DK) KLineEdit has been changed by QLineEdit ! 
 * Since KLineEdit extends QLineEdit and KCompletionBase it is very likely that some funcionality has been lost!. 
 */

/* TODO (DK) Provide class that extends QLineEdit and provide completion feature into widgets and use it instead of QLineEdit.
 */
#include <QLineEdit>

namespace KoProperty
{

class KOPROPERTY_EXPORT StringEdit : public QLineEdit // WARNING (DK) KLineEdit has been changed by QLineEdit! Some funcionality has been lost!
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue USER true)
public:
    StringEdit(QWidget *parent = 0);

    ~StringEdit();

    QString value() const;

signals:
    void commitData( QWidget * editor );

public slots:
    void setValue(const QString& value);

private slots:
    void slotTextChanged( const QString & text );

private:
    bool m_slotTextChangedEnabled;
};

class KOPROPERTY_EXPORT StringDelegate : public EditorCreatorInterface
{
public:
    StringDelegate() {}

    virtual QWidget * createEditor( int type, QWidget *parent, 
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

}

#endif
