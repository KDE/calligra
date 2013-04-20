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

#ifndef ODTTRAVERSERDOCXBACKEND_H
#define ODTTRAVERSERDOCXBACKEND_H

// Calligra
#include <KoXmlReader.h>
#include <OdtTraverser.h>
#include <OdtTraverserBackend.h>
#include <OdfTraverserContext.h>

// This filter
#include <OdtTraverserDocxContext.h>

class QByteArray;
class QSizeF;
class QStringList;
class QFile;
class QTextStream;

class KoStore;
class DocxFile;


class OdtTraverserDocxBackend : public OdtTraverserBackend
{
 public:
    OdtTraverserDocxBackend(OdfTraverserContext *context);
    virtual ~OdtTraverserDocxBackend();

    // Backend functions
    void beginEndTraversal(OdfTraverserContext *context, BeginEndTag beginEnd);

    void tagP(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    void tagH(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);
    void tagSpan(KoXmlElement &element, OdfTraverserContext *context, BeginEndTag beginEnd);

    void characterData(KoXmlNode &node, OdfTraverserContext *context, BeginEndTag beginEnd);

 private:
    void startRun(KoXmlNode &node, KoXmlWriter *writer, OdfTraverserContext *context);
    void endRun(KoXmlNode &node, KoXmlWriter *writer, OdfTraverserContext *context);

 private:
    bool  isInsideSpan;
};

#endif // ODTTRAVERSERDOCXBACKEND_H
