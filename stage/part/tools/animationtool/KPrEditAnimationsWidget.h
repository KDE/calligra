/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPREDITANIMATIONSWIDGET_H
#define KPREDITANIMATIONSWIDGET_H

#include <QWidget>
#include <QMap>
#include <KoXmlReader.h>

class KPrAnimationsTimeLineView;
class KPrAnimationsDataModel;
class KPrView;
class KoPAViewBase;
class QModelIndex;
class KoShape;
class QModelIndex;
class KPrCustomAnimationItem;
class QComboBox;
class QTimeEdit;
class QListWidget;
class QListView;
class KPrShapeAnimation;
class KoXmlElement;
class KoShapeLoadingContext;
class KPrCollectionItemModel;
class QListWidgetItem;

/** This Widget holds the configuration popup dialog
    used to edit animation properties of existing or new
    animation.
    Animations on this view are displayed in groups that
    have as parent animations started by a click*/
class KPrEditAnimationsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrEditAnimationsWidget(QWidget *parent = 0);
    void setView(KoPAViewBase *view);
    void setParentItem(KPrCustomAnimationItem *item, KPrCustomAnimationItem *rootItem);
    void setCurrentItem(KPrCustomAnimationItem *item);
    void setActiveShape(KoShape *shape);
    
signals:
    void itemClicked(const QModelIndex &index);
    void requestAnimationPreview();
    void rootRemoved();

public slots:
    void updateIndex(const QModelIndex &index);
    void setBeginTime();
    void setDuration();
    void setTriggerEvent(int row);
    void syncCurrentItem();

protected slots:
    /**
     * Changes the current shape collection
     */
    void activateShapeCollection(QListWidgetItem *item);

    void setAnimation(const QModelIndex& index);

private:
    void loadDefaultAnimations();
    void readDefaultAnimations();
    QString animationName(const QString id) const;
    QIcon loadAnimationIcon(const QString id);
    KPrShapeAnimation *loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape = 0);

    /**
     * Add a collection to the docker
     */
    bool addCollection(const QString& id, const QString& title, KPrCollectionItemModel* model);

    KPrView* m_view;
    KPrAnimationsTimeLineView *m_timeLineView;
    KPrAnimationsDataModel *m_timeLineModel;
    QComboBox *m_triggerEventList;
    QTimeEdit *m_delayEdit;
    QTimeEdit *m_durationEdit;
    QListWidget *m_collectionChooser;
    QListView *m_collectionView;
    QList<KPrShapeAnimation *> m_animations;
    QList<KoXmlElement> m_animationContext;
    QMap<QString, KPrCollectionItemModel*> m_modelMap;
    
};

#endif // KPREDITANIMATIONSWIDGET_H
