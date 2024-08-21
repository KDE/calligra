// SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
// SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KoShapeCollectionMenu.h"

#include <KoCanvasController.h>
#include <KoCreateShapesTool.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoToolManager.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QAbstractItemModel>
#include <QButtonGroup>
#include <QDebug>
#include <QGridLayout>
#include <QIcon>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMimeData>
#include <QString>
#include <QToolButton>

class KoProperties;

inline bool operator<(const struct KoShapeTemplate &a, const struct KoShapeTemplate &b)
{
    if (a.order != b.order) {
        return a.order < b.order;
    }
    if (a.id != b.id) {
        return a.id < b.id;
    }
    return a.templateId < b.templateId;
}

/**
 * Struct containing the information stored in CollectionItemModel item
 */
struct KoCollectionItem {
    KoCollectionItem()
    {
        properties = nullptr;
    };

    QString id;
    QString name;
    QString toolTip;
    QIcon icon;
    const KoProperties *properties;
};

class CollectionItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CollectionItemModel(QObject *parent = nullptr);

    Qt::DropActions supportedDragActions() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * Set the list of KoCollectionItem to be stored in the model
     */
    void setShapeTemplateList(const QList<KoCollectionItem> &newlist);
    QList<KoCollectionItem> shapeTemplateList() const
    {
        return m_shapeTemplateList;
    }

    const KoProperties *properties(const QModelIndex &index) const;

private:
    QList<KoCollectionItem> m_shapeTemplateList;
    QString m_family;
};

CollectionItemModel::CollectionItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

Qt::DropActions CollectionItemModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QVariant CollectionItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > m_shapeTemplateList.count())
        return QVariant();

    switch (role) {
    case Qt::ToolTipRole:
        return m_shapeTemplateList[index.row()].toolTip;

    case Qt::DecorationRole:
        return m_shapeTemplateList[index.row()].icon;

    case Qt::UserRole:
        return m_shapeTemplateList[index.row()].id;

    case Qt::DisplayRole:
        return m_shapeTemplateList[index.row()].name;

    default:
        return {};
    }
}

int CollectionItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_shapeTemplateList.count();
}

void CollectionItemModel::setShapeTemplateList(const QList<KoCollectionItem> &newlist)
{
    beginResetModel();
    m_shapeTemplateList = newlist;
    endResetModel();
}

QMimeData *CollectionItemModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty())
        return nullptr;

    QModelIndex index = indexes.first();

    if (!index.isValid())
        return nullptr;

    if (m_shapeTemplateList.isEmpty())
        return nullptr;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << m_shapeTemplateList[index.row()].id;
    const KoProperties *props = m_shapeTemplateList[index.row()].properties;

    if (props)
        dataStream << props->store("shapes");
    else
        dataStream << QString();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData(SHAPETEMPLATE_MIMETYPE, itemData);

    return mimeData;
}

QStringList CollectionItemModel::mimeTypes() const
{
    QStringList mimetypes;
    mimetypes << SHAPETEMPLATE_MIMETYPE;

    return mimetypes;
}

Qt::ItemFlags CollectionItemModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled;

    return QAbstractListModel::flags(index);
}

const KoProperties *CollectionItemModel::properties(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() > m_shapeTemplateList.count())
        return nullptr;

    return m_shapeTemplateList[index.row()].properties;
}

QDebug operator<<(QDebug dbg, const KoCollectionItem &i)
{
    return dbg << "CollectionItem[" << i.id << ',' << i.name << "]";
}

class KoShapeCollectionMenu::Private
{
public:
    Private(KoShapeCollectionMenu *_q)
        : q(_q)
        , buttonGroup(new QButtonGroup(q))
    {
    }

    KoShapeCollectionMenu *q;
    QHBoxLayout *collectionChooser;
    QMap<QString, CollectionItemModel *> modelMap;
    QListView *collectionView;
    QButtonGroup *const buttonGroup;
};

KoShapeCollectionMenu::KoShapeCollectionMenu(QWidget *parent)
    : QMenu(parent)
    , d(std::make_unique<KoShapeCollectionMenu::Private>(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins({1, 1, 1, 1});

    d->collectionChooser = new QHBoxLayout;
    mainLayout->addLayout(d->collectionChooser);

    auto frame = new QFrame(this);
    frame->setFrameStyle(QFrame::HLine);
    frame->setFixedHeight(1);
    mainLayout->addWidget(frame);

    d->collectionView = new QListView(this);
    mainLayout->addWidget(d->collectionView);
    d->collectionView->setViewMode(QListView::IconMode);
    d->collectionView->setDragDropMode(QListView::DragOnly);
    d->collectionView->setSelectionMode(QListView::SingleSelection);
    d->collectionView->setResizeMode(QListView::Adjust);
    d->collectionView->setGridSize(QSize(48 + 20, 48));
    d->collectionView->setWordWrap(true);

    connect(d->collectionView, &QAbstractItemView::clicked, this, &KoShapeCollectionMenu::activateShapeCreationTool);

    // Load the default shapes and add them to the combobox
    loadDefaultShapes();
}

KoShapeCollectionMenu::~KoShapeCollectionMenu() = default;

// Returns a sizehint based on the layout and not on the number (0) of menu items
QSize KoShapeCollectionMenu::sizeHint() const
{
    return layout()->sizeHint();
}

void KoShapeCollectionMenu::activateShapeCollection(const QString &id)
{
    if (d->modelMap.contains(id)) {
        d->collectionView->setModel(d->modelMap[id]);
    } else {
        qCritical() << "Didn't find a model with id ==" << id;
    }
}

void KoShapeCollectionMenu::loadDefaultShapes()
{
    QMap<KoShapeTemplate, KoCollectionItem> defaultList;
    QMap<KoShapeTemplate, KoCollectionItem> arrowList;
    QMap<KoShapeTemplate, KoCollectionItem> funnyList;
    QMap<KoShapeTemplate, KoCollectionItem> geometricList;
    QMap<KoShapeTemplate, KoCollectionItem> quicklist;
    QMap<QString, QMap<KoShapeTemplate, KoCollectionItem>> extras;
    QMap<QString, QString> extraNames;

    int quickCount = 0;

    QStringList quickShapes;
    quickShapes << "TextShapeID"
                << "PictureShape"
                << "ChartShape_Bar"
                << "ArtisticText";
    KConfigGroup cfg = KSharedConfig::openConfig()->group("KoShapeCollection");
    quickShapes = cfg.readEntry("QuickShapes", quickShapes);

    const auto keys = KoShapeRegistry::instance()->keys();
    for (const QString &id : keys) {
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(id);
        if (!factory) {
            continue;
        }
        // don't show hidden factories
        if (factory->hidden()) {
            continue;
        }
        // don't show stencil factories for now, exclusively available by StencilBox
        if (factory->family() == QLatin1String("stencil")) {
            continue;
        }
        bool oneAdded = false;

        const auto templates = factory->templates();
        for (const KoShapeTemplate &shapeTemplate : templates) {
            oneAdded = true;
            KoCollectionItem temp;
            temp.id = shapeTemplate.id;
            temp.name = shapeTemplate.name;
            temp.toolTip = shapeTemplate.toolTip;
            temp.icon = QIcon::fromTheme(shapeTemplate.iconName);
            temp.properties = shapeTemplate.properties;
            if (shapeTemplate.family == "funny")
                funnyList[shapeTemplate] = temp;
            else if (shapeTemplate.family == "arrow")
                arrowList[shapeTemplate] = temp;
            else if (shapeTemplate.family == "geometric")
                geometricList[shapeTemplate] = temp;
            else if (factory->family() == "default" || factory->family().isEmpty()) {
                defaultList[shapeTemplate] = temp;
            } else {
                extras[factory->family()][shapeTemplate] = temp;
                extraNames[factory->family()] = factory->name();
            }

            QString id = temp.id;
            if (!shapeTemplate.templateId.isEmpty()) {
                id += '_' + shapeTemplate.templateId;
            }

            if (quickShapes.contains(id)) {
                quicklist[shapeTemplate] = temp;
                quickCount++;
            }
        }

        if (!oneAdded) {
            KoCollectionItem temp;
            KoShapeTemplate tempShape;
            temp.id = tempShape.id = factory->id();
            temp.name = tempShape.name = factory->name();
            temp.toolTip = tempShape.toolTip = factory->toolTip();
            temp.icon = QIcon::fromTheme(factory->iconName());
            tempShape.iconName = factory->iconName();
            temp.properties = tempShape.properties = nullptr;
            if (factory->family() == "funny")
                funnyList[tempShape] = temp;
            else if (factory->family() == "arrow")
                arrowList[tempShape] = temp;
            else if (factory->family() == "geometric")
                geometricList[tempShape] = temp;
            else
                defaultList[tempShape] = temp;

            if (quickShapes.contains(temp.id)) {
                quicklist[tempShape] = temp;
                quickCount++;
            }
        }
    }

    CollectionItemModel *model = new CollectionItemModel(this);
    model->setShapeTemplateList(defaultList.values());
    auto defaultCollection = addCollection("default", i18n("Default"), model);

    model = new CollectionItemModel(this);
    model->setShapeTemplateList(geometricList.values());
    addCollection("geometric", i18n("Geometrics"), model);

    QMap<QString, QMap<KoShapeTemplate, KoCollectionItem>>::const_iterator it;
    for (it = extras.constBegin(); it != extras.constEnd(); ++it) {
        model = new CollectionItemModel(this);
        model->setShapeTemplateList(it.value().values());
        addCollection(it.key(), extraNames[it.key()], model);
    }

    model = new CollectionItemModel(this);
    model->setShapeTemplateList(arrowList.values());
    addCollection("arrow", i18n("Arrows"), model);

    model = new CollectionItemModel(this);
    model->setShapeTemplateList(funnyList.values());
    addCollection("funny", i18n("Funny"), model);

    defaultCollection->toggle();
}

QToolButton *KoShapeCollectionMenu::addCollection(const QString &id, const QString &title, CollectionItemModel *model)
{
    if (d->modelMap.contains(id)) {
        return nullptr;
    }

    d->modelMap.insert(id, model);
    auto toolButton = new QToolButton(this);
    toolButton->setText(title);
    toolButton->setCheckable(true);
    toolButton->setAutoRaise(true);
    d->buttonGroup->addButton(toolButton);
    connect(toolButton, &QToolButton::toggled, this, [this, id](bool state) {
        if (state) {
            activateShapeCollection(id);
        }
    });
    d->collectionChooser->addWidget(toolButton);
    return toolButton;
}

void KoShapeCollectionMenu::activateShapeCreationTool(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();

    if (canvasController) {
        KoCreateShapesTool *tool = KoToolManager::instance()->shapeCreatorTool(canvasController->canvas());
        QString id = d->collectionView->model()->data(index, Qt::UserRole).toString();
        const KoProperties *properties = static_cast<CollectionItemModel *>(d->collectionView->model())->properties(index);

        tool->setShapeId(id);
        tool->setShapeProperties(properties);
        KoToolManager::instance()->switchToolRequested(KoCreateShapesTool_ID);
    }
    hide();
}

#include "KoShapeCollectionMenu.moc"
