/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION
*/

#ifndef PPTXML_H
#define PPTXML_H

#include <pptdoc.h>
#include <qobject.h>
#include <qstring.h>

#define ALIGN_LEFT 		1
#define ALIGN_RIGHT 	2
#define ALIGN_CENTER 	4
#define ALIGN_JUSTIFY 	8

class myFile;
class QDomDocument;

class PptXml:
    public QObject,
    private PptDoc
{

    Q_OBJECT

public:
    PptXml(
        const myFile &mainStream,
        const myFile &currentUser,
        const myFile &pictures);
    ~PptXml();

    bool convert();

    const QString getXml() const;

signals:
    // See olefilter.h for information
    void signalSavePic(
        const QString &nameIN,
        QString &storageId,
        const QString &extension,
        unsigned int length,
        const char *data);

    void signalSavePart(
        const QString &nameIN,
        QString &storageId,
        QString &mimeType,
        const QString &extension,
        unsigned int length,
        const char *data);

    void signalPart(
        const QString& nameIN,
        QString &storageId,
        QString &mimeType);

private:
    PptXml(const PptXml &);
    const PptXml &operator=(const PptXml &);

    // Error handling and reporting support.

    static const int s_area;

    // The conversion is done exactly once. Has it already happened?

    bool m_isConverted;
    bool m_success;
    bool m_half;
    unsigned m_y;
    QString m_embedded;
    QString m_pages;
    QString m_titles;
    QString m_notes;
    QString m_text;
    void encode(QString &text);

    // Override the base class functions.

    void gotDrawing(
        unsigned id,
        QString type,
        unsigned length,
        const char *data);
    void gotSlide(PptSlide &slide);
    void setPlaceholder(PptSlide &slide);
};
#endif
