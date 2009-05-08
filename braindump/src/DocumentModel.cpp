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
    return static_cast<SectionGroup*>(parent.internalPointer())->sections().count();
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
    group = static_cast<SectionGroup*>(parent.internalPointer());
  } else {
    group = m_document;
  }
  if(row >= 0 and row < group->sections().count())
  {
    return createIndex(row, column, static_cast<SectionGroup*>(group->sections()[row]));
  } else {
    return QModelIndex();
  }
}

QModelIndex DocumentModel::parent( const QModelIndex& child ) const
{
  if(child.isValid())
  {
    SectionGroup* childSection = static_cast<SectionGroup*>(child.internalPointer());
    SectionGroup* childSectionParent = childSection->sectionParent();
    Q_ASSERT(childSectionParent);
    if( childSectionParent->sectionParent())
    {
      Section* parentAsSection = dynamic_cast<Section*>(childSectionParent);
      Q_ASSERT(parentAsSection);
      Q_ASSERT(childSectionParent->sectionParent()->sections().contains(parentAsSection));
      return createIndex( childSectionParent->sectionParent()->sections().indexOf(parentAsSection),
                          1, childSectionParent );
    } else {
      return QModelIndex();
    }
    
  } else {
    return QModelIndex();
  }
}
