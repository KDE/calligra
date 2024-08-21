/*
    This file is part of the Calligra project, made within the KDE community.

    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef XFIGPARSER_H
#define XFIGPARSER_H

// filter
#include "XFigStreamLineReader.h"
#include <QStringDecoder>

class XFigDocument;
class XFigAbstractObject;
class XFigArrowHead;
class QIODevice;
struct XFigPoint;

class XFigParser
{
public:
    static XFigDocument *parse(QIODevice *device);

private:
    explicit XFigParser(QIODevice *device);
    ~XFigParser();

    XFigDocument *takeDocument()
    {
        XFigDocument *result = m_Document;
        m_Document = nullptr;
        return result;
    }

    bool parseHeader();

    void parseColorObject();

    XFigAbstractObject *parseArc();
    XFigAbstractObject *parseEllipse();
    XFigAbstractObject *parsePolyline();
    XFigAbstractObject *parseSpline();
    XFigAbstractObject *parseText();
    XFigAbstractObject *parseCompoundObject();

    XFigArrowHead *parseArrowHead();
    QVector<XFigPoint> parsePoints(int pointCount);
    QVector<double> parseFactors(int pointCount);

private:
    XFigDocument *m_Document;

    int m_XFigVersion;

    QStringDecoder m_TextDecoder;
    XFigStreamLineReader m_XFigStreamLineReader;
};

#endif
