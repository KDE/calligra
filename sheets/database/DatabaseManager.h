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

#ifndef CALLIGRA_SHEETS_DATABASE_MANAGER
#define CALLIGRA_SHEETS_DATABASE_MANAGER

#include <QObject>

#include <KoXmlReader.h>

#include "sheets_odf_export.h"

class KoXmlWriter;

namespace Calligra
{
namespace Sheets
{
class Map;

class CALLIGRA_SHEETS_ODF_EXPORT DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    explicit DatabaseManager(const Map *map);

    /**
     * Destructor.
     */
    ~DatabaseManager() override;

    /**
     * Creates a unique database name.
     */
    QString createUniqueName() const;

    /**
     * Loads databases.
     * \ingroup OpenDocument
     */
    bool loadOdf(const KoXmlElement& element);

    /**
     * Saves databases.
     * \ingroup OpenDocument
     */
    void saveOdf(KoXmlWriter& xmlWriter) const;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DATABASE_MANAGER
