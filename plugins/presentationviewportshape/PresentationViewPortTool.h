/* This file is part of the KDE project
 * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
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
#ifndef PRESENTATIONVIEWPORT_TOOL
#define PRESENTATIONVIEWPORT_TOOL

#include <KoToolBase.h>
#include <KJob>
#include <KoInteractionTool.h>
#include <qvarlengtharray.h>
//#include "PresentationViewPortConfigWidget.h"

class PresentationViewPortShape;
class PresentationViewPortConfigWidget;

class PresentationViewPortTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit PresentationViewPortTool(KoCanvasBase* canvas);

    
    /// reimplemented from KoToolBase
    virtual void paint(QPainter&, const KoViewConverter&) {}
    /// reimplemented from KoToolBase
    virtual void mousePressEvent(KoPointerEvent*);
    /// reimplemented from superclass
  //  virtual void mouseDoubleClickEvent(KoPointerEvent *event);
    /// reimplemented from KoToolBase
    virtual void mouseMoveEvent(KoPointerEvent*) {}
    /// reimplemented from KoToolBase
    virtual void mouseReleaseEvent(KoPointerEvent*) {}

    /// reimplemented from KoToolBase
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    /// reimplemented from KoToolBase
    virtual void deactivate();

    PresentationViewPortShape* currentShape();
signals:
    void shapeSelected();
    void sequenceChanged(int);
    void zoomChanged(int);
    void durationChanged(int);
    void transitionProfileChanged(QString);
    void widgetDataChanged();
    void shapeSelectionChanged();
    
protected:
    /// reimplemented from KoToolBase
    virtual QWidget *createOptionWidget();
    virtual QList<QWidget *> createOptionWidgets();
    
    
private:
    PresentationViewPortShape* m_shape;
    PresentationViewPortConfigWidget* m_widget;
  
private slots:
    void setSequence(int newSeq);
    void setZoom(int newZoom);
    void setDuration(int newDuration);
    void setTransitionProfile(const QString profile);

    void setChangedProperty(QString attrName, QString attrValue);
    
    void setCurrentShape();

};

#endif
