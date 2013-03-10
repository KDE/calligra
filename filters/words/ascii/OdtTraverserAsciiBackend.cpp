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
    : OdtTraverserContext(store)
    , outStream(&file)
{
}

OdtTraverserAsciiContext::~OdtTraverserAsciiContext()
{
}


// ----------------------------------------------------------------
//                 class OdtTraverserAsciiBackend



OdtTraverserAsciiBackend::OdtTraverserAsciiBackend(OdtTraverserContext *context)
    : OdtTraverserBackend(context)
{
    Q_UNUSED(context)
}

OdtTraverserAsciiBackend::~OdtTraverserAsciiBackend()
{
}


void OdtTraverserAsciiBackend::beginTraversal(OdtTraverserContext *context)
{
    Q_UNUSED(context);
}

void OdtTraverserAsciiBackend::endTraversal(OdtTraverserContext *context)
{
    Q_UNUSED(context);
}


void OdtTraverserAsciiBackend::beginCharacterData(KoXmlNode &node,
                                                  OdtTraverserContext *context)
{
    OdtTraverserAsciiContext *newContext = dynamic_cast<OdtTraverserAsciiContext*>(context);
    if (!newContext) {
        return;
    }

    newContext->outStream << node.toText().data();
}


void OdtTraverserAsciiBackend::endTagP(KoXmlElement &element, OdtTraverserContext *context)
{
    Q_UNUSED(element);

    OdtTraverserAsciiContext *newContext = dynamic_cast<OdtTraverserAsciiContext*>(context);
    if (!newContext) {
        return;
    }

    newContext->outStream << "\n\n";
}

