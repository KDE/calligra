/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2003 thierry lorthiois <lorthioist@wanadoo.fr>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#ifndef WMFEXPORT_H
#define WMFEXPORT_H

#include <KoFilter.h>
#include <QPen>
#include <QVariantList>

#include <WmfWriter.h>

class KarbonDocument;
class KoShape;
class KoShapeStrokeModel;

class WmfExport : public KoFilter
{
    Q_OBJECT

public:
    WmfExport(QObject *parent, const QVariantList &);
    ~WmfExport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    void paintDocument(KarbonDocument *document);
    void paintShape(KoShape *shape);

    QPen getPen(const KoShapeStrokeModel *stroke);

    // coordinate transformation
    // scale to wmf size
    int coordX(double left);
    int coordY(double top);

private:
    Libwmf::WmfWriter *mWmf;
    int mDpi;
    double mScaleX;
    double mScaleY;
};

#endif
