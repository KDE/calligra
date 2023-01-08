/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
class MapBase;
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
    explicit BindingManager(const MapBase *map);

    /**
     * Destructor.
     */
    ~BindingManager() override;

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
