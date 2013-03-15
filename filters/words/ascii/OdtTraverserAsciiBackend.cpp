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


// Own
#include "OdtTraverserAsciiBackend.h"

// Calligra
#include <KoXmlReader.h>


// ----------------------------------------------------------------
//                 class OdtTraverserAsciiContext


OdtTraverserAsciiContext::OdtTraverserAsciiContext(KoStore *store, QFile &file)
    : OdfTraverserContext(store)
    , outStream(&file)
{
}

OdtTraverserAsciiContext::~OdtTraverserAsciiContext()
{
}


// ----------------------------------------------------------------
//                 class OdtTraverserAsciiBackend



OdtTraverserAsciiBackend::OdtTraverserAsciiBackend(OdfTraverserContext *context)
    : OdtTraverserBackend(context)
{
}

OdtTraverserAsciiBackend::~OdtTraverserAsciiBackend()
{
}

void OdtTraverserAsciiBackend::characterData(KoXmlNode &node, OdfTraverserContext *context,
                                             BeginEndTag beginEnd)
{
    if (beginEnd != EndTag)
        return;

    OdtTraverserAsciiContext *asciiContext = dynamic_cast<OdtTraverserAsciiContext*>(context);
    if (!asciiContext) {
        return;
    }

    asciiContext->outStream << node.toText().data();
}


void OdtTraverserAsciiBackend::tagP(KoXmlElement &element, OdfTraverserContext *context,
                                    BeginEndTag beginEnd)
{
    Q_UNUSED(element);

    if (beginEnd != EndTag)
        return;

    OdtTraverserAsciiContext *asciiContext = dynamic_cast<OdtTraverserAsciiContext*>(context);
    if (!asciiContext) {
        return;
    }

    // At the end of a paragraph, output two newlines.
    asciiContext->outStream << "\n\n";
}
