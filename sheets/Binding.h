/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef CALLIGRA_SHEETS_BINDING
#define CALLIGRA_SHEETS_BINDING

#include <QSharedDataPointer>
#include <QVariant>

#include "Region.h"

#include "sheets_odf_export.h"

class QAbstractItemModel;

namespace Calligra
{
namespace Sheets
{

/**
 * Abstracts read-only access to the ValueStorage.
 * Useful for KoChart (or other apps, that want read-only access to Calligra Sheets' data).
 *
 * If a cell in the region is updated, the BindingManager informs this Binding, which
 * in turn informs the model it holds.
 */
class CALLIGRA_SHEETS_ODF_EXPORT Binding
{
public:
    Binding();
    explicit Binding(const Region& region);
    Binding(const Binding& other);
    ~Binding();

    bool isEmpty() const;

    QAbstractItemModel* model() const;

    const Region& region() const;
    void setRegion(const Region& region);

    void update(const Region& region);

    void operator=(const Binding& other);
    bool operator==(const Binding& other) const;
    bool operator<(const Binding& other) const;

private:
    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Binding)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Binding, Q_MOVABLE_TYPE);

#endif // CALLIGRA_SHEETS_BINDING
