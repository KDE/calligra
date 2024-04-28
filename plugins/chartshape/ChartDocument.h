/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_CHARTDOCUMENT_H
#define KCHART_CHARTDOCUMENT_H

// Calligra
#include <KoDocument.h>
#include <KoXmlReaderForward.h>

class QIODevice;
class KoView;
class KoOdfReadStore;
class QPainter;
class QWidget;
class KoShapeLoadingContext;

namespace KoChart
{

#define CHART_MIME_TYPE "application/vnd.oasis.opendocument.chart"

class ChartShape;

class ChartDocument : public KoDocument
{
public:
    explicit ChartDocument(ChartShape *parent);
    ~ChartDocument();

    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override
    {
        return CHART_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override
    {
        return CHART_MIME_TYPE;
    }
    /// reimplemented from KoDocument
    QStringList extraNativeMimeTypes() const override
    {
        return QStringList();
    }

    bool loadOdf(KoOdfReadStore &odfStore) override;
    bool loadXML(const KoXmlDocument &doc, KoStore *store) override;

    bool saveOdf(SavingContext &context) override;

    void paintContent(QPainter &painter, const QRect &rect) override;

private:
    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KCHART_CHARTDOCUMENT_H
