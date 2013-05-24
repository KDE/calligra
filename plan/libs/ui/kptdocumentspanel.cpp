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

#include "kptdocumentspanel.h"

#include "kptdocumentseditor.h"
#include "kptdocumentmodel.h"
#include "kptnode.h"
#include "kptcommand.h"
#include "kptdebug.h"

#include <QDialog>
#include <QString>
#include <QModelIndex>
#include <QVBoxLayout>

#include <klocale.h>
#include <kurlrequesterdialog.h>
#include <kmessagebox.h>


namespace KPlato
{

DocumentsPanel::DocumentsPanel( Node &node, QWidget *parent )
    : QWidget( parent ),
    m_node( node ),
    m_docs( node.documents() )
{
    widget.setupUi( this );
    QVBoxLayout *l = new QVBoxLayout( widget.itemViewHolder );
    m_view = new DocumentTreeView( widget.itemViewHolder );
    l->setMargin(0);
    l->addWidget( m_view );
    m_view->setDocuments( &m_docs );
    m_view->setReadWrite( true );
    
    currentChanged( QModelIndex() );
    
    foreach ( Document *doc, m_docs.documents() ) {
        m_orgurl.insert( doc, doc->url() );
    }

    connect( widget.pbAdd, SIGNAL( clicked() ), SLOT( slotAddUrl() ) );
    connect( widget.pbChange, SIGNAL( clicked() ), SLOT( slotChangeUrl() ) );
    connect( widget.pbRemove, SIGNAL( clicked() ), SLOT( slotRemoveUrl() ) );
    connect( widget.pbView, SIGNAL( clicked() ), SLOT( slotViewUrl() ) );
    
    connect( m_view->model(), SIGNAL( dataChanged ( const QModelIndex&, const QModelIndex& ) ), SLOT( dataChanged( const QModelIndex& ) ) );
    
    connect( m_view, SIGNAL( selectionChanged( const QModelIndexList& ) ), SLOT( slotSelectionChanged( const QModelIndexList& ) ) );
}

DocumentItemModel *DocumentsPanel::model() const
{
    return m_view->model();
}

void DocumentsPanel::dataChanged( const QModelIndex &index )
{
    Document *doc = m_docs.value( index.row() );
    if ( doc == 0 ) {
        return;
    }
    m_state.insert( doc, (State)( m_state[ doc ] | Modified ) );
    emit changed();
    kDebug(planDbg())<<index<<doc<<m_state[ doc ];
}

void DocumentsPanel::slotSelectionChanged( const QModelIndexList & )
{
    QModelIndexList list = m_view->selectedRows();
    kDebug(planDbg())<<list;
    widget.pbChange->setEnabled( list.count() == 1 );
    widget.pbRemove->setEnabled( ! list.isEmpty() );
    widget.pbView->setEnabled( false ); //TODO
}

void DocumentsPanel::currentChanged( const QModelIndex &index )
{
    widget.pbChange->setEnabled( index.isValid() );
    widget.pbRemove->setEnabled( index.isValid() );
    widget.pbView->setEnabled( false ); //TODO
}

Document *DocumentsPanel::selectedDocument() const
{
    QList<Document*> lst = m_view->selectedDocuments();
    return lst.isEmpty() ? 0 : lst.first();
}

void DocumentsPanel::slotAddUrl()
{
    QPointer<KUrlRequesterDialog> dlg = new KUrlRequesterDialog( QString(), QString(), this );
    dlg->setWindowTitle( i18nc( "@title:window", "Attach Document" ) );
    if ( dlg->exec() == QDialog::Accepted && dlg ) {
        if ( m_docs.findDocument( dlg->selectedUrl() ) ) {
            kWarning()<<"Document (url) already exists: "<<dlg->selectedUrl();
            KMessageBox::sorry( this, i18nc( "@info", "Document is already attached:<br/><filename>%1</filename>", dlg->selectedUrl().prettyUrl() ), i18nc( "@title:window", "Cannot Attach Document" ) );
        } else {
            Document *doc = new Document( dlg->selectedUrl() );
            //DocumentAddCmd *cmd = new DocumentAddCmd( m_docs, doc, i18nc( "(qtundo-format)", "Add document" ) );
            //m_cmds.push( cmd );
            m_docs.addDocument( doc );
            m_state.insert( doc, Added );
            model()->setDocuments( &m_docs ); // refresh
            emit changed();
        }
    }
    delete dlg;
}

void DocumentsPanel::slotChangeUrl()
{
    Document *doc = selectedDocument();
    if ( doc == 0 ) {
        return slotAddUrl();
    }
    KUrlRequesterDialog *dlg = new KUrlRequesterDialog( doc->url().url(), QString(), this );
    dlg->setWindowTitle( i18nc( "@title:window", "Modify Url" ) );
    if ( dlg->exec() == QDialog::Accepted ) {
        if ( doc->url() != dlg->selectedUrl() ) {
            if ( m_docs.findDocument( dlg->selectedUrl() ) ) {
                kWarning()<<"Document url already exists";
                KMessageBox::sorry( this, i18n( "Document url already exists: %1", dlg->selectedUrl().prettyUrl() ), i18n( "Cannot Modify Url" ) );
            } else {
                kDebug(planDbg())<<"Modify url: "<<doc->url()<<" : "<<dlg->selectedUrl();
                doc->setUrl( dlg->selectedUrl() );
                m_state.insert( doc, (State)( m_state[ doc ] | Modified ) );
                model()->setDocuments( &m_docs );
                emit changed();
                kDebug(planDbg())<<"State: "<<doc->url()<<" : "<<m_state[ doc ];
            }
        }
    }
    delete dlg;
}

void DocumentsPanel::slotRemoveUrl()
{
    QList<Document*> lst = m_view->selectedDocuments();
    bool mod = false;
    foreach ( Document *doc, lst ) {
        if ( doc == 0 ) {
            continue;
        }
        m_docs.takeDocument( doc );
        if ( m_state.contains( doc ) && m_state[ doc ] & Added ) {
            m_state.remove( doc );
        } else {
            m_state.insert( doc, Removed );
        }
        mod = true;
    }
    if ( mod ) {
        model()->setDocuments( &m_docs ); // refresh
        emit changed();
    }
}

void DocumentsPanel::slotViewUrl()
{
}

MacroCommand *DocumentsPanel::buildCommand()
{
    if ( m_docs == m_node.documents() ) {
        kDebug(planDbg())<<"No changes to save";
        return 0;
    }
    Documents &docs = m_node.documents();
    Document *d = 0;
    QString txt = i18n( "Modify documents" );
    MacroCommand *m = 0;
    QMap<Document*, State>::const_iterator i = m_state.constBegin();
    for ( ; i != m_state.constEnd(); ++i) {
        kDebug(planDbg())<<i.key()<<i.value();
        if ( i.value() & Removed ) {
            d = docs.findDocument( m_orgurl[ i.key() ] );
            Q_ASSERT( d );
            if ( m == 0 ) m = new MacroCommand( txt );
            kDebug(planDbg())<<"remove document "<<i.key();
            m->addCommand( new DocumentRemoveCmd( m_node.documents(), d, i18nc( "(qtundo-format)", "Remove document" ) ) );
        } else if ( ( i.value() & Added ) == 0 && i.value() & Modified ) {
            d = docs.findDocument( m_orgurl[ i.key() ] );
            Q_ASSERT( d );
            // do plain modifications before additions
            kDebug(planDbg())<<"modify document "<<d;
            if ( i.key()->url() != d->url() ) {
                if ( m == 0 ) m = new MacroCommand( txt );
                m->addCommand( new DocumentModifyUrlCmd( d, i.key()->url(), i18nc( "(qtundo-format)", "Modify document url" ) ) );
            }
            if ( i.key()->type() != d->type() ) {
                if ( m == 0 ) m = new MacroCommand( txt );
                m->addCommand( new DocumentModifyTypeCmd( d, i.key()->type(), i18nc( "(qtundo-format)", "Modify document type" ) ) );
            }
            if ( i.key()->status() != d->status() ) {
                if ( m == 0 ) m = new MacroCommand( txt );
                m->addCommand( new DocumentModifyStatusCmd( d, i.key()->status(), i18nc( "(qtundo-format)", "Modify document status" ) ) );
            }
            if ( i.key()->sendAs() != d->sendAs() ) {
                if ( m == 0 ) m = new MacroCommand( txt );
                m->addCommand( new DocumentModifySendAsCmd( d, i.key()->sendAs(), i18nc( "(qtundo-format)", "Modify document send control" ) ) );
            }
            if ( i.key()->name() != d->name() ) {
                if ( m == 0 ) m = new MacroCommand( txt );
                m->addCommand( new DocumentModifyNameCmd( d, i.key()->name()/*, i18nc( "(qtundo-format)", "Modify document name" )*/ ) );
            }
        } else if ( i.value() & Added ) {
            if ( m == 0 ) m = new MacroCommand( txt );
            kDebug(planDbg())<<i.key()<<m_docs.documents();
            d = m_docs.takeDocument( i.key() );
            kDebug(planDbg())<<"add document "<<d;
            m->addCommand( new DocumentAddCmd( docs, d, i18nc( "(qtundo-format)", "Add document" ) ) );
        }
    }
    return m;
}


} //namespace KPlato

#include "kptdocumentspanel.moc"
