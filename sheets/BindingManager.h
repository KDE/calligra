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

#ifndef CALLIGRA_SHEETS_BINDING_MANAGER
#define CALLIGRA_SHEETS_BINDING_MANAGER

#include <QObject>

class QAbstractItemModel;
class QString;

namespace Calligra
{
namespace Sheets
{
class Map;
class Region;

/**
 * Manages cell ranges acting as data sources.
 */
class BindingManager : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    explicit BindingManager(const Map *map);

    /**
     * Destructor.
     */
    ~BindingManager() override;

    // KoTable::SourceRangeManager interface
    virtual const QAbstractItemModel* createModel(const QString& regionName);
    virtual bool removeModel(const QAbstractItemModel* model);
    virtual bool isCellRegionValid(const QString& regionName) const;

    void regionChanged(const Region& region);
    void updateAllBindings();

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_BINDING_MANAGER
