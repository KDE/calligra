/* This file is part of the KDE project
 *
 * Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
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

#ifndef KCHART_CHART_DATASET_TOOL
#define KCHART_CHART_DATASET_TOOL

// KOffice
#include <KoTool.h>

// KChart
#include "ChartShape.h"

#include <QMouseEvent>

class QAction;

namespace KChart
{

class ChartDatasetTool : public KoTool
{
    Q_OBJECT
public:
    explicit ChartDatasetTool( KoCanvasBase *canvas );
    ~ChartDatasetTool();

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
    
    void updateWidget();
    
public slots:
    void setDatasetColor( int dataset, const QColor& color );
    
private:
    void updateActions();
    
    class Private;
    Private * const d;
};

} // namespace KChart


#endif // KCHART_CHART_DATASET_TOOL
