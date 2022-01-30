/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DATABASE
#define CALLIGRA_SHEETS_DATABASE

#include <QSharedDataPointer>

#include "sheets_core_export.h"


namespace Calligra
{
namespace Sheets
{
class Filter;
class Map;
class Region;

/**
 * OpenDocument, 8.6.1 Database Range
 *
 * Source of data mapped to a database range. Largely unused right now, we just load/save these, and use them for filtering.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Database
{
public:
    /**
     * Constructor.
     * Creates an empty database.
     */
    Database();

    /**
     * Copy Constructor.
     */
    Database(const Database& other);

    /**
     * Destructor.
     */
    ~Database();

    /**
     * \return \c true if this is the default/empty database
     */
    bool isEmpty() const;

    bool isSelection() const;
    void setIsSelection(bool sel);
    bool onUpdateKeepStyles() const;
    bool onUpdateKeepSize() const;
    bool hasPersistentData() const;
    void setOnUpdateKeepStyles(bool val);
    void setOnUpdateKeepSize(bool val);
    void setHasPersistentData(bool val);
    int refreshDelay() const;
    void setRefreshDelay(int delay);

    /**
     * \return the database's orientation
     */
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation o);

    /**
     * \return \c true if the range contains a header column/row
     */
    bool containsHeader() const;

    /**
     * Sets whether the range contains a header column/row
     */
    void setContainsHeader(bool enable);

    /**
     * \return \c true if filter buttons should be displayed
     */
    bool displayFilterButtons() const;

    /**
     * Sets whether filter buttons should be displayed.
     */
    void setDisplayFilterButtons(bool enable);

    /**
     * \return the actual database cell range
     */
    const Region& range() const;

    /**
     * Sets the actual database cell range.
     * \p region has to be contiguous.
     */
    void setRange(const Region& region);

    const Filter& filter() const;
    void setFilter(const Filter& filter);

    void operator=(const Database& other);
    bool operator==(const Database& other) const;
    bool operator<(const Database& other) const;

    void dump() const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Database)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Database, Q_MOVABLE_TYPE);

#endif // CALLIGRA_SHEETS_DATABASE
