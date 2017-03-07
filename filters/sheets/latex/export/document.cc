/* TODO : Manage File problems !
 */
/*
** A program to convert the XML rendered by Words into LATEX.
**
** Copyright (C) 2000, 2003 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include "document.h"

#include "LatexDebug.h"

Document::Document(const KoStore* in, QString fileOut):
        XmlParser(in), _file(fileOut), _in(in)
{
    //debugLatex << fileIn;
    debugLatex << fileOut;
    _filename = fileOut;
    //setFileHeader(_fileHeader);
    //setRoot(&_document);
    Config::instance()->setEmbeded(false);
    //analyze_config(config);
}

Document::~Document()
{

}

void Document::analyze()
{
    QDomNode node;
    node = init();
    debugLatex << "ANALYZE A DOC";
    _document.analyze(node);
    debugLatex << "END ANALYZE";
}

void Document::generate()
{
    if (_file.open(QIODevice::WriteOnly)) {
        debugLatex << "GENERATION";
        _out.setDevice(&_file);
        _document.generate(_out, !isEmbeded());
        //_out << getDocument();
        _file.close();
    } else
        debugLatex << "Can't use the file ...";
}
