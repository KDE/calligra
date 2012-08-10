#include "presetmodel.h"

#include <KoResourceServerAdapter.h>
#include <ui/kis_resource_server_provider.h>
#include <image/brushengine/kis_paintop_preset.h>

class PresetModel::Private {
public:
    Private()
    {
        rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    }

    KoResourceServer<KisPaintOpPreset> * rserver;
};

PresetModel::PresetModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TextRole] = "text";
    setRoleNames(roles);
}

PresetModel::~PresetModel()
{
    delete d;
}

int PresetModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return d->rserver->resources().count();
}

QVariant PresetModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if(index.isValid())
    {
        switch(role)
        {
        case ImageRole:
            result = "";
            break;
        case TextRole:
            result = "";
            break;
        default:
            result = "";
            break;
        }
    }
    return result;
}

QVariant PresetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

#include "presetmodel.moc"
