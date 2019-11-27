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

/** This class holds the data model for predefined animations
  parsed from the animations.xml file*/
class KPrPredefinedAnimationsLoader : public QObject
{
    Q_OBJECT
public:
    explicit KPrPredefinedAnimationsLoader(QObject *parent = 0);
    ~KPrPredefinedAnimationsLoader() override;

    // Data is not ready until this method is called
    /**
     * @brief Populate categories view
     * @param view QListWidget pointer of the view to be populated with predefined animations data
     */
    bool populateMainView(QListWidget *view);

    /**
     * @brief Return model for each id category
     * @param id of the model
     */
    KPrCollectionItemModel *modelById(const QString &id);

    /**
     * @brief Return sub model (Model for preset sub types
     * @param id of the sub model
     */
    KPrCollectionItemModel *subModelById(const QString &id);

    /**
     * @brief Returns animation from context xml data
     * @param element KoXmlElemnt with xml data of the animation
     * @param context KoShapeLoadingContext class
     * @param animShape Shape that holds the animation
     */
    KPrShapeAnimation *loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context,
                                             KoShape *animShape = 0);

private:
    /**
     * @brief Create KPrAnimationItem using data stored on animations list
     */
    void loadDefaultAnimations();

    /**
     * @brief Read animations from xml file and populate animations list
     */
    void readDefaultAnimations();

    /**
      * Return a printable animation name using animation id
      *
      * @param id QString holding the animation id
      * @return a QString holding the name
      */
    QString animationName(const QString &id) const;

    /**
      * Return the animation class icon (Ex of classes: zoom, swivel, appear, etc)
      *
      * @param id QString holding the animation id
      * @return a QIcon depending on animation class
      */
    QIcon loadAnimationIcon(const QString &id);

    /**
      * Return the animation sub type icon (Ex of subtypes: up, down, from left, etc)
      *
      * @param id QString holding the animation id
      * @return a QIcon depending on animation sub type
      */
    QIcon loadSubTypeIcon(const QString &mainId, const QString &subTypeId);

    /**
      * Return an animation icon created using the motion path
      *
      * @param id QString holding the animation id
      * @return a QIcon depending on motion path
      */
    QIcon loadMotionPathIcon(const KoXmlElement &element);

    /**
     * @brief Add a collection to the docker
     */
    bool addCollection(const QString &id, const QString &title, KPrCollectionItemModel *model);

    /**
     * @brief Add a sub collection to the docker
     */
    bool addSubCollection(const QString &id, KPrCollectionItemModel *model);

    QList<KPrShapeAnimation *> m_animations;
    QList<KoXmlElement> m_animationContext;
    QMap<QString, KPrCollectionItemModel*> m_modelMap;
    QMap<QString, KPrCollectionItemModel*> m_subModelMap;
    QList<QListWidgetItem *> m_mainItemsCollection;
    bool m_isInitialized;
};

#endif // KPRPREDEFINEDANIMATIONSLOADER_H
