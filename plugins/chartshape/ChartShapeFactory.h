/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_CHART_SHAPE_FACTORY
#define KCHART_CHART_SHAPE_FACTORY

// Qt
#include <QStringList>

// Calligra
#include <KoShapeFactoryBase.h>
#include <QVariantList>

class KoShape;
class KoShapeConfigWidgetBase;

namespace KoChart
{
class ChartShape;
}

class ChartShapePlugin : public QObject
{
    Q_OBJECT
public:
    ChartShapePlugin(QObject *parent, const QVariantList &);
    ~ChartShapePlugin() = default;
};

class ChartShapeFactory : public KoShapeFactoryBase
{
public:
    ChartShapeFactory();
    ~ChartShapeFactory() = default;

    bool supports(const KoXmlElement &element, KoShapeLoadingContext &context) const override;

    KoShape *createShape(const KoProperties *properties, KoDocumentResourceManager *documentResources) const override;

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = nullptr) const override;
    // reimplemented to not create a default shape to just overwrite it afterwards
    KoShape *createShapeFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void newDocumentResourceManager(KoDocumentResourceManager *manager) const override;

    QList<KoShapeConfigWidgetBase *> createShapeOptionPanels() override;

private:
    KoChart::ChartShape *createBarChart(KoDocumentResourceManager *documentResources, int subtype) const;
    KoChart::ChartShape *createLineChart(KoDocumentResourceManager *documentResources, int subtype) const;
    KoChart::ChartShape *createAreaChart(KoDocumentResourceManager *documentResources, int subtype) const;
    KoChart::ChartShape *createStockChart(KoDocumentResourceManager *documentResources, int subtype) const;
    KoChart::ChartShape *createPieChart(KoDocumentResourceManager *documentResources) const;
    KoChart::ChartShape *createRingChart(KoDocumentResourceManager *documentResources) const;
    KoChart::ChartShape *createBubbleChart(KoDocumentResourceManager *documentResources) const;
    KoChart::ChartShape *createScatterChart(KoDocumentResourceManager *documentResources) const;
    KoChart::ChartShape *createRadarChart(KoDocumentResourceManager *documentResources) const;
    KoChart::ChartShape *createFilledRadarChart(KoDocumentResourceManager *documentResources) const;

    void radarData(KoChart::ChartShape *shape) const;
};

#endif // KCHART_CHART_SHAPE_FACTORY
