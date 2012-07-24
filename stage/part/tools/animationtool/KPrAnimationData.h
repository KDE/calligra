#ifndef KPRANIMATIONDATA_H
#define KPRANIMATIONDATA_H


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

class KPrAnimationData
{
public:
    KPrAnimationData();

private:

    void loadDefaultAnimations();
    void readDefaultAnimations();

    QString animationName(const QString id) const;
    QIcon loadAnimationIcon(const QString id);
    QIcon loadSubTypeIcon(const QString mainId, const QString subTypeId);
    KPrShapeAnimation *loadOdfShapeAnimation(const KoXmlElement &element, KoShapeLoadingContext &context, KoShape *animShape = 0);

    /**
     * Add a collection to the docker
     */
    bool addCollection(const QString& id, const QString& title, KPrCollectionItemModel* model);

    bool addSubCollection(const QString &id, KPrCollectionItemModel *model);

    QListWidget *m_collectionChooser;
    QListView *m_collectionView;
    QListView *m_subTypeView;
    QList<KPrShapeAnimation *> m_animations;
    QList<KoXmlElement> m_animationContext;
    QMap<QString, KPrCollectionItemModel*> m_modelMap;
    QMap<QString, KPrCollectionItemModel*> m_subModelMap;
    KPrShapeAnimationDocker *m_docker;
    KPrShapeAnimation *m_previewAnimation;
    bool showAutomaticPreview;
};

#endif // KPRANIMATIONDATA_H
