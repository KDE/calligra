/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <montel@kde.org>
             (C) 2000 Torben Weis <weis@kde.org>

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
