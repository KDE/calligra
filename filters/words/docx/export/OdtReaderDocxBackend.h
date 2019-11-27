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
   Boston, MA 02110-1301, USA.
*/

#ifndef ODTREADERDOCXBACKEND_H
#define ODTREADERDOCXBACKEND_H

// Calligra
#include <KoXmlStreamReader.h>
#include <KoFilter.h>
#include <OdtReaderBackend.h>



class OdfReaderContext;

class OdtReaderDocxBackend : public OdtReaderBackend
{
 public:
    OdtReaderDocxBackend();
    ~OdtReaderDocxBackend() override;

    // Called before and after the actual traversal.
    // FIXME: NYI

    // Called when there is a document level element with children.

    // ----------------------------------------------------------------
    // ODT document level functions

    void elementOfficeBody(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    //void elementOfficeText(KoXmlStreamReader &reader, OdfReaderContext *context);
};


#endif // ODTREADERDOCXBACKEND_H
