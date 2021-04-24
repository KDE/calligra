/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_ODF_LOADING_CONTEXT
#define KSPREAD_ODF_LOADING_CONTEXT

#include <KoOdfLoadingContext.h>

#include <QHash>

#include "Region.h"

class KoShapeLoadingContext;
class KoShape;

namespace Calligra
{
namespace Sheets
{
namespace Odf
{

/**
 * \ingroup OpenDocument
 * Data used while loading.
 */
class OdfLoadingContext
{
public:
    explicit OdfLoadingContext(KoOdfLoadingContext &odfContext)
            : odfContext(odfContext), shapeContext(0) {}

public:
    KoOdfLoadingContext& odfContext;
    KoShapeLoadingContext* shapeContext;
    QHash<QString, KoXmlElement> validities;
};

struct ShapeLoadingData {
    KoShape* shape;
    QPoint startCell;
    QPointF offset;
    Region endCell;
    QPointF endPoint;
};

} // namespace Odf
} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_ODF_LOADING_CONTEXT
