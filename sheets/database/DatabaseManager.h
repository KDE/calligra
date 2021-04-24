/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
