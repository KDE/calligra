/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009,2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "documentlistmodel.h"

// lib
#include "documentstool.h"
// Kasten core
#include <abstractmodelsynchronizer.h>
#include <abstractdocument.h>
// KDE
#include <KLocale>
#include <KIcon>
#include <unistd.h>


namespace Kasten2
{

DocumentListModel::DocumentListModel( DocumentsTool* documentsTool, QObject* parent )
 : QAbstractTableModel( parent ),
   mDocumentsTool( documentsTool )
{
    connect( mDocumentsTool, SIGNAL(documentsAdded(QList<Kasten2::AbstractDocument*>)),
             SLOT(onDocumentsAdded(QList<Kasten2::AbstractDocument*>)) );
    connect( mDocumentsTool, SIGNAL(documentsClosing(QList<Kasten2::AbstractDocument*>)),
             SLOT(onDocumentsClosing(QList<Kasten2::AbstractDocument*>)) );
    connect( mDocumentsTool, SIGNAL(focussedDocumentChanged(Kasten2::AbstractDocument*)),
             SLOT(onFocussedDocumentChanged(Kasten2::AbstractDocument*)) );
}

int DocumentListModel::rowCount( const QModelIndex& parent ) const
{
    return (! parent.isValid()) ? mDocumentsTool->documents().size() : 0;
}

int DocumentListModel::columnCount( const QModelIndex& parent ) const
{
    return (! parent.isValid()) ? NoOfColumnIds : 0;
}

QVariant DocumentListModel::data( const QModelIndex& index, int role ) const
{
    QVariant result;

    if( role == Qt::DisplayRole )
    {
        const int documentIndex = index.row();
        const AbstractDocument* document = mDocumentsTool->documents().at( documentIndex );

        const int tableColumn = index.column();
        switch( tableColumn )
        {
            case TitleColumnId:
                result = document->title();
                break;
            default:
                ;
        }
    }
    else if( role == Qt::DecorationRole )
    {
        const int documentIndex = index.row();
        const AbstractDocument* document = mDocumentsTool->documents().at( documentIndex );
        const AbstractModelSynchronizer* synchronizer = document ? document->synchronizer() : 0;

        const int tableColumn = index.column();
        switch( tableColumn )
        {
            case CurrentColumnId:
                if( document == mDocumentsTool->focussedDocument() )
                    result = KIcon( QLatin1String("arrow-right") );
                break;
            case LocalStateColumnId:
                if( synchronizer && synchronizer->localSyncState() == LocalHasChanges )
                    result = KIcon( QLatin1String("document-save") );
                break;
            case RemoteStateColumnId:
                // TODO: use static map, syncState int -> iconname
                if( ! synchronizer )
                    result = KIcon( QLatin1String("document-new") );
                else
                {
                    const RemoteSyncState remoteSyncState = synchronizer->remoteSyncState();
                    if( remoteSyncState == RemoteHasChanges )
                        result = KIcon( QLatin1String("document-save") );
                    else if( remoteSyncState == RemoteDeleted )
                        result = KIcon( QLatin1String("edit-delete") );
                    else if( remoteSyncState == RemoteUnknown )
                        result = KIcon( QLatin1String("flag-yellow") );
                    else if( remoteSyncState == RemoteUnreachable )
                        result = KIcon( QLatin1String("network-disconnect") );
                }
                break;
            default:
                ;
        }
    }

    return result;
}

QVariant DocumentListModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    QVariant result;

    if( role == Qt::DisplayRole )
    {
        const QString titel =
//             section == LocalStateColumnId ?  i18nc("@title:column Id of the version",         "Id") :
            section == TitleColumnId ?     i18nc("@title:column description of the change", "Title") :
            QString();
        result = titel;
    }
    else if( role == Qt::ToolTipRole )
    {
        const QString titel =
//             section == LocalStateColumnId ?                i18nc("@info:tooltip","Id of the version") :
            section == TitleColumnId ? i18nc("@info:tooltip","Title of the document") :
            QString();
        result = titel;
    }
    else
        result = QAbstractTableModel::headerData( section, orientation, role );

    return result;
}

void DocumentListModel::onFocussedDocumentChanged( AbstractDocument* document )
{
Q_UNUSED( document )

    reset();
// TODO: store current focused document, then only emit for changed
#if 0
    const int oldVersionIndex = mVersionIndex;
    mVersionIndex = versionIndex;

    emit dataChanged( index(versionIndex,CurrentColumnId), index(versionIndex,CurrentColumnId) );
    emit dataChanged( index(oldVersionIndex,CurrentColumnId), index(oldVersionIndex,CurrentColumnId) );
#endif
}

void DocumentListModel::onDocumentsAdded( const QList<Kasten2::AbstractDocument*>& documents )
{
    foreach( AbstractDocument* document, documents )
    {
        connect( document, SIGNAL(synchronizerChanged(Kasten2::AbstractModelSynchronizer*)),
                 SLOT(onSynchronizerChanged(Kasten2::AbstractModelSynchronizer*)) );
        AbstractModelSynchronizer* synchronizer = document->synchronizer();
        if( synchronizer )
        {
            connect( synchronizer, SIGNAL(localSyncStateChanged(Kasten2::LocalSyncState)),
                    SLOT(onSyncStatesChanged()) );
            connect( synchronizer, SIGNAL(remoteSyncStateChanged(Kasten2::RemoteSyncState)),
                    SLOT(onSyncStatesChanged()) );
        }
    }
    // TODO: try to understand how this whould be done with {begin,end}{Insert,Remove}Columns
    reset();
}

void DocumentListModel::onDocumentsClosing( const QList<Kasten2::AbstractDocument*>& documents )
{
Q_UNUSED( documents )
    // TODO: try to understand how this whould be done with {begin,end}{Insert,Remove}Columns
    reset();
}


void DocumentListModel::onSynchronizerChanged( AbstractModelSynchronizer* synchronizer )
{
    // TODO: what about the old synchronizer? assume it is deleted and that way disconnects?
    if( synchronizer )
    {
        connect( synchronizer, SIGNAL(localSyncStateChanged(Kasten2::LocalSyncState)),
                 SLOT(onSyncStatesChanged()) );
        connect( synchronizer, SIGNAL(remoteSyncStateChanged(Kasten2::RemoteSyncState)),
                 SLOT(onSyncStatesChanged()) );
    }
    // TODO: try to understand how this whould be done with {begin,end}{Insert,Remove}Columns
    reset();
}

void DocumentListModel::onSyncStatesChanged()
{
    // TODO: try to understand how this whould be done with {begin,end}{Insert,Remove}Columns
    reset();
}


DocumentListModel::~DocumentListModel() {}

}
