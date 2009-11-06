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
  Boston, MA 02110-1301, USA.
*/

#include "kptsplitterview.h"

#include "KoDocument.h"

#include <KTabWidget>

#include <QVBoxLayout>
#include <QSplitter>
#include <QTabWidget>

#include <kdebug.h>

namespace KPlato
{

SplitterView::SplitterView(KoDocument *doc, QWidget *parent)
    : ViewBase( doc, parent ),
    m_activeview( 0 )
{
    QVBoxLayout *b = new QVBoxLayout( this );
    b->setContentsMargins( 0, 0, 0, 0 );
    m_splitter = new QSplitter( this );
    m_splitter->setOrientation( Qt::Vertical );
    b->addWidget( m_splitter );
}
    
QTabWidget *SplitterView::addTabWidget(  )
{
    KTabWidget *w = new KTabWidget( m_splitter );
    m_splitter->addWidget( w );
    return w;
}

void SplitterView::addView( ViewBase *view )
{
    m_splitter->addWidget( view );
    connect( view, SIGNAL( guiActivated( ViewBase*, bool ) ), this, SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    connect( view, SIGNAL( requestPopupMenu( const QString&, const QPoint& ) ), SIGNAL( requestPopupMenu( const QString&, const QPoint& ) ) );
}

void SplitterView::addView( ViewBase *view, QTabWidget *tab, const QString &label )
{
    tab->addTab( view, label );
    connect( view, SIGNAL( guiActivated( ViewBase*, bool ) ), this, SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    connect( view, SIGNAL( requestPopupMenu( const QString&, const QPoint& ) ), SIGNAL( requestPopupMenu( const QString&, const QPoint& ) ) );
}

// reimp
void SplitterView::setGuiActive( bool active ) // virtual slot
{
    kDebug()<<active<<m_activeview;
    if ( m_activeview ) {
        m_activeview->setGuiActive( active );
    } else {
        emit guiActivated( this, active );
    }
}

void SplitterView::slotGuiActivated( ViewBase *v, bool active )
{
    kDebug()<<active<<m_activeview<<" -> "<<v;
    if ( active ) {
        if ( m_activeview ) {
            emit guiActivated( m_activeview, false );
        }
        m_activeview = v;
    } else {
        m_activeview = 0;
    }
    emit guiActivated( v, active );
}

ViewBase *SplitterView::hitView( const QPoint &glpos )
{
    kDebug()<<glpos;
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        kDebug()<<m_splitter->widget( i );
        ViewBase *w = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( w && w->frameGeometry().contains( w->mapFromGlobal( glpos ) ) ) {
            kDebug()<<w<<glpos<<"->"<<w->mapFromGlobal( glpos )<<"in"<<w->frameGeometry();
            return w;
        }
        QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
        /*if (tw && tw->frameGeometry().contains( tw->mapFromGlobal( glpos ) ) ) {
        //FIXME: tw->frameGeometry() returns geometry ex tw->tabBar()*/
        //FIXME: and when hitting the tab, this is called before currentWidget() has changed
        
        if ( tw ) {
            w = dynamic_cast<ViewBase*>( tw->currentWidget() );
            if ( w && w->frameGeometry().contains( w->mapFromGlobal( glpos ) ) ) {
                kDebug()<<w<<glpos<<"->"<<w->mapFromGlobal( glpos )<<"in"<<w->frameGeometry();
            }
            kDebug()<<w;
            return w;
        }
    }
    return const_cast<SplitterView*>( this );
}

ViewBase *SplitterView::findView( const QPoint &pos ) const
{
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *w = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( w && w->frameGeometry().contains( pos ) ) {
            kDebug()<<pos<<" in "<<w->frameGeometry();
            return w;
        }
        QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
        if (tw && tw->frameGeometry().contains( pos ) ) {
            w = dynamic_cast<ViewBase*>( tw->currentWidget() );
            if ( w ) {
                kDebug()<<pos<<" in "<<w->frameGeometry();
                return w;
            }
        }
    }
    return const_cast<SplitterView*>( this );
}

void SplitterView::setProject( Project *project )
{
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v ) {
            v->setProject( project );
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                for ( int j = 0; j < tw->count(); ++j ) {
                    v = dynamic_cast<ViewBase*>( tw->widget( j ) );
                    if ( v ) {
                        v->setProject( project );
                    }
                }
            }
        }
    }
}
    
void SplitterView::draw()
{
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v ) {
            v->draw();
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                for ( int j = 0; j < tw->count(); ++j ) {
                    v = dynamic_cast<ViewBase*>( tw->widget( j ) );
                    if ( v ) {
                        v->draw();
                    }
                }
            }
        }
    }
}

void SplitterView::draw( Project &project )
{
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v ) {
            v->draw( project );
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                for ( int j = 0; j < tw->count(); ++j ) {
                    v = dynamic_cast<ViewBase*>( tw->widget( j ) );
                    if ( v ) {
                        v->draw( project );
                    }
                }
            }
        }
    }
}


void SplitterView::updateReadWrite( bool mode )
{
    for ( int i = 0; i < m_splitter->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_splitter->widget( i ) );
        if ( v ) {
            v->updateReadWrite( mode );
        } else {
            QTabWidget *tw = dynamic_cast<QTabWidget*>( m_splitter->widget( i ) );
            if (tw ) {
                for ( int j = 0; j < tw->count(); ++j ) {
                    v = dynamic_cast<ViewBase*>( tw->widget( j ) );
                    if ( v ) {
                        v->updateReadWrite( mode );
                    }
                }
            }
        }
    }
}

ViewBase *SplitterView::focusView() const
{
    QList<ViewBase*> lst = findChildren<ViewBase*>();
    kDebug()<<lst;
    foreach ( ViewBase *v, lst ) {
        if ( v->isActive() ) {
            kDebug()<<v;
            return v;
        }
    }
    return 0;
}

QStringList SplitterView::actionListNames() const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->actionListNames();
    }
    return QStringList();
}
    
QList<QAction*> SplitterView::actionList( const QString name ) const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->actionList( name );
    }
    return QList<QAction*>();
}
    
QList<QAction*> SplitterView::contextActionList() const
{
    ViewBase *view = focusView();
    kDebug()<<this<<view;
    if ( view ) {
        return view->contextActionList();
    }
    return QList<QAction*>();
}

Node* SplitterView::currentNode() const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->currentNode();
    }
    return 0;
}
    
Resource* SplitterView::currentResource() const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->currentResource();
    }
    return 0;
}

ResourceGroup* SplitterView::currentResourceGroup() const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->currentResourceGroup();
    }
    return 0;
}

Calendar* SplitterView::currentCalendar() const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->currentCalendar();
    }
    return 0;
}

Relation *SplitterView::currentRelation() const
{
    ViewBase *view = focusView();
    if ( view ) {
        return view->currentRelation();
    }
    return 0;
}

bool SplitterView::loadContext( const KoXmlElement &context )
{
    KoXmlElement e = context.namedItem( "views" ).toElement();
    if ( e.isNull() ) {
        return true;
    }
#ifndef KOXML_USE_QDOM
    foreach ( const QString &s, e.attributeNames() ) {
        ViewBase *v = findChildren<ViewBase*>( s ).first();
        if ( v == 0 ) {
            continue;
        }
        KoXmlElement e1 = e.namedItem( s ).toElement();
        if ( e1.isNull() ) {
            continue;
        }
        v->loadContext( e1 );
    }
#endif
    return true;
}

void SplitterView::saveContext( QDomElement &context ) const
{
    QList<ViewBase*> lst = findChildren<ViewBase*>();
    if ( lst.isEmpty() ) {
        return;
    }
    QDomElement e = context.ownerDocument().createElement( "views" );
    context.appendChild( e );
    foreach ( ViewBase *v, lst ) {
        e.setAttribute( v->objectName(), "" );
    }
    foreach ( ViewBase *v, lst ) {
        QDomElement e1 = e.ownerDocument().createElement( v->objectName() );
        e.appendChild( e1 );
        v->saveContext( e1 );
    }
}

void SplitterView::slotEditCopy()
{
    ViewBase *v = focusView();
    if ( v ) {
        v->slotEditCopy();
    }
}

} // namespace KPlato

#include "kptsplitterview.moc"
