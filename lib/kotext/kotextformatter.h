/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kotextformatter_h
#define kotextformatter_h

#include "qrichtext_p.h"
class KoZoomHandler;

/**
 * We implement our own text formatter to implement WYSIWYG:
 * It is heavily based on KoTextFormatterBaseBreakWords, but stores the x position
 * of characters (and their width) in pixels, whereas all the rest is in L.U.
 * Having our own text formatter will also enable implementing word-breaking later.
 */
class KoTextFormatter : public KoTextFormatterBase
{
public:
    KoTextFormatter() {}
    virtual ~KoTextFormatter() {}

    virtual int format( KoTextDocument *doc, KoTextParag *parag, int start, const QMap<int, KoTextParagLineStart*> &oldLineStarts );

protected:
    KoTextParagLineStart *koFormatLine(
        KoZoomHandler *zh,
        KoTextParag * /*parag*/, KoTextString *string, KoTextParagLineStart *line,
        KoTextStringChar *startChar, KoTextStringChar *lastChar, int align, int space );

    KoTextParagLineStart *koBidiReorderLine(
        KoZoomHandler *zh,
        KoTextParag * /*parag*/, KoTextString *text, KoTextParagLineStart *line,
        KoTextStringChar *startChar, KoTextStringChar *lastChar, int align, int space );

    void moveChar( KoTextStringChar& chr, KoZoomHandler *zh,
                   int deltaX, int deltaPixelX );
};

#endif
