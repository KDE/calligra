/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
   Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#ifndef ODFREADERWIKICONTEXT_H
#define ODFREADERWIKICONTEXT_H

// Qt
#include <QHash>
#include <QTextStream>
#include <QStack>

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>

// libodfreader
#include "OdfReaderContext.h"


class QFile;
class KoStore;
class KoOdfStyle;
class KoOdfListStyle;


class OdfReaderWikiContext : public OdfReaderContext
{
 public:
    OdfReaderWikiContext(KoStore *store, QFile &file);
    ~OdfReaderWikiContext() override;

    void pushStyle(KoOdfStyle*);
    KoOdfStyle *popStyle();
    KoOdfStyle *styleTop();

    void pushListStyle(KoOdfListStyle*);
    KoOdfListStyle *popListStyle();
    KoOdfListStyle *listStyleTop();

 private:
    friend class OdtReaderWikiBackend;

    QTextStream outStream;

    QStack<KoOdfStyle*> styleStack;
    QStack<KoOdfListStyle*> listStyleStack;

    int listLevelCounter;       // FIXME: rename to currentListLevel
    int outlineLevel;           // FIXME: rename to currentOutlineLevel
};


#endif // ODFREADERWIKICONTEXT_H
