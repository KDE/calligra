/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

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

namespace KoChart {

#define CHART_MIME_TYPE "application/vnd.oasis.opendocument.chart"

class ChartShape;

class ChartDocument : public KoDocument
{
public:
    explicit ChartDocument(ChartShape *parent);
    ~ChartDocument();

    /// reimplemented from KoDocument
    QByteArray nativeFormatMimeType() const override { return CHART_MIME_TYPE; }
    /// reimplemented from KoDocument
    QByteArray nativeOasisMimeType() const override { return CHART_MIME_TYPE; }
    /// reimplemented from KoDocument
    QStringList extraNativeMimeTypes() const override { return QStringList(); }


    bool loadOdf(KoOdfReadStore &odfStore) override;
    bool loadXML(const KoXmlDocument &doc, KoStore *store) override;
    
    bool saveOdf(SavingContext &context) override;
    
    void paintContent(QPainter &painter, const QRect &rect) override;
    
private:
    class Private;
    Private * const d;
};

} // namespace KoChart

#endif // KCHART_CHARTDOCUMENT_H

