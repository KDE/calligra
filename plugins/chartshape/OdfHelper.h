/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCHART_ODFHELPER_H
#define KOCHART_ODFHELPER_H

// Qt
#include <QSharedPointer>

// Calligra
#include <KoFrameShape.h>
#include <KoShapeContainer.h>

// KoChart
#include "KoChartInterface.h"
#include "kochart_global.h"

class QAbstractItemModel;

class QPointF;
class QSizeF;
class QPen;
class QBrush;
class QColor;
class QString;
class QFont;

class KoCanvasBase;
class KoDocumentResourceManager;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoStore;
#include "KoXmlReaderForward.h"
class KoXmlWriter;
class KoGenStyles;
class KoOdfLoadingContext;

namespace KoChart
{
namespace OdfHelper
{

void saveOdfFont(KoGenStyle &style, const QFont &font, const QColor &color);
QString saveOdfFont(KoGenStyles &mainStyles, const QFont &font, const QColor &color);

void saveOdfTitleStyle(KoShape *title, KoGenStyle &style, KoShapeSavingContext &context);

void saveOdfTitle(KoShape *title, KoXmlWriter &bodyWriter, const char *titleType, KoShapeSavingContext &context);

QString getStyleProperty(const char *property, KoShapeLoadingContext &context);
QSharedPointer<KoShapeBackground> loadOdfFill(KoShape *title, KoShapeLoadingContext &context);
KoShapeStrokeModel *loadOdfStroke(KoShape *title, const KoXmlElement &element, KoShapeLoadingContext &context);
KoShapeShadow *loadOdfShadow(KoShape *title, KoShapeLoadingContext &context);
KoBorder *loadOdfBorder(KoShape *title, KoShapeLoadingContext &context);

bool loadOdfTitle(KoShape *title, KoXmlElement &titleElement, KoShapeLoadingContext &context);

}

} // Namespace KoChart

#endif
