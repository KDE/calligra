 /* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>

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

#ifndef ODTTRAVERSERASCIIBACKEND_H
#define ODTTRAVERSERASCIIBACKEND_H

// Calligra
#include <KoXmlReader.h>
#include <OdtTraverser.h>
#include <OdtTraverserBackend.h>

class QByteArray;
class QSizeF;
class QStringList;
class QFile;
class QDataStream;

class KoStore;


class OdtTraverserAsciiContext : public OdtTraverserContext
{
 public:
    OdtTraverserAsciiContext(KoStore *store, QFile &outfile);
    ~OdtTraverserAsciiContext();

    //QFile &outfile;
    QDataStream  outStream;
};


class OdtTraverserAsciiBackend : public OdtTraverserBackend
{
 public:
    OdtTraverserAsciiBackend(OdtTraverserContext *context);
    virtual ~OdtTraverserAsciiBackend();

    virtual void beginTraversal(OdtTraverserContext *context);
    virtual void endTraversal(OdtTraverserContext *context);

    // The only output function that we need.
    virtual void beginCharacterData(KoXmlNode &node,
                                    OdtTraverserContext *context);
    virtual void endCharacterData(KoXmlNode &node,
                                  OdtTraverserContext *context);

};

#endif // ODTTRAVERSERASCIIBACKEND_H
