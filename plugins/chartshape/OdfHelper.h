/* This file is part of the KDE project

   Copyright 2018 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KOCHART_ODFHELPER_H
#define KOCHART_ODFHELPER_H


// Qt
#include <QSharedPointer>

// Calligra
#include <KoShapeContainer.h>
#include <KoFrameShape.h>

// KoChart
#include "kochart_global.h"
#include "KoChartInterface.h"


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

namespace KoChart {
namespace OdfHelper {

void saveOdfFont(KoGenStyle &style, const QFont& font, const QColor& color);
QString saveOdfFont(KoGenStyles& mainStyles, const QFont& font, const QColor& color);

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
