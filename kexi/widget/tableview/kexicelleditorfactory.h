/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXICELLEDITORFACTORY_H
#define KEXICELLEDITORFACTORY_H

#include <qvariant.h>
#include <qwidget.h>

#include <kexi_global.h>
#include <kexidb/field.h>

class KexiCellEditorFactoryItem;
class KexiTableEdit;
class KexiTableViewColumn;

//! A singleton class providing access to cell editor factories
class KEXIDATATABLE_EXPORT KexiCellEditorFactory
{
public:
    KexiCellEditorFactory();
    virtual ~KexiCellEditorFactory();

    /*! Registers factory item for \a type and (optional) \a subType.
     \a subType is usually obtained (e.g. in KexiTableView) from KexiDB::Field::subType().
     Passing KexiDB::Field::Invalid as type will set default item,
     i.e. the one that will be used when no other item is defined for given data type.
     You can register the same \a item many times for different types and subtypes.
     Once registered, \a item object will be owned by the factory, so you shouldn't
     care about deleting it. */
    static void registerItem(KexiCellEditorFactoryItem& item, uint type,
                             const QString& subType = QString());

    /*! \return item for \a type and (optional) \a subType.
     If no item found, the one with empty subtype is tried.
     If still no item found, the default is tried. Eventually, may return NULL. */
    static KexiCellEditorFactoryItem* item(uint type, const QString& subType = QString());

    /*! Creates a new editor for \a column. If \a parent is of QScrollArea, the new editor
     will be created inside parent->viewport() instead. */
    static KexiTableEdit* createEditor(KexiTableViewColumn &column, QWidget* parent = 0);

protected:
    static void init();
};

//! A base class for implementing cell editor factories
class KEXIDATATABLE_EXPORT KexiCellEditorFactoryItem
{
public:
    KexiCellEditorFactoryItem();
    virtual ~KexiCellEditorFactoryItem();
    QString className() {
        return m_className;
    }

protected:
    virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QWidget* parent = 0) = 0;

    QString m_className;
    friend class KexiCellEditorFactory;
};

#endif
