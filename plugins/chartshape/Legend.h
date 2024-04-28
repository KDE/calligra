/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007-2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_LEGEND_H
#define KCHART_LEGEND_H

// Qt
#include <QObject>

// Calligra
#include <KoShape.h>

// KoChart
#include "ChartShape.h"

namespace KoChart
{

class Legend : public QObject, public KoShape
{
    Q_OBJECT

public:
    explicit Legend(ChartShape *parent);
    ~Legend();

    QString title() const;
    QFont font() const;
    qreal fontSize() const;
    QColor fontColor() const;
    QFont titleFont() const;
    qreal titleFontSize() const;
    LegendExpansion expansion() const;
    Qt::Alignment alignment() const;
    Position legendPosition() const;

    void setTitle(const QString &title);
    void setFont(const QFont &font);
    void setFontSize(qreal size);
    void setFontColor(const QColor &color);
    void setTitleFont(const QFont &font);
    void setTitleFontSize(qreal size);
    void setExpansion(LegendExpansion expansion);
    void setAlignment(Qt::Alignment alignment);
    void setLegendPosition(Position position);
    void setSize(const QSizeF &size) override;

    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    void paintPixmap(QPainter &painter, const KoViewConverter &converter);

    bool loadOdf(const KoXmlElement &legendElement, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    KChart::Legend *kdLegend() const;

    void rebuild();
    using KoShape::update;
    void update() const override;

Q_SIGNALS:
    void updateConfigWidget();

private Q_SLOTS:
    void slotChartTypeChanged(KoChart::ChartType chartType);
    void slotKdLegendChanged();

private:
    class Private;
    Private *const d;
};

}

#endif // KCHART_LEGEND_H
