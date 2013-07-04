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
#include "event.h"
#include "part.h"
#include <kglobal.h>
#include <kdebug.h>

#include <QtGui/QApplication>
#include <QtGui/QScrollBar>
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
        m_bAllowNestedParts = false;
        m_bIgnoreScrollBars = false;
        m_activationButtonMask = Qt::LeftButton | Qt::MidButton | Qt::RightButton;
        m_reason = PartManager::NoReason;
        m_bIgnoreExplicitFocusRequest = false;
    }
    ~PartManagerPrivate()
    {
    }
    void setReason( QEvent* ev ) {
        switch( ev->type() ) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick: {
            QMouseEvent* mev = static_cast<QMouseEvent *>( ev );
            m_reason = mev->button() == Qt::LeftButton
                       ? PartManager::ReasonLeftClick
                       : ( mev->button() == Qt::MidButton
                           ? PartManager::ReasonMidClick
                           : PartManager::ReasonRightClick );
            break;
        }
        case QEvent::FocusIn:
            m_reason = static_cast<QFocusEvent *>( ev )->reason();
            break;
        default:
            kWarning(1000) << "PartManagerPrivate::setReason got unexpected ev type " << ev->type();
            break;
        }
    }

    bool allowExplicitFocusEvent(QEvent* ev) const
    {
        if (ev->type() == QEvent::FocusIn) {
            QFocusEvent* fev = static_cast<QFocusEvent*>(ev);
            return (!m_bIgnoreExplicitFocusRequest || fev->reason() != Qt::OtherFocusReason);
        }
        return true;
    }

    Part * m_activePart;
    QWidget *m_activeWidget;

    QList<Part *> m_parts;

    PartManager::SelectionPolicy m_policy;

    Part *m_selectedPart;
    QWidget *m_selectedWidget;

    QList<const QWidget *> m_managedTopLevelWidgets;
    short int m_activationButtonMask;
    bool m_bIgnoreScrollBars;
    bool m_bAllowNestedParts;
    int m_reason;
    bool m_bIgnoreExplicitFocusRequest;
};

}

PartManager::PartManager( QWidget * parent )
 : QObject( parent ),d(new PartManagerPrivate)
{

  qApp->installEventFilter( this );

  d->m_policy = Direct;

  addManagedTopLevelWidget( parent );
}

PartManager::PartManager( QWidget *topLevel, QObject *parent )
    : QObject( parent ),d(new PartManagerPrivate)
{

    qApp->installEventFilter( this );

    d->m_policy = Direct;

    addManagedTopLevelWidget( topLevel );
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

void PartManager::setSelectionPolicy( SelectionPolicy policy )
{
    d->m_policy = policy;
}

PartManager::SelectionPolicy PartManager::selectionPolicy() const
{
    return d->m_policy;
}

void PartManager::setAllowNestedParts( bool allow )
{
    d->m_bAllowNestedParts = allow;
}

bool PartManager::allowNestedParts() const
{
    return d->m_bAllowNestedParts;
}

void PartManager::setIgnoreScrollBars( bool ignore )
{
    d->m_bIgnoreScrollBars = ignore;
}

bool PartManager::ignoreScrollBars() const
{
    return d->m_bIgnoreScrollBars;
}

void PartManager::setActivationButtonMask( short int buttonMask )
{
    d->m_activationButtonMask = buttonMask;
}

short int PartManager::activationButtonMask() const
{
    return d->m_activationButtonMask;
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
    if ( ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick )
    {
        mev = static_cast<QMouseEvent *>( ev );
#ifdef DEBUG_PARTMANAGER
        kDebug(1000) << "PartManager::eventFilter button: " << mev->button() << " " << "d->m_activationButtonMask=" << d->m_activationButtonMask;
#endif
        if ( ( mev->button() & d->m_activationButtonMask ) == 0 )
            return false; // ignore this button
    }

    Part * part;
    while ( w )
    {
        QPoint pos;

        if ( !d->m_managedTopLevelWidgets.contains( w->topLevelWidget() ) )
            return false;

        if ( d->m_bIgnoreScrollBars && ::qobject_cast<QScrollBar *>(w) )
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
            if ( d->m_policy == PartManager::TriState )
            {
                if ( ev->type() == QEvent::MouseButtonDblClick )
                {
                    if ( part == d->m_activePart && w == d->m_activeWidget )
                        return false;

#ifdef DEBUG_PARTMANAGER
                    kDebug(1000) << "PartManager::eventFilter dblclick -> setActivePart" << part;
#endif
                    d->setReason( ev );
                    setActivePart( part, w );
                    d->m_reason = NoReason;
                    return true;
                }

                if ( ( d->m_selectedWidget != w || d->m_selectedPart != part ) &&
                     ( d->m_activeWidget != w || d->m_activePart != part ) )
                {
                    if ( part->isSelectable() )
                        setSelectedPart( part, w );
                    else {
#ifdef DEBUG_PARTMANAGER
                        kDebug(1000) << "Part " << part << " (non-selectable) made active because " << w->metaObject()->className() << " got event" << " " << evType;
#endif
                        d->setReason( ev );
                        setActivePart( part, w );
                        d->m_reason = NoReason;
                    }
                    return true;
                }
                else if ( d->m_selectedWidget == w && d->m_selectedPart == part )
                {
#ifdef DEBUG_PARTMANAGER
                    kDebug(1000) << "Part " << part << " made active (from selected) because " << w->metaObject()->className() << " got event" << " " << evType;
#endif
                    d->setReason( ev );
                    setActivePart( part, w );
                    d->m_reason = NoReason;
                    return true;
                }
                else if ( d->m_activeWidget == w && d->m_activePart == part )
                {
                    setSelectedPart( 0 );
                    return false;
                }

                return false;
            }
            else if ( part != d->m_activePart && d->allowExplicitFocusEvent(ev) )
            {
#ifdef DEBUG_PARTMANAGER
                kDebug(1000) << "Part " << part << " made active because " << w->metaObject()->className() << " got event" << " " << evType;
#endif
                d->setReason( ev );
                setActivePart( part, w );
                d->m_reason = NoReason;
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

void PartManager::addPart( Part *part, bool setActive )
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

    if ( setActive ) {
        setActivePart( part );

        if ( QWidget *w = part->widget() ) {
            // Prevent focus problems
            if ( w->focusPolicy() == Qt::NoFocus ) {
                kWarning(1000) << "Part '" << part->objectName() << "' has a widget "
                               << w->objectName() << " with a focus policy of NoFocus. It should have at least a"
                               << "ClickFocus policy, for part activation to work well." << endl;
            }
            if ( part->widget() && part->widget()->focusPolicy() == Qt::TabFocus ) {
                kWarning(1000) << "Part '" << part->objectName() << "' has a widget "
                               << w->objectName() << " with a focus policy of TabFocus. It should have at least a"
                               << "ClickFocus policy, for part activation to work well." << endl;
            }
            w->setFocus();
            w->show();
        }
    }
    emit partAdded( part );
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

    emit partRemoved( part );

    if ( part == d->m_activePart )
        setActivePart( 0 );
    if ( part == d->m_selectedPart )
        setSelectedPart( 0 );
}

void PartManager::replacePart( Part * oldPart, Part * newPart, bool setActive )
{
    //kDebug(1000) << "replacePart " << oldPart->name() << "-> " << newPart->name() << " setActive=" << setActive;
    // This methods does exactly removePart + addPart but without calling setActivePart(0) in between
    if ( !d->m_parts.contains( oldPart ) )
    {
        kFatal(1000) << QString("Can't remove part %1, not in KPartManager's list.").arg(oldPart->objectName());
        return;
    }

    d->m_parts.removeAll( oldPart );
    oldPart->setManager(0);

    emit partRemoved( oldPart );

    addPart( newPart, setActive );
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
    if ( part && !d->m_bAllowNestedParts )
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

    setSelectedPart( 0 );

    d->m_activePart = part;
    d->m_activeWidget = widget;

    if ( oldActivePart )
    {
        KoParts::Part *savedActivePart = part;
        QWidget *savedActiveWidget = widget;

        PartActivateEvent ev( false, oldActivePart, oldActiveWidget );
        QApplication::sendEvent( oldActivePart, &ev );
        if ( oldActiveWidget )
        {
            disconnect( oldActiveWidget, SIGNAL(destroyed()),
                        this, SLOT(slotWidgetDestroyed()) );
            QApplication::sendEvent( oldActiveWidget, &ev );
        }

        d->m_activePart = savedActivePart;
        d->m_activeWidget = savedActiveWidget;
    }

    if ( d->m_activePart )
    {
        if ( !widget )
            d->m_activeWidget = part->widget();

        PartActivateEvent ev( true, d->m_activePart, d->m_activeWidget );
        QApplication::sendEvent( d->m_activePart, &ev );
        if ( d->m_activeWidget )
        {
            connect( d->m_activeWidget, SIGNAL(destroyed()),
                     this, SLOT(slotWidgetDestroyed()) );
            QApplication::sendEvent( d->m_activeWidget, &ev );
        }
    }
    // Set the new active instance in KGlobal
    setActiveComponent(d->m_activePart ? d->m_activePart->componentData() : KGlobal::mainComponent());

#ifdef DEBUG_PARTMANAGER
    kDebug(1000) << this << " emitting activePartChanged " << d->m_activePart;
#endif
    emit activePartChanged( d->m_activePart );
}

void PartManager::setActiveComponent(const KComponentData &instance)
{
    // It's a separate method to allow redefining this behavior
    KGlobal::setActiveComponent(instance);
}

Part *PartManager::activePart() const
{
    return d->m_activePart;
}

QWidget *PartManager::activeWidget() const
{
    return  d->m_activeWidget;
}

void PartManager::setSelectedPart( Part *part, QWidget *widget )
{
    if ( part == d->m_selectedPart && widget == d->m_selectedWidget )
        return;

    Part *oldPart = d->m_selectedPart;
    QWidget *oldWidget = d->m_selectedWidget;

    d->m_selectedPart = part;
    d->m_selectedWidget = widget;

    if ( part && !widget )
        d->m_selectedWidget = part->widget();

    if ( oldPart )
    {
        PartSelectEvent ev( false, oldPart, oldWidget );
        QApplication::sendEvent( oldPart, &ev );
        QApplication::sendEvent( oldWidget, &ev );
    }

    if ( d->m_selectedPart )
    {
        PartSelectEvent ev( true, d->m_selectedPart, d->m_selectedWidget );
        QApplication::sendEvent( d->m_selectedPart, &ev );
        QApplication::sendEvent( d->m_selectedWidget, &ev );
    }
}

Part *PartManager::selectedPart() const
{
    return d->m_selectedPart;
}

QWidget *PartManager::selectedWidget() const
{
    return d->m_selectedWidget;
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

const QList<Part *> PartManager::parts() const
{
    return d->m_parts;
}

void PartManager::addManagedTopLevelWidget( const QWidget *topLevel )
{
    if ( !topLevel->isTopLevel() )
        return;

    if ( d->m_managedTopLevelWidgets.contains( topLevel ) )
        return;

    d->m_managedTopLevelWidgets.append( topLevel );
    connect( topLevel, SIGNAL(destroyed()),
             this, SLOT(slotManagedTopLevelWidgetDestroyed()) );
}

void PartManager::removeManagedTopLevelWidget( const QWidget *topLevel )
{
    if ( !topLevel->isTopLevel() )
        return;

    d->m_managedTopLevelWidgets.removeAll( topLevel );
}

void PartManager::slotManagedTopLevelWidgetDestroyed()
{
    const QWidget *widget = static_cast<const QWidget *>( sender() );
    removeManagedTopLevelWidget( widget );
}

int PartManager::reason() const
{
    return d->m_reason;
}

void PartManager::setIgnoreExplictFocusRequests(bool ignore)
{
    d->m_bIgnoreExplicitFocusRequest = ignore;
}

#include "partmanager.moc"
