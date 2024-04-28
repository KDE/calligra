/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODTREADERDOCXBACKEND_H
#define ODTREADERDOCXBACKEND_H

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>
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
    // void elementOfficeText(KoXmlStreamReader &reader, OdfReaderContext *context);
};

#endif // ODTREADERDOCXBACKEND_H
