/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/
#include "ppttoodp.h"
#include <KoOdf.h>
#include <QtCore/QCoreApplication>

int
main(int argc, char** argv)
{
    if (argc != 3) {
        return 1;
    }
    QCoreApplication app(argc, argv);

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    // open inputFile
    POLE::Storage storage(inputFile);
    if (!storage.open()) {
        qDebug() << "Cannot open " << inputFile;
        return KoFilter::StupidError;
    }
    // create output store
    KoStore* storeout = KoStore::createStore(outputFile, KoStore::Write,
                        KoOdf::mimeType(KoOdf::Presentation), KoStore::Zip);
    if (!storeout) {
        return KoFilter::FileNotFound;
    }

    PptToOdp ppttoodp;
    int r = ppttoodp.convert(storage, storeout);
    delete storeout;

    return r;
}
