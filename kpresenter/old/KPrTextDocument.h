// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef kprtextdocument_h
#define kprtextdocument_h

#include <KoRichText.h>
#include "KoTextDocument.h"

class KPrTextObject;
class KoTextFormatCollection;

/**
 * This is our KoTextDocument reimplementation, to relate it
 * to the textobject it's in.
 */
class KPrTextDocument : public KoTextDocument
{
    Q_OBJECT
public:
    /** A text document inside a kptextobject */
    KPrTextDocument( KPrTextObject * textobj, KoTextFormatCollection *fc, KoTextFormatter *formatter = 0L );

    ~KPrTextDocument();

    /** Return the text object in which this document is.
     * Note that this can be 0L (e.g. for paragraphs in the paragdia preview) */
    KPrTextObject * textObject() const { return m_textobj; }
    /// Extensions to KoTextParag::loadOasisSpan
    bool loadSpanTag( const QDomElement& tag, KoOasisContext& context,
                      KoTextParag* parag, uint pos,
                      QString& textData, KoTextCustomItem* & customItem );

private:
    KPrTextObject * m_textobj;
};

#endif
