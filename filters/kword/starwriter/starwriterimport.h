/*
   This file is part of the KDE project
   Copyright (C) 2002 Marco Zanon <info@marcozanon.com>
                  and Ariya Hidayat <ariya@kde.org>

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

#ifndef __STARWRITERIMPORT_H
#define __STARWRITERIMPORT_H

#include <KoFilter.h>
#include <QString>
#include <q3cstring.h>

class StarWriterImport: public KoFilter
{
    Q_OBJECT

public:
    StarWriterImport(QObject* parent, const QStringList&);
    virtual ~StarWriterImport();
    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

private:
    // most important OLE streams
    QByteArray SwPageStyleSheets;
    QByteArray StarWriterDocument;

    // supplementary variables
    quint8 tablesNumber;
    QString bodyStuff, tablesStuff, picturesStuff;

    // needed for ATTRIBUTES
    bool hasHeader;
    bool hasFooter;

    // Preliminary check
    bool checkDocumentVersion();

    // Formatting routines
    bool addKWordHeader();
    bool addPageProperties();
    bool addStyles();
    bool addHeaders();
    bool addFooters();
    bool addBody();
    QString convertToKWordString(QByteArray s);

    // Node routines
    bool parseNodes(QByteArray n);
    bool parseText(QByteArray n);
    bool parseTable(QByteArray n);
    bool parseGraphics(QByteArray n);

    // finished KWord document
    QString maindoc;
};

#endif
