/* This file is part of the Calligra project

  SPDX-FileCopyrightText: 2011 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef SVMPARSER_H
#define SVMPARSER_H

#include "SvmAbstractBackend.h"
#include "SvmGraphicsContext.h"
#include "kovectorimage_export.h"

class QByteArray;
class QDataStream;

namespace Libsvm
{

class KOVECTORIMAGE_EXPORT SvmParser
{
public:
    SvmParser();

    void setBackend(SvmAbstractBackend *backend);

    bool parse(const QByteArray &data);

private:
    void parseRect(QDataStream &stream, QRect &rect);
    void parsePolygon(QDataStream &stream, QPolygon &polygon);
    void parseString(QDataStream &stream, QString &string);
    void parseFont(QDataStream &stream, QFont &font);

    void dumpAction(QDataStream &stream, quint16 version, quint32 totalSize);

private:
    SvmGraphicsContext mContext;
    SvmAbstractBackend *mBackend;
};
}

#endif
