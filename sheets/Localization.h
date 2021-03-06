/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LOCALIZATION
#define CALLIGRA_SHEETS_LOCALIZATION

#include <klocale.h>
#include <KoXmlReader.h>

#include "sheets_odf_export.h"

class QDomElement;
class QDomDocument;

namespace Calligra
{
namespace Sheets
{

// This should be ported to QLocale, however, there is no substitute for currency formatting with precision support.
class CALLIGRA_SHEETS_ODF_EXPORT Localization : public KLocale
{
public:
    Localization();

    void load(const KoXmlElement& element);
    QDomElement save(QDomDocument& doc) const;
    void defaultSystemConfig();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LOCALIZATION
