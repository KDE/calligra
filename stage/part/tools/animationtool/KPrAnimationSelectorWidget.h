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


#ifndef KPRANIMATIONSELECTORWIDGET_H
#define KPRANIMATIONSELECTORWIDGET_H

#include <QWidget>

#include <QWidget>
#include <QMap>
#include <KoXmlReader.h>

class QModelIndex;
class KoShape;
class QModelIndex;
class QListWidget;
class QListView;
class KPrShapeAnimation;
class KoXmlElement;
class KoShapeLoadingContext;
class KPrCollectionItemModel;
class QListWidgetItem;
class KPrShapeAnimationDocker;

class KPrAnimationSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrAnimationSelectorWidget(KPrShapeAnimationDocker *docker, QWidget *parent = 0);
    ~KPrAnimationSelectorWidget();
    
signals:
    void requestPreviewAnimation(KPrShapeAnimation *animation);
    void requestAcceptAnimation(KPrShapeAnimation *animation);
    
public slots:

private slots:
    void automaticPreviewRequested(const QModelIndex &index);
    /**
     * Changes the current shape collection
     */
    void activateShapeCollection(QListWidgetItem *item);

    void setAnimation(const QModelIndex& index);

    void setPreviewState(bool isEnable);

    void loadDefaultAnimations();
    void readDefaultAnimations();

private:
    QString animationName(const QString id) const;
    QIcon loadAnimationIcon(const QString id);
    KPrShapeAnimation *loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape = 0);

    /**
     * Add a collection to the docker
     */
    bool addCollection(const QString& id, const QString& title, KPrCollectionItemModel* model);

    /// load / save automatic preview checkbox state
    bool loadPreviewConfig();
    void savePreviewConfig();

    QListWidget *m_collectionChooser;
    QListView *m_collectionView;
    QList<KPrShapeAnimation *> m_animations;
    QList<KoXmlElement> m_animationContext;
    QMap<QString, KPrCollectionItemModel*> m_modelMap;
    KPrShapeAnimationDocker *m_docker;
    KPrShapeAnimation *m_previewAnimation;
    bool showAutomaticPreview;
    
};

#endif // KPRANIMATIONSELECTORWIDGET_H
