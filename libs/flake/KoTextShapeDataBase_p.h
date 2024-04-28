/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTSHAPEDATABASE_P_H
#define KOTEXTSHAPEDATABASE_P_H

#include "KoInsets.h"

class QTextDocument;

/// \internal
class FLAKE_EXPORT KoTextShapeDataBasePrivate
{
public:
    KoTextShapeDataBasePrivate();
    virtual ~KoTextShapeDataBasePrivate();

    QTextDocument *document;
    KoInsets margins;
    Qt::Alignment textAlignment;
    KoTextShapeDataBase::ResizeMethod resizeMethod;
};

#endif
