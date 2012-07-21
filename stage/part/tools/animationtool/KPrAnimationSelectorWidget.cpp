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


#include "KPrAnimationSelectorWidget.h"

//Stage Headers
#include "KPrShapeAnimationDocker.h"
#include "KPrCollectionItemModel.h"
#include "KPrViewModePreviewShapeAnimations.h"
#include "animations/KPrAnimationBase.h"
#include "animations/KPrShapeAnimation.h"
#include "animations/KPrAnimationFactory.h"
#include "KPrFactory.h"

//Qt Headers
#include <QCheckBox>
#include <QGridLayout>
#include <QListWidget>
#include <QListView>

//KDE Headers
#include <KLocale>
#include <KIconLoader>
#include <KIcon>
#include <KStandardDirs>
#include <KConfigGroup>
#include <KDebug>

//Calligra Headers
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>
#include <KoStore.h>

KPrAnimationSelectorWidget::KPrAnimationSelectorWidget(KPrShapeAnimationDocker *docker, QWidget *parent)
    : QWidget(parent)
    , m_docker(docker)
    , showAutomaticPreview(true)
{
    QGridLayout *containerLayout = new QGridLayout;
    m_animations = QList<KPrShapeAnimation *>();
    m_animationContext = QList<KoXmlElement>();

    QCheckBox *previewCheckBox = new QCheckBox(i18n("Automatic animation preview"), this);
    previewCheckBox->setChecked(loadPreviewConfig());
    showAutomaticPreview = previewCheckBox->isChecked();

    m_collectionChooser = new QListWidget;
    m_collectionChooser->setViewMode(QListView::IconMode);
    m_collectionChooser->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_collectionChooser->setSelectionMode(QListView::SingleSelection);
    m_collectionChooser->setResizeMode(QListView::Adjust);
    m_collectionChooser->setGridSize(QSize(75, 64));
    m_collectionChooser->setFixedWidth(90);
    m_collectionChooser->setMovement(QListView::Static);
    m_collectionChooser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_collectionChooser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(m_collectionChooser, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(activateShapeCollection(QListWidgetItem *)));

    m_collectionView = new QListView;
    m_collectionView->setViewMode(QListView::IconMode);
    m_collectionView->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_collectionView->setDragDropMode(QListView::DragOnly);
    m_collectionView->setSelectionMode(QListView::SingleSelection);
    m_collectionView->setResizeMode(QListView::Adjust);
    m_collectionView->setGridSize(QSize(75, 64));
    m_collectionView->setWordWrap(true);
    m_collectionView->viewport()->setMouseTracking(true);
    connect(m_collectionView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(setAnimation(const QModelIndex&)));

    m_subTypeView = new QListView;
    m_subTypeView->setViewMode(QListView::IconMode);
    m_subTypeView->setIconSize(QSize(KIconLoader::SizeLarge, KIconLoader::SizeLarge));
    m_subTypeView->setDragDropMode(QListView::DragOnly);
    m_subTypeView->setSelectionMode(QListView::SingleSelection);
    m_subTypeView->setResizeMode(QListView::Adjust);
    m_subTypeView->setGridSize(QSize(75, 64));
    m_subTypeView->setFixedHeight(79);
    m_subTypeView->setWordWrap(true);
    m_subTypeView->viewport()->setMouseTracking(true);
    m_subTypeView->hide();
    connect(m_subTypeView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(setAnimation(const QModelIndex&)));

    containerLayout->addWidget(m_collectionChooser, 0, 0,2,1);
    containerLayout->addWidget(m_collectionView, 0, 1, 1, 1);
    containerLayout->addWidget(m_subTypeView, 1, 1, 1, 1);
    containerLayout->addWidget(previewCheckBox, 2, 0, 1, 2);


    // set signals
    connect(m_collectionView, SIGNAL(entered(QModelIndex)), this, SLOT(automaticPreviewRequested(QModelIndex)));
    connect(m_subTypeView, SIGNAL(entered(QModelIndex)), this, SLOT(automaticPreviewRequested(QModelIndex)));
    connect(previewCheckBox, SIGNAL(toggled(bool)), this, SLOT(setPreviewState(bool)));

    setLayout(containerLayout);

    loadDefaultAnimations();
}

KPrAnimationSelectorWidget::~KPrAnimationSelectorWidget()
{
    savePreviewConfig();
    // stop animation
    if (m_docker->previewMode()) {
        m_docker->previewMode()->stopAnimation();
    }
}

void KPrAnimationSelectorWidget::automaticPreviewRequested(const QModelIndex &index)
{
    if(!index.isValid() || !showAutomaticPreview) {
        return;
    }
    KoXmlElement newAnimationContext;
    if (QObject::sender() == m_collectionView) {
        newAnimationContext = static_cast<KPrCollectionItemModel*>(m_collectionView->model())->animationContext(index);
    }
    else if (QObject::sender() == m_subTypeView) {
        newAnimationContext = static_cast<KPrCollectionItemModel*>(m_subTypeView->model())->animationContext(index);
    }
    else {
        return;
    }

    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);

    KoShape *shape = m_docker->getSelectedShape();
    if (!shape) {
        return;
    }
    m_previewAnimation = loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (m_previewAnimation) {
        emit requestPreviewAnimation(m_previewAnimation);
    }
}

void KPrAnimationSelectorWidget::activateShapeCollection(QListWidgetItem *item)
{
    QString id = item->data(Qt::UserRole).toString();

    if(m_modelMap.contains(id)) {
        m_collectionView->setModel(m_modelMap[id]);
    }
    else {
        kWarning(31000) << "Didn't find a model with id ==" << id;
    }
    m_subTypeView->setModel(0);
    m_subTypeView->hide();
}

void KPrAnimationSelectorWidget::setAnimation(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    KoXmlElement newAnimationContext;
    if (QObject::sender() == m_collectionView) {
        m_subTypeView->hide();
        QString id = static_cast<KPrCollectionItemModel*>(m_collectionView->model())->data(index, Qt::UserRole).toString();
        if (m_subModelMap.contains(id)){
            m_subTypeView->setModel(m_subModelMap[id]);
            m_subTypeView->show();
            return;
        }
        newAnimationContext = static_cast<KPrCollectionItemModel*>(m_collectionView->model())->animationContext(index);
    }
    else if (QObject::sender() == m_subTypeView) {
        newAnimationContext = static_cast<KPrCollectionItemModel*>(m_subTypeView->model())->animationContext(index);
    }
    else {
        return;
    }

    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);

    KoShape *shape = m_docker->getSelectedShape();
    if (!shape) {
        kWarning(31000) << "No shape found";
        return;
    }

    KPrShapeAnimation *newAnimation = loadOdfShapeAnimation(newAnimationContext, shapeContext, shape);
    if (newAnimation) {
        emit requestAcceptAnimation(newAnimation);
    }
}

void KPrAnimationSelectorWidget::setPreviewState(bool isEnable)
{
    showAutomaticPreview = isEnable;
}

void KPrAnimationSelectorWidget::loadDefaultAnimations()
{
    readDefaultAnimations();
    if (m_animations.isEmpty()) {
        return;
    }
    QList<KPrCollectionItem> entranceList;
    QList<KPrCollectionItem> emphasisList;
    QList<KPrCollectionItem> exitList;
    QList<KPrCollectionItem> customList;
    QList<KPrCollectionItem> motion_PathList;
    QList<KPrCollectionItem> ole_ActionList;
    QList<KPrCollectionItem> media_CallList;

    QMap<QString, QList<KPrCollectionItem> > subModelList;

    foreach(KPrShapeAnimation *animation, m_animations) {
        bool isSubItem = false;
        KPrCollectionItem temp;
        temp.id = animation->id();
        temp.name = animationName(animation->id());
        temp.toolTip = animationName(animation->id());
        temp.icon = loadAnimationIcon(animation->id());
        temp.animationContext = m_animationContext.value(m_animations.indexOf(animation));

        if (!animation->presetSubType().isEmpty()) {
            isSubItem = true;
            if (!subModelList.contains(animation->id())) {
                QList<KPrCollectionItem> tempList = QList<KPrCollectionItem>();
                subModelList.insert(animation->id(), tempList);
                isSubItem = false;
            }
            KPrCollectionItem subItem;
            QString newId = animation->id();
            newId.append("-");
            newId.append(animation->presetSubType());
            subItem.id = newId;
            subItem.name = animationName(newId);
            subItem.toolTip = animationName(newId);
            subItem.icon = loadSubTypeIcon(animation->id(), animation->presetSubType());
            subItem.animationContext = m_animationContext.value(m_animations.indexOf(animation));

            subModelList[animation->id()].append(subItem);
            if (isSubItem) {
                continue;
            }
        }

        if (animation->presetClass() == KPrShapeAnimation::Entrance) {
            entranceList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Exit) {
            exitList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Emphasis) {
            emphasisList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Custom) {
            customList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Motion_Path) {
            motion_PathList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Ole_Action) {
            ole_ActionList.append(temp);
        }
        if (animation->presetClass() == KPrShapeAnimation::Media_Call) {
            media_CallList.append(temp);
        }
    }


    KPrCollectionItemModel* model = new KPrCollectionItemModel(this);
    model->setAnimationClassList(entranceList);
    addCollection("entrance", i18n("Entrance"), model);

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

    if (!subModelList.isEmpty()) {
        QMap<QString, QList<KPrCollectionItem> >::const_iterator i;
        for (i = subModelList.constBegin(); i != subModelList.constEnd(); ++i) {
            model = new KPrCollectionItemModel(this);
            model->setAnimationClassList(i.value());
            addSubCollection(i.key(), model);
        }
    }

    m_collectionChooser->setCurrentRow(0);
    activateShapeCollection(m_collectionChooser->item(0));
}

void KPrAnimationSelectorWidget::readDefaultAnimations()
{
    // use the same mechanism for loading the markers that are available
    // per default as when loading the normal markers.
    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext context(stylesReader, 0);
    KoShapeLoadingContext shapeContext(context, 0);
    KoXmlDocument doc;

    const KStandardDirs* dirs = KGlobal::activeComponent().dirs();
    const QString filePath = dirs->findResource("data", "stage/animations/animations.xml");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        QString errorMessage;
        if (KoOdfReadStore::loadAndParse(&file, doc, errorMessage, filePath)) {
            const KoXmlElement docElement  = doc.documentElement();
            QString nodeType(docElement.attributeNS(KoXmlNS::presentation, "node-type"));
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
                        }
                    }
                }
            }
        }
        else {
            kWarning(30006) << "reading of" << filePath << "failed:" << errorMessage;
        }
    }
    else {
        kDebug(30006) << "animations.xml not found";
    }
}

QString KPrAnimationSelectorWidget::animationName(const QString id) const
{
    QStringList descriptionList = id.split("-");
    if (descriptionList.count() > 2) {
        descriptionList.removeFirst();
        descriptionList.removeFirst();
        return descriptionList.join(QString(" "));
    }
    return QString();
}

QIcon KPrAnimationSelectorWidget::loadAnimationIcon(const QString id)
{
    // Animation icon names examples: zoom_animation, spiral_in_animation
    QString name = animationName(id);
    if (!name.isEmpty()) {
        name = name.append("_animation");
        name.replace(" ", "_");
        QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            return KIcon(name);
        }
    }
    return KIcon("unrecognized_animation");
}

QIcon KPrAnimationSelectorWidget::loadSubTypeIcon(const QString mainId, const QString subTypeId)
{
    // icon name ex: entrance_zoom_in_animations
    QString subId = subTypeId;
    subId.replace("-", "_");

    QString name = animationName(mainId);
    name.append("_");
    name.append(subId);
    name.append("_animation");

    QIcon icon;
    QString path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = KIcon(name);
    }
    else {
        // icon name ex: in_animations
        name = subId;
        name.append("_animations");
        path = KIconLoader::global()->iconPath(name, KIconLoader::Toolbar, true);
        if (!path.isNull()) {
            icon = KIcon(name);
        }
        else {
            icon = KIcon("unrecognized_animation");
        }
    }
    return icon;
}

KPrShapeAnimation *KPrAnimationSelectorWidget::loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape)
{
    // load preset and id
    //TODO: motion-path, ole-action, media-call are not supported
    QString presetClass = element.attributeNS(KoXmlNS::presentation, "preset-class");
    QString animationId = element.attributeNS(KoXmlNS::presentation, "preset-id");
    QString presetSubType = element.attributeNS(KoXmlNS::presentation, "preset-sub-type");

    KPrShapeAnimation::Node_Type l_nodeType = KPrShapeAnimation::On_Click;

    KPrShapeAnimation *shapeAnimation = 0;
    // The shape info and create a KPrShapeAnimation. If there is
    KoXmlElement e;
    forEachElement(e, element) {
        if (shapeAnimation == 0) {
            KoShape *shape = animShape;
            KoTextBlockData *textBlockData = 0;
            shapeAnimation = new KPrShapeAnimation(shape, textBlockData);
        }
        KPrAnimationBase *animation(KPrAnimationFactory::createAnimationFromOdf(e, context, shapeAnimation));
        if (shapeAnimation && animation) {
            shapeAnimation->addAnimation(animation);
        }
    }

    if (shapeAnimation) {
        shapeAnimation->setNodeType(l_nodeType);
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
            shapeAnimation->setPresetClass(KPrShapeAnimation::Motion_Path);
        }
        else if (presetClass == "ole-action") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Ole_Action);
        }
        else if (presetClass == "media-call") {
            shapeAnimation->setPresetClass(KPrShapeAnimation::Media_Call);
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

bool KPrAnimationSelectorWidget::addCollection(const QString &id, const QString &title, KPrCollectionItemModel *model)
{
    if(m_modelMap.contains(id))
        return false;
    m_modelMap.insert(id, model);
    QString iconName = id;
    iconName.append("_animations");
    QIcon icon;
    QString path = KIconLoader::global()->iconPath(iconName, KIconLoader::Toolbar, true);
    if (!path.isNull()) {
        icon = KIcon(iconName);
    }
    else {
        icon = KIcon("unrecognized_animation");
    }

    QListWidgetItem *collectionChooserItem = new QListWidgetItem(icon, title);
    collectionChooserItem->setData(Qt::UserRole, id);
    m_collectionChooser->addItem(collectionChooserItem);
    return true;
}

bool KPrAnimationSelectorWidget::addSubCollection(const QString &id, KPrCollectionItemModel *model)
{
    if(m_subModelMap.contains(id))
        return false;
    m_subModelMap.insert(id, model);
    return true;
}

bool KPrAnimationSelectorWidget::loadPreviewConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    bool showPreview = true;

    if (config->hasGroup("Interface")) {
        const KConfigGroup interface = config->group("Interface");
        showPreview = interface.readEntry("ShowAutomaticPreviewAnimationEditDocker", showPreview);
    }
    return showPreview;
}

void KPrAnimationSelectorWidget::savePreviewConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ShowAutomaticPreviewAnimationEditDocker", showAutomaticPreview);
}
