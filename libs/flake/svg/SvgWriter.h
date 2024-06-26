/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Lars Siebold <khandha5@gmx.net>
   SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2005, 2008 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SVGWRITER_H
#define SVGWRITER_H

#include "flake_export.h"
#include <QList>
#include <QSizeF>

class SvgSavingContext;
class KoShapeLayer;
class KoShapeGroup;
class KoShape;
class KoPathShape;
class QIODevice;
class QString;

/// Implements exporting shapes to SVG
class FLAKE_EXPORT SvgWriter
{
public:
    /// Creates svg writer to export specified layers
    SvgWriter(const QList<KoShapeLayer *> &layers, const QSizeF &pageSize);

    /// Creates svg writer to export specified shapes
    SvgWriter(const QList<KoShape *> &toplevelShapes, const QSizeF &pageSize);

    /// Destroys the svg writer
    virtual ~SvgWriter();

    /// Writes svg to specified output device
    bool save(QIODevice &outputDevice);

    /// Writes svg to the specified file
    bool save(const QString &filename, bool writeInlineImages);

private:
    void saveLayer(KoShapeLayer *layer, SvgSavingContext &context);
    void saveGroup(KoShapeGroup *group, SvgSavingContext &context);
    void saveShape(KoShape *shape, SvgSavingContext &context);
    void savePath(KoPathShape *path, SvgSavingContext &context);
    void saveGeneric(KoShape *shape, SvgSavingContext &context);

    QList<KoShape *> m_toplevelShapes;
    QSizeF m_pageSize;
    bool m_writeInlineImages;
};

#endif // SVGWRITER_H
