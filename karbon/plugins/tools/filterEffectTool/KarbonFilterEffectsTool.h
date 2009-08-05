/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBONFILTEREFFECTSTOOL_H
#define KARBONFILTEREFFECTSTOOL_H

#include "KoTool.h"

class KarbonFilterEffectsTool : public KoTool
{
    Q_OBJECT
public:
    explicit KarbonFilterEffectsTool(KoCanvasBase *canvas);
    virtual ~KarbonFilterEffectsTool();
    
    /// reimplemented from KoTool
    virtual void paint(QPainter &painter, const KoViewConverter &converter);
    /// reimplemented from KoTool
    virtual void mousePressEvent(KoPointerEvent *event);
    /// reimplemented from KoTool
    virtual void mouseMoveEvent(KoPointerEvent *event);
    /// reimplemented from KoTool
    virtual void mouseReleaseEvent(KoPointerEvent *event);

    /// reimplemented from KoTool
    virtual void activate (bool temporary=false);
    
protected:
    /// reimplemented from KoTool
    virtual QMap<QString, QWidget *> createOptionWidgets();

private slots:
    void addFilter();
    void removeFilter();
    void raiseFilter();
    void lowerFilter();
    void editFilter();
    void filterChanged();
    void filterSelected(int index);
    void selectionChanged();

private:
    class Private;
    Private * const d;
};

#endif // KARBONFILTEREFFECTSTOOL_H
