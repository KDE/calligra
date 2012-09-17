/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Dan Leinir Turthra Jensen <admin@leinir.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "CompositeOpModel.h"
#include <kis_composite_ops_model.h>
#include <kis_view2.h>
#include <kis_canvas_resource_provider.h>
#include <kis_node.h>
#include <KoCompositeOpRegistry.h>
#include <KoColorSpace.h>

class CompositeOpModel::Private
{
public:
    Private()
        : model(KisCompositeOpListModel::sharedInstance())
        , view(0)
    {};

    KisCompositeOpListModel* model;
    KisView2* view;
    QString currentCompositeOpID;
    QString prevCompositeOpID;

    void updateCompositeOp(QString compositeOpID)
    {
        if(!view)
            return;

        KisNodeSP node = view->resourceProvider()->currentNode();

        if(node && node->paintDevice()) {
            if(!node->paintDevice()->colorSpace()->hasCompositeOp(compositeOpID))
                compositeOpID = KoCompositeOpRegistry::instance().getDefaultCompositeOp().id();
            
            // erase mode button logic here...
            //m_eraseModeButton->blockSignals(true);
            //m_eraseModeButton->setChecked(compositeOpID == COMPOSITE_ERASE);
            //m_eraseModeButton->blockSignals(false);
            
            if(compositeOpID != currentCompositeOpID) {
                //m_activePreset->settings()->setProperty("CompositeOp", compositeOpID);
                //m_optionWidget->setConfiguration(m_activePreset->settings().data());
                view->resourceProvider()->setCurrentCompositeOp(compositeOpID);
                prevCompositeOpID = currentCompositeOpID;
                currentCompositeOpID = compositeOpID;
            }
        }
    }
};

CompositeOpModel::CompositeOpModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int,QByteArray> roles;
    roles[TextRole] = "text";
    roles[IsCategoryRole] = "isCategory";
    setRoleNames(roles);
}

CompositeOpModel::~CompositeOpModel()
{
    delete d;
}

QVariant CompositeOpModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if(index.isValid())
    {
        QModelIndex otherIndex = d->model->index(index.row(), index.column(), QModelIndex());
        switch(role)
        {
            case TextRole:
                data = d->model->data(otherIndex, Qt::DisplayRole);
                break;
            case IsCategoryRole:
                data = d->model->data(otherIndex, IsHeaderRole);
                break;
            default:
                break;
        }
    }
    return data;
}

int CompositeOpModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->model->rowCount(QModelIndex());
}

void CompositeOpModel::activateItem(int index)
{
    if(index > -1 && index < d->model->rowCount(QModelIndex()))
    {
        KoID compositeOp;

        if(d->model->entryAt(compositeOp, index))
            d->updateCompositeOp(compositeOp.id());
    }
}

QObject* CompositeOpModel::view() const
{
    return d->view;
}

void CompositeOpModel::setView(QObject* newView)
{
    d->view = qobject_cast<KisView2*>( newView );
    emit viewChanged();
}

#include "CompositeOpModel.moc"
