/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "DocumentModel.h"

#include <QMimeData>

#include <KoShapeRenameCommand.h>

#include "RootSection.h"
#include "Section.h"
#include "commands/RenameSectionCommand.h"
#include "commands/InsertSectionCommand.h"
#include "commands/MoveSectionCommand.h"

DocumentModel::DocumentModel(QObject* parent, RootSection* document) : KoDocumentSectionModel(parent), m_document(document)
{
    Q_ASSERT(m_document);
}

DocumentModel::~DocumentModel()
{
}

int DocumentModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid()) {
        return m_document->sections().count();
    } else {
        return dataFromIndex(parent)->sections().count();
    }
}

int DocumentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex DocumentModel::index(int row, int column, const QModelIndex& parent) const
{
    SectionGroup* group;
    if(parent.isValid()) {
        group = dataFromIndex(parent);
    } else {
        group = m_document;
    }
    if(row >= 0 && row < group->sections().count() && column == 0) {
        return createIndex(row, column, dataToIndex(group->sections()[row]));
    } else {
        return QModelIndex();
    }
}

QModelIndex DocumentModel::parent(const QModelIndex& child) const
{
    if(child.isValid()) {
        Section* childSection = dataFromIndex(child);
        SectionGroup* childSectionParent = childSection->sectionParent();
        Q_ASSERT(childSectionParent);
        if(childSectionParent->sectionParent()) {
            Section* parentAsSection = dynamic_cast<Section*>(childSectionParent);
            Q_ASSERT(parentAsSection);
            Q_ASSERT(childSectionParent->sectionParent()->sections().contains(parentAsSection));
            return createIndex(childSectionParent->sectionParent()->sections().indexOf(parentAsSection),
                               0, dataToIndex(parentAsSection));
        } else {
            return QModelIndex();
        }

    } else {
        return QModelIndex();
    }
}

QVariant DocumentModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid()) {
        Section* section = dataFromIndex(index);
        switch(role) {
        case Qt::EditRole:
        case Qt::DisplayRole: {
            return section->name();
        }
        case SectionPtr:
            return qVariantFromValue(section);
        }
    }
    return QVariant();
}

bool DocumentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid()) {
        switch(role) {
        case Qt::DisplayRole:
        case Qt::EditRole: {
            Section* section = dataFromIndex(index);
            m_document->addCommand(section, new RenameSectionCommand(this, section, value.toString()));
            return true;
        }
        case ActiveRole: {
            Section* section = dataFromIndex(index);
            emit(activeSectionChanged(section));
            return true;
        }
        }
    }
    return false;
}

Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
    if(index.isValid()) {
        Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
        return flags;
    } else {
        return Qt::ItemIsDropEnabled;
    }
}

Section* DocumentModel::dataFromIndex(const QModelIndex& index) const
{
    Q_ASSERT(index.internalPointer());
    return static_cast<Section*>(index.internalPointer());
}

void* DocumentModel::dataToIndex(Section* section) const
{
    return section;
}

Qt::DropActions DocumentModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QStringList DocumentModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/x-braindumpsectionmodeldatalist");
    return types;
}

QMimeData * DocumentModel::mimeData(const QModelIndexList & indexes) const
{
    // check if there is data to encode
    if(!indexes.count())
        return 0;

    // check if we support a format
    QStringList types = mimeTypes();
    if(types.isEmpty())
        return 0;

    QMimeData *data = new QMimeData();
    QString format = types[0];
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    // encode the data
    QModelIndexList::ConstIterator it = indexes.begin();
    for(; it != indexes.end(); ++it)
        stream << qVariantFromValue(qulonglong(it->internalPointer()));

    data->setData(format, encoded);
    return data;
}

bool DocumentModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
    Q_UNUSED(column);

    // check if the action is supported
    if(!data || (action != Qt::MoveAction && action != Qt::CopyAction))
        return false;
    // check if the format is supported
    QStringList types = mimeTypes();
    Q_ASSERT(!types.isEmpty());
    QString format = types[0];
    if(!data->hasFormat(format))
        return false;

    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QList<Section*> shapes;

    // decode the data
    while(! stream.atEnd()) {
        QVariant v;
        stream >> v;
        shapes.append(static_cast<Section*>((void*)v.value<qulonglong>()));
    }

    SectionGroup* group;
    if(parent.isValid()) {
        group = dataFromIndex(parent);
    } else {
        group = m_document;

    }

    if(row > group->sections().count()) row = group->sections().count();
    foreach(Section * section, shapes) {
        if(action == Qt::CopyAction) {
            if(row < 0) {
                row = group->sections().count();
            }
            m_document->addCommand(section, new InsertSectionCommand(m_document->sectionsIO(), new Section(*section), group, this, row));
        } else {
            int idx = group->indexOf(section);
            if(0 <= idx && idx < row) {
                --row;
            }
            if(row < 0) {
                row = group->sections().count();
                if(group == section->sectionParent()) {
                    --row;
                }
            }
            m_document->addCommand(section, new MoveSectionCommand(section, group, this, row));
        }
    }
    return true;
}

void DocumentModel::removeSection(Section* section)
{
    SectionGroup* parentGrp = section->sectionParent();
    Q_ASSERT(parentGrp);

    QModelIndex parentIndex = parent(index(section));
    int idx = parentGrp->sections().indexOf(section);
    beginRemoveRows(parentIndex, idx, idx);
    parentGrp->removeSection(section);
    endRemoveRows();
}

void DocumentModel::insertSection(Section* section, SectionGroup* parentGrp, Section* before)
{
    Q_ASSERT(before == 0 || parentGrp == before->sectionParent());
    int idx = (before) ? parentGrp->sections().indexOf(before) : parentGrp->sections().count();
    insertSection(section, parentGrp, idx);
}

void DocumentModel::insertSection(Section* section, SectionGroup* parentGrp, int idx)
{
    QModelIndex parentIndex = index(parentGrp);
    Q_ASSERT(idx >= 0 && idx <= parentGrp->sections().count());
    beginInsertRows(parentIndex, idx, idx);
    parentGrp->insertSection(section, idx);
    Q_ASSERT(section->sectionParent() == parentGrp);
    Q_ASSERT((!(parentIndex.isValid()) && section->sectionParent() == m_document)
             || section->sectionParent() == dataFromIndex(parentIndex));
    endInsertRows();
}

QModelIndex DocumentModel::index(Section* section)
{
    SectionGroup* group = section->sectionParent();
    Q_ASSERT(group);
    return createIndex(group->sections().indexOf(section), 0, dataToIndex(section));
}

QModelIndex DocumentModel::index(SectionGroup* section)
{
    Section* sec = dynamic_cast<Section*>(section);
    if(sec) {
        return index(sec);
    } else {
        return QModelIndex();
    }
}

void DocumentModel::changeSectionName(Section* _section, const QString& _name)
{
    _section->setName(_name);
    QModelIndex idx = index(_section);
    emit(dataChanged(idx, idx));
}
