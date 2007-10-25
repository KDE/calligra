/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptdocumentmodel.h"

#include "kptdocuments.h"
#include <kdebug.h>

class KoDocument;

namespace KPlato
{

QVariant DocumentModel::url( const Document *doc, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return doc->url().url();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}
    
QVariant DocumentModel::type( const Document *doc, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return doc->typeToString( true );
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
        default: 
            break;
    }
    return QVariant();
}

QVariant DocumentModel::status( const Document *doc, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole: {
            return doc->status();
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant DocumentModel::data( const Document *doc, int property, int role ) const
{
    QVariant result;
    switch ( property ) {
        case 0: result = url( doc, role ); break;
        case 1: result = type( doc, role ); break;
        case 2: result = status( doc, role ); break;
        default:
            //kDebug()<<"Invalid property number: "<<property<<endl;;
            return result;
    }
    return result;
}

int DocumentModel::propertyCount()
{
    return 3;
}

bool DocumentModel::setData( Document *doc, int property, const QVariant & value, int role )
{
    return false;
}

QVariant DocumentModel::headerData( int section, int role )
{
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case 0: return i18n( "Url" );
            case 1: return i18n( "Type" );
            case 2: return i18n( "Status" );
    
            default: return QVariant();
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case 0: return ToolTip::DocumentUrl;
            case 1: return ToolTip::DocumentType;
            case 2: return ToolTip::DocumentStatus;

            default: return QVariant();
        }
    }
    return QVariant();
}

//----------------------------
DocumentItemModel::DocumentItemModel( KoDocument *part, QObject *parent )
    : ItemModelBase( part, parent ),
    m_documents( 0 )
{
}

DocumentItemModel::~DocumentItemModel()
{
}
    
void DocumentItemModel::slotDocumentToBeInserted( Documents *parent, int row )
{
    if ( parent == m_documents ) {
        beginInsertRows( QModelIndex(), row, row );
    }
}

void DocumentItemModel::slotDocumentInserted( Document *doc )
{
    if ( m_documents->contains( doc ) ) {
        endInsertRows();
    }
}

void DocumentItemModel::slotDocumentToBeRemoved( Document *doc )
{
    if ( m_documents->contains( doc ) ) {
        int row = m_documents->indexOf( doc );
        beginRemoveRows( QModelIndex(), row, row );
    }
}

void DocumentItemModel::slotDocumentRemoved( Document *doc )
{
    //FIXME
    endRemoveRows();
}

void DocumentItemModel::setDocuments( Documents *docs )
{
    //kDebug()<<m_documents<<docs;
    if ( m_documents ) {
    }
    m_documents = docs;
    if ( m_documents ) {
    }
    reset();
}

Qt::ItemFlags DocumentItemModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( !index.isValid() ) {
        if ( m_readWrite ) {
            flags |= Qt::ItemIsDropEnabled;
        }
        return flags;
    }
    if ( m_readWrite ) {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        switch ( index.column() ) {
            case 0: // url
                flags |= Qt::ItemIsEditable;
                break;
            case 1: break; // type
                flags |= Qt::ItemIsEditable;
                break;
            case 2: // status
                flags &= ~Qt::ItemIsEditable;
                break;
            default: 
                flags &= ~Qt::ItemIsEditable;
        }
    }
    return flags;
}

QModelIndex DocumentItemModel::parent( const QModelIndex &/*index*/ ) const
{
    return QModelIndex();
}

bool DocumentItemModel::hasChildren( const QModelIndex &parent ) const
{
    return rowCount() > 0;
}

QModelIndex DocumentItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    if ( m_documents == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        //kDebug()<<"No index for"<<row<<","<<column;
        return QModelIndex();
    }
    if ( row >= m_documents->count() ) {
        return QModelIndex();
    }
    return createIndex(row, column );
}

QModelIndex DocumentItemModel::index( const Document *doc ) const
{
    if ( m_documents == 0 || ! doc->isValid() ) {
        return QModelIndex();
    }
    if ( ! m_documents->contains( doc ) ) {
        return QModelIndex();
    }
    return createIndex( m_documents->indexOf( doc ), 0 );
}

bool DocumentItemModel::setUrl( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( KUrl( value.toString() ) == doc->url() ) {
                return false;
            }
            //m_part->addCommand( new DocumentModifyUrlCmd( *doc, value.toString(), "Modify Document Url" ) );
            return true;
    }
    return false;
}

bool DocumentItemModel::setType( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toString() == doc->typeToString( true ) ) {
                return false;
            }
            //m_part->addCommand( new DocumentModifyTypeCmd( *doc, value.toString(), "Modify Document Type" ) );
            return true;
    }
    return false;
}


QVariant DocumentItemModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    const Document *doc = document( index );
    if ( doc ) {
        result = m_model.data( doc, index.column(), role );
    }
    if ( result.isValid() ) {
        if ( role == Qt::DisplayRole && result.type() == QVariant::String && result.toString().isEmpty()) {
            // HACK to show focus in empty cells
            result = " ";
        }
        return result;
    }
    return result;
}

bool DocumentItemModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    if ( ( flags(index) &Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    Document *doc = document( index );
    switch (index.column()) {
        case 0: return setUrl( doc, value, role );
        case 1: return setType( doc, value, role );
        default:
            qWarning("data: invalid display value column %d", index.column());
            return false;
    }
    return false;
}

QVariant DocumentItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_model.headerData( section, role );
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case 1: return Qt::AlignCenter;
                case 2: return Qt::AlignCenter;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return DocumentModel::headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QItemDelegate *DocumentItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        //case 0: return new KUrlDelegate( parent ); //???????
        case 1: return new EnumDelegate( parent );
        default: return 0;
    }
    return 0;
}

int DocumentItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return m_model.propertyCount();
}

int DocumentItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_documents == 0 || parent.isValid() ) {
        return 0;
    }
    //kDebug()<<parent<<": "<<m_documents->count();
    return m_documents->count();
}

Qt::DropActions DocumentItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList DocumentItemModel::mimeTypes() const
{
    return QStringList() << "application/x-vnd.kde.kplato.documentitemmodel.internal";
}

QMimeData *DocumentItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QList<int> rows;
    foreach (QModelIndex index, indexes) {
        m->setData("application/x-vnd.kde.kplato.documentitemmodel.internal", encodedData);
    }
    return m;
}

bool DocumentItemModel::dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data )
{
    //kDebug();
    return true;
}

bool DocumentItemModel::dropAllowed( Document *on, const QMimeData *data )
{
    if ( !data->hasFormat("application/x-vnd.kde.kplato.documentitemmodel.internal") ) {
        return false;
    }
    return true;
}

bool DocumentItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent )
{
    //kDebug()<<action;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.kplato.documentitemmodel.internal" ) ) {
        return false;
    }
    return false;
}

Document *DocumentItemModel::document( const QModelIndex &index ) const
{
    if ( m_documents == 0 ) {
        return 0;
    }
    return m_documents->value( index.row() );
}

void DocumentItemModel::slotDocumentChanged( Document *doc )
{
    if ( m_documents == 0 ) {
        return;
    }
    int row = m_documents->indexOf( doc );
    if ( row == -1 ) {
        return;
    }
    emit dataChanged( createIndex( row, 0 ), createIndex( row, columnCount() ) );
}

QModelIndex DocumentItemModel::insertDocument( Document *doc, Document *after )
{
//    m_part->addCommand( new DocumentAddCmd( doc, after, i18n( "Add Document") ) );
    int row = m_documents->indexOf( doc );
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0 );
}

} //namespace KPlato

#include "kptdocumentmodel.moc"
