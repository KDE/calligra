/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2021 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHEETS_KSP
#define SHEETS_KSP

#include "sheets_core_export.h"

static const int CURRENT_SYNTAX_VERSION = 1;

class KoXmlDocument;
class KoXmlElement;
class QDomDocument;
class QDomElement;

namespace Calligra {
namespace Sheets {

class DocBase;
class Style;
class Sheet;
class Cell;

namespace Ksp {
    CALLIGRA_SHEETS_CORE_EXPORT bool loadDoc(DocBase *obj, const KoXmlDocument& doc);
    CALLIGRA_SHEETS_CORE_EXPORT QDomDocument saveDoc(DocBase *document);

    CALLIGRA_SHEETS_CORE_EXPORT bool loadStyle(Style *style, KoXmlElement& format);
}

}  // namespace Sheets
}  // namespace Calligra

#endif  // SHEETS_KSP
