#ifndef KPRANIMATIONSDATAMODEL_H
#define KPRANIMATIONSDATAMODEL_H

#include "animations/KPrShapeAnimation.h"

#include <QAbstractTableModel>
#include <QPixmap>
#include <QIcon>

#include "stage_export.h"
/*Data Model for Animations Time Line View
It requires an activePage to be set*/

class KPrPage;
class QImage;
class KoShape;
class KPrAnimationStep;
class KoPADocument;
class KPrView;

class STAGE_TEST_EXPORT KPrAnimationsDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    struct AnimationsData {   // Declare struct type
        QString name;
        int order;
        QPixmap thumbnail;
        QPixmap animationIcon;
        QString animationName;
        KPrShapeAnimation::Preset_Class type;
        KPrShapeAnimation::Node_Type triggerEvent;
        qreal startTime;
        qreal duration;
        KoShape* shape;

    };   // Define object

    explicit KPrAnimationsDataModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setActivePage(KPrPage *activePage);
    void setDocumentView(KPrView *view);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex indexByShape(KoShape* shape);

signals:
    void dataReinitialized();

public slots:
    /// Triggers an update of the complete model
    void update();

private:
    QImage createThumbnail(KoShape* shape, const QSize &thumbSize) const;
    void setNameAndAnimationIcon(AnimationsData &data, QString id);

    QList<AnimationsData> m_data;
    QList<KPrAnimationStep*> m_steps;
    KPrPage *m_activePage;
    KPrView *m_view;
};

#endif // KPRANIMATIONSDATAMODEL_H
