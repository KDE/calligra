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

#ifndef KPRPREDEFINEDANIMATIONSLOADER_H
#define KPRPREDEFINEDANIMATIONSLOADER_H

#include <QObject>
#include <QMap>
#include <KoXmlReader.h>

class KPrShapeAnimation;
class KoXmlElement;
class KoShapeLoadingContext;
class KPrCollectionItemModel;
class QListWidgetItem;
class QListWidget;
class KoShape;
class KPrShapeAnimationDocker;

class KPrPredefinedAnimationsLoader : public QObject
{
    Q_OBJECT
public:
    KPrPredefinedAnimationsLoader(QObject* parent = 0);

    //Data is not ready until this method is called
    bool populateMainView (QListWidget *view);
    KPrCollectionItemModel *modelById(const QString &id);
    KPrCollectionItemModel *subModelById(const QString &id);

    KPrShapeAnimation *loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context,
                                             KoShape *animShape = 0);

private:

    void loadDefaultAnimations();
    void readDefaultAnimations();

    QString animationName(const QString id) const;
    QIcon loadAnimationIcon(const QString id);
    QIcon loadSubTypeIcon(const QString mainId, const QString subTypeId);

    /**
     * Add a collection to the docker
     */
    bool addCollection(const QString& id, const QString& title, KPrCollectionItemModel* model);

    bool addSubCollection(const QString &id, KPrCollectionItemModel *model);

    QList<KPrShapeAnimation *> m_animations;
    QList<KoXmlElement> m_animationContext;
    QMap<QString, KPrCollectionItemModel*> m_modelMap;
    QMap<QString, KPrCollectionItemModel*> m_subModelMap;
    QList<QListWidgetItem *> m_mainItemsCollection;
    bool isInitialized;
};

#endif // KPRPREDEFINEDANIMATIONSLOADER_H
