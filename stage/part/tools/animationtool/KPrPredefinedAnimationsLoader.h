/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPREDEFINEDANIMATIONSLOADER_H
#define KPRPREDEFINEDANIMATIONSLOADER_H

#include <KoXmlReader.h>
#include <QMap>
#include <QObject>

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
    explicit KPrPredefinedAnimationsLoader(QObject *parent = nullptr);
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
    KPrShapeAnimation *loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape = nullptr);

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
    QMap<QString, KPrCollectionItemModel *> m_modelMap;
    QMap<QString, KPrCollectionItemModel *> m_subModelMap;
    QList<QListWidgetItem *> m_mainItemsCollection;
    bool m_isInitialized;
};

#endif // KPRPREDEFINEDANIMATIONSLOADER_H
