/*
 * Copyright (c) 2002-2003 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DATA_H
#define DATA_H

#include <math.h>

#include <qdom.h>
#include <q3valuevector.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <KoPageLayout.h>

#include "misc.h"

class KoFilterChain;


namespace PDFImport
{
class Options;

class Data
{
public:
    Data(KoFilterChain *, const DRect &pageRect, KoPageLayout,
         const Options &);

    QDomElement createElement(const QString &name)
        { return _document.createElement(name); }
    void createParagraph(const QString &text, ParagraphType type,
                         const Q3ValueVector<QDomElement> &layouts,
                         const Q3ValueVector<QDomElement> &formats);

    KoFilterChain *chain() const { return _chain; }
    const Options &options() const { return _options; }
    QDomDocument document() const { return _document; }
    const DRect &pageRect() const { return _pageRect; }
    uint imageIndex() const { return _imageIndex; }
    uint textIndex() const { return _textIndex; }
    QDomElement bookmarks() const { return _bookmarks; }
    QDomElement pictures() const { return _pictures; }

    QDomElement pictureFrameset(const DRect &);

    void initPage(const Q3ValueVector<DRect> &,
                  const Q3ValueList<QDomElement> &pictures);
    void endDump();

public:
    uint pageIndex;

private:
    enum FramesetType { Text, Picture };
    QDomElement createFrameset(FramesetType, const QString &name);
    QDomElement createFrame(FramesetType, const DRect &,
                            bool forceMainFrameset);

private:
    KoFilterChain *_chain;
    QDomDocument   _document;
    uint           _imageIndex, _textIndex;
    QDomElement    _mainElement, _framesets, _pictures, _bookmarks, _paper;
    Q3ValueVector<QDomElement> _textFramesets;
    DRect          _pageRect, _marginRect;
    const Options &_options;
};

} // namespace

#endif
