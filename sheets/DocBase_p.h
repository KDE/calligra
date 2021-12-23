/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_DOCBASE_P_H
#define KSPREAD_DOCBASE_P_H

#include "DocBase.h"

static const int CURRENT_SYNTAX_VERSION = 1;

namespace Calligra {
namespace Sheets {
class Map;
class SheetAccessModel;

class Q_DECL_HIDDEN DocBase::Private
{
public:
    Map *map;
    static QList<DocBase*> s_docs;
    static int s_docId;

    // document properties
    bool configLoadFromFile       : 1;
    QStringList spellListIgnoreAll;
    SheetAccessModel *sheetAccessModel;
    KoDocumentResourceManager *resourceManager;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_DOCBASE_P_H
