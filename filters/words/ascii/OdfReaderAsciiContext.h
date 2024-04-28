/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODFREADERASCIICONTEXT_H
#define ODFREADERASCIICONTEXT_H

// Qt
#include <QHash>
#include <QTextStream>

// Calligra
#include <KoFilter.h>

// libodfreader
#include "OdfReaderContext.h"

class QFile;
class KoStore;

class OdfReaderAsciiContext : public OdfReaderContext
{
public:
    OdfReaderAsciiContext(KoStore *store, QFile &file);
    ~OdfReaderAsciiContext() override;

    QTextStream outStream;
};

#endif // ODFREADERASCIICONTEXT_H
