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

#include "KPrPredefinedAnimationsLoader.h"

//Stage Headers
#include "KPrCollectionItemModel.h"
#include "animations/KPrAnimationBase.h"
#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationFactory.h"
#include "KPrFactory.h"
#include "StageDebug.h"

//Qt Headers
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainterPath>
#include <QPainter>
#include <QImage>
#include <QFont>
#include <QStandardPaths>

//KF5 Headers
#include <klocalizedstring.h>
#include <kiconloader.h>

//Calligra Headers
#include <KoIcon.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>
#include <KoPathShape.h>
#include <KoPathShapeLoader.h>

KPrPredefinedAnimationsLoader::KPrPredefinedAnimationsLoader(QObject *parent)
    : QObject(parent)
    , m_isInitialized(false)
{
    readDefaultAnimations();
}

KPrPredefinedAnimationsLoader::~KPrPredefinedAnimationsLoader()
{
    qDeleteAll(m_animations);
    qDeleteAll(m_mainItemsCollection);
}

bool KPrPredefinedAnimationsLoader::populateMainView(QListWidget *view)
{
    loadDefaultAnimations();
    if (m_mainItemsCollection.isEmpty()) {
        return false;
    }
    foreach(QListWidgetItem *item, m_mainItemsCollection) {
        QListWidgetItem *collectionChooserItem = new QListWidgetItem(item->icon(), item->text());
        collectionChooserItem->setData(Qt::UserRole, item->data(Qt::UserRole).toString());
        view->addItem(collectionChooserItem);
    }
    return true;
}

KPrCollectionItemModel *KPrPredefinedAnimationsLoader::modelById(const QString &id)
{
    if (m_modelMap.contains(id)) {
       return m_modelMap[id];
    }
    else {
        warnStageAnimation << "Didn't find a model with id ==" << id;
    }
    return 0;
}

KPrCollectionItemModel *KPrPredefinedAnimationsLoader::subModelById(const QString &id)
{
    if (m_subModelMap.contains(id)) {
        return m_subModelMap[id];
    }
    return 0;
}

void KPrPredefinedAnimationsLoader::loadDefaultAnimations()
{
    if (m_animations.isEmpty() || m_isInitialized) {
        return;
    }
    m_isInitialized = true;

    // Initialize animation class lists
    QVector<KPrCollectionItem> entranceList;
    QVector<KPrCollectionItem> emphasisList;
    QVector<KPrCollectionItem> exitList;
    QVector<KPrCollectionItem> customList;
    QVector<KPrCollectionItem> motion_PathList;
    QVector<KPrCollectionItem> ole_ActionList;
    QVector<KPrCollectionItem> media_CallList;

    QMap<QString, QVector<KPrCollectionItem> > subModelList;
    int row = -1;
    foreach(KPrShapeAnimation *animation, m_animations) {
        row++;
        bool isSubItem = true;
        QString animId = animation->id();
        // Check if this animation has sub types (and add them to the sub model list)
        if (!animation->presetSubType().isEmpty()) {
            if (!subModelList.contains(animId)) {
                subModelList.insert(animId, QVector<KPrCollectionItem>());
                isSubItem = false;
            }
            KPrCollectionItem subItem;
            QString newId = animId;
            newId.append("-");
            newId.append(animation->presetSubType());
            subItem.id = newId;
            subItem.name = animationName(newId);
            subItem.toolTip = animationName(newId);
            subItem.icon = loadSubTypeIcon(animId, animation->presetSubType());
            subItem.animationContext = m_animationContext.value(row);

            subModelList[animation->id()].append(subItem);
            if (isSubItem) {
                continue;
            }
        }
        KPrCollectionItem temp;
        temp.id = animId;
        temp.name = animationName(animId);
        temp.toolTip = temp.name;
        temp.animationContext = m_animationContext.value(row);
        if (animation->presetClass() == KPrShapeAnimation::MotionPath) {
            temp.icon = loadMotionPathIcon(temp.animationContext);
        }
        else {
            temp.icon = loadAnimationIcon(temp.name);
        }

        if (animation->presetClass() == KPrShapeAnimation::Entrance) {
            entranceList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Exit) {
            exitList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Emphasis) {
            emphasisList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::Custom) {
            customList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::MotionPath) {
            motion_PathList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::OleAction) {
            ole_ActionList.append(temp);
        }
        else if (animation->presetClass() == KPrShapeAnimation::MediaCall) {
            media_CallList.append(temp);
        }
    }
    KPrCollectionItemModel* model = new KPrCollectionItemModel(this);
    model->setAnimationClassList(entranceList);
    addCollection("entrance", i18n("Entrance"), model);

    // Populate animation class models using animation class lists
    if (!exitList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(exitList);
        addCollection("exit", i18n("Exit"), model);
    }
    if (!emphasisList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(emphasisList);
        addCollection("emphasis", i18n("Emphasis"), model);
    }
    if (!customList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(customList);
        addCollection("custom", i18n("Custom"), model);
    }
    if (!motion_PathList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(motion_PathList);
        addCollection("motion_path", i18n("Motion path"), model);
    }
    if (!ole_ActionList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(ole_ActionList);
        addCollection("ole_action", i18n("Ole Action"), model);
    }
    if (!media_CallList.isEmpty()) {
        model = new KPrCollectionItemModel(this);
        model->setAnimationClassList(media_CallList);
        addCollection("media_call", i18n("Media Call"), model);
    }

    // Create models for subtypes and populate them using sub model list
    if (!subModelList.isEmpty()) {
        QMap<QString, QVector<KPrCollectionItem> >::const_iterator i;
        for (i = subModelList.constBegin(); i != subModelList.constEnd(); ++i) {
            model = new KPrCollectionItemModel(this);
            model->setAnimationClassList(i.value());
            addSubCollection(i.key(), model);
        }
    }
}

void KPrPredefinedAnimationsLoader::readDefaultAnimations()
{
    // Read animation data from xml file and store data on lists
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);
    KoXmlDocument doc;

    const QString filePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligrastage/animations/animations.xml");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        QString errorMessage;
        int row = 0;
        if (KoOdfReadStore::loadAndParse(&file, doc, errorMessage, filePath)) {
            const KoXmlElement docElement  = doc.documentElement();
            KoXmlElement animationElement;
            forEachElement(animationElement, docElement) {
                KoXmlElement parAnimation;
                forEachElement(parAnimation, animationElement) {
                    KoXmlElement animation;
                    forEachElement(animation, parAnimation) {
                        KPrShapeAnimation *shapeAnimation = 0;
                        shapeAnimation = loadOdfShapeAnimation(animation, shapeContext);
                        if (shapeAnimation) {
                            m_animations.append(shapeAnimation);
                            m_animationContext.append(animation);
                            row++;
                        }
                    }
                }
            }
        }
        else {
            warnStageAnimation << "reading of" << filePath << "failed:" << errorMessage;
        }
    }
    else {
        debugStageAnimation << "animations.xml not found";
    }
}

QString KPrPredefinedAnimationsLoader::animationName(const QString &id) const
{
    QStringList descriptionList = id.split(QLatin1Char('-'));
    if (descriptionList.count() > 2) {
        // Remove the "stage-type" part (type: entrance, exit, motion-path, etc.)
        descriptionList.removeFirst();
        descriptionList.removeFirst();
        // Replace "-" on id with spaces
        return descriptionList.join(QChar::fromLatin1(' '));
    }
    return QString();
}

QIcon KPrPredefinedAnimationsLoader::loadAnimationIcon(const QString &id)
{
    // Animation icon names examples: zoom_animation, spiral_in_animation
    // If an specific animation icon does not exist then return a generic
    // unrecognized animation icon
    QString name = id;
    if (!name.isEmpty()) {
        name = name.append("_animation");
        name.replace(QLatin1Char(' '), QLatin1Char('_'));
        QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            return QIcon::fromTheme(name);
        }
    }
    return koIcon("unrecognized_animation");
}

QIcon KPrPredefinedAnimationsLoader::loadSubTypeIcon(const QString &mainId, const QString &subTypeId)
{
    Q_UNUSED(mainId);
    QIcon icon;
    // icon name ex: entrance_zoom_in_animations
    QString subId = subTypeId;
    subId.replace(QLatin1Char('-'), QLatin1Char('_'));
    // icon name ex: in_animations
    QString name = subId;
    name.append("_animations");
    QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = QIcon::fromTheme(name);
    }
    else {
        // If an specific animation icon does not exist then return a generic
        // unrecognized animation icon
        icon = koIcon("unrecognized_animation");
    }
    return icon;
}

QIcon KPrPredefinedAnimationsLoader::loadMotionPathIcon(const KoXmlElement &element)
{
    KoXmlElement e;
    QString path;
    forEachElement(e, element) {
        path = e.attributeNS(KoXmlNS::svg, "path");
        if (!path.isEmpty()) {
            break;
        }
    }
    // Create icon drawing the path
    if (!path.isEmpty()) {
        const int margin = 8;
        const int width = 4;
        KoPathShape pathShape;
        KoPathShapeLoader loader(&pathShape);
        loader.parseSvg(path, true);
        QPixmap thumbnail(QSize(KIconLoader::SizeHuge, KIconLoader::SizeHuge));
        // fill backgroung
        thumbnail.fill(QColor(Qt::white).rgb());
        QRect imageRect = thumbnail.rect();
        // adjust to left space for margins
        imageRect.adjust(margin, margin, -margin, -margin);
        pathShape.setSize(imageRect.size());
        QPainterPath m_path = pathShape.outline();
        //Center path
        m_path.translate(-m_path.boundingRect().x() + margin, -m_path.boundingRect().y() + margin);
        QPainter painter(&thumbnail);
        painter.setPen(QPen(QColor(0, 100, 224), width, Qt::SolidLine,
                            Qt::FlatCap, Qt::MiterJoin));
        // Draw path
        painter.drawPath(m_path);
        return QIcon(thumbnail);
    }
    // Default icon if path was not found (It must be a error?)
    return koIcon("unrecognized_animation");
}

KPrShapeAnimation *KPrPredefinedAnimationsLoader::loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape)
{
    // load preset and id
    //TODO: ole-action, media-call are not supported

    KPrShapeAnimation *shapeAnimation = 0;
    // The shape info and create a KPrShapeAnimation. If there is
    KoXmlElement e;
    forEachElement(e, element) {
        if (shapeAnimation == 0) {
            KoShape *shape = animShape;
            QTextBlockUserData *textBlockData = 0;
            shapeAnimation = new KPrShapeAnimation(shape, textBlockData);
        }
        KPrAnimationBase *animation(KPrAnimationFactory::createAnimationFromOdf(e, context, shapeAnimation));
        if (animation) {
            shapeAnimation->addAnimation(animation);
        }
    }

    if (shapeAnimation) {
        QString presetClass = element.attributeNS(KoXmlNS::presentation, "preset-class");
        QString animationId = element.attributeNS(KoXmlNS::presentation, "preset-id");
        QString presetSubType = element.attributeNS(KoXmlNS::presentation, "preset-sub-type");
        if (presetClass == "custom") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Custom);
        }
        if (presetClass == "entrance") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Entrance);
        }
        else if (presetClass == "exit") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Exit);
        }
        else if (presetClass == "emphasis") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Emphasis);
        }
        else if (presetClass == "motion-path") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::MotionPath);
        }
        else if (presetClass == "ole-action") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::OleAction);
        }
        else if (presetClass == "media-call") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::MediaCall);
        }
        else{
            shapeAnimation->setPresetClass(KPrShapeAnimation::None);
        }
        if (!animationId.isEmpty()) {
            shapeAnimation->setId(animationId);
        }
        if (!presetSubType.isEmpty()) {
            shapeAnimation->setPresetSubType(presetSubType);
        }
    }
    return shapeAnimation;
}

bool KPrPredefinedAnimationsLoader::addCollection(const QString &id, const QString &title, KPrCollectionItemModel *model)
{
    if (m_modelMap.contains(id)) {
        return false;
    }
    m_modelMap.insert(id, model);
    QString iconName = id;
    iconName.append("_animations");
    QIcon icon;
    QString path = KIconLoader::global()->iconPath(iconName, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = QIcon::fromTheme(iconName);
    }
    else {
        icon = koIcon("unrecognized_animation");
    }

    QListWidgetItem *collectionChooserItem = new QListWidgetItem(icon, title);
    collectionChooserItem->setData(Qt::UserRole, id);
    m_mainItemsCollection.append(collectionChooserItem);
    return true;
}

bool KPrPredefinedAnimationsLoader::addSubCollection(const QString &id, KPrCollectionItemModel *model)
{
    if (m_subModelMap.contains(id)) {
        return false;
    }
    m_subModelMap.insert(id, model);
    return true;
}
