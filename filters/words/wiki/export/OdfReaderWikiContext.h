/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFREADERWIKICONTEXT_H
#define ODFREADERWIKICONTEXT_H

// Qt
#include <QHash>
#include <QStack>
#include <QTextStream>

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

    void pushStyle(KoOdfStyle *);
    KoOdfStyle *popStyle();
    KoOdfStyle *styleTop();

    void pushListStyle(KoOdfListStyle *);
    KoOdfListStyle *popListStyle();
    KoOdfListStyle *listStyleTop();

private:
    friend class OdtReaderWikiBackend;

    QTextStream outStream;

    QStack<KoOdfStyle *> styleStack;
    QStack<KoOdfListStyle *> listStyleStack;

    int listLevelCounter; // FIXME: rename to currentListLevel
    int outlineLevel; // FIXME: rename to currentOutlineLevel
};

#endif // ODFREADERWIKICONTEXT_H
