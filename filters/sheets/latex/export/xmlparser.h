
/*
** Header file for inclusion with words_xml2latex.c
**
** SPDX-FileCopyrightText: 2000-2002 Robert JACOLIN
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

#ifndef __LATEX_XMLPARSER_H__
#define __LATEX_XMLPARSER_H__

#include <QDomDocument>
#include <QString>

class FileHeader;
class Document;
class KoStore;

class XmlParser
{
    /** Latex output file */
    QString _filename;
    /** The Calligra app document stored in a XML DOM Tree. */
    QDomDocument _document;
    /** The calligra document (maindoc, picture, ...). */
    static KoStore *_in;

protected:
    /* All the inherit class must be have a link with
     * the header to specify to use special package
     */
    static FileHeader *_fileHeader;
    static Document *_root;

public:
    explicit XmlParser(const QString &);
    explicit XmlParser(QByteArray); /* deprecated */
    explicit XmlParser(const KoStore *);
    XmlParser();
    virtual ~XmlParser();

    QString getFilename() const
    {
        return _filename;
    }
    QString getDocument() const
    {
        return _document.toString();
    }
    Document *getRoot() const
    {
        return _root;
    }
    FileHeader *getFileHeader() const
    {
        return _fileHeader;
    }
    QString getChildName(const QDomNode &, int);
    QDomNode getChild(const QDomNode &, QString);
    QDomNode getChild(const QDomNode &, QString, int);
    QDomNode getChild(const QDomNode &, int);
    QString getData(const QDomNode &, int);
    QString getData(const QDomNode &, QString);
    int getNbChild(const QDomNode &, QString);
    int getNbChild(const QDomNode &);
    QString getAttr(const QDomNode &, QString) const;
    bool isChild(const QDomNode &, QString);

    void setFileHeader(FileHeader *h)
    {
        _fileHeader = h;
    }
    void setRoot(Document *r)
    {
        _root = r;
    }

    QDomNode init()
    {
        return _document.documentElement();
    }
};

#endif /* __LATEX_XMLPARSER_H__ */
