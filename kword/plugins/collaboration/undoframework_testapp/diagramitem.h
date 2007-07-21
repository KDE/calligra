/****************************************************************************
**
** Copyright (C) 2007-2007 Trolltech ASA. All rights reserved.
** Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
**
** This file was part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPolygonItem>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QPointF;
class QColor;

class DiagramItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 1 };
    enum DiagramType { Box, Triangle };

    DiagramItem(DiagramType diagramType, QGraphicsItem *item = 0,
            QGraphicsScene *scene = 0);

    DiagramType diagramType() const {
        return polygon() == boxPolygon ? Box : Triangle;
    }
    int type() const { return Type; }
    const QColor& color() const { return color_; }

private:
    QColor color_;
    QPolygonF boxPolygon;
    QPolygonF trianglePolygon;
};

#endif
