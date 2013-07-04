/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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

#include "partmanager.h"
#include "part.h"
#include <kglobal.h>
#include <kdebug.h>

#include <QApplication>
#include <QScrollBar>
#include <QMouseEvent>
#include <kcomponentdata.h>

//#define DEBUG_PARTMANAGER

using namespace KoParts;

namespace KoParts {

class PartManagerPrivate
{
public:
    PartManagerPrivate()
    {
        m_activeWidget = 0;
        m_activePart = 0;
        m_selectedPart = 0;
        m_selectedWidget = 0;
    }
    ~PartManagerPrivate()
    {
    }

    Part * m_activePart;
    QWidget *m_activeWidget;

    QList<Part *> m_parts;

    Part *m_selectedPart;
    QWidget *m_selectedWidget;

    QList<const QWidget *> m_managedTopLevelWidgets;
};

}

PartManager::PartManager( QWidget * parent )
    : QObject( parent )
    , d(new PartManagerPrivate)
{

  qApp->installEventFilter( this );


  if ( !parent->isTopLevel() )
      return;

  if ( d->m_managedTopLevelWidgets.contains( parent ) )
      return;

  d->m_managedTopLevelWidgets.append( parent );
  connect( parent, SIGNAL(destroyed()),
           this, SLOT(slotManagedTopLevelWidgetDestroyed()) );
}

PartManager::~PartManager()
{
    foreach( const QWidget* w, d->m_managedTopLevelWidgets )
    {
        disconnect( w, SIGNAL(destroyed()),
                    this, SLOT(slotManagedTopLevelWidgetDestroyed()) );
    }

    foreach( Part* it, d->m_parts )
    {
        it->setManager( 0 );
    }

    // core dumps ... setActivePart( 0 );
    qApp->removeEventFilter( this );
    delete d;
}

bool PartManager::eventFilter( QObject *obj, QEvent *ev )
{

    if ( ev->type() != QEvent::MouseButtonPress &&
         ev->type() != QEvent::MouseButtonDblClick &&
         ev->type() != QEvent::FocusIn )
        return false;

    if ( !obj->isWidgetType() )
        return false;

    QWidget *w = static_cast<QWidget *>( obj );

    if ( ( ( w->windowFlags().testFlag(Qt::Dialog) ) && w->isModal() ) ||
         ( w->windowFlags().testFlag(Qt::Popup) ) || ( w->windowFlags().testFlag(Qt::Tool) ) )
        return false;

    QMouseEvent* mev = 0;
    Part * part;
    while ( w )
    {
        QPoint pos;

        if ( !d->m_managedTopLevelWidgets.contains( w->topLevelWidget() ) )
            return false;

        if ( mev ) // mouse press or mouse double-click event
        {
            pos = mev->globalPos();
            part = findPartFromWidget( w, pos );
        } else
            part = findPartFromWidget( w );

#ifdef DEBUG_PARTMANAGER
        const char* evType = ( ev->type() == QEvent::MouseButtonPress ) ? "MouseButtonPress"
                             : ( ev->type() == QEvent::MouseButtonDblClick ) ? "MouseButtonDblClick"
                             : ( ev->type() == QEvent::FocusIn ) ? "FocusIn" : "OTHER! ERROR!";
#endif
        if ( part ) // We found a part whose widget is w
        {
            if ( part != d->m_activePart )
            {
#ifdef DEBUG_PARTMANAGER
                kDebug(1000) << "Part " << part << " made active because " << w->metaObject()->className() << " got event" << " " << evType;
#endif
                setActivePart( part, w );
            }

            return false;
        }

        w = w->parentWidget();

        if ( w && ( ( ( w->windowFlags() & Qt::Dialog ) && w->isModal() ) ||
                    ( w->windowFlags() & Qt::Popup ) || ( w->windowFlags() & Qt::Tool ) ) )
        {
#ifdef DEBUG_PARTMANAGER
            kDebug(1000) << QString("No part made active although %1/%2 got event - loop aborted").arg(obj->objectName()).arg(obj->metaObject()->className());
#endif
            return false;
        }

    }

#ifdef DEBUG_PARTMANAGER
    kDebug(1000) << QString("No part made active although %1/%2 got event").arg(obj->objectName()).arg(obj->metaObject()->className());
#endif
    return false;
}

Part * PartManager::findPartFromWidget( QWidget * widget, const QPoint &pos )
{
    for ( QList<Part *>::iterator it = d->m_parts.begin(), end = d->m_parts.end() ; it != end ; ++it )
    {
        Part *part = (*it)->hitTest( widget, pos );
        if ( part && d->m_parts.contains( part ) )
            return part;
    }
    return 0;
}

Part * PartManager::findPartFromWidget( QWidget * widget )
{
    for ( QList<Part *>::iterator it = d->m_parts.begin(), end = d->m_parts.end() ; it != end ; ++it )
    {
        if ( widget == (*it)->widget() )
            return (*it);
    }
    return 0;
}

void PartManager::addPart(Part *part)
{
    Q_ASSERT( part );

    // don't add parts more than once :)
    if ( d->m_parts.contains( part ) ) {
#ifdef DEBUG_PARTMANAGER
        kWarning(1000) << part << " already added" << kBacktrace(5);
#endif
        return;
    }

    d->m_parts.append( part );
    part->setManager( this );
}

void PartManager::removePart( Part *part )
{
    if (!d->m_parts.contains(part)) {
        return;
    }

    const int nb = d->m_parts.removeAll(part);
    Q_ASSERT(nb == 1);
    Q_UNUSED(nb); // no warning in release mode
    part->setManager(0);

    if ( part == d->m_activePart )
        setActivePart( 0 );
    if ( part == d->m_selectedPart ) {
        d->m_selectedPart = 0;
        d->m_selectedWidget = 0;
    }
}

void PartManager::setActivePart( Part *part, QWidget *widget )
{
    if ( part && !d->m_parts.contains( part ) )
    {
        kWarning(1000) << "trying to activate a non-registered part!" << part->objectName();
        return; // don't allow someone call setActivePart with a part we don't know about
    }

    //check whether nested parts are disallowed and activate the top parent part then, by traversing the
    //tree recursively (Simon)
    if ( part )
    {
        QObject *parentPart = part->parent(); // ### this relies on people using KoParts::Factory!
        KoParts::Part *parPart = ::qobject_cast<KoParts::Part *>( parentPart );
        if ( parPart )
        {
            setActivePart( parPart, parPart->widget() );
            return;
        }
    }

#ifdef DEBUG_PARTMANAGER
    kDebug(1000) << "PartManager::setActivePart d->m_activePart=" << d->m_activePart << "<->part=" << part
                 << " d->m_activeWidget=" << d->m_activeWidget << "<->widget=" << widget << endl;
#endif

    // don't activate twice
    if ( d->m_activePart && part && d->m_activePart == part &&
         (!widget || d->m_activeWidget == widget) )
        return;

    KoParts::Part *oldActivePart = d->m_activePart;
    QWidget *oldActiveWidget = d->m_activeWidget;

    d->m_selectedPart = 0;
    d->m_selectedWidget = 0;

    d->m_activePart = part;
    d->m_activeWidget = widget;

    if ( oldActivePart )
    {
        KoParts::Part *savedActivePart = part;
        QWidget *savedActiveWidget = widget;

        if ( oldActiveWidget ) {
            disconnect( oldActiveWidget, SIGNAL(destroyed()), this, SLOT(slotWidgetDestroyed()) );
        }

        d->m_activePart = savedActivePart;
        d->m_activeWidget = savedActiveWidget;
    }

    if ( d->m_activePart ) {
        if ( !widget )
            d->m_activeWidget = part->widget();

        if ( d->m_activeWidget ) {
            connect( d->m_activeWidget, SIGNAL(destroyed()), this, SLOT(slotWidgetDestroyed()) );
        }
    }
    // Set the new active instance in KGlobal
    KGlobal::setActiveComponent(d->m_activePart ? d->m_activePart->componentData() : KGlobal::mainComponent());

#ifdef DEBUG_PARTMANAGER
    kDebug(1000) << this << " emitting activePartChanged " << d->m_activePart;
#endif
    emit activePartChanged( d->m_activePart );
}

QWidget *PartManager::activeWidget() const
{
    return  d->m_activeWidget;
}

void PartManager::slotObjectDestroyed()
{
    kDebug(1000);
    removePart( const_cast<Part *>( static_cast<const Part *>( sender() ) ) );
}

void PartManager::slotWidgetDestroyed()
{
    kDebug(1000);
    if ( static_cast<const QWidget *>( sender() ) == d->m_activeWidget )
        setActivePart( 0 ); //do not remove the part because if the part's widget dies, then the
    //part will delete itself anyway, invoking removePart() in its destructor
}

void PartManager::slotManagedTopLevelWidgetDestroyed()
{
    const QWidget *topLevel = static_cast<const QWidget *>( sender() );

    if ( !topLevel->isTopLevel() )
        return;

    d->m_managedTopLevelWidgets.removeAll( topLevel );
}


#include "partmanager.moc"
