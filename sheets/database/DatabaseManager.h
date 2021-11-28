/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DATABASE_MANAGER
#define CALLIGRA_SHEETS_DATABASE_MANAGER

#include <QObject>

#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{

class CALLIGRA_SHEETS_ODF_EXPORT DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    explicit DatabaseManager();

    /**
     * Destructor.
     */
    ~DatabaseManager() override;

    /**
     * Creates a unique database name.
     */
    QString createUniqueName() const;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DATABASE_MANAGER
