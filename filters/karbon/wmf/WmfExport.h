/* This file is part of the KDE project
 * Copyright (c) 2003 thierry lorthiois <lorthioist@wanadoo.fr>
 * Copyright (c) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef WMFEXPORT_H
#define WMFEXPORT_H

#include <QPen>
#include <KoFilter.h>
#include <QVariantList>

#include <WmfWriter.h>

class KarbonDocument;
class KoShape;
class KoShapeStrokeModel;

class WmfExport : public KoFilter
{
    Q_OBJECT

public:
    WmfExport(QObject* parent, const QVariantList&);
    ~WmfExport() override;

    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;

private:
    void paintDocument(KarbonDocument* document);
    void paintShape(KoShape* shape);

    QPen getPen(const KoShapeStrokeModel *stroke);

    // coordinate transformation
    // scale to wmf size
    int coordX(double left);
    int coordY(double top);

private:
    Libwmf::WmfWriter *mWmf;
    int       mDpi;
    double    mScaleX;
    double    mScaleY;
};

#endif
