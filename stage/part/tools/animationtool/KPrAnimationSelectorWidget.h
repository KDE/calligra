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
