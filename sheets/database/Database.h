/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DATABASE
#define CALLIGRA_SHEETS_DATABASE

#include <QSharedDataPointer>
#include <QVariant>

#include <KoXmlReader.h>

#include "sheets_odf_export.h"


class KoXmlWriter;

namespace Calligra
{
namespace Sheets
{
class Filter;
class Map;
class Region;

/**
 * OpenDocument, 8.6.1 Database Range
 */
class CALLIGRA_SHEETS_ODF_EXPORT Database
{
public:
    /**
     * Constructor.
     * Creates an empty database.
     */
    Database();

    /**
     * Constructor.
     * Creates a database named \p name.
     */
    explicit Database(const QString &name);

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

    /**
     * \return the database's name
     */
    const QString& name() const;

    /**
     * Sets the database's name.
     */
    void setName(const QString& name);

    /**
     * \return the database's orientation
     */
    Qt::Orientation orientation() const;

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

    bool loadOdf(const KoXmlElement& element, const Map* map);
    void saveOdf(KoXmlWriter& xmlWriter) const;

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
