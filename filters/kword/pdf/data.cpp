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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "data.h"

#include <kglobal.h>
#include <kdebug.h>


using namespace PDFImport;


Data::Data(KoFilterChain *chain, const DRect &pageRect,
           KoPageLayout page, const Options &options)
    : pageIndex(0), _chain(chain), _imageIndex(1), _textIndex(1),
      _needNewTextFrameset(false), _pageRect(pageRect), _options(options)
{
    _marginRect.left = pageRect.right;
    _marginRect.right = pageRect.left;
    _marginRect.top = pageRect.bottom;
    _marginRect.bottom = pageRect.top;

    _document = QDomDocument("DOC");
    _document.appendChild(
        _document.createProcessingInstruction(
            "xml","version=\"1.0\" encoding=\"UTF-8\""));

    _mainElement = _document.createElement("DOC");
    _mainElement.setAttribute("editor", "KWord's PDF Import Filter");
    _mainElement.setAttribute("mime", "application/x-kword");
    _mainElement.setAttribute("syntaxVersion", 2);
    _document.appendChild(_mainElement);

    QDomElement element = _document.createElement("ATTRIBUTES");
    element.setAttribute("processing", 0);
    element.setAttribute("hasHeader", 0);
    element.setAttribute("hasFooter", 0);
    element.setAttribute("hasTOC", 0);
    element.setAttribute("unit", "mm");
    _mainElement.appendChild(element);

    _paper = _document.createElement("PAPER");
    _paper.setAttribute("format", page.format);
    _paper.setAttribute("width", pageRect.width());
    _paper.setAttribute("height", pageRect.height());
    _paper.setAttribute("orientation", page.orientation);
    _paper.setAttribute("columns", 1);
    _paper.setAttribute("hType", 0);
    _paper.setAttribute("fType", 0);
    _mainElement.appendChild(_paper);

    // framesets
    _framesets = _document.createElement("FRAMESETS");
    _mainElement.appendChild(_framesets);

    // main text frameset
    _mainTextFrameset = createFrameset(Text);

    // standard style
    QDomElement styles = _document.createElement("STYLES");
    _mainElement.appendChild(styles);

    QDomElement style = _document.createElement("STYLE");
    styles.appendChild(style);

    element = _document.createElement("FORMAT");
    Font font;
    font.format(_document, element, 0, 0, true);
    style.appendChild(element);

    element = _document.createElement("NAME");
    element.setAttribute("value","Standard");
    style.appendChild(element);

    element = _document.createElement("FOLLOWING");
    element.setAttribute("name","Standard");
    style.appendChild(element);

    // pictures
    _pictures = _document.createElement("PICTURES");
    _mainElement.appendChild(_pictures);

    // treat pages
    _bookmarks = _document.createElement("BOOKMARKS");
    _mainElement.appendChild(_bookmarks);
}

void Data::checkTextFrameset()
{
// #### only use main text frameset
//    if ( !_needNewTextFrameset ) return;
//    createFrameset(Text, _pageSize);
}

QDomElement Data::pictureFrameset(const DRect &r)
{
    QDomElement frameset = createFrameset(Picture);
    QDomElement frame = createFrame(Picture, r, false);
    frameset.appendChild(frame);
    return frameset;
}

QDomElement Data::createFrameset(FramesetType type)
{
    bool text = (type==Text);
    uint &index = (text ? _textIndex : _imageIndex);

    QDomElement frameset = _document.createElement("FRAMESET");
    frameset.setAttribute("frameType", (text ? 1 : 2));
    QString name = (text ? QString("Text Frameset %1")
                    : QString("Picture %1")).arg(index);
    frameset.setAttribute("name", name);
    frameset.setAttribute("frameInfo", 0);
    _framesets.appendChild(frameset);

    if (text) _textFrameset = frameset;
    _needNewTextFrameset = !text;
    kdDebug(30516) << "new frameset " << index << (text ? " text" : " image")
                   << endl;
    index++;
    return frameset;
}

QDomElement Data::createFrame(FramesetType type, const DRect &r,
                              bool forceMainFrameset)
{
    bool text = (type==Text);
    bool mainFrameset =
        (text ? (forceMainFrameset ? true : _textIndex==1) : false);

    QDomElement frame = _document.createElement("FRAME");
    if (text) frame.setAttribute("autoCreateNewFrame", (mainFrameset ? 1 : 0));
    frame.setAttribute("newFrameBehavior", (mainFrameset ? 0 : 1));
    frame.setAttribute("runaround", 0);
    frame.setAttribute("left", r.left);
    frame.setAttribute("right", r.right);
    double offset = pageIndex * _pageRect.height();
    frame.setAttribute("top", r.top + offset);
    frame.setAttribute("bottom", r.bottom + offset);
    if ( text && !mainFrameset ) frame.setAttribute("bkStyle", 0);
    return frame;
}

void Data::startPage()
{
    _textFrameset = _mainTextFrameset;
    _needNewTextFrameset = false;
    if ( !_lastMainLayout.isNull() ) {
        QDomElement element = _document.createElement("PAGEBREAKING");
        element.setAttribute("hardFrameBreakAfter", "true");
        _lastMainLayout.appendChild(element);
    }
    _lastMainLayout = QDomElement();
    QDomElement frame = createFrame(Text, _marginRect, true);
    _mainTextFrameset.appendChild(frame);

    // page bookmark
    QDomElement element = createElement("BOOKMARKITEM");
    element.setAttribute("name", Link::pageLinkName(pageIndex));
    element.setAttribute("cursorIndexStart", 0); // ?
    element.setAttribute("cursorIndexEnd", 0); // ?
    element.setAttribute("frameset", "Text Frameset 1");
    element.setAttribute("startparag", 0); // #### FIXME
    element.setAttribute("endparag", 0); // ?
    bookmarks().appendChild(element);
}

QDomElement Data::createParagraph(const QString &text,
                                      const QValueVector<QDomElement> &layouts,
                                      const QValueVector<QDomElement> &formats)
{
    QDomElement paragraph = _document.createElement("PARAGRAPH");
    _textFrameset.appendChild(paragraph);

    QDomElement textElement = _document.createElement("TEXT");
    textElement.appendChild( _document.createTextNode(text) );
    paragraph.appendChild(textElement);

    QDomElement layout = _document.createElement("LAYOUT");
    paragraph.appendChild(layout);
    QDomElement element = _document.createElement("NAME");
    element.setAttribute("value", "Standard");
    layout.appendChild(element);
    for (uint i=0; i<layouts.size(); i++)
        layout.appendChild(layouts[i]);
    if ( _textFrameset==_mainTextFrameset )
        _lastMainLayout = layout;

    if ( formats.size() ) {
        QDomElement format = _document.createElement("FORMATS");
        paragraph.appendChild(format);
        for (uint i=0; i<formats.size(); i++)
            format.appendChild(formats[i]);
    }
    return paragraph;
}

void Data::endPage(const DRect &r)
{
    kdDebug(30516) << "page #" << pageIndex
                   << " rect: " << r.toString() << endl;
    _marginRect = _marginRect.getUnion(r);
    kdDebug(30516) << "margin rect: " << _marginRect.toString() << endl;
}

void Data::endDump()
{
    if ( _marginRect.left>=_marginRect.right
         || _marginRect.top>=_marginRect.bottom ) _marginRect = _pageRect;
    QDomElement element = _document.createElement("PAPERBORDERS");
    element.setAttribute("left", _marginRect.left - _pageRect.left);
    element.setAttribute("top", _marginRect.top - _pageRect.top);
    element.setAttribute("right", _pageRect.right - _marginRect.right);
    element.setAttribute("bottom", _pageRect.bottom - _marginRect.bottom);
    _paper.appendChild(element);
}
