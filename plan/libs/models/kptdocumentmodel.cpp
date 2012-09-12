/* This file is part of the KDE project
   Copyright (C) 2007, 2012 Dag Andersen <danders@get2net.dk>

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
#include "kptglobal.h"
#include "kptcommonstrings.h"
#include "kptdebug.h"

#include <QMimeData>


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

QVariant DocumentModel::name( const Document *doc, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return doc->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

bool DocumentModel::setName( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            doc->setName( value.toString() );
            return true;
        default:
            break;
    }
    return false;
}

QVariant DocumentModel::type( const Document *doc, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return Document::typeToString( doc->type(), true );
        case Role::EnumList:
            return Document::typeList( true );
        case Qt::EditRole:
        case Role::EnumListValue:
            return (int)doc->type();
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

bool DocumentModel::setType( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            doc->setType( static_cast<Document::Type>( value.toInt() ) );
            return true;
        default:
            break;
    }
    return false;
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

QVariant DocumentModel::sendAs( const Document *doc, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return Document::sendAsToString( doc->sendAs(), true );
        case Role::EnumList:
            return Document::sendAsList( true );
        case Qt::EditRole:
        case Role::EnumListValue:
            return (int)doc->sendAs();
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

bool DocumentModel::setSendAs( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            doc->setSendAs( static_cast<Document::SendAs>( value.toInt() ) );
            return true;
        default:
            break;
    }
    return false;
}

QVariant DocumentModel::data( const Document *doc, int property, int role ) const
{
    QVariant result;
    switch ( property ) {
        case Property_Url: result = url( doc, role ); break;
        case Property_Name: result = name( doc, role ); break;
        case Property_Type: result = type( doc, role ); break;
        case Property_SendAs: result = sendAs( doc, role ); break;
        case Property_Status: result = status( doc, role ); break;
        default:
            //kDebug(planDbg())<<"Invalid property number: "<<property;
            return result;
    }
    return result;
}

int DocumentModel::propertyCount()
{
    return 5;
}

bool DocumentModel::setData( Document *doc, int property, const QVariant & /*value*/, int role )
{
    Q_UNUSED(doc);
    Q_UNUSED(property);
    Q_UNUSED(role);
    switch ( property ) {
        //case 0: result = url( doc, role ); break;
        //case 1: return setType( doc, value, role );
        //case 2: result = status( doc, role ); break;
        default:
            //kDebug(planDbg())<<"Invalid property number: "<<property;
            break;
    }
    return false;
}

QVariant DocumentModel::headerData( int section, int role )
{
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case Property_Url: return i18n( "Url" );
            case Property_Name: return i18n( "Name" );
            case Property_Type: return i18n( "Type" );
            case Property_SendAs: return i18n( "Send As" );
            case Property_Status: return i18n( "Status" );

            default: return QVariant();
        }
    }
    if ( role == Qt::ToolTipRole ) {
        switch ( section ) {
            case Property_Url: return ToolTip::documentUrl();
            case Property_Name: return QVariant(); //TODO
            case Property_Type: return ToolTip::documentType();
            case Property_SendAs: return ToolTip::documentSendAs();
            case Property_Status: return ToolTip::documentStatus();

            default: return QVariant();
        }
    }
    return QVariant();
}

//----------------------------
DocumentItemModel::DocumentItemModel( QObject *parent )
    : ItemModelBase( parent ),
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
    Q_UNUSED(doc);
    //FIXME
    endRemoveRows();
}

void DocumentItemModel::setDocuments( Documents *docs )
{
    //kDebug(planDbg())<<m_documents<<docs;
    if ( m_documents ) {
    }
    m_documents = docs;
    if ( m_documents ) {
    }
    reset();
}

Documents *DocumentItemModel::documents() const
{
    return m_documents;
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
    //kDebug(planDbg())<<index<<m_readWrite;
    if ( m_readWrite ) {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        switch ( index.column() ) {
            case DocumentModel::Property_Url: // url
                flags &= ~Qt::ItemIsEditable; // wee need a full path
                break;
            case DocumentModel::Property_Name: // name
                flags |= Qt::ItemIsEditable;
                break;
            case DocumentModel::Property_Type: // type
                flags |= Qt::ItemIsEditable;
                break;
            case DocumentModel::Property_SendAs: // sendAs
                flags |= Qt::ItemIsEditable;
                break;
            case DocumentModel::Property_Status: // status
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

QModelIndex DocumentItemModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    if ( m_documents == 0 || column < 0 || column >= columnCount() || row < 0 ) {
        //kDebug(planDbg())<<"No index for"<<row<<","<<column;
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

bool DocumentItemModel::setName( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            //m_part->addCommand( new DocumentModifyTypeCmd( *doc, value.toString(), "Modify Document Type" ) );
            return m_model.setName( doc, value, role );
    }
    return false;
}

bool DocumentItemModel::setType( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toInt() == doc->type() ) {
                return false;
            }
            m_model.setType( doc, value, role );
            //m_part->addCommand( new DocumentModifyTypeCmd( *doc, value.toString(), "Modify Document Type" ) );
            return true;
    }
    return false;
}

bool DocumentItemModel::setSendAs( Document *doc, const QVariant &value, int role )
{
    switch ( role ) {
        case Qt::EditRole:
            if ( value.toInt() == doc->sendAs() ) {
                return false;
            }
            m_model.setSendAs( doc, value, role );
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
    if ( ! index.isValid() ) {
        return ItemModelBase::setData( index, value, role );
    }
    if ( ( flags(index) & Qt::ItemIsEditable ) == 0 || role != Qt::EditRole ) {
        return false;
    }
    bool result = false;
    Document *doc = document( index );
    switch (index.column()) {
        case DocumentModel::Property_Url:
            result = setUrl( doc, value, role );
            break;
        case DocumentModel::Property_Name:
            result = setName( doc, value, role );
            break;
        case DocumentModel::Property_Type:
            result = setType( doc, value, role );
            break;
        case DocumentModel::Property_SendAs:
            result = setSendAs( doc, value, role );
            break;
        default:
            qWarning("data: invalid display value column %d", index.column());
            break;
    }
    if ( result ) {
        emit dataChanged( index, index );
    }
    return result;
}

QVariant DocumentItemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return m_model.headerData( section, role );
        } else if ( role == Qt::TextAlignmentRole ) {
            switch (section) {
                case DocumentModel::Property_Type: return Qt::AlignCenter;
                case DocumentModel::Property_SendAs: return Qt::AlignCenter;
                default: return QVariant();
            }
        }
    }
    if ( role == Qt::ToolTipRole ) {
        return DocumentModel::headerData( section, role );
    }
    return ItemModelBase::headerData(section, orientation, role);
}

QAbstractItemDelegate *DocumentItemModel::createDelegate( int column, QWidget *parent ) const
{
    switch ( column ) {
        //case 0: return new KUrlDelegate( parent ); //???????
        case DocumentModel::Property_Type: { kDebug(planDbg())<< column; return new EnumDelegate( parent ); }
        case DocumentModel::Property_SendAs: { kDebug(planDbg())<< column; return new EnumDelegate( parent ); }
        default: break;
    }
    return 0;
}

int DocumentItemModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    //kDebug(planDbg())<<m_model.propertyCount();
    return m_model.propertyCount();
}

int DocumentItemModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_documents == 0 || parent.isValid() ) {
        //kDebug(planDbg())<<parent;
        return 0;
    }
    //kDebug(planDbg())<<parent<<": "<<m_documents->count();
    return m_documents->count();
}

Qt::DropActions DocumentItemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


QStringList DocumentItemModel::mimeTypes() const
{
    return QStringList() << "application/x-vnd.kde.plan.documentitemmodel.internal";
}

QMimeData *DocumentItemModel::mimeData( const QModelIndexList & indexes ) const
{
    QMimeData *m = new QMimeData();
    QByteArray encodedData;
    //QDataStream stream(&encodedData, QIODevice::WriteOnly);
    //QList<int> rows;
    foreach (const QModelIndex &index, indexes) {
        Q_UNUSED(index);
        m->setData("application/x-vnd.kde.plan.documentitemmodel.internal", encodedData);
    }
    return m;
}

bool DocumentItemModel::dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data )
{
    Q_UNUSED(index);
    Q_UNUSED(dropIndicatorPosition);
    Q_UNUSED(data);
    //kDebug(planDbg());
    return true;
}

bool DocumentItemModel::dropAllowed( Document *on, const QMimeData *data )
{
    Q_UNUSED(on)
    if ( !data->hasFormat("application/x-vnd.kde.plan.documentitemmodel.internal") ) {
        return false;
    }
    return true;
}

bool DocumentItemModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent )
{
    Q_UNUSED(row);
    Q_UNUSED(parent);
    //kDebug(planDbg())<<action;
    if (action == Qt::IgnoreAction) {
        return true;
    }
    if ( !data->hasFormat( "application/x-vnd.kde.plan.documentitemmodel.internal" ) ) {
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
    emit dataChanged( createIndex( row, 0 ), createIndex( row, columnCount() - 1 ) );
}

QModelIndex DocumentItemModel::insertDocument( Document *doc, Document *after )
{
    Q_UNUSED(after);
//    m_part->addCommand( new DocumentAddCmd( doc, after, i18nc( "(qtundo-format)", "Add Document") ) );
    int row = m_documents->indexOf( doc );
    if ( row == -1 ) {
        return QModelIndex();
    }
    return createIndex( row, 0 );
}

} //namespace KPlato

#include "kptdocumentmodel.moc"
