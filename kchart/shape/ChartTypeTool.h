/* This file is part of the KDE project
 * Copyright (C) 2007      Inge Wallin  <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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


#ifndef CHARTTYPETOOL_H
#define CHARTTYPETOOL_H


#include <KoTool.h>

#include "ChartShape.h"


class QAction;


namespace KChart
{


/**
 * This is the tool for the text-shape (which is a flake-based plugin).
 */
class ChartTypeTool : public KoTool
{
    Q_OBJECT
public:
    explicit ChartTypeTool(KoCanvasBase *canvas);
    ~ChartTypeTool();

    /// reimplemented from superclass
    virtual void paint( QPainter &painter, const KoViewConverter &converter );

    /// reimplemented from superclass
    virtual void mousePressEvent( KoPointerEvent *event ) ;
    /// reimplemented from superclass
    virtual void mouseMoveEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void mouseReleaseEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void activate (bool temporary=false);
    /// reimplemented from superclass
    virtual void deactivate();
    /// reimplemented from superclass
    virtual QWidget *createOptionWidget();

private slots:
    void setChartType( KChart::OdfChartType type );
    void setChartSubtype( KChart::OdfChartSubtype subtype );
    void setThreeDMode( bool );
    void setDataDirection( Qt::Orientation );
    void setFirstRowIsLabel( bool b );
    void setFirstColumnIsLabel( bool b );

private:
    void updateActions();

    ChartShape  *m_currentShape;
};

} // namespace KChart


#endif
