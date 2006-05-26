// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrDocument.h"
#include "KPrPage.h"
#include "KPrCommand.h"
#include "KPrBackground.h"
#include "KPrGroupObject.h"


#include "KPrLineObject.h"
#include "KPrEllipseObject.h"
#include "KPrAutoformObject.h"
#include "KPrFreehandObject.h"
#include "KPrPolylineObject.h"
#include "KPrBezierCurveObject.h"
#include "KPrPolygonObject.h"
#include "KPrClosedLineObject.h"

#include "KPrTextObject.h"
#include "KPrPixmapObject.h"

#include "KPrPartObject.h"
#include <KoRuler.h>
#include "KPrPieObject.h"
#include "KPrRectObject.h"
#include "KPrView.h"
#include "KoTextObject.h"
#include "KPrTextDocument.h"
#include <kdebug.h>
#include "KPrVariableCollection.h"
#include <KoRect.h>
#include <KoSize.h>
#include <KoPoint.h>
#include <KoDom.h>
#include <KoTextParag.h>
#include <KoXmlNS.h>
#include <KoStore.h>
#include <KoOasisContext.h>
#include <KoOasisStyles.h>
#include <KoOasisStore.h>

#include <qxml.h>
#include <qbuffer.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>


KPrShadowCmd::KPrShadowCmd( const QString &_name, Q3PtrList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
                      Q3PtrList<KPrObject> &_objects, KPrDocument *_doc )
    : KNamedCommand( _name ), oldShadow( _oldShadow ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldShadow.setAutoDelete( false );
    doc = _doc;
    newShadow = _newShadow;

    m_page = doc->findPage( objects );

    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrShadowCmd::~KPrShadowCmd()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldShadow.setAutoDelete( true );
    oldShadow.clear();
}

void KPrShadowCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowParameter(newShadow.shadowDistance,
                                         newShadow.shadowDirection,
                                         newShadow.shadowColor);
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

void KPrShadowCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setShadowParameter(oldShadow.at(i)->shadowDistance,
                                            oldShadow.at(i)->shadowDirection,
                                            oldShadow.at(i)->shadowColor);
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}


KPrSetOptionsCmd::KPrSetOptionsCmd( const QString &_name, Q3ValueList<KoPoint> &_diffs, Q3PtrList<KPrObject> &_objects,
                              double _gridX, double _gridY, double _oldGridX, double _oldGridY,
                              const QColor &_txtBackCol, const QColor &_otxtBackCol, KPrDocument *_doc )
    : KNamedCommand( _name ),
      diffs( _diffs ),
      objects( _objects ),
      txtBackCol( _txtBackCol ),
      otxtBackCol( _otxtBackCol )
{
    gridX = _gridX;
    gridY = _gridY;
    oldGridX = _oldGridX;
    oldGridY = _oldGridY;
    doc = _doc;
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrSetOptionsCmd::~KPrSetOptionsCmd()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrSetOptionsCmd::execute()
{
    // ## use iterator
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( *diffs.at( i ) );
    doc->setGridValue( gridX, gridY, false );
    doc->updateRuler();
    doc->setTxtBackCol( txtBackCol );
    doc->repaint( false );
}

void KPrSetOptionsCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( -(*diffs.at( i )).x(), -(*diffs.at( i )).y() );
    doc->setGridValue( oldGridX, oldGridY, false );
    doc->updateRuler();
    doc->setTxtBackCol( otxtBackCol );
    doc->repaint( false );
}

KPrSetBackCmd::KPrSetBackCmd( const QString &name, const KPrBackGround::Settings &settings,
                        const KPrBackGround::Settings &oldSettings,
                        bool useMasterBackground,
                        bool takeGlobal, KPrDocument *doc, KPrPage *page )
: KNamedCommand( name )
, m_settings( settings )
, m_oldSettings( oldSettings )
, m_useMasterBackground( useMasterBackground )
, m_oldUseMasterBackground( page->useMasterBackground() )
, m_takeGlobal( takeGlobal )
, m_doc( doc )
, m_page( page )
{
}

void KPrSetBackCmd::execute()
{
    if ( !m_takeGlobal ) {
        m_page->background()->setBackGround( m_settings );
        m_page->setUseMasterBackground( m_useMasterBackground );
        m_doc->restoreBackground( m_page );
    } else {
        Q3PtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( ; it.current() ; ++it )
        {
            it.current()->background()->setBackGround( m_settings );
            it.current()->setUseMasterBackground( m_useMasterBackground );
            m_doc->restoreBackground(it.current());
        }

    }
    m_doc->repaint( false );

    if ( m_takeGlobal ) {
        Q3PtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( int pos = 0; it.current(); ++it, ++pos ) {
            m_doc->updateSideBarItem( it.current() );
        }
    }
    else {
        m_doc->updateSideBarItem( m_page );
    }
}

void KPrSetBackCmd::unexecute()
{
    if ( !m_takeGlobal ) {
        m_page->background()->setBackGround( m_oldSettings );
        m_page->setUseMasterBackground( m_oldUseMasterBackground );
        m_doc->restoreBackground( m_page );
    } else {
        Q3PtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( ; it.current() ; ++it )
        {
            it.current()->background()->setBackGround( m_oldSettings );
            it.current()->setUseMasterBackground( m_oldUseMasterBackground );
            m_doc->restoreBackground(it.current());
        }
    }
    m_doc->repaint( false );

    if ( m_takeGlobal ) {
        Q3PtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( int pos = 0; it.current(); ++it, ++pos ) {
            m_doc->updateSideBarItem( it.current() );
        }
    }
    else {
        m_doc->updateSideBarItem( m_page );
    }
}

KPrRotateCmd::KPrRotateCmd( const QString &_name, float newAngle, Q3PtrList<KPrObject> &objects,
                      KPrDocument *doc, bool addAngle )
    : KNamedCommand( _name ), m_doc( doc ), m_newAngle( newAngle ), m_addAngle( addAngle )
{
    m_objects.setAutoDelete( false );
    m_oldAngles.setAutoDelete( false );

    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        m_objects.append( it.current() );

        RotateValues *old = new RotateValues;
        old->angle = it.current()->getAngle();
        m_oldAngles.append( old );

        it.current()->incCmdRef();
    }

    m_page = m_doc->findPage( m_objects );
}

KPrRotateCmd::~KPrRotateCmd()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldAngles.setAutoDelete( true );
    m_oldAngles.clear();
}

void KPrRotateCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        if ( m_addAngle )
            it.current()->rotate( it.current()->getAngle() + m_newAngle );
        else
            it.current()->rotate( m_newAngle );
    }
    m_doc->updateRuler();
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrRotateCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
        m_objects.at(i)->rotate( m_oldAngles.at( i )->angle );
    m_doc->updateRuler();
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrChgPixCmd::KPrChgPixCmd( const QString &_name, KPrPixmapObject *_oldObject, KPrPixmapObject *_newObject,
                      KPrDocument *_doc, KPrPage *_page)
    : KNamedCommand( _name )
{
    oldObject = _oldObject;
    newObject = _newObject;
    m_page=_page;
    doc = _doc;
    oldObject->incCmdRef();
    newObject->incCmdRef();
    newObject->setSize( oldObject->getSize() );
    newObject->setOrig( oldObject->getOrig() );
}

KPrChgPixCmd::~KPrChgPixCmd()
{
    oldObject->decCmdRef();
    newObject->decCmdRef();
}

void KPrChgPixCmd::execute()
{
    m_page->replaceObject( oldObject, newObject );
    doc->repaint( newObject );

    doc->updateSideBarItem( m_page );
}

void KPrChgPixCmd::unexecute()
{
    m_page->replaceObject( newObject, oldObject );
    doc->repaint( oldObject );

    doc->updateSideBarItem( m_page );
}

KPrDeleteCmd::KPrDeleteCmd( const QString &_name, Q3PtrList<KPrObject> &_objects,
                      KPrDocument *_doc, KPrPage *_page )
: KNamedCommand( _name )
, m_oldObjectList( _page->objectList() )
, m_objectsToDelete( _objects )
, m_doc( _doc )
, m_page( _page )
{
    Q3PtrListIterator<KPrObject> it( m_oldObjectList );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrDeleteCmd::~KPrDeleteCmd()
{
    Q3PtrListIterator<KPrObject> it( m_oldObjectList );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrDeleteCmd::execute()
{
    bool textObj=false;

    Q3PtrListIterator<KPrObject> it( m_oldObjectList );
    Q3PtrListIterator<KPrObject> itDelete( m_objectsToDelete );
    Q3PtrList<KPrObject> newObjectList;
    for ( ; it.current(); ++it )
    {
        if ( it.current() == itDelete.current() )
        {
            it.current()->setSelected( false );
            it.current()->removeFromObjList();

            if ( !textObj && it.current()->getType() == OT_TEXT )
            {
                KPrTextObject * tmp = dynamic_cast<KPrTextObject *>( it.current() );
                if ( tmp )
                    tmp->setEditingTextObj( false );
                textObj=true;
            }
            ++itDelete;
        }
        else
        {
            newObjectList.append( it.current() );
        }
    }

    m_page->setObjectList( newObjectList );

    for ( itDelete.toFirst(); itDelete.current(); ++itDelete )
    {
        QRect oldRect = m_doc->zoomHandler()->zoomRect( itDelete.current()->getRepaintRect() );
        m_doc->repaint( oldRect );
    }
    if(textObj)
        m_doc->updateRuler();

    m_doc->updateSideBarItem( m_page );
}

void KPrDeleteCmd::unexecute()
{
    m_page->setObjectList( m_oldObjectList );
    Q3PtrListIterator<KPrObject> it( m_objectsToDelete );
    for ( ; it.current(); ++it )
    {
        it.current()->addToObjList();
        m_doc->repaint( it.current() );
    }

    m_doc->updateSideBarItem( m_page );
}


KPrEffectCmd::KPrEffectCmd( const QString &_name, const Q3PtrList<KPrObject> &_objs,
                      const Q3ValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect )
    : KNamedCommand( _name ), oldEffects( _oldEffects ),
      newEffect( _newEffect ), objs( _objs )
{
    Q3PtrListIterator<KPrObject> it( objs );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrEffectCmd::~KPrEffectCmd()
{
    Q3PtrListIterator<KPrObject> it( objs );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrEffectCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( objs );
    for ( ; it.current() ; ++it )
    {
        it.current()->setAppearStep( newEffect.appearStep );
        it.current()->setEffect( newEffect.effect );
        it.current()->setEffect2( newEffect.effect2 );
        it.current()->setDisappear( newEffect.disappear );
        it.current()->setEffect3( newEffect.effect3 );
        it.current()->setDisappearStep( newEffect.disappearStep );
        it.current()->setAppearSpeed( newEffect.m_appearSpeed );
        it.current()->setDisappearSpeed( newEffect.m_disappearSpeed );
        it.current()->setAppearTimer( newEffect.appearTimer );
        it.current()->setDisappearTimer( newEffect.disappearTimer );
        it.current()->setAppearSoundEffect( newEffect.appearSoundEffect );
        it.current()->setDisappearSoundEffect( newEffect.disappearSoundEffect );
        it.current()->setAppearSoundEffectFileName( newEffect.a_fileName );
        it.current()->setDisappearSoundEffectFileName( newEffect.d_fileName );
    }
}

void KPrEffectCmd::unexecute()
{
    KPrObject *object = 0;
    for ( unsigned int i = 0; i < objs.count(); ++i ) {
        object = objs.at( i );

        object->setAppearStep( oldEffects[ i ].appearStep );
        object->setEffect( oldEffects[ i ].effect );
        object->setEffect2( oldEffects[ i ].effect2 );
        object->setDisappear( oldEffects[ i ].disappear );
        object->setEffect3( oldEffects[ i ].effect3 );
        object->setDisappearStep( oldEffects[ i ].disappearStep );
        object->setAppearSpeed( oldEffects[ i ].m_appearSpeed );
        object->setDisappearSpeed( oldEffects[ i ].m_disappearSpeed );
        object->setAppearTimer( oldEffects[ i ].appearTimer );
        object->setDisappearTimer( oldEffects[ i ].disappearTimer );
        object->setAppearSoundEffect( oldEffects[ i ].appearSoundEffect );
        object->setDisappearSoundEffect( oldEffects[ i ].disappearSoundEffect );
        object->setAppearSoundEffectFileName( oldEffects[ i ].a_fileName );
        object->setDisappearSoundEffectFileName( oldEffects[ i ].d_fileName );
    }
}

KPrGroupObjCmd::KPrGroupObjCmd( const QString &_name,
                          const Q3PtrList<KPrObject> &_objects,
                          KPrDocument *_doc, KPrPage *_page )
: KNamedCommand( _name )
, m_objectsToGroup( _objects )
, m_oldObjectList( _page->objectList() )
, m_doc( _doc )
, m_page( _page )
{
    m_groupObject = new KPrGroupObject( m_objectsToGroup );
    m_groupObject->incCmdRef();
}

KPrGroupObjCmd::~KPrGroupObjCmd()
{
    m_groupObject->decCmdRef();
}

void KPrGroupObjCmd::execute()
{
    KoRect r;
    int position = 0;
    Q3PtrListIterator<KPrObject> it( m_objectsToGroup );
    for ( ; it.current() ; ++it )
    {
        it.current()->setSelected( false );
        position = m_page->takeObject(it.current() );
        r |= it.current()->getRealRect();
    }

    m_groupObject->setUpdateObjects( false );
    m_groupObject->setOrig( r.x(), r.y() );
    m_groupObject->setSize( r.width(), r.height() );
    m_page->insertObject( m_groupObject, position );
    m_groupObject->addToObjList();
    m_groupObject->setUpdateObjects( true );
    m_groupObject->setSelected( true );
    m_doc->refreshGroupButton();

    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrGroupObjCmd::unexecute()
{
    m_groupObject->setUpdateObjects( false );

    m_page->setObjectList( m_oldObjectList );
    m_groupObject->removeFromObjList();

    Q3PtrListIterator<KPrObject> it( m_objectsToGroup );
    for ( ; it.current() ; ++it )
    {
        it.current()->addToObjList();
        it.current()->setSelected( true );
    }

    m_doc->refreshGroupButton();

    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

UnGroupObjCmd::UnGroupObjCmd( const QString &_name,
                              KPrGroupObject *grpObj_,
                              KPrDocument *_doc, KPrPage *_page )
: KNamedCommand( _name )
, m_groupedObjects( grpObj_->getObjects() )
, m_groupObject( grpObj_ )
, m_doc( _doc )
, m_page( _page )
{
    m_groupObject->incCmdRef();
}

UnGroupObjCmd::~UnGroupObjCmd()
{
    m_groupObject->decCmdRef();
}

void UnGroupObjCmd::execute()
{
    m_groupObject->setUpdateObjects( false );

    int position = m_page->takeObject( m_groupObject );
    m_groupObject->removeFromObjList();

    Q3PtrListIterator<KPrObject> it( m_groupedObjects );
    for ( it.toLast(); it.current() ; --it )
    {
        m_page->insertObject( it.current(), position );
        it.current()->addToObjList();
        it.current()->setSelected( true );
    }

    m_doc->refreshGroupButton();

    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void UnGroupObjCmd::unexecute()
{
    KoRect r=KoRect();
    int position = 0;
    Q3PtrListIterator<KPrObject> it( m_groupedObjects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setSelected( false );
        position = m_page->takeObject( it.current() );
        r |= it.current()->getRealRect();
    }

    m_groupObject->setUpdateObjects( false );
    m_groupObject->setOrig( r.x(), r.y() );
    m_groupObject->setSize( r.width(), r.height() );
    m_page->insertObject( m_groupObject, position );
    m_groupObject->addToObjList();
    m_groupObject->setUpdateObjects( true );
    m_groupObject->setSelected( true );
    m_doc->refreshGroupButton();

    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

KPrInsertCmd::KPrInsertCmd( const QString &name, const Q3ValueList<KPrObject *> objects, 
                            KPrDocument *doc, KPrPage *page )
: KNamedCommand( name )
, m_objects( objects )    
, m_object( 0 )
, m_doc( doc )
, m_page( page )    
{
    Q3ValueListConstIterator<KPrObject *> it( m_objects.begin() );
    for ( ; it != m_objects.end(); ++it )
    {
        ( *it )->incCmdRef();
    }
}

KPrInsertCmd::KPrInsertCmd( const QString &name, KPrObject *object,
                            KPrDocument *doc, KPrPage *page )
: KNamedCommand( name )
, m_object( object )
, m_doc( doc )
, m_page( page )    
{
    m_object->incCmdRef();
}

KPrInsertCmd::~KPrInsertCmd()
{
    if ( m_object )
    {
        m_object->decCmdRef();
    }
    else
    {
        Q3ValueListConstIterator<KPrObject *> it( m_objects.begin() );
        for ( ; it != m_objects.end(); ++it )
        {
            ( *it )->decCmdRef();
        }
    }
}

void KPrInsertCmd::execute()
{
    if ( m_object )
    {
        m_page->appendObject( m_object );
        m_object->addToObjList();
        if ( m_object->getType() == OT_TEXT )
            m_doc->updateRuler();
        m_doc->repaint( m_object );
    }
    else
    {
        m_page->appendObjects( m_objects );
        Q3ValueListConstIterator<KPrObject *> it( m_objects.begin() );
        bool updateRuler = false;
        for ( ; it != m_objects.end(); ++it )
        {
            ( *it )->addToObjList();
            if ( ( *it )->getType() == OT_TEXT )
                updateRuler = true;
            m_doc->repaint( *it );
        }
        if ( updateRuler )
            m_doc->updateRuler();
    }

    m_doc->updateSideBarItem( m_page );
}

void KPrInsertCmd::unexecute()
{
    if ( m_object )
    {
        QRect oldRect = m_doc->zoomHandler()->zoomRect( m_object->getRepaintRect() );
        Q3PtrList<KPrObject> list(m_page->objectList());
        if ( list.findRef( m_object ) != -1 ) {
            m_page->takeObject( m_object );
            m_object->removeFromObjList();
            if ( m_object->getType() == OT_TEXT )
            {
                m_doc->terminateEditing( (KPrTextObject*)m_object );
                ((KPrTextObject*)m_object)->setEditingTextObj( false );
                m_doc->updateRuler();
            }
        }
        m_doc->repaint( oldRect );
    }
    else
    {
        Q3PtrList<KPrObject> list(m_page->objectList());
        bool updateRuler = false;

        Q3ValueListConstIterator<KPrObject *> it( m_objects.begin() );
        for ( ; it != m_objects.end(); ++it )
        {
            if ( list.findRef( *it ) != -1 )
            {
                m_page->takeObject( *it );
                ( *it )->removeFromObjList();
                if ( ( *it )->getType() == OT_TEXT )
                {
                    m_doc->terminateEditing( (KPrTextObject*)( *it ) );
                    ( (KPrTextObject*) *it )->setEditingTextObj( false );
                    updateRuler = true;
                }
            }
        }
        if ( updateRuler )
            m_doc->updateRuler();

        m_doc->repaint( false );
    }

    m_doc->updateSideBarItem( m_page );
}

KPrLowerRaiseCmd::KPrLowerRaiseCmd( const QString &_name, const Q3PtrList<KPrObject>& _oldList,
                              const Q3PtrList<KPrObject>& _newList, KPrDocument *_doc,
                              KPrPage *_page )
    : KNamedCommand( _name )
{
    oldList = _oldList;
    newList = _newList;
    m_page=_page;
    oldList.setAutoDelete( false );
    newList.setAutoDelete( false );
    doc = _doc;

    Q3PtrListIterator<KPrObject> it( oldList );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrLowerRaiseCmd::~KPrLowerRaiseCmd()
{
    Q3PtrListIterator<KPrObject> it( oldList );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrLowerRaiseCmd::execute()
{
    m_page->setObjectList( newList );
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

void KPrLowerRaiseCmd::unexecute()
{
    m_page->setObjectList( oldList );
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}


KPrMoveByCmd::KPrMoveByCmd( const QString &_name, const KoPoint &_diff, Q3PtrList<KPrObject> &_objects,
                      KPrDocument *_doc,KPrPage *_page )
    : KNamedCommand( _name ), diff( _diff ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->incCmdRef();
    }
}

KPrMoveByCmd::~KPrMoveByCmd()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrMoveByCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect( objects.at( i )->getRepaintRect() );
        objects.at( i )->moveBy( diff );
        if ( objects.at( i )->getType() == OT_TEXT )
        {
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }
        doc->repaint( oldRect );
        doc->repaint( objects.at( i ) );
    }

    doc->updateSideBarItem( m_page );
    doc->updateObjectStatusBarItem();
}

void KPrMoveByCmd::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect( objects.at( i )->getRepaintRect() );
        objects.at( i )->moveBy( -diff.x(), -diff.y() );
        if ( objects.at( i )->getType() == OT_TEXT )
        {
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }
        doc->repaint( oldRect );
        doc->repaint( objects.at( i ) );
    }

    doc->updateSideBarItem( m_page );
    doc->updateObjectStatusBarItem();
}

KPrAlignCmd::KPrAlignCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, AlignType _at, KPrDocument *_doc )
    : KNamedCommand( _name ), doc(_doc)
{
    objects.setAutoDelete( false );
    diffs.setAutoDelete( true );
    m_page = doc->findPage( _objects );

    Q3PtrListIterator<KPrObject> it( _objects );
    KoRect boundingRect;
    for ( ; it.current() ; ++it )
    {
        boundingRect |= it.current()->getRealRect();
    }

    if ( _objects.count() == 1 )
        boundingRect = m_page->getPageRect();

    it.toFirst();
    for ( ; it.current() ; ++it )
    {
        KoPoint * diff = NULL;
        switch ( _at )
        {
            case AT_LEFT:
              diff = new KoPoint( boundingRect.x() - it.current()->getRealOrig().x(), 0 );
              break;
            case AT_TOP:
              diff = new KoPoint( 0, boundingRect.y() - it.current()->getRealOrig().y() );
              break;
            case AT_RIGHT:
              diff = new KoPoint( boundingRect.x() + boundingRect.width() -
                                  it.current()->getRealOrig().x() - it.current()->getRealSize().width(), 0 );
              break;
            case AT_BOTTOM:
              diff = new KoPoint( 0, boundingRect.y() + boundingRect.height() -
                                  it.current()->getRealOrig().y() - it.current()->getRealSize().height() );
              break;
            case AT_HCENTER:
              diff = new KoPoint( ( boundingRect.width() - it.current()->getRealSize().width() ) / 2 -
                                  it.current()->getRealOrig().x() + boundingRect.x(), 0 );
              break;
            case AT_VCENTER:
              diff = new KoPoint( 0, ( boundingRect.height() - it.current()->getRealSize().height() ) / 2 -
                                  it.current()->getRealOrig().y() + boundingRect.y() );
              break;
        }
        if ( diff )
        {
            objects.append( it.current() );
            diffs.append( diff );
            it.current()->incCmdRef();
        }
    }
}

KPrAlignCmd::~KPrAlignCmd()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    diffs.clear();
}

void KPrAlignCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect( objects.at( i )->getRepaintRect() );
        objects.at( i )->moveBy( *diffs.at( i ) );
        if ( objects.at( i )->getType() == OT_TEXT )
        {
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }

        doc->repaint( oldRect );
        doc->repaint( objects.at( i ) );
    }

    doc->updateSideBarItem( m_page );
}

void KPrAlignCmd::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getRepaintRect() );
        objects.at( i )->moveBy( -diffs.at( i )->x(), -diffs.at( i )->y() );
        if ( objects.at( i )->getType() == OT_TEXT )
        {
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }
        doc->repaint( oldRect );
        doc->repaint( objects.at( i ) );
        doc->updateRuler();
    }

    doc->updateSideBarItem( m_page );
}

KoPenCmd::KoPenCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, Pen _newPen,
                KPrDocument *_doc, KPrPage *_page, int _flags )
: KNamedCommand(_name), doc(_doc), m_page( _page ), newPen(_newPen), flags(_flags)
{
    objects.setAutoDelete( false );
    oldPen.setAutoDelete( false );

    addObjects( _objects );
}

KoPenCmd::~KoPenCmd()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldPen.setAutoDelete( true );
    oldPen.clear();
}

void KoPenCmd::execute()
{
    for ( int i = 0; i < static_cast<int>( objects.count() ); i++ )
    {
        Pen tmpPen = *oldPen.at( i );

        if ( flags & LineBegin )
            tmpPen.lineBegin = newPen.lineBegin;

        if ( flags & LineEnd )
            tmpPen.lineEnd = newPen.lineEnd;

        if ( flags & Color )
            tmpPen.pen.setColor( newPen.pen.color() );

        if ( flags & Width )
            tmpPen.pen.setPointWidth( newPen.pen.pointWidth() );

        if ( flags & Style )
            tmpPen.pen.setStyle( newPen.pen.style() );

        applyPen( objects.at( i ), &tmpPen );
    }

    // this has to be called as the outline could have been changed so 
    // that the toolbar is updated correctly
    doc->updateObjectSelected();
    doc->updateSideBarItem( m_page );
}

void KoPenCmd::applyPen( KPrObject *object, Pen *tmpPen )
{
    switch (object->getType()) {
        case OT_LINE:
        {
            KPrLineObject* obj=dynamic_cast<KPrLineObject*>( object );
            if( obj )
            {
                //obj->setPen( tmpPen->pen );
                obj->setLineBegin( tmpPen->lineBegin );
                obj->setLineEnd( tmpPen->lineEnd );
                //doc->repaint( obj );
            }
        } break;
        case OT_FREEHAND:
        case OT_POLYLINE:
        case OT_QUADRICBEZIERCURVE:
        case OT_CUBICBEZIERCURVE:
        {
            KPrPointObject *obj = dynamic_cast<KPrPointObject*>( object );
            if ( obj )
            {
                //obj->setPen( tmpPen->pen );
                obj->setLineBegin( tmpPen->lineBegin );
                obj->setLineEnd( tmpPen->lineEnd );
                //doc->repaint( obj );
            }
        } break;
        case OT_PIE:
        {
            KPrPieObject *obj = dynamic_cast<KPrPieObject*>( object );
            if ( obj )
            {
                obj->setLineBegin( tmpPen->lineBegin );
                obj->setLineEnd( tmpPen->lineEnd );
            }
        } break;
        case OT_AUTOFORM:
        {
            KPrAutoformObject *obj = dynamic_cast<KPrAutoformObject*>( object );
            if ( obj )
            {
                obj->setLineBegin( tmpPen->lineBegin );
                obj->setLineEnd( tmpPen->lineEnd );
            }
        } break;
        default:
            break;
    }

    KPrShadowObject *obj = dynamic_cast<KPrShadowObject*>( object );
    if ( obj )
    {
        obj->setPen( tmpPen->pen );
        doc->repaint( obj );
    }
}

void KoPenCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
        if( oldPen.count() > i )
            applyPen( objects.at( i ), oldPen.at( i ) );
    }

    // this has to be called as the outline could have been changed so 
    // that the toolbar is updated correctly
    doc->updateObjectSelected();
    doc->updateSideBarItem( m_page );
}

void KoPenCmd::addObjects( const Q3PtrList<KPrObject> &_objects )
{
    Q3PtrListIterator<KPrObject> it( _objects );
    for ( ; it.current(); ++it )
    {
        KPrObject * object( it.current() );
        if ( object->getType() == OT_GROUP )
        {
            KPrGroupObject * obj=dynamic_cast<KPrGroupObject*>( object );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            // tz TODO fix name
            ::LineEnd lineBegin = L_NORMAL;
            ::LineEnd lineEnd = L_NORMAL;
            switch ( it.current()->getType() ) {
                case OT_LINE:
                {
                    KPrLineObject *obj=dynamic_cast<KPrLineObject*>( object );
                    if ( obj )
                    {
                        lineBegin = obj->getLineBegin();
                        lineEnd = obj->getLineEnd();
                    }
                } break;
                case OT_FREEHAND:
                case OT_POLYLINE:
                case OT_QUADRICBEZIERCURVE:
                case OT_CUBICBEZIERCURVE:
                {
                    KPrPointObject *obj = dynamic_cast<KPrPointObject*>( object );
                    if ( obj )
                    {
                        lineBegin = obj->getLineBegin();
                        lineEnd = obj->getLineEnd();
                    }
                } break;
                case OT_PIE:
                {
                    KPrPieObject *obj = dynamic_cast<KPrPieObject*>( object );
                    if ( obj )
                    {
                        lineBegin = obj->getLineBegin();
                        lineEnd = obj->getLineEnd();
                    }
                } break;
                case OT_AUTOFORM:
                {
                    KPrAutoformObject *obj = dynamic_cast<KPrAutoformObject*>( object );
                    if ( obj )
                    {
                        lineBegin = obj->getLineBegin();
                        lineEnd = obj->getLineEnd();
                    }
                } break;
                default:
                   break;
            }

            KPrShadowObject *obj = dynamic_cast<KPrShadowObject*>( object );
            if ( obj )
            {
                objects.append( obj );
                obj->incCmdRef();

                Pen * pen = new KoPenCmd::Pen( obj->getPen(), lineBegin, lineEnd );

                oldPen.append( pen );
            }
        }
    }
}


KPrBrushCmd::KPrBrushCmd( const QString &_name, Q3PtrList<KPrObject> &_objects, Brush _newBrush,
                    KPrDocument *_doc, KPrPage *_page, int _flags )
: KNamedCommand(_name), doc(_doc), newBrush(_newBrush), m_page(_page), flags(_flags)
{
    objects.setAutoDelete( false );
    oldBrush.setAutoDelete( false );

    addObjects( _objects );
}

void KPrBrushCmd::addObjects( const Q3PtrList<KPrObject> &_objects )
{
    Q3PtrListIterator<KPrObject> it( _objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj=dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPr2DObject * obj = dynamic_cast<KPr2DObject *>( it.current() );
            if( obj )
            {
                objects.append( obj );
                obj->incCmdRef();

                Brush * brush = new KPrBrushCmd::Brush;
                brush->brush = obj->getBrush();
                brush->fillType = obj->getFillType();
                brush->gColor1 = obj->getGColor1();
                brush->gColor2 = obj->getGColor2();
                brush->gType = obj->getGType();
                brush->unbalanced = obj->getGUnbalanced();
                brush->xfactor = obj->getGXFactor();
                brush->yfactor = obj->getGYFactor();

                oldBrush.append( brush );
            }
        }
    }
}

KPrBrushCmd::~KPrBrushCmd()
{
    Q3PtrListIterator<KPr2DObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldBrush.setAutoDelete( true );
    oldBrush.clear();
}

void KPrBrushCmd::execute()
{
    for ( int i = 0; i < static_cast<int>( objects.count() ); i++ )
    {
        Brush tmpBrush = *oldBrush.at( i );

        if ( flags & BrushColor )
            tmpBrush.brush.setColor( newBrush.brush.color() );

        if ( flags & BrushStyle )
            tmpBrush.brush.setStyle( newBrush.brush.style() );

        if ( flags & BrushGradientSelect )
            tmpBrush.fillType = newBrush.fillType;

        if ( flags & GradientColor1 )
            tmpBrush.gColor1 = newBrush.gColor1;

        if ( flags & GradientColor2 )
            tmpBrush.gColor2 = newBrush.gColor2;

        if ( flags & GradientType )
            tmpBrush.gType = newBrush.gType;

        if ( flags & GradientBalanced )
            tmpBrush.unbalanced = newBrush.unbalanced;

        if ( flags & GradientXFactor )
            tmpBrush.xfactor = newBrush.xfactor;

        if ( flags & GradientYFactor )
            tmpBrush.yfactor = newBrush.yfactor;

        applyBrush( objects.at( i ), &tmpBrush );
    }

    doc->updateSideBarItem( m_page );
}

void KPrBrushCmd::applyBrush( KPr2DObject *object, Brush *tmpBrush )
{
    object->setBrush( tmpBrush->brush );
    object->setFillType( tmpBrush->fillType );
    object->setGColor1( tmpBrush->gColor1 );
    object->setGColor2( tmpBrush->gColor2 );
    object->setGType( tmpBrush->gType );
    object->setGUnbalanced( tmpBrush->unbalanced );
    object->setGXFactor( tmpBrush->xfactor );
    object->setGYFactor( tmpBrush->yfactor );
    doc->repaint( object );
}

void KPrBrushCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        applyBrush( objects.at( i ), oldBrush.at( i ) );
    }

    doc->updateSideBarItem( m_page );
}


KPrPgConfCmd::KPrPgConfCmd( const QString &_name, bool _manualSwitch, bool _infiniteLoop,
                      bool _showPresentationDuration, QPen _pen,
                      Q3ValueList<bool> _selectedSlides, const QString & _presentationName,
                      bool _oldManualSwitch, bool _oldInfiniteLoop,
                      bool _oldShowPresentationDuration, QPen _oldPen,
                      Q3ValueList<bool> _oldSelectedSlides, const QString & _oldPresentationName,
                      KPrDocument *_doc )
    : KNamedCommand( _name )
{
    manualSwitch = _manualSwitch;
    infiniteLoop = _infiniteLoop;
    showPresentationDuration = _showPresentationDuration;
    pen = _pen;
    selectedSlides = _selectedSlides;
    oldManualSwitch = _oldManualSwitch;
    oldInfiniteLoop = _oldInfiniteLoop;
    oldShowPresentationDuration = _oldShowPresentationDuration;
    oldPen = _oldPen;
    oldSelectedSlides = _oldSelectedSlides;
    oldPresentationName = _oldPresentationName;
    presentationName = _presentationName;
    doc = _doc;
}

void KPrPgConfCmd::execute()
{
    doc->setManualSwitch( manualSwitch );
    doc->setInfiniteLoop( infiniteLoop );
    doc->setPresentationDuration( showPresentationDuration );
    doc->setPresPen( pen );
    doc->setPresentationName( presentationName );
    Q3PtrList<KPrPage> pages = doc->pageList();
    unsigned count = selectedSlides.count();
    if( count > pages.count() ) count = pages.count();
    for( unsigned i = 0; i < selectedSlides.count(); i++ )
        pages.at( i )->slideSelected( selectedSlides[ i ] );
}

void KPrPgConfCmd::unexecute()
{
    doc->setManualSwitch( oldManualSwitch );
    doc->setInfiniteLoop( oldInfiniteLoop );
    doc->setPresentationDuration( oldShowPresentationDuration );
    doc->setPresPen( oldPen );
    doc->setPresentationName( oldPresentationName );

    Q3PtrList<KPrPage> pages = doc->pageList();
    unsigned count = oldSelectedSlides.count();
    if( count > pages.count() ) count = pages.count();
    for( unsigned i = 0; i < oldSelectedSlides.count(); i++ )
        pages.at( i )->slideSelected( oldSelectedSlides[ i ] );
}


KPrTransEffectCmd::KPrTransEffectCmd( Q3ValueVector<PageEffectSettings> oldSettings,
                                PageEffectSettings newSettings,
                                KPrPage* page, KPrDocument* doc )
{
    m_newSettings = newSettings;
    m_oldSettings = oldSettings;
    Q_ASSERT( !m_oldSettings.isEmpty() );
    m_page = page;
    m_doc = doc;
}

void KPrTransEffectCmd::PageEffectSettings::applyTo( KPrPage *page )
{
    page->setPageEffect( pageEffect );
    page->setPageEffectSpeed( effectSpeed );
    page->setPageSoundEffect( soundEffect );
    page->setPageSoundFileName( soundFileName );
    // TODO page->setAutoAdvance( autoAdvance );
    page->setPageTimer( slideTime );
}

void KPrTransEffectCmd::execute()
{
    if ( m_page )
        m_newSettings.applyTo( m_page );
    else
        for( Q3PtrListIterator<KPrPage> it( m_doc->getPageList() ); *it; ++it )
            m_newSettings.applyTo( it.current() );
}

void KPrTransEffectCmd::unexecute()
{
    if ( m_page )
        m_oldSettings[0].applyTo( m_page );
    else {
        int i = 0;
        for( Q3PtrListIterator<KPrPage> it( m_doc->getPageList() ); *it; ++it, ++i )
            m_oldSettings[i].applyTo( it.current() );
    }
}

QString KPrTransEffectCmd::name() const
{
    if ( m_page )
        return i18n( "Modify Slide Transition" );
    else
        return i18n( "Modify Slide Transition For All Pages" );
}

KPrPgLayoutCmd::KPrPgLayoutCmd( const QString &_name, KoPageLayout _layout, KoPageLayout _oldLayout,
                          KoUnit::Unit _oldUnit, KoUnit::Unit _unit,KPrDocument *_doc )
    : KNamedCommand( _name )
{
    m_doc=_doc;
    layout = _layout;
    oldLayout = _oldLayout;
    oldUnit = _oldUnit;
    unit = _unit;
}

void KPrPgLayoutCmd::execute()
{
    m_doc->setUnit( unit );
    m_doc->setPageLayout( layout );
    m_doc->updateHeaderFooterPosition();
    m_doc->updateRuler();
    m_doc->updateRulerPageLayout();
}

void KPrPgLayoutCmd::unexecute()
{
    m_doc->setUnit( oldUnit );
    m_doc->setPageLayout( oldLayout );
    m_doc->updateHeaderFooterPosition();
    m_doc->updateRuler();
    m_doc->updateRulerPageLayout();
}


KPrPieValueCmd::KPrPieValueCmd( const QString &name, PieValues newValues,
                          Q3PtrList<KPrObject> &objects, KPrDocument *doc,
                          KPrPage *page, int flags )
: KNamedCommand( name )
, m_doc( doc )
, m_page( page )
, m_newValues( newValues )
, m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );

    addObjects( objects );
}

KPrPieValueCmd::KPrPieValueCmd( const QString &_name, Q3PtrList<PieValues> &_oldValues, PieValues _newValues,
                          Q3PtrList<KPrObject> &_objects, KPrDocument *_doc, KPrPage *_page, int _flags )
    : KNamedCommand( _name ), m_oldValues( _oldValues ), m_objects( _objects ), m_flags(_flags)
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );
    m_doc = _doc;
    m_page = _page;
    m_newValues = _newValues;

    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrPieValueCmd::~KPrPieValueCmd()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldValues.setAutoDelete( true );
    m_oldValues.clear();
}

void KPrPieValueCmd::addObjects( const Q3PtrList<KPrObject> &objects )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj = dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPrPieObject *obj = dynamic_cast<KPrPieObject*>( it.current() );
            if( obj )
            {
                m_objects.append( obj );
                obj->incCmdRef();

                PieValues * pieValues = new PieValues;
                pieValues->pieType = obj->getPieType();
                pieValues->pieAngle = obj->getPieAngle();
                pieValues->pieLength = obj->getPieLength();
                m_oldValues.append( pieValues );
            }
        }
    }
}

void KPrPieValueCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        KPrPieObject* obj=dynamic_cast<KPrPieObject*>( it.current() );
        if(obj)
        {
            if (m_flags & Type)
                obj->setPieType( m_newValues.pieType );
            if (m_flags & Angle)
                obj->setPieAngle( m_newValues.pieAngle );
            if (m_flags & Length)
                obj->setPieLength( m_newValues.pieLength );
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrPieValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
    {
        KPrPieObject* obj=dynamic_cast<KPrPieObject*>( m_objects.at( i ) );
        if(obj)
        {
            obj->setPieType( m_oldValues.at( i )->pieType );
            obj->setPieAngle( m_oldValues.at( i )->pieAngle );
            obj->setPieLength( m_oldValues.at( i )->pieLength );
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrPolygonSettingCmd::KPrPolygonSettingCmd( const QString &name, PolygonSettings newSettings,
                                            Q3PtrList<KPrObject> &objects, KPrDocument *doc,
                                            KPrPage *page, int flags )
: KNamedCommand( name )
, m_doc( doc )
, m_page( page )
, m_newSettings( newSettings )
, m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldSettings.setAutoDelete( false );

    addObjects( objects );
}


KPrPolygonSettingCmd::~KPrPolygonSettingCmd()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldSettings.setAutoDelete( true );
    m_oldSettings.clear();
}

void KPrPolygonSettingCmd::addObjects( const Q3PtrList<KPrObject> &objects )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj = dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPrPolygonObject *obj = dynamic_cast<KPrPolygonObject*>( it.current() );
            if( obj )
            {
                m_objects.append( obj );
                obj->incCmdRef();

                PolygonSettings * polygonSettings = new PolygonSettings;

                polygonSettings->checkConcavePolygon = obj->getCheckConcavePolygon();
                polygonSettings->cornersValue = obj->getCornersValue();
                polygonSettings->sharpnessValue = obj->getSharpnessValue();

                m_oldSettings.append( polygonSettings );
            }
        }
    }
}

void KPrPolygonSettingCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        KPrPolygonObject * obj=dynamic_cast<KPrPolygonObject*>( it.current() );
        if(obj)
        {
            if (m_flags & ConcaveConvex)
                obj->setCheckConcavePolygon(m_newSettings.checkConcavePolygon);
            if (m_flags & Corners)
                obj->setCornersValue(m_newSettings.cornersValue);
            if (m_flags & Sharpness)
                obj->setSharpnessValue(m_newSettings.sharpnessValue );
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrPolygonSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); ++i )
    {
        KPrPolygonObject * obj=dynamic_cast<KPrPolygonObject*>( m_objects.at(i) );
        if(obj)
        {
            obj->setCheckConcavePolygon(m_oldSettings.at( i )->checkConcavePolygon);
            obj->setCornersValue(m_oldSettings.at( i )->cornersValue);
            obj->setSharpnessValue(m_oldSettings.at( i )->sharpnessValue);
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrPictureSettingCmd::KPrPictureSettingCmd( const QString &name, PictureSettings newSettings,
                                      Q3PtrList<KPrObject> &objects, KPrDocument *doc,
                                      KPrPage *page, int flags )
: KNamedCommand( name )
, m_doc( doc )
, m_newSettings( newSettings )
, m_page( page )
, m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );

    addObjects( objects );
}

KPrPictureSettingCmd::KPrPictureSettingCmd( const QString &_name, Q3PtrList<PictureSettings> &_oldSettings,
                                      PictureSettings _newSettings, Q3PtrList<KPrObject> &_objects,
                                      KPrDocument *_doc, int flags )
    : KNamedCommand( _name ), m_oldValues( _oldSettings ), m_objects( _objects ), m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );
    m_doc = _doc;
    m_newSettings = _newSettings;

    m_page = m_doc->findPage( m_objects );

    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrPictureSettingCmd::~KPrPictureSettingCmd()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldValues.setAutoDelete( true );
    m_oldValues.clear();
}

void KPrPictureSettingCmd::addObjects( const Q3PtrList<KPrObject> &objects )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj = dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPrPixmapObject *obj = dynamic_cast<KPrPixmapObject*>( it.current() );
            if( obj )
            {
                m_objects.append( obj );
                obj->incCmdRef();

                PictureSettings * pictureSettings = new PictureSettings;

                pictureSettings->mirrorType = obj->getPictureMirrorType();
                pictureSettings->depth = obj->getPictureDepth();
                pictureSettings->swapRGB = obj->getPictureSwapRGB();
                pictureSettings->grayscal = obj->getPictureGrayscal();
                pictureSettings->bright = obj->getPictureBright();

                m_oldValues.append( pictureSettings );
            }
        }
    }
}

void KPrPictureSettingCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it ) {
        KPrPixmapObject * obj = dynamic_cast<KPrPixmapObject*>( it.current() );
        if ( obj ) {
            if ( m_flags & MirrorType )
                obj->setPictureMirrorType( m_newSettings.mirrorType );
            if ( m_flags & Depth )
                obj->setPictureDepth( m_newSettings.depth );
            if ( m_flags & SwapRGB )
                obj->setPictureSwapRGB( m_newSettings.swapRGB );
            if ( m_flags & Grayscal )
                obj->setPictureGrayscal( m_newSettings.grayscal );
            if ( m_flags & Bright )
                obj->setPictureBright( m_newSettings.bright );
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrPictureSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); ++i ) {
        KPrPixmapObject * obj = dynamic_cast<KPrPixmapObject*>( m_objects.at(i) );
        if ( obj ) {
            PictureSettings *pictureSettings = m_oldValues.at( i );
            obj->setPictureMirrorType( pictureSettings->mirrorType );
            obj->setPictureDepth( pictureSettings->depth );
            obj->setPictureSwapRGB( pictureSettings->swapRGB );
            obj->setPictureGrayscal( pictureSettings->grayscal );
            obj->setPictureBright( pictureSettings->bright );
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrRectValueCmd::KPrRectValueCmd( const QString &_name, Q3PtrList<RectValues> &_oldValues, RectValues _newValues,
                            Q3PtrList<KPrObject> &_objects, KPrDocument *_doc, KPrPage *_page, int _flags )
    : KNamedCommand( _name ), m_oldValues( _oldValues ), m_objects( _objects ), m_flags(_flags)
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );
    m_doc = _doc;
    m_page = _page;
    m_newValues = _newValues;

    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}


KPrRectValueCmd::KPrRectValueCmd( const QString &name, Q3PtrList<KPrObject> &objects, RectValues newValues,
                            KPrDocument *doc, KPrPage *page, int flags )
: KNamedCommand( name )
, m_doc( doc )
, m_page( page )
, m_newValues( newValues )
, m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );

    addObjects( objects );
}


KPrRectValueCmd::~KPrRectValueCmd()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    m_oldValues.setAutoDelete( true );
    m_oldValues.clear();
}


void KPrRectValueCmd::addObjects( const Q3PtrList<KPrObject> &objects )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj = dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPrRectObject *obj = dynamic_cast<KPrRectObject*>( it.current() );
            if( obj )
            {
                m_objects.append( obj );
                obj->incCmdRef();

                RectValues * rectValue = new RectValues;

                int xtmp, ytmp;
                obj->getRnds( xtmp, ytmp );
                rectValue->xRnd = xtmp;
                rectValue->yRnd = ytmp;

                m_oldValues.append( rectValue );
            }
        }
    }
}


void KPrRectValueCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        KPrRectObject *obj = dynamic_cast<KPrRectObject*>( it.current() );
        if( obj )
        {
            int xtmp, ytmp;
            obj->getRnds( xtmp, ytmp );

            if ( m_flags & XRnd )
            {
                xtmp = m_newValues.xRnd;
            }

            if ( m_flags & YRnd )
            {
                ytmp = m_newValues.yRnd;
            }

            obj->setRnds( xtmp, ytmp );
        }
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrRectValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
    {
        KPrRectObject *obj = dynamic_cast<KPrRectObject*>( m_objects.at( i ) );

        if( obj )
            obj->setRnds( m_oldValues.at( i )->xRnd, m_oldValues.at( i )->yRnd );
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrResizeCmd::KPrResizeCmd( const QString &_name, const KoPoint &_m_diff, const KoSize &_r_diff,
                      KPrObject *_object, KPrDocument *_doc )
    : KNamedCommand( _name ), m_diff( _m_diff ), r_diff( _r_diff )
{
    object = _object;
    doc = _doc;
    m_page = doc->findPage( object );

    object->incCmdRef();
}

KPrResizeCmd::~KPrResizeCmd()
{
    object->decCmdRef();
}

void KPrResizeCmd::execute()
{
    QRect oldRect;

    oldRect = doc->zoomHandler()->zoomRect( object->getRepaintRect() );
    object->moveBy( m_diff );
    object->resizeBy( r_diff );
    if ( object->getType() == OT_TEXT )
    {
        if(object->isSelected())
            doc->updateRuler();
        doc->layout( object );
    }
    if ( object->isSelected())
        doc->updateObjectStatusBarItem();
    doc->repaint( oldRect );
    doc->repaint( object );

    doc->updateSideBarItem( m_page );
}

void KPrResizeCmd::unexecute()
{
    QRect oldRect;

    oldRect = doc->zoomHandler()->zoomRect( object->getRepaintRect() );
    object->moveBy( -m_diff.x(), -m_diff.y() );
    object->resizeBy( -r_diff.width(), -r_diff.height() );
    if ( object->getType() == OT_TEXT )
    {
        if(object->isSelected())
            doc->updateRuler();
        doc->layout( object );
    }
    if ( object->isSelected())
        doc->updateObjectStatusBarItem();

    doc->repaint( oldRect );
    doc->repaint( object );

    doc->updateSideBarItem( m_page );
}


KPrOasisPasteTextCommand::KPrOasisPasteTextCommand( KoTextDocument *d, int parag, int idx,
                                const QByteArray &data )
    : KoTextDocCommand( d ), m_parag( parag ), m_idx( idx ), m_data( data ), m_oldParagLayout( 0 )
{
}

KoTextCursor * KPrOasisPasteTextCommand::execute( KoTextCursor *c )
{
    KoTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    //kDebug() << "KWOasisPasteCommand::execute m_parag=" << m_parag << " m_idx=" << m_idx
    //          << " firstParag=" << firstParag << " " << firstParag->paragId() << endl;
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    c->setParag( firstParag );
    c->setIndex( m_idx );
    
    QBuffer buffer( m_data );
    KoStore * store = KoStore::createStore( &buffer, KoStore::Read ); 

    if ( store->bad() || !store->hasFile( "content.xml" ) )
    {
        kError(33001) << "Invalid ZIP store in memory" << endl;
        if ( !store->hasFile( "content.xml" ) )
            kError(33001) << "No content.xml file" << endl;
        return c;
    }
    store->disallowNameExpansion();

    KoOasisStore oasisStore( store );
    QDomDocument contentDoc;
    QString errorMessage;
    bool ok = oasisStore.loadAndParse( "content.xml", contentDoc, errorMessage );
    if ( !ok ) {
        kError(33001) << "Error parsing content.xml: " << errorMessage << endl;
        return c;
    }

    KoOasisStyles oasisStyles;
    QDomDocument stylesDoc;
    (void)oasisStore.loadAndParse( "styles.xml", stylesDoc, errorMessage );
    // Load styles from style.xml
    oasisStyles.createStyleMap( stylesDoc, true );
    // Also load styles from content.xml
    oasisStyles.createStyleMap( contentDoc, false );

    QDomElement content = contentDoc.documentElement();

    QDomElement body ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    
    // We then want to use whichever element is the child of <office:body>,
    // whether that's <office:text> or <office:presentation> or whatever.
    QDomElement iter, realBody;
    forEachElement( iter, body ) {
        realBody = iter;
    }
    if ( realBody.isNull() ) {
        kError(33001) << "No element found inside office:body!" << endl;
        return c;
    }

    KPrTextDocument * textdoc = static_cast<KPrTextDocument *>(c->parag()->document());
    KPrDocument *doc = textdoc->textObject()->kPresenterDocument();
    KoOasisContext context( doc, *doc->getVariableCollection(), oasisStyles, store );

    *c = textdoc->textObject()->textObject()->pasteOasisText( realBody, context, cursor, doc->styleCollection() );
    textdoc->textObject()->textObject()->setNeedSpellCheck( true );

    m_lastParag = c->parag()->paragId();
    m_lastIndex = c->index();
    return c;
}

KoTextCursor * KPrOasisPasteTextCommand::unexecute( KoTextCursor *c )
{
    KoTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    doc->setSelectionStart( KoTextDocument::Temp, &cursor );

    KoTextParag *lastParag = doc->paragAt( m_lastParag );
    if ( !lastParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_lastParag, doc->lastParag()->paragId() );
        return 0;
    }
    //Q_ASSERT( lastParag->document() );
    // Get hold of the document before deleting the parag
    //KoTextDocument* textdoc = lastParag->document();

    //kDebug() << "Undoing paste: deleting from (" << firstParag->paragId() << "," << m_idx << ")"
    //          << " to (" << lastParag->paragId() << "," << m_lastIndex << ")" << endl;

    cursor.setParag( lastParag );
    cursor.setIndex( m_lastIndex );
    doc->setSelectionEnd( KoTextDocument::Temp, &cursor );
    doc->removeSelectedText( KoTextDocument::Temp, c /* sets c to the correct position */ );

    if ( m_idx == 0 ) {
        Q_ASSERT( m_oldParagLayout );
        if ( m_oldParagLayout )
            firstParag->setParagLayout( *m_oldParagLayout );
    }
    return c;
}


KPrChangeStartingPageCommand::KPrChangeStartingPageCommand( const QString &name, KPrDocument *_doc,
                                                            int _oldStartingPage, int _newStartingPage):
    KNamedCommand(name),
    m_doc(_doc),
    oldStartingPage(_oldStartingPage),
    newStartingPage(_newStartingPage)
{
}

void KPrChangeStartingPageCommand::execute()
{
    m_doc->getVariableCollection()->variableSetting()->setStartingPageNumber(newStartingPage);
    m_doc->recalcVariables( VT_PGNUM );
}

void KPrChangeStartingPageCommand::unexecute()
{
    m_doc->getVariableCollection()->variableSetting()->setStartingPageNumber(oldStartingPage);
    m_doc->recalcVariables( VT_PGNUM );
}


KPrChangeVariableSettingsCommand::KPrChangeVariableSettingsCommand( const QString &name, KPrDocument *_doc,
                                                                    bool _oldValue, bool _newValue,
                                                                    VariableProperties _type):
    KNamedCommand(name),
    m_doc(_doc),
    type(_type),
    m_bOldValue(_oldValue),
    m_bNewValue(_newValue)
{
}

void KPrChangeVariableSettingsCommand::changeValue( bool b )
{
    switch(type)
    {
    case VS_DISPLAYLINK:
        m_doc->getVariableCollection()->variableSetting()->setDisplayLink(b);
        m_doc->recalcVariables( VT_LINK );
        break;
    case  VS_UNDERLINELINK:
        m_doc->getVariableCollection()->variableSetting()->setUnderlineLink(b);
        m_doc->recalcVariables( VT_LINK );
        break;
    case VS_DISPLAYCOMMENT:
        m_doc->getVariableCollection()->variableSetting()->setDisplayComment(b);
        m_doc->recalcVariables( VT_NOTE );
        break;
    case VS_DISPLAYFIELDCODE:
        m_doc->getVariableCollection()->variableSetting()->setDisplayFieldCode(b);
        m_doc->recalcVariables( VT_ALL );
        break;
    }
}

void KPrChangeVariableSettingsCommand::execute()
{
    changeValue(m_bNewValue);
}

void KPrChangeVariableSettingsCommand::unexecute()
{
    changeValue(m_bOldValue);
}

KPrDeletePageCmd::KPrDeletePageCmd( const QString &name, int pageNum, KPrDocument *doc )
: KNamedCommand( name )
, m_doc( doc )
, m_pageNum( pageNum )
{
    m_page = m_doc->pageList().at( m_pageNum );
}

KPrDeletePageCmd::~KPrDeletePageCmd()
{
}

void KPrDeletePageCmd::execute()
{
    m_doc->deSelectAllObj();
    m_doc->takePage( m_page, qMax( m_pageNum - 1, 0 ) );
    m_doc->updatePresentationButton();
}

void KPrDeletePageCmd::unexecute()
{
    m_doc->deSelectAllObj();
    m_doc->insertPage( m_page, qMax( m_pageNum - 1, 0 ), m_pageNum );
    m_doc->updatePresentationButton();
}

KPrInsertPageCmd::KPrInsertPageCmd( const QString &name, int pageNum, InsertPos pos,
                                    KPrPage *page, KPrDocument *doc )
: KNamedCommand(name)
, m_doc( doc )
, m_page( page )
, m_currentPageNum( pageNum )
, m_insertPageNum( 0 )
{
    switch( pos )
    {
        case IP_BEFORE:
            m_insertPageNum = m_currentPageNum;
            break;
        case IP_AFTER:
            m_insertPageNum = m_currentPageNum + 1;
            break;
    }
}

KPrInsertPageCmd::~KPrInsertPageCmd()
{
}

void KPrInsertPageCmd::execute()
{
    m_doc->deSelectAllObj();
    m_doc->insertPage( m_page, m_currentPageNum, m_insertPageNum );
    m_page->completeLoading( false, -1 );
    m_doc->updatePresentationButton();
}

void KPrInsertPageCmd::unexecute()
{
    m_doc->deSelectAllObj();
    m_doc->takePage( m_page, m_currentPageNum );
    m_doc->updatePresentationButton();
}

KPrMovePageCmd::KPrMovePageCmd( const QString &_name,int from, int to, KPrDocument *_doc ) :
    KNamedCommand( _name ),
    m_doc( _doc ),
    m_oldPosition( from ),
    m_newPosition( to )
{
}

KPrMovePageCmd::~KPrMovePageCmd()
{
}

void KPrMovePageCmd::execute()
{
    m_doc->deSelectAllObj();
    m_doc->movePageTo( m_oldPosition, m_newPosition );
}

void KPrMovePageCmd::unexecute()
{
    m_doc->deSelectAllObj();
    m_doc->movePageTo( m_newPosition, m_oldPosition );
}


KPrChangeTitlePageNameCommand::KPrChangeTitlePageNameCommand( const QString &_name,KPrDocument *_doc,
                                                              const QString &_oldPageName,
                                                              const QString &_newPageName, KPrPage *_page ) :
    KNamedCommand(_name),
    m_doc(_doc),
    oldPageName(_oldPageName),
    newPageName(_newPageName),
    m_page(_page)
{
}

void KPrChangeTitlePageNameCommand::execute()
{
    m_page->insertManualTitle(newPageName);
    m_doc->updateSideBarItem( m_page );
    m_doc->recalcVariables( VT_PGNUM );
}

void KPrChangeTitlePageNameCommand::unexecute()
{
    m_page->insertManualTitle(oldPageName);
    m_doc->updateSideBarItem( m_page );
    m_doc->recalcVariables( VT_PGNUM );
}

KPrChangeCustomVariableValue::KPrChangeCustomVariableValue( const QString &name, KPrDocument *_doc,
                                                            const QString & _oldValue, const QString & _newValue,
                                                            KoCustomVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

void KPrChangeCustomVariableValue::execute()
{
    Q_ASSERT(m_var);
    m_var->setValue(newValue);
    m_doc->recalcVariables( VT_CUSTOM );
}

void KPrChangeCustomVariableValue::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setValue(oldValue);
    m_doc->recalcVariables( VT_CUSTOM );
}

KPrChangeLinkVariable::KPrChangeLinkVariable( const QString &name, KPrDocument *_doc,
                                              const QString & _oldHref, const QString & _newHref,
                                              const QString & _oldLink,const QString &_newLink,
                                              KoLinkVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    oldHref(_oldHref),
    newHref(_newHref),
    oldLink(_oldLink),
    newLink(_newLink),
    m_var(var)
{
}


void KPrChangeLinkVariable::execute()
{
    m_var->setLink(newLink,newHref);
    m_doc->recalcVariables(VT_LINK);
}

void KPrChangeLinkVariable::unexecute()
{
    m_var->setLink(oldLink,oldHref);
    m_doc->recalcVariables(VT_LINK);
}


KPrNameObjectCommand::KPrNameObjectCommand( const QString &_name, const QString &_objectName,
                                            KPrObject *_obj, KPrDocument *_doc ):
    KNamedCommand( _name ),
    newObjectName( _objectName ),
    object( _obj ),
    doc( _doc )
{
    oldObjectName = object->getObjectName();

    m_page = doc->findPage( object );
}

KPrNameObjectCommand::~KPrNameObjectCommand()
{
}

void KPrNameObjectCommand::execute()
{
    object->setObjectName( newObjectName );
    m_page->unifyObjectName( object );

    doc->updateSideBarItem( m_page );
}

void KPrNameObjectCommand::unexecute()
{
    object->setObjectName( oldObjectName );

    doc->updateSideBarItem( m_page );
}

KPrDisplayObjectFromMasterPage::KPrDisplayObjectFromMasterPage(const QString &name, KPrDocument *_doc, KPrPage *_page, bool _newValue)
    :KNamedCommand(name),
     m_doc( _doc ),
     m_page(_page),
     newValue(_newValue)
{
}

void KPrDisplayObjectFromMasterPage::execute()
{
    m_page->setDisplayObjectFromMasterPage( newValue );
    m_doc->updateSideBarItem( m_doc->masterPage() );
}

void KPrDisplayObjectFromMasterPage::unexecute()
{
    m_page->setDisplayObjectFromMasterPage( !newValue );
    m_doc->updateSideBarItem( m_doc->masterPage() );
}


KPrDisplayBackgroundPage::KPrDisplayBackgroundPage(const QString &name, KPrDocument *_doc, KPrPage *_page, bool _newValue)
    :KNamedCommand(name),
     m_doc( _doc ),
     m_page(_page),
     newValue(_newValue)
{
}

void KPrDisplayBackgroundPage::execute()
{
    m_page->setDisplayBackground( newValue );
    m_doc->updateSideBarItem( m_doc->masterPage() );
}

void KPrDisplayBackgroundPage::unexecute()
{
    m_page->setDisplayBackground( !newValue );
    m_doc->updateSideBarItem( m_doc->masterPage() );
}


KPrHideShowHeaderFooter::KPrHideShowHeaderFooter( const QString &name, KPrDocument *_doc, KPrPage *_page,
                                                  bool _newValue, KPrTextObject *_textObject):
    KNamedCommand(name),
    m_doc( _doc ),
    m_page(_page),
    m_textObject(_textObject),
    newValue(_newValue)
{
}


void KPrHideShowHeaderFooter::execute()
{
    if( m_textObject==m_doc->footer())
        m_page->setFooter( newValue );
    else if( m_textObject==m_doc->header())
        m_page->setHeader( newValue );
    else
        kDebug(33001)<<"Error in void KPrHideShowHeaderFooter::execute()\n";

    m_doc->updateSideBarItem( m_doc->masterPage() );
}

void KPrHideShowHeaderFooter::unexecute()
{
    if( m_textObject==m_doc->footer())
        m_page->setFooter( !newValue );
    else if( m_textObject==m_doc->header())
        m_page->setHeader( !newValue );
    else
        kDebug(33001)<<"Error in void KPrHideShowHeaderFooter::unexecute()\n";

    m_doc->updateSideBarItem( m_doc->masterPage() );
}

KPrFlipObjectCommand::KPrFlipObjectCommand( const QString &name, KPrDocument *_doc,
                                            bool _horizontal, Q3PtrList<KPrObject> &_objects ):
    KNamedCommand( name ),
    m_doc( _doc ),
    objects( _objects ),
    horizontal( _horizontal )
{
    objects.setAutoDelete( false );

    m_page = m_doc->findPage( objects );

    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrFlipObjectCommand::~KPrFlipObjectCommand()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrFlipObjectCommand::execute()
{
    flipObjects();
}

void KPrFlipObjectCommand::unexecute()
{
    flipObjects();
}

void KPrFlipObjectCommand::flipObjects()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->flip( horizontal );
        m_doc->repaint( it.current() );
    }

    m_doc->updateSideBarItem( m_page );
}


KPrGeometryPropertiesCommand::KPrGeometryPropertiesCommand( const QString &name, Q3PtrList<KPrObject> &objects,
                                                            bool newValue, KgpType type,KPrDocument *_doc )
: KNamedCommand( name )
, m_objects( objects )
, m_newValue( newValue )
, m_type( type )
    , m_doc( _doc )
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->incCmdRef();
        if ( m_type == ProtectSize )
            m_oldValue.append( it.current()->isProtect() );
        else if ( m_type == KeepRatio)
            m_oldValue.append( it.current()->isKeepRatio() );
    }
}

KPrGeometryPropertiesCommand::KPrGeometryPropertiesCommand( const QString &name, Q3ValueList<bool> &lst,
                                                            Q3PtrList<KPrObject> &objects, bool newValue,
                                                            KgpType type, KPrDocument *_doc)
: KNamedCommand( name )
, m_oldValue( lst )
, m_objects( objects )
, m_newValue( newValue )
, m_type( type )
, m_doc ( _doc )
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrGeometryPropertiesCommand::~KPrGeometryPropertiesCommand()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrGeometryPropertiesCommand::execute()
{
    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        if ( m_type == ProtectSize )
        {
            it.current()->setProtect( m_newValue );
            if ( it.current()->isSelected() )
                m_doc->repaint( it.current() );
        }
        else if ( m_type == KeepRatio)
            it.current()->setKeepRatio( m_newValue );
    }
}

void KPrGeometryPropertiesCommand::unexecute()
{
    KPrObject *obj = 0;
    for ( unsigned int i = 0; i < m_objects.count(); ++i ) {
        obj = m_objects.at( i );
        if ( m_type == ProtectSize )
        {
            obj->setProtect( *m_oldValue.at(i) );
            if ( obj->isSelected() )
                m_doc->repaint( obj );
        }
        else if ( m_type == KeepRatio)
            obj->setKeepRatio( *m_oldValue.at(i) );
    }
}

KPrProtectContentCommand::KPrProtectContentCommand( const QString &name, Q3PtrList<KPrObject> &objects,
                                                    bool protectContent, KPrDocument *doc )
: KNamedCommand( name )
, m_protectContent( protectContent )
, m_doc( doc )
{
    m_objects.setAutoDelete( false );

    addObjects( objects );
}

KPrProtectContentCommand::KPrProtectContentCommand( const QString &name, bool protectContent,
                                                    KPrTextObject *obj, KPrDocument *doc )
: KNamedCommand( name )
, m_protectContent( protectContent )
, m_doc( doc )
{
    obj->incCmdRef();
    m_objects.append( obj );
    m_oldValues.append( obj->isProtectContent() );
}

KPrProtectContentCommand::~KPrProtectContentCommand()
{
    Q3PtrListIterator<KPrTextObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrProtectContentCommand::addObjects( const Q3PtrList<KPrObject> &objects )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj = dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPrTextObject *obj = dynamic_cast<KPrTextObject*>( it.current() );
            if( obj )
            {
                m_objects.append( obj );
                obj->incCmdRef();

                m_oldValues.append( obj->isProtectContent() );
            }
        }
    }
}

void KPrProtectContentCommand::execute()
{
    Q3PtrListIterator<KPrTextObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setProtectContent( m_protectContent );
    }
    m_doc->updateObjectSelected();
    m_doc->updateRulerInProtectContentMode();

}

void KPrProtectContentCommand::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
    {
        m_objects.at( i )->setProtectContent( m_oldValues[i] );
    }
    m_doc->updateObjectSelected();
    m_doc->updateRulerInProtectContentMode();
}

KPrCloseObjectCommand::KPrCloseObjectCommand( const QString &name, Q3PtrList<KPrObject> objects, KPrDocument *doc )
: KNamedCommand( name )
, m_doc( doc )
, m_page( doc->findPage( objects.at( 0 ) ) )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        KPrPointObject * pointObject = dynamic_cast<KPrPointObject *>( *it );
        if ( pointObject )
        {
            m_openObjects.append( *it );
            ( *it )->incCmdRef();
            KPrClosedLineObject * closedObject = new KPrClosedLineObject( *pointObject );
            closedObject->incCmdRef();
            m_closedObjects.append( closedObject );
        }
    }
}

KPrCloseObjectCommand::~KPrCloseObjectCommand()
{
    Q3PtrListIterator<KPrObject> it( m_openObjects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    Q3PtrListIterator<KPrObject> it2( m_closedObjects );
    for ( ; it2.current() ; ++it2 )
        it2.current()->decCmdRef();
}

void KPrCloseObjectCommand::execute()
{
    Q3PtrListIterator<KPrObject> openIt( m_openObjects );
    Q3PtrListIterator<KPrObject> closeIt( m_closedObjects );
    for ( ; openIt.current() ; ++openIt, ++closeIt )
    {
        m_page->replaceObject( *openIt, *closeIt );
        bool selected = ( *openIt )->isSelected();
        ( *openIt )->removeFromObjList();
        ( *closeIt )->addToObjList();
        ( *openIt )->setSelected( false );
        ( *closeIt )->setSelected( selected );
        m_doc->repaint( *closeIt );
    }
    m_doc->updateSideBarItem( m_page );
}

void KPrCloseObjectCommand::unexecute()
{
    Q3PtrListIterator<KPrObject> openIt( m_openObjects );
    Q3PtrListIterator<KPrObject> closeIt( m_closedObjects );
    for ( ; openIt.current() ; ++openIt, ++closeIt )
    {
        m_page->replaceObject( *closeIt, *openIt );
        bool selected = ( *closeIt )->isSelected();
        ( *closeIt )->removeFromObjList();
        ( *openIt )->addToObjList();
        ( *closeIt )->setSelected( false );
        ( *openIt )->setSelected( selected );
        m_doc->repaint( *openIt );
    }
    m_doc->updateSideBarItem( m_page );
}

MarginsStruct::MarginsStruct( KPrTextObject *obj )
{
    topMargin = obj->bTop();
    bottomMargin= obj->bBottom();
    leftMargin = obj->bLeft();
    rightMargin= obj->bRight();
}

MarginsStruct::MarginsStruct( double _left, double _top, double _right, double _bottom ):
    topMargin(_top),
    bottomMargin(_bottom),
    leftMargin(_left),
    rightMargin(_right)
{
}


KPrChangeMarginCommand::KPrChangeMarginCommand( const QString &name, Q3PtrList<KPrObject> &objects,
                                                MarginsStruct newMargins, KPrDocument *doc,
                                                KPrPage *page )
: KNamedCommand( name )
, m_newMargins( newMargins )
, m_page( page )
, m_doc( doc )
{
    m_objects.setAutoDelete( false );
    m_oldMargins.setAutoDelete( false );

    addObjects( objects );
}


KPrChangeMarginCommand::~KPrChangeMarginCommand()
{
    Q3PtrListIterator<KPrTextObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldMargins.setAutoDelete( true );
    m_oldMargins.clear();
}


void KPrChangeMarginCommand::addObjects( const Q3PtrList<KPrObject> &objects )
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPrGroupObject * obj = dynamic_cast<KPrGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPrTextObject *obj = dynamic_cast<KPrTextObject*>( it.current() );
            if( obj )
            {
                m_objects.append( obj );
                obj->incCmdRef();

                m_oldMargins.append( new MarginsStruct( obj ) );
            }
        }
    }
}


void KPrChangeMarginCommand::execute()
{
    Q3PtrListIterator<KPrTextObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setTextMargins( m_newMargins.leftMargin, m_newMargins.topMargin,
                                      m_newMargins.rightMargin, m_newMargins.bottomMargin);
        it.current()->resizeTextDocument();
        it.current()->layout();
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}

void KPrChangeMarginCommand::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
    {
        KPrTextObject *object = m_objects.at( i );
        MarginsStruct *marginsStruct = m_oldMargins.at( i );
        object->setTextMargins( marginsStruct->leftMargin, marginsStruct->topMargin,
                                marginsStruct->rightMargin, marginsStruct->bottomMargin);
        object->resizeTextDocument();
        object->layout();
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrChangeVerticalAlignmentCommand::KPrChangeVerticalAlignmentCommand( const QString &name, KPrTextObject *_obj,
                                                                      VerticalAlignmentType _oldAlign,
                                                                      VerticalAlignmentType _newAlign,
                                                                      KPrDocument *_doc) :
    KNamedCommand(name),
    m_obj( _obj ),
    m_oldAlign(_oldAlign),
    m_newAlign(_newAlign),
    m_doc( _doc )
{
    m_page = m_doc->findPage( _obj );
}

void KPrChangeVerticalAlignmentCommand::execute()
{
    m_obj->setVerticalAligment( m_newAlign );
    m_obj->kPresenterDocument()->layout(m_obj);
    m_obj->kPresenterDocument()->repaint(m_obj);

    m_doc->updateSideBarItem( m_page );
}

void KPrChangeVerticalAlignmentCommand::unexecute()
{
    m_obj->setVerticalAligment( m_oldAlign );
    m_obj->kPresenterDocument()->layout(m_obj);
    m_obj->kPresenterDocument()->repaint(m_obj);

    m_doc->updateSideBarItem( m_page );
}


KPrChangeTabStopValueCommand::KPrChangeTabStopValueCommand( const QString &name, double _oldValue, double _newValue,
                                                            KPrDocument *_doc):
    KNamedCommand(name),
    m_doc( _doc ),
    m_oldValue(_oldValue),
    m_newValue(_newValue)
{
}

void KPrChangeTabStopValueCommand::execute()
{
    m_doc->setTabStopValue ( m_newValue );
}

void KPrChangeTabStopValueCommand::unexecute()
{
    m_doc->setTabStopValue ( m_oldValue );
}

KPrImageEffectCmd::KPrImageEffectCmd(const QString &_name, Q3PtrList<ImageEffectSettings> &_oldSettings,
                               ImageEffectSettings _newSettings, Q3PtrList<KPrObject> &_objects,
                               KPrDocument *_doc )
    :KNamedCommand( _name ), oldSettings( _oldSettings ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldSettings.setAutoDelete( false );
    doc = _doc;
    newSettings = _newSettings;

    m_page = doc->findPage( objects );

    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrImageEffectCmd::~KPrImageEffectCmd()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldSettings.setAutoDelete( true );
    oldSettings.clear();
}

void KPrImageEffectCmd::execute()
{
    Q3PtrListIterator<KPrObject> it( objects );
    for ( ; it.current() ; ++it ) {
        KPrPixmapObject * obj = dynamic_cast<KPrPixmapObject*>( it.current() );
        if ( obj ) {
            obj->setImageEffect(newSettings.effect);
            obj->setIEParams(newSettings.param1, newSettings.param2, newSettings.param3);
        }
    }
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

void KPrImageEffectCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
        KPrPixmapObject * obj = dynamic_cast<KPrPixmapObject*>( objects.at(i) );
        if ( obj ) {
            obj->setImageEffect(oldSettings.at( i )->effect);
            obj->setIEParams(oldSettings.at( i )->param1, oldSettings.at( i )->param2,
                             oldSettings.at( i )->param3);
        }
    }
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

KPrChangeVariableNoteText::KPrChangeVariableNoteText( const QString &name, KPrDocument *_doc,
                        const QString &_oldValue,const QString &_newValue,
                        KoNoteVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangeVariableNoteText::~KPrChangeVariableNoteText()
{
}

void KPrChangeVariableNoteText::execute()
{
    Q_ASSERT(m_var);
    m_var->setNote(newValue);
}

void KPrChangeVariableNoteText::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setNote(oldValue);
}
