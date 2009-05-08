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

#include <KoShapeRenameCommand.h>

#include "Document.h"
#include "Section.h"

DocumentModel::DocumentModel( QObject* parent, Document *document ) : KoDocumentSectionModel(parent), m_document(document)
{
  Q_ASSERT(m_document);
}

DocumentModel::~DocumentModel()
{
}

int DocumentModel::rowCount(const QModelIndex &parent) const
{
  if(not parent.isValid())
  {
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
  if(parent.isValid())
  {
    group = dataFromIndex(parent);
  } else {
    group = m_document;
  }
  if(row >= 0 and row < group->sections().count())
  {
    return createIndex(row, column, dataToIndex(group->sections()[row]));
  } else {
    return QModelIndex();
  }
}

QModelIndex DocumentModel::parent( const QModelIndex& child ) const
{
  if(child.isValid())
  {
    Section* childSection = dataFromIndex(child);
    SectionGroup* childSectionParent = childSection->sectionParent();
    Q_ASSERT(childSectionParent);
    if( childSectionParent->sectionParent())
    {
      Section* parentAsSection = dynamic_cast<Section*>(childSectionParent);
      Q_ASSERT(parentAsSection);
      Q_ASSERT(childSectionParent->sectionParent()->sections().contains(parentAsSection));
      return createIndex( childSectionParent->sectionParent()->sections().indexOf(parentAsSection),
                          1, dataToIndex(parentAsSection) );
    } else {
      return QModelIndex();
    }
    
  } else {
    return QModelIndex();
  }
}

QVariant DocumentModel::data(const QModelIndex &index, int role ) const
{
  if( index.isValid() )
  {
    Section* section = dataFromIndex(index);
    switch (role)
    {
      case Qt::EditRole:
      case Qt::DisplayRole:
      {
        return section->name();
      }
    }
  }
  return QVariant();
}

bool DocumentModel::setData(const QModelIndex &index, const QVariant &value, int role )
{
  if( index.isValid() )
  {
    switch (role)
    {
      case Qt::DisplayRole:
      case Qt::EditRole:
      {
        QUndoCommand * cmd = new KoShapeRenameCommand( dataFromIndex(index), value.toString() );
        // TODO 2.1 use different text for the command if e.g. it is a page/slide or layer
        m_document->addCommand( cmd );
        return true;
      }
    }
  }
  return false;
}

Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
  if( index.isValid() )
  {
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable| Qt::ItemIsDropEnabled;
    return flags;
  } else {
    return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
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

QMimeData * DocumentModel::mimeData( const QModelIndexList & indexes ) const
{
  // check if there is data to encode
  if( !indexes.count() )
    return 0;

  // check if we support a format
  QStringList types = mimeTypes();
  if( types.isEmpty() )
    return 0;

  QMimeData *data = new QMimeData();
  QString format = types[0];
  QByteArray encoded;
  QDataStream stream(&encoded, QIODevice::WriteOnly);

  // encode the data
  QModelIndexList::ConstIterator it = indexes.begin();
  for( ; it != indexes.end(); ++it)
    stream << qVariantFromValue( qulonglong( it->internalPointer() ) );

  data->setData(format, encoded);
  return data;
}
