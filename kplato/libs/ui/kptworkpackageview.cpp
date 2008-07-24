/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen kplato@kde.org>

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

#include "kptworkpackageview.h"
#include <kptworkpackagemodel.h>

#include "kptitemmodelbase.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptschedule.h"

#include <KoDocument.h>


#include <QList>
#include <QObject>
#include <QVBoxLayout>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextTable>

#include <kaction.h>
#include <kicon.h>
#include <kglobal.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <ktextbrowser.h>

#include <kdebug.h>

namespace KPlato
{


WorkPackageTableView::WorkPackageTableView( QWidget *parent )
    : QTableView( parent )
{
//    header()->setContextMenuPolicy( Qt::CustomContextMenu );
//    setStretchLastSection( false );
    
    setModel( new WorkPackageModel() );
    
//    setSelectionMode( QAbstractItemView::ExtendedSelection );

}

Resource *WorkPackageTableView::currentResource() const
{
    return 0;//itemModel()->object( selectionModel()->currentIndex() );
}

QList<Resource*> WorkPackageTableView::selectedResources() const
{
    QList<Resource*> lst;
/*    foreach (QModelIndex i, selectionModel()->selectedRows() ) {
        lst << static_cast<QObject*>( i.internalPointer() );
    }*/
    return lst;
}

WorkPackageModel *WorkPackageTableView::itemModel() const
{
    return static_cast<WorkPackageModel*>( model() );
}

Project *WorkPackageTableView::project() const
{
    return itemModel()->project();
}

void WorkPackageTableView::setProject( Project *project )
{
    itemModel()->setProject( project );
}

Task *WorkPackageTableView::task() const
{
    return itemModel()->task();
}

void WorkPackageTableView::setTask( Task *task )
{
    itemModel()->setTask( task );
}

//-------------------------------------------
WorkPackageInfoView::WorkPackageInfoView( KoDocument *doc, QWidget *parent )
    : ViewBase( doc, parent ),
    m_view( new KTextBrowser( this ) ),
    m_project( 0 ),
    m_task( 0 ),
    m_manager( 0 )
{
    kDebug()<<"--------------------------------------";
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin(0);
    l->addWidget( m_view );

}

Project *WorkPackageInfoView::project() const
{
    return m_project;
}

void WorkPackageInfoView::setProject( Project *project )
{
    kDebug()<<project;
    m_project = project;
    draw();
}

Task *WorkPackageInfoView::task() const
{
    return m_task;
}

void WorkPackageInfoView::setTask( Task *task )
{
    kDebug()<<task;
    m_task = task;
    draw();
}

ScheduleManager *WorkPackageInfoView::scheduleManager() const
{
    return m_manager;
}

void WorkPackageInfoView::setScheduleManager( ScheduleManager *sm )
{
    kDebug()<<sm;
    m_manager = sm;
    draw();
}


void WorkPackageInfoView::draw()
{
    kDebug();
    m_view->clear();
    
    QTextCursor c = m_view->textCursor();
    QTextBlockFormat bf;
    
    if ( m_project == 0 ) {
        m_view->setText(i18n("No project available") );
        return;
    }
    m_view->setText( i18n( "Project: %1", m_project->name() ) );
    m_view->append( "" );
    if ( m_task == 0 ) {
        m_view->append("No task available" );
        return;
    }
    QString s;
    m_view->append( i18n( "Task: %1", m_task->name() ) );
    m_view->append( "" );
    long id = -1;
    if ( m_manager == 0 ) {
        m_view->append( i18n( "Not scheduled" ) );
    } else {
        m_view->setFontUnderline( true );
        m_view->append( i18n( "Schedule:" ) );
        m_view->setFontUnderline( false );
        
        bf.setIndent( 1 );
        c.insertBlock( bf );
        c.setBlockCharFormat( QTextCharFormat() );
        
        id = m_manager->id();
        c.insertText( i18n( "Start: %1", m_task->startTime( id ).toLocalZone().dateTime().toString( Qt::SystemLocaleDate ) ) );
        c.insertText( "\n" );
        c.insertText( i18n( "Finish: %1", m_task->endTime( id ).toLocalZone().dateTime().toString( Qt::SystemLocaleDate ) ) );
    }
    c.insertText( "\n" );
    
    m_view->setFontUnderline( true );
    m_view->append( i18n( "Description:" ) );
    m_view->setFontUnderline( false );
    
    c.movePosition( QTextCursor::End );
    bf.setIndent( 1 );
    c.insertBlock( bf );
    c.setBlockCharFormat( QTextCharFormat() );
    
    s = m_task->description();
    if ( s.isEmpty() ) {
        s = i18n( "No description available" );
    }
    c.insertText( s );
    
    c.insertBlock( QTextBlockFormat() );

    m_view->setFontUnderline( true );
    m_view->append( i18n( "Contacts:" ) );
    m_view->setFontUnderline( false );
    
    c.movePosition( QTextCursor::End );
    c.insertBlock( bf );
    c.setBlockCharFormat( QTextCharFormat() );
    
    c.insertText( i18n( "Project Manager: %1", m_project->leader() ) );
    c.insertText( "\n" );
    c.insertText( i18n( "Responsible: %1", m_task->leader() ) );
    c.insertText( "\n" );
    if ( id != -1 ) {
        QList<Resource*> lst = m_task->assignedResources( id );
        if ( lst.isEmpty() ) {
            c.insertText( i18n( "Participants: None" ) );
            c.insertText( "\n" );
        } else {
            c.insertText( i18n( "Participants:" ) );
            bf.setIndent( 2 );
            c.insertBlock( bf );
            foreach ( Resource *r, lst ) {
                s = r->email();
                if ( ! s.isEmpty() ) {
                    s = '<' + s + '>';
                }
                c.insertText( r->name() + ' ' + s );
                c.insertText( "\n" );
            }
        }
    }
}



} // namespace KPlato

#include "kptworkpackageview.moc"
