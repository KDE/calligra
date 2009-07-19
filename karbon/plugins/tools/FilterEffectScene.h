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

#ifndef FILTEREFFECTSCENE_H
#define FILTEREFFECTSCENE_H

#include <QtGui/QGraphicsScene>
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QMap>

class KoShape;
class KoFilterEffect;
class QGraphicsItem;
class EffectItemBase;
class EffectItem;
class DefaultInputItem;
class ConnectionItem;

class SceneItem
{
public:
    enum Type { 
        EffectItem,      ///< a complete effect item
        SourceGraphic,   ///< SourceGraphic predefined input image
        SourceAlpha,     ///< SourceAlpha predefined input image
        BackgroundImage, ///< BackgroundImage predefined input image
        BackgroundAlpha, ///< BackgroundAlpha predefined input image
        FillPaint,       ///< FillPaint predefined input image
        StrokePaint      ///< StrokePaint predefined input image
    };
    
    SceneItem();
    SceneItem(KoFilterEffect *effect, Type type);
    /// Returns the selected items type
    Type type() const;
    /// Returns the corresponding filter effect 
    KoFilterEffect * effect() const;
    
    static Type typeFromString(const QString &str);
    static QString typeToString(Type type);
private:
    KoFilterEffect * m_effect;
    Type m_type;
};

struct SceneConnection
{
    SceneItem source; ///< the start of the connection (effect output)
    SceneItem target; ///< the end of a connection (effect input)
    int targetIndex;  ///< the input index of the connection end
};

class FilterEffectScene : public QGraphicsScene
{
    Q_OBJECT
public:
    FilterEffectScene(QObject *parent = 0);
    
    /// initializes the scene from the filter effect stack
    void initialize(const QList<KoFilterEffect*> &effects);
    
    /// Returns list of selected effect items
    QList<SceneItem> selectedEffectItems() const;
    
signals:
    void connectionCreated(SceneConnection connection);
    
protected:
    /// reimplemented from QGraphicsScene
    virtual void dropEvent(QGraphicsSceneDragDropEvent * event);
    
private slots:
    void selectionChanged();
    
private:
    void createEffectItems(KoFilterEffect *effect);
    void addSceneItem(QGraphicsItem *item);
    void layoutConnections();
    void layoutEffects();
    
    QSet<QString> m_defaultInputs;
    QList<KoFilterEffect*> m_effects;
    QList<EffectItemBase*> m_items;
    QList<ConnectionItem*> m_connectionItems;
    QMap<QString, EffectItemBase*> m_outputs;
};

#endif // FILTEREFFECTSCENE_H
