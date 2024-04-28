/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_NAMED_AREA_MANAGER
#define CALLIGRA_SHEETS_NAMED_AREA_MANAGER

#include <QObject>

#include "sheets_engine_export.h"

class QString;

namespace Calligra
{
namespace Sheets
{
class MapBase;
class Region;
class SheetBase;

/**
 * Manages named cell areas.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT NamedAreaManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    explicit NamedAreaManager(const MapBase *map);

    /**
     * Destructor.
     */
    ~NamedAreaManager() override;

    const MapBase *map() const;

    void remove(SheetBase *sheet);

    Region namedArea(const QString &name) const;
    SheetBase *sheet(const QString &name) const;
    bool contains(const QString &name) const;

    /**
     * Returns the list of all registered named areas.
     * \return the list of named areas
     */
    QList<QString> areaNames() const;

    void regionChanged(const Region &region);
    void updateAllNamedAreas();

public Q_SLOTS:
    /**
     * Adds a named area.
     * \note The name is valid for the whole document.
     * \param region the cell range to be named
     * \param name the name of the new area
     */
    void insert(const Region &region, const QString &name);

    void remove(const QString &name);

Q_SIGNALS:
    void namedAreaAdded(const QString &);
    void namedAreaRemoved(const QString &);
    void namedAreaModified(const QString &);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_NAMED_AREA_MANAGER
