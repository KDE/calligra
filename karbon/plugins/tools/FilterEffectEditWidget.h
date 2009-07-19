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

#ifndef FILTEREFFECTEDITWIDGET_H
#define FILTEREFFECTEDITWIDGET_H

#include "ui_FilterEffectEditWidget.h"
#include <QtGui/QWidget>

class KoShape;
class KoFilterEffect;
class EffectItemBase;
class DefaultInputItem;
class EffectItem;
class ConnectionItem;
class QGrapicsItem;

class FilterEffectEditWidget : public QWidget, Ui::FilterEffectEditWidget
{
    Q_OBJECT
public:
    FilterEffectEditWidget(QWidget *parent = 0);
    
    /// Edits effects of given shape
    void editShape(KoShape *shape);
    
protected:
    /// reimplemented from QWidget
    virtual void resizeEvent ( QResizeEvent * event );
    /// reimplemented from QWidget
    virtual void showEvent ( QShowEvent * event );
private slots:
    void addSelectedEffect();
    void removeSelectedItem();
    void sceneSelectionChanged();
    
private:
    void initScene();
    void fitScene();
    void layoutConnections();
    void layoutEffects();
    void addItem(QGraphicsItem *item);
    void createEffectItems(KoFilterEffect *effect);
    EffectItem * nextEffectItemFromIndex(int index);
    EffectItem * prevEffectItemFromIndex(int index);
    void removeDefaultInputItem(DefaultInputItem * item);
    
    QGraphicsScene * m_scene;
    KoShape * m_shape;
    QList<KoFilterEffect*> m_effects;
    QList<EffectItemBase*> m_items;
    QList<ConnectionItem*> m_connectionItems;
    QMap<QString, EffectItemBase*> m_outputs;
    QSet<QString> m_defaultInputs;
};

#endif // FILTEREFFECTEDITWIDGET_H
