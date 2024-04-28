/* This file is part of the Calligra project

  SPDX-FileCopyrightText: 2011 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef SVMSTRUCT_H
#define SVMSTRUCT_H

#include <QPoint>
#include <QtGlobal>

class QDataStream;

/**
 * @file
 *
 * Structs used in various parts of SVM files.
 */

/**
   Namespace for StarView Metafile (SVM) classes
*/
namespace Libsvm
{

/**
 * Contains version and length of an action.
 */
struct VersionCompat {
    VersionCompat();
    VersionCompat(QDataStream &stream);

    quint16 version;
    quint32 length;
};

QDataStream &operator>>(QDataStream &stream, VersionCompat &compat);

struct Fraction {
    Fraction();
    Fraction(QDataStream &stream);

    quint32 numerator;
    quint32 denominator;
};

QDataStream &operator>>(QDataStream &stream, Fraction &fract);

struct MapMode {
    MapMode();
    MapMode(QDataStream &stream);

    VersionCompat version;
    quint16 unit;
    QPoint origin;
    Fraction scaleX;
    Fraction scaleY;
    bool isSimple;
};

QDataStream &operator>>(QDataStream &stream, MapMode &mm);

/**
 * The header of an SVM file.
 */
struct SvmHeader {
    SvmHeader();
    SvmHeader(QDataStream &stream);

    VersionCompat versionCompat;
    quint32 compressionMode;
    MapMode mapMode;
    quint32 width;
    quint32 height;
    quint32 actionCount;
};

QDataStream &operator>>(QDataStream &stream, SvmHeader &header);
}

#endif
