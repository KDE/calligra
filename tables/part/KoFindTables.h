/* This file is part of the KDE project
 *
 * Copyright (c) 2011 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOFINDTABLES_H
#define KOFINDTABLES_H

#include "calligra_tables_export.h"
#include <KoFindBase.h>
#include <Cell.h>

namespace Calligra
{
namespace Tables
{

class Sheet;
class CALLIGRA_TABLES_COMMON_EXPORT KoFindTables : public KoFindBase
{
    Q_OBJECT
public:
    explicit KoFindTables ( QObject* parent = 0 );

public Q_SLOTS:
    void setCurrentSheet(Sheet* sheet);
    
protected:
    virtual void replaceImplementation ( const KoFindMatch& match, const QVariant& value );
    virtual void findImplementation ( const QString& pattern, KoFindBase::KoFindMatchList& matchList );

private:
    class Private;
    Private * const d;
};

} //namespace Tables
} //namespace Calligra

Q_DECLARE_METATYPE(Calligra::Tables::Cell)
Q_DECLARE_METATYPE(Calligra::Tables::Sheet*)

#endif // KOFINDTABLES_H
