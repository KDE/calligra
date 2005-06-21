// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_doc.h"
#include "kprpage.h"
#include "kprcommand.h"
#include "kpbackground.h"
#include "kpgroupobject.h"


#include "kplineobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kpfreehandobject.h"
#include "kppolylineobject.h"
#include "kpquadricbeziercurveobject.h"
#include "kpcubicbeziercurveobject.h"
#include "kppolygonobject.h"
#include "kpclosedlineobject.h"

#include "kptextobject.h"
#include "kppixmapobject.h"

#include "kppartobject.h"
#include <koRuler.h>
#include "kppieobject.h"
#include "kprectobject.h"
#include "kpresenter_view.h"
#include "kotextobject.h"
#include "kprtextdocument.h"
#include <kdebug.h>
#include "kprvariable.h"
#include <koRect.h>
#include <koSize.h>
#include <koPoint.h>
#include <kodom.h>
#include <kotextparag.h>
#include <koxmlns.h>

#include <qxml.h>
#include <qbuffer.h>


ShadowCmd::ShadowCmd( const QString &_name, QPtrList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
                      QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldShadow( _oldShadow ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldShadow.setAutoDelete( false );
    doc = _doc;
    newShadow = _newShadow;

    m_page = doc->findPage( objects );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

ShadowCmd::~ShadowCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldShadow.setAutoDelete( true );
    oldShadow.clear();
}

void ShadowCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->setShadowParameter(newShadow.shadowDistance,
                                         newShadow.shadowDirection,
                                         newShadow.shadowColor);
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

void ShadowCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->setShadowParameter(oldShadow.at(i)->shadowDistance,
                                            oldShadow.at(i)->shadowDirection,
                                            oldShadow.at(i)->shadowColor);
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}


SetOptionsCmd::SetOptionsCmd( const QString &_name, QValueList<KoPoint> &_diffs, QPtrList<KPObject> &_objects,
                              double _gridX, double _gridY, double _oldGridX, double _oldGridY,
                              const QColor &_txtBackCol, const QColor &_otxtBackCol, KPresenterDoc *_doc )
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
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

SetOptionsCmd::~SetOptionsCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void SetOptionsCmd::execute()
{
    // ## use iterator
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( *diffs.at( i ) );
    doc->setGridValue( gridX, gridY, false );
    doc->updateRuler();
    doc->setTxtBackCol( txtBackCol );
    doc->repaint( false );
}

void SetOptionsCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( -(*diffs.at( i )).x(), -(*diffs.at( i )).y() );
    doc->setGridValue( oldGridX, oldGridY, false );
    doc->updateRuler();
    doc->setTxtBackCol( otxtBackCol );
    doc->repaint( false );
}

SetBackCmd::SetBackCmd( const QString &name, const KPBackGround::Settings &settings,
                        const KPBackGround::Settings &oldSettings,
                        bool useMasterBackground,
                        bool takeGlobal, KPresenterDoc *doc, KPrPage *page )
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

void SetBackCmd::execute()
{
    if ( !m_takeGlobal ) {
        m_page->background()->setBackGround( m_settings );
        m_page->setUseMasterBackground( m_useMasterBackground );
        m_doc->restoreBackground( m_page );
    } else {
        QPtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( ; it.current() ; ++it )
        {
            it.current()->background()->setBackGround( m_settings );
            it.current()->setUseMasterBackground( m_useMasterBackground );
            m_doc->restoreBackground(it.current());
        }

    }
    m_doc->repaint( false );

    if ( m_takeGlobal ) {
        QPtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( int pos = 0; it.current(); ++it, ++pos ) {
            m_doc->updateSideBarItem( it.current() );
        }
    }
    else {
        m_doc->updateSideBarItem( m_page );
    }
}

void SetBackCmd::unexecute()
{
    if ( !m_takeGlobal ) {
        m_page->background()->setBackGround( m_oldSettings );
        m_page->setUseMasterBackground( m_oldUseMasterBackground );
        m_doc->restoreBackground( m_page );
    } else {
        QPtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( ; it.current() ; ++it )
        {
            it.current()->background()->setBackGround( m_oldSettings );
            it.current()->setUseMasterBackground( m_oldUseMasterBackground );
            m_doc->restoreBackground(it.current());
        }
    }
    m_doc->repaint( false );

    if ( m_takeGlobal ) {
        QPtrListIterator<KPrPage> it( m_doc->getPageList() );
        for ( int pos = 0; it.current(); ++it, ++pos ) {
            m_doc->updateSideBarItem( it.current() );
        }
    }
    else {
        m_doc->updateSideBarItem( m_page );
    }
}

RotateCmd::RotateCmd( const QString &_name, float newAngle, QPtrList<KPObject> &objects,
                      KPresenterDoc *doc, bool addAngle )
    : KNamedCommand( _name ), m_doc( doc ), m_newAngle( newAngle ), m_addAngle( addAngle )
{
    m_objects.setAutoDelete( false );
    m_oldAngles.setAutoDelete( false );

    QPtrListIterator<KPObject> it( objects );
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

RotateCmd::~RotateCmd()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldAngles.setAutoDelete( true );
    m_oldAngles.clear();
}

void RotateCmd::execute()
{
    QPtrListIterator<KPObject> it( m_objects );
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

void RotateCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
        m_objects.at(i)->rotate( m_oldAngles.at( i )->angle );
    m_doc->updateRuler();
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


ChgPixCmd::ChgPixCmd( const QString &_name, KPPixmapObject *_oldObject, KPPixmapObject *_newObject,
                      KPresenterDoc *_doc, KPrPage *_page)
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

ChgPixCmd::~ChgPixCmd()
{
    oldObject->decCmdRef();
    newObject->decCmdRef();
}

void ChgPixCmd::execute()
{
    m_page->replaceObject( oldObject, newObject );
    doc->repaint( newObject );

    doc->updateSideBarItem( m_page );
}

void ChgPixCmd::unexecute()
{
    m_page->replaceObject( newObject, oldObject );
    doc->repaint( oldObject );

    doc->updateSideBarItem( m_page );
}

DeleteCmd::DeleteCmd( const QString &_name, QPtrList<KPObject> &_objects,
                      KPresenterDoc *_doc, KPrPage *_page )
: KNamedCommand( _name )
, m_oldObjectList( _page->objectList() )
, m_objectsToDelete( _objects )
, m_doc( _doc )
, m_page( _page )
{
    QPtrListIterator<KPObject> it( m_oldObjectList );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

DeleteCmd::~DeleteCmd()
{
    QPtrListIterator<KPObject> it( m_oldObjectList );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void DeleteCmd::execute()
{
    bool textObj=false;

    QPtrListIterator<KPObject> it( m_oldObjectList );
    QPtrListIterator<KPObject> itDelete( m_objectsToDelete );
    QPtrList<KPObject> newObjectList;
    for ( ; it.current(); ++it )
    {
        if ( it.current() == itDelete.current() )
        {
            it.current()->setSelected( false );
            it.current()->removeFromObjList();

            if ( !textObj && it.current()->getType() == OT_TEXT )
            {
                KPTextObject * tmp = dynamic_cast<KPTextObject *>( it.current() );
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
        QRect oldRect = m_doc->zoomHandler()->zoomRect( itDelete.current()->getBoundingRect() );
        m_doc->repaint( oldRect );
        //m_doc->repaint( objects.at( i ) );
    }
    if(textObj)
        m_doc->updateRuler();

    m_doc->updateSideBarItem( m_page );
}

void DeleteCmd::unexecute()
{
    m_page->setObjectList( m_oldObjectList );
    QPtrListIterator<KPObject> it( m_objectsToDelete );
    for ( ; it.current(); ++it )
    {
        it.current()->addToObjList();
        m_doc->repaint( it.current() );
    }

    m_doc->updateSideBarItem( m_page );
}


EffectCmd::EffectCmd( const QString &_name, const QPtrList<KPObject> &_objs,
                      const QValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect )
    : KNamedCommand( _name ), oldEffects( _oldEffects ),
      newEffect( _newEffect ), objs( _objs )
{
    QPtrListIterator<KPObject> it( objs );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

EffectCmd::~EffectCmd()
{
    QPtrListIterator<KPObject> it( objs );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void EffectCmd::execute()
{
    QPtrListIterator<KPObject> it( objs );
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

void EffectCmd::unexecute()
{
    KPObject *object = 0;
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

GroupObjCmd::GroupObjCmd( const QString &_name,
                          const QPtrList<KPObject> &_objects,
                          KPresenterDoc *_doc, KPrPage *_page )
: KNamedCommand( _name )
, m_objectsToGroup( _objects )
, m_oldObjectList( _page->objectList() )
, m_doc( _doc )
, m_page( _page )
{
    m_groupObject = new KPGroupObject( m_objectsToGroup );
    m_groupObject->incCmdRef();
}

GroupObjCmd::~GroupObjCmd()
{
    m_groupObject->decCmdRef();
}

void GroupObjCmd::execute()
{
    KoRect r;
    int position = 0;
    QPtrListIterator<KPObject> it( m_objectsToGroup );
    for ( ; it.current() ; ++it )
    {
        it.current()->setSelected( false );
        position = m_page->takeObject(it.current() );
        r |= it.current()->getBoundingRect();
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

void GroupObjCmd::unexecute()
{
    m_groupObject->setUpdateObjects( false );

    m_page->setObjectList( m_oldObjectList );
    m_groupObject->removeFromObjList();

    QPtrListIterator<KPObject> it( m_objectsToGroup );
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
                              KPGroupObject *grpObj_,
                              KPresenterDoc *_doc, KPrPage *_page )
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

    QPtrListIterator<KPObject> it( m_groupedObjects );
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
    QPtrListIterator<KPObject> it( m_groupedObjects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setSelected( false );
        position = m_page->takeObject( it.current() );
        r |= it.current()->getBoundingRect();
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

InsertCmd::InsertCmd( const QString &_name, KPObject *_object,
                      KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name )
{
    object = _object;
    doc = _doc;
    m_page=_page;
    object->incCmdRef();
}

InsertCmd::~InsertCmd()
{
    object->decCmdRef();
}

void InsertCmd::execute()
{
    m_page->appendObject( object );
    object->addToObjList();
    if ( object->getType() == OT_TEXT )
        doc->updateRuler();
    doc->repaint( object );

    doc->updateSideBarItem( m_page );
}

void InsertCmd::unexecute()
{
    QRect oldRect = doc->zoomHandler()->zoomRect(object->getBoundingRect());
    QPtrList<KPObject> list(m_page->objectList());
    if ( list.findRef( object ) != -1 ) {
        m_page->takeObject(  object );
        object->removeFromObjList();
        if ( object->getType() == OT_TEXT )
        {
            doc->terminateEditing( (KPTextObject*)object );
            ((KPTextObject*)object)->setEditingTextObj( false );
            doc->updateRuler();
        }
    }
    doc->repaint( oldRect );

    doc->updateSideBarItem( m_page );
}

LowerRaiseCmd::LowerRaiseCmd( const QString &_name, QPtrList<KPObject> _oldList,
                              QPtrList<KPObject> _newList, KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name )
{
    oldList = _oldList;
    newList = _newList;
    m_page=_page;
    oldList.setAutoDelete( false );
    newList.setAutoDelete( false );
    doc = _doc;

    QPtrListIterator<KPObject> it( oldList );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

LowerRaiseCmd::~LowerRaiseCmd()
{
    QPtrListIterator<KPObject> it( oldList );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void LowerRaiseCmd::execute()
{
    m_page->setObjectList( newList );
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

void LowerRaiseCmd::unexecute()
{
    m_page->setObjectList( oldList );
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}


MoveByCmd::MoveByCmd( const QString &_name, const KoPoint &_diff, QPtrList<KPObject> &_objects,
                      KPresenterDoc *_doc,KPrPage *_page )
    : KNamedCommand( _name ), diff( _diff ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->incCmdRef();
    }
}

MoveByCmd::~MoveByCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void MoveByCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect());
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
}

void MoveByCmd::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect());
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
}

AlignCmd::AlignCmd( const QString &_name, QPtrList<KPObject> &_objects, AlignType _at, KPresenterDoc *_doc )
    : KNamedCommand( _name ), doc(_doc)
{
    objects.setAutoDelete( false );
    diffs.setAutoDelete( true );
    m_page = doc->findPage( _objects );

    QPtrListIterator<KPObject> it( _objects );
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

AlignCmd::~AlignCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    diffs.clear();
}

void AlignCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect( objects.at( i )->getBoundingRect());
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

void AlignCmd::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect());
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

PenCmd::PenCmd( const QString &_name, QPtrList<KPObject> &_objects, Pen _newPen,
                KPresenterDoc *_doc, KPrPage *_page, int _flags )
: KNamedCommand(_name), doc(_doc), m_page( _page ), newPen(_newPen), flags(_flags)
{
    objects.setAutoDelete( false );
    oldPen.setAutoDelete( false );

    addObjects( _objects );
}

PenCmd::~PenCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldPen.setAutoDelete( true );
    oldPen.clear();
}

void PenCmd::execute()
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
            tmpPen.pen.setWidth( newPen.pen.width() );

        if ( flags & Style )
            tmpPen.pen.setStyle( newPen.pen.style() );

        applyPen( objects.at( i ), &tmpPen );
    }

    doc->updateSideBarItem( m_page );
}

void PenCmd::applyPen( KPObject *object, Pen *tmpPen )
{
    switch (object->getType()) {
        case OT_LINE:
        {
            KPLineObject* obj=dynamic_cast<KPLineObject*>( object );
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
            KPPointObject *obj = dynamic_cast<KPPointObject*>( object );
            if ( obj )
            {
                //obj->setPen( tmpPen->pen );
                obj->setLineBegin( tmpPen->lineBegin );
                obj->setLineEnd( tmpPen->lineEnd );
                //doc->repaint( obj );
            }
        } break;
        default:
            break;
    }

    KPShadowObject *obj = dynamic_cast<KPShadowObject*>( object );
    if ( obj )
    {
        obj->setPen( tmpPen->pen );
        doc->repaint( obj );
    }
}

void PenCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
        if( oldPen.count() > i )
            applyPen( objects.at( i ), oldPen.at( i ) );
    }

    doc->updateSideBarItem( m_page );
}

void PenCmd::addObjects( const QPtrList<KPObject> &_objects )
{
    QPtrListIterator<KPObject> it( _objects );
    for ( ; it.current(); ++it )
    {
        KPObject * object( it.current() );
        if ( object->getType() == OT_GROUP )
        {
            KPGroupObject * obj=dynamic_cast<KPGroupObject*>( object );
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
                    KPLineObject *obj=dynamic_cast<KPLineObject*>( object );
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
                    KPPointObject *obj = dynamic_cast<KPPointObject*>( object );
                    if ( obj )
                    {
                        lineBegin = obj->getLineBegin();
                        lineEnd = obj->getLineEnd();
                    }
                } break;
                default:
                   break;
            }

            KPShadowObject *obj = dynamic_cast<KPShadowObject*>( object );
            if ( obj )
            {
                objects.append( obj );
                obj->incCmdRef();

                Pen * pen = new PenCmd::Pen( obj->getPen(), lineBegin, lineEnd );

                oldPen.append( pen );
            }
        }
    }
}


BrushCmd::BrushCmd( const QString &_name, QPtrList<KPObject> &_objects, Brush _newBrush,
                    KPresenterDoc *_doc, KPrPage *_page, int _flags )
: KNamedCommand(_name), doc(_doc), newBrush(_newBrush), m_page(_page), flags(_flags)
{
    objects.setAutoDelete( false );
    oldBrush.setAutoDelete( false );

    addObjects( _objects );
}

void BrushCmd::addObjects( const QPtrList<KPObject> &_objects )
{
    QPtrListIterator<KPObject> it( _objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj=dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KP2DObject * obj = dynamic_cast<KP2DObject *>( it.current() );
            if( obj )
            {
                objects.append( obj );
                obj->incCmdRef();

                Brush * brush = new BrushCmd::Brush;
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

BrushCmd::~BrushCmd()
{
    QPtrListIterator<KP2DObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldBrush.setAutoDelete( true );
    oldBrush.clear();
}

void BrushCmd::execute()
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

void BrushCmd::applyBrush( KP2DObject *object, Brush *tmpBrush )
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

void BrushCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        applyBrush( objects.at( i ), oldBrush.at( i ) );
    }

    doc->updateSideBarItem( m_page );
}


PgConfCmd::PgConfCmd( const QString &_name, bool _manualSwitch, bool _infiniteLoop,
                      bool _showPresentationDuration, QPen _pen,
                      QValueList<bool> _selectedSlides, const QString & _presentationName,
                      bool _oldManualSwitch, bool _oldInfiniteLoop,
                      bool _oldShowPresentationDuration, QPen _oldPen,
                      QValueList<bool> _oldSelectedSlides, const QString & _oldPresentationName,
                      KPresenterDoc *_doc )
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

void PgConfCmd::execute()
{
    doc->setManualSwitch( manualSwitch );
    doc->setInfiniteLoop( infiniteLoop );
    doc->setPresentationDuration( showPresentationDuration );
    doc->setPresPen( pen );
    doc->setPresentationName( presentationName );
    QPtrList<KPrPage> pages = doc->pageList();
    unsigned count = selectedSlides.count();
    if( count > pages.count() ) count = pages.count();
    for( unsigned i = 0; i < selectedSlides.count(); i++ )
        pages.at( i )->slideSelected( selectedSlides[ i ] );
}

void PgConfCmd::unexecute()
{
    doc->setManualSwitch( oldManualSwitch );
    doc->setInfiniteLoop( oldInfiniteLoop );
    doc->setPresentationDuration( oldShowPresentationDuration );
    doc->setPresPen( oldPen );
    doc->setPresentationName( oldPresentationName );

    QPtrList<KPrPage> pages = doc->pageList();
    unsigned count = oldSelectedSlides.count();
    if( count > pages.count() ) count = pages.count();
    for( unsigned i = 0; i < oldSelectedSlides.count(); i++ )
        pages.at( i )->slideSelected( oldSelectedSlides[ i ] );
}


TransEffectCmd::TransEffectCmd( QValueVector<PageEffectSettings> oldSettings,
                                PageEffectSettings newSettings,
                                KPrPage* page, KPresenterDoc* doc )
{
    m_newSettings = newSettings;
    m_oldSettings = oldSettings;
    Q_ASSERT( !m_oldSettings.isEmpty() );
    m_page = page;
    m_doc = doc;
}

void TransEffectCmd::PageEffectSettings::applyTo( KPrPage *page )
{
    page->setPageEffect( pageEffect );
    page->setPageEffectSpeed( effectSpeed );
    page->setPageSoundEffect( soundEffect );
    page->setPageSoundFileName( soundFileName );
    // TODO page->setAutoAdvance( autoAdvance );
    page->setPageTimer( slideTime );
}

void TransEffectCmd::execute()
{
    if ( m_page )
        m_newSettings.applyTo( m_page );
    else
        for( QPtrListIterator<KPrPage> it( m_doc->getPageList() ); *it; ++it )
            m_newSettings.applyTo( it.current() );
}

void TransEffectCmd::unexecute()
{
    if ( m_page )
        m_oldSettings[0].applyTo( m_page );
    else {
        int i = 0;
        for( QPtrListIterator<KPrPage> it( m_doc->getPageList() ); *it; ++it, ++i )
            m_oldSettings[i].applyTo( it.current() );
    }
}

QString TransEffectCmd::name() const
{
    if ( m_page )
        return i18n( "Modify Slide Transition" );
    else
        return i18n( "Modify Slide Transition For All Pages" );
}

PgLayoutCmd::PgLayoutCmd( const QString &_name, KoPageLayout _layout, KoPageLayout _oldLayout,
                          KoUnit::Unit _oldUnit, KoUnit::Unit _unit,KPresenterDoc *_doc )
    : KNamedCommand( _name )
{
    m_doc=_doc;
    layout = _layout;
    oldLayout = _oldLayout;
    oldUnit = _oldUnit;
    unit = _unit;
}

void PgLayoutCmd::execute()
{
    m_doc->setUnit( unit );
    m_doc->setPageLayout( layout );
    m_doc->updateHeaderFooterPosition();
    m_doc->updateRuler();
    m_doc->updateRulerPageLayout();
}

void PgLayoutCmd::unexecute()
{
    m_doc->setUnit( oldUnit );
    m_doc->setPageLayout( oldLayout );
    m_doc->updateHeaderFooterPosition();
    m_doc->updateRuler();
    m_doc->updateRulerPageLayout();
}


PieValueCmd::PieValueCmd( const QString &name, PieValues newValues,
                          QPtrList<KPObject> &objects, KPresenterDoc *doc,
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

PieValueCmd::PieValueCmd( const QString &_name, QPtrList<PieValues> &_oldValues, PieValues _newValues,
                          QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page, int _flags )
    : KNamedCommand( _name ), m_oldValues( _oldValues ), m_objects( _objects ), m_flags(_flags)
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );
    m_doc = _doc;
    m_page = _page;
    m_newValues = _newValues;

    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

PieValueCmd::~PieValueCmd()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldValues.setAutoDelete( true );
    m_oldValues.clear();
}

void PieValueCmd::addObjects( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj = dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPPieObject *obj = dynamic_cast<KPPieObject*>( it.current() );
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

void PieValueCmd::execute()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        KPPieObject* obj=dynamic_cast<KPPieObject*>( it.current() );
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

void PieValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
    {
        KPPieObject* obj=dynamic_cast<KPPieObject*>( m_objects.at( i ) );
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


PolygonSettingCmd::PolygonSettingCmd( const QString &name, PolygonSettings newSettings,
                                      QPtrList<KPObject> &objects, KPresenterDoc *doc,
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


PolygonSettingCmd::PolygonSettingCmd( const QString &name, QPtrList<PolygonSettings> &oldSettings,
                                      PolygonSettings newSettings, QPtrList<KPObject> &objects,
                                      KPresenterDoc *doc, KPrPage *page, int flags )
: KNamedCommand( name )
, m_doc( doc )
, m_page( page )
, m_oldSettings( oldSettings )
, m_objects( objects )
, m_newSettings( newSettings )
, m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldSettings.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

PolygonSettingCmd::~PolygonSettingCmd()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldSettings.setAutoDelete( true );
    m_oldSettings.clear();
}

void PolygonSettingCmd::addObjects( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj = dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPPolygonObject *obj = dynamic_cast<KPPolygonObject*>( it.current() );
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

void PolygonSettingCmd::execute()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        KPPolygonObject * obj=dynamic_cast<KPPolygonObject*>( it.current() );
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

void PolygonSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); ++i )
    {
        KPPolygonObject * obj=dynamic_cast<KPPolygonObject*>( m_objects.at(i) );
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


PictureSettingCmd::PictureSettingCmd( const QString &name, PictureSettings newSettings,
                                      QPtrList<KPObject> &objects, KPresenterDoc *doc,
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

PictureSettingCmd::PictureSettingCmd( const QString &_name, QPtrList<PictureSettings> &_oldSettings,
                                      PictureSettings _newSettings, QPtrList<KPObject> &_objects,
                                      KPresenterDoc *_doc, int flags )
    : KNamedCommand( _name ), m_oldValues( _oldSettings ), m_objects( _objects ), m_flags( flags )
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );
    m_doc = _doc;
    m_newSettings = _newSettings;

    m_page = m_doc->findPage( m_objects );

    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

PictureSettingCmd::~PictureSettingCmd()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldValues.setAutoDelete( true );
    m_oldValues.clear();
}

void PictureSettingCmd::addObjects( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj = dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
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

void PictureSettingCmd::execute()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it ) {
        KPPixmapObject * obj = dynamic_cast<KPPixmapObject*>( it.current() );
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

void PictureSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); ++i ) {
        KPPixmapObject * obj = dynamic_cast<KPPixmapObject*>( m_objects.at(i) );
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


RectValueCmd::RectValueCmd( const QString &_name, QPtrList<RectValues> &_oldValues, RectValues _newValues,
                            QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page, int _flags )
    : KNamedCommand( _name ), m_oldValues( _oldValues ), m_objects( _objects ), m_flags(_flags)
{
    m_objects.setAutoDelete( false );
    m_oldValues.setAutoDelete( false );
    m_doc = _doc;
    m_page = _page;
    m_newValues = _newValues;

    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}


RectValueCmd::RectValueCmd( const QString &name, QPtrList<KPObject> &objects, RectValues newValues,
                            KPresenterDoc *doc, KPrPage *page, int flags )
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


RectValueCmd::~RectValueCmd()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    m_oldValues.setAutoDelete( true );
    m_oldValues.clear();
}


void RectValueCmd::addObjects( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj = dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPRectObject *obj = dynamic_cast<KPRectObject*>( it.current() );
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


void RectValueCmd::execute()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        KPRectObject *obj = dynamic_cast<KPRectObject*>( it.current() );
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

void RectValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < m_objects.count(); i++ )
    {
        KPRectObject *obj = dynamic_cast<KPRectObject*>( m_objects.at( i ) );

        if( obj )
            obj->setRnds( m_oldValues.at( i )->xRnd, m_oldValues.at( i )->yRnd );
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


ResizeCmd::ResizeCmd( const QString &_name, const KoPoint &_m_diff, const KoSize &_r_diff,
                      KPObject *_object, KPresenterDoc *_doc )
    : KNamedCommand( _name ), m_diff( _m_diff ), r_diff( _r_diff )
{
    object = _object;
    doc = _doc;
    m_page = doc->findPage( object );

    object->incCmdRef();
}

ResizeCmd::~ResizeCmd()
{
    object->decCmdRef();
}

void ResizeCmd::execute()
{
    QRect oldRect;

    oldRect = doc->zoomHandler()->zoomRect( object->getBoundingRect());
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

void ResizeCmd::unexecute()
{
    QRect oldRect;

    oldRect = doc->zoomHandler()->zoomRect(object->getBoundingRect());
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
                                const QCString & data )
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
    //kdDebug() << "KWOasisPasteCommand::execute m_parag=" << m_parag << " m_idx=" << m_idx
    //          << " firstParag=" << firstParag << " " << firstParag->paragId() << endl;
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    c->setParag( firstParag );
    c->setIndex( m_idx );
    QBuffer buffer( m_data );
    QXmlInputSource source( &buffer );
    QXmlSimpleReader reader;
    KoDocument::setupXmlReader( reader,true );
    QDomDocument domDoc;
    domDoc.setContent( &source, &reader );

    QDomElement content = domDoc.documentElement();

    QDomElement body ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( body.isNull() ) {
        kdError(30518) << "No office:body found!" << endl;
        return 0;
    }
    QDomElement tmpbody = KoDom::namedItemNS( body, KoXmlNS::office, "presentation" );
    if ( tmpbody.isNull() )
    {
        //find a better method to search body element
        tmpbody = KoDom::namedItemNS( body, KoXmlNS::office, "text" );
        if ( tmpbody.isNull() ) {
            kdError(30518) << "No office:text found!" << endl;
            return 0;
        }

    }
    KPrTextDocument * textdoc = static_cast<KPrTextDocument *>(c->parag()->document());

    KoOasisStyles oasisStyles;
    oasisStyles.createStyleMap( domDoc );
    KPresenterDoc *doc = textdoc->textObject()->kPresenterDocument();
    KoOasisContext context( doc, *doc->getVariableCollection(), oasisStyles, 0 /*TODO store*/ );
    *c = textdoc->textObject()->textObject()->pasteOasisText( tmpbody, context, cursor, doc->styleCollection() );
    textdoc->textObject()->textObject()->setNeedSpellCheck( true );
    // In case loadFormatting queued any image request


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
    Q_ASSERT( lastParag->document() );
    // Get hold of the document before deleting the parag
    KoTextDocument* textdoc = lastParag->document();

    //kdDebug() << "Undoing paste: deleting from (" << firstParag->paragId() << "," << m_idx << ")"
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


KPrChangeStartingPageCommand::KPrChangeStartingPageCommand( const QString &name, KPresenterDoc *_doc,
                                                            int _oldStartingPage, int _newStartingPage):
    KNamedCommand(name),
    m_doc(_doc),
    oldStartingPage(_oldStartingPage),
    newStartingPage(_newStartingPage)
{
}

void KPrChangeStartingPageCommand::execute()
{
    m_doc->getVariableCollection()->variableSetting()->setStartingPage(newStartingPage);
    m_doc->recalcVariables( VT_PGNUM );
}

void KPrChangeStartingPageCommand::unexecute()
{
    m_doc->getVariableCollection()->variableSetting()->setStartingPage(oldStartingPage);
    m_doc->recalcVariables( VT_PGNUM );
}


KPrChangeVariableSettingsCommand::KPrChangeVariableSettingsCommand( const QString &name, KPresenterDoc *_doc,
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

KPrDeletePageCmd::KPrDeletePageCmd( const QString &name, int pageNum, KPresenterDoc *doc )
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
    m_doc->takePage( m_page, QMAX( m_pageNum - 1, 0 ) );
    m_doc->updatePresentationButton();
}

void KPrDeletePageCmd::unexecute()
{
    m_doc->deSelectAllObj();
    m_doc->insertPage( m_page, QMAX( m_pageNum - 1, 0 ), m_pageNum );
    m_doc->updatePresentationButton();
}

KPrInsertPageCmd::KPrInsertPageCmd( const QString &name, int pageNum, InsertPos pos,
                                    KPrPage *page, KPresenterDoc *doc )
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

KPrMovePageCmd::KPrMovePageCmd( const QString &_name,int from, int to, KPresenterDoc *_doc ) :
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


KPrChangeTitlePageNameCommand::KPrChangeTitlePageNameCommand( const QString &_name,KPresenterDoc *_doc,
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

KPrChangeCustomVariableValue::KPrChangeCustomVariableValue( const QString &name, KPresenterDoc *_doc,
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

KPrChangeLinkVariable::KPrChangeLinkVariable( const QString &name, KPresenterDoc *_doc,
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

KPrStickyObjCommand::KPrStickyObjCommand( const QString &_name, QPtrList<KPObject> &_objects,
                                          bool sticky, KPrPage*_page, KPresenterDoc *_doc )
    : KNamedCommand( _name ),
      objects( _objects ),
      m_bSticky(sticky)
{
    objects.setAutoDelete( false );
    m_doc = _doc;
    m_page=_page;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrStickyObjCommand::~KPrStickyObjCommand()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrStickyObjCommand::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        if(m_bSticky)
            stickObj(it.current());
        else
            unstickObj(it.current());
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_doc->masterPage() );
}

void KPrStickyObjCommand::unexecute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        if(m_bSticky)
            unstickObj(it.current());
        else
            stickObj(it.current());
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_doc->masterPage() );
}

void KPrStickyObjCommand::stickObj(KPObject *_obj)
{
    m_page->takeObject(_obj);
    m_doc->masterPage()->appendObject(_obj);
    _obj->setSticky(true);
}

void KPrStickyObjCommand::unstickObj(KPObject *_obj)
{
    m_doc->masterPage()->takeObject(_obj);
    m_page->appendObject(_obj);
    _obj->setSticky(false);
}

KPrNameObjectCommand::KPrNameObjectCommand( const QString &_name, const QString &_objectName,
                                            KPObject *_obj, KPresenterDoc *_doc ):
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

KPrDisplayObjectFromMasterPage::KPrDisplayObjectFromMasterPage(const QString &name, KPresenterDoc *_doc, KPrPage *_page, bool _newValue)
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


KPrHideShowHeaderFooter::KPrHideShowHeaderFooter( const QString &name, KPresenterDoc *_doc, KPrPage *_page,
                                                  bool _newValue, KPTextObject *_textObject):
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
        kdDebug(33001)<<"Error in void KPrHideShowHeaderFooter::execute()\n";

    m_doc->updateSideBarItem( m_doc->masterPage() );
}

void KPrHideShowHeaderFooter::unexecute()
{
    if( m_textObject==m_doc->footer())
        m_page->setFooter( !newValue );
    else if( m_textObject==m_doc->header())
        m_page->setHeader( !newValue );
    else
        kdDebug(33001)<<"Error in void KPrHideShowHeaderFooter::unexecute()\n";

    m_doc->updateSideBarItem( m_doc->masterPage() );
}

KPrFlipObjectCommand::KPrFlipObjectCommand( const QString &name, KPresenterDoc *_doc,
                                            bool _horizontal, QPtrList<KPObject> &_objects ):
    KNamedCommand( name ),
    m_doc( _doc ),
    objects( _objects ),
    horizontal( _horizontal )
{
    objects.setAutoDelete( false );

    m_page = m_doc->findPage( objects );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrFlipObjectCommand::~KPrFlipObjectCommand()
{
    QPtrListIterator<KPObject> it( objects );
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
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->flip( horizontal );
        m_doc->repaint( it.current() );
    }

    m_doc->updateSideBarItem( m_page );
}


KPrGeometryPropertiesCommand::KPrGeometryPropertiesCommand( const QString &name, QPtrList<KPObject> &objects,
                                                            bool newValue, KgpType type )
: KNamedCommand( name )
, m_objects( objects )
, m_newValue( newValue )
, m_type( type )
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->incCmdRef();
        if ( m_type == ProtectSize )
            m_oldValue.append( it.current()->isProtect() );
        else if ( m_type == KeepRatio)
            m_oldValue.append( it.current()->isKeepRatio() );
    }
}

KPrGeometryPropertiesCommand::KPrGeometryPropertiesCommand( const QString &name, QValueList<bool> &lst,
                                                            QPtrList<KPObject> &objects, bool newValue,
                                                            KgpType type)
: KNamedCommand( name )
, m_oldValue( lst )
, m_objects( objects )
, m_newValue( newValue )
, m_type( type )
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrGeometryPropertiesCommand::~KPrGeometryPropertiesCommand()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrGeometryPropertiesCommand::execute()
{
    QPtrListIterator<KPObject> it( m_objects );
    for ( ; it.current() ; ++it )
    {
        if ( m_type == ProtectSize )
            it.current()->setProtect( m_newValue );
        else if ( m_type == KeepRatio)
            it.current()->setKeepRatio( m_newValue );
    }
}

void KPrGeometryPropertiesCommand::unexecute()
{
    KPObject *obj = 0;
    for ( unsigned int i = 0; i < m_objects.count(); ++i ) {
        obj = m_objects.at( i );
        if ( m_type == ProtectSize )
            obj->setProtect( *m_oldValue.at(i) );
        else if ( m_type == KeepRatio)
            obj->setKeepRatio( *m_oldValue.at(i) );
    }
}

KPrProtectContentCommand::KPrProtectContentCommand( const QString &name, QPtrList<KPObject> &objects,
                                                    bool protectContent, KPresenterDoc *doc )
: KNamedCommand( name )
, m_protectContent( protectContent )
, m_doc( doc )
{
    m_objects.setAutoDelete( false );

    addObjects( objects );
}

KPrProtectContentCommand::KPrProtectContentCommand( const QString &name, bool protectContent,
                                                    KPTextObject *obj, KPresenterDoc *doc )
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
    QPtrListIterator<KPTextObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrProtectContentCommand::addObjects( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj = dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPTextObject *obj = dynamic_cast<KPTextObject*>( it.current() );
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
    QPtrListIterator<KPTextObject> it( m_objects );
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

KPrCloseObjectCommand::KPrCloseObjectCommand( const QString &_name, KPObject *_obj, KPresenterDoc *_doc )
    : KNamedCommand( _name ),
      objects( _obj ),
      doc(_doc)
{
    m_page = doc->findPage( _obj );
}

KPrCloseObjectCommand::~KPrCloseObjectCommand()
{
}

void KPrCloseObjectCommand::execute()
{
    closeObject(true);
}

void KPrCloseObjectCommand::unexecute()
{
    closeObject(false);
}

void KPrCloseObjectCommand::closeObject(bool close)
{
    ObjType tmpType = objects->getType();

    if ( tmpType==OT_POLYLINE )
    {
        KPPolylineObject * obj = dynamic_cast<KPPolylineObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
    else if ( tmpType==OT_FREEHAND )
    {
        KPFreehandObject * obj = dynamic_cast<KPFreehandObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
    else if ( tmpType==OT_QUADRICBEZIERCURVE )
    {
        KPQuadricBezierCurveObject * obj = dynamic_cast<KPQuadricBezierCurveObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
    else if ( tmpType==OT_CUBICBEZIERCURVE )
    {
        KPCubicBezierCurveObject * obj = dynamic_cast<KPCubicBezierCurveObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }

    doc->updateSideBarItem( m_page );
}

MarginsStruct::MarginsStruct( KPTextObject *obj )
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


KPrChangeMarginCommand::KPrChangeMarginCommand( const QString &name, QPtrList<KPObject> &objects,
                                                MarginsStruct newMargins, KPresenterDoc *doc,
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


KPrChangeMarginCommand::KPrChangeMarginCommand( const QString &name, KPTextObject *_obj, MarginsStruct _MarginsBegin,
                                                MarginsStruct _MarginsEnd, KPresenterDoc *_doc ) :
    KNamedCommand(name),
    m_newMargins(_MarginsEnd),
    m_doc( _doc )
{
    _obj->incCmdRef();
    m_objects.append( _obj );
    m_oldMargins.append( new MarginsStruct( _obj ) );
    m_page = m_doc->findPage( _obj );
}


KPrChangeMarginCommand::~KPrChangeMarginCommand()
{
    QPtrListIterator<KPTextObject> it( m_objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    m_oldMargins.setAutoDelete( true );
    m_oldMargins.clear();
}


void KPrChangeMarginCommand::addObjects( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current(); ++it )
    {
        if ( it.current()->getType() == OT_GROUP )
        {
            KPGroupObject * obj = dynamic_cast<KPGroupObject*>( it.current() );
            if ( obj )
            {
                addObjects( obj->objectList() );
            }
        }
        else
        {
            KPTextObject *obj = dynamic_cast<KPTextObject*>( it.current() );
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
    QPtrListIterator<KPTextObject> it( m_objects );
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
        KPTextObject *object = m_objects.at( i );
        MarginsStruct *marginsStruct = m_oldMargins.at( i );
        object->setTextMargins( marginsStruct->leftMargin, marginsStruct->topMargin,
                                marginsStruct->rightMargin, marginsStruct->bottomMargin);
        object->resizeTextDocument();
        object->layout();
    }
    m_doc->repaint( false );

    m_doc->updateSideBarItem( m_page );
}


KPrChangeVerticalAlignmentCommand::KPrChangeVerticalAlignmentCommand( const QString &name, KPTextObject *_obj,
                                                                      VerticalAlignmentType _oldAlign,
                                                                      VerticalAlignmentType _newAlign,
                                                                      KPresenterDoc *_doc) :
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
                                                            KPresenterDoc *_doc):
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

ImageEffectCmd::ImageEffectCmd(const QString &_name, QPtrList<ImageEffectSettings> &_oldSettings,
                               ImageEffectSettings _newSettings, QPtrList<KPObject> &_objects,
                               KPresenterDoc *_doc )
    :KNamedCommand( _name ), oldSettings( _oldSettings ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldSettings.setAutoDelete( false );
    doc = _doc;
    newSettings = _newSettings;

    m_page = doc->findPage( objects );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

ImageEffectCmd::~ImageEffectCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldSettings.setAutoDelete( true );
    oldSettings.clear();
}

void ImageEffectCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it ) {
        KPPixmapObject * obj = dynamic_cast<KPPixmapObject*>( it.current() );
        if ( obj ) {
            obj->setImageEffect(newSettings.effect);
            obj->setIEParams(newSettings.param1, newSettings.param2, newSettings.param3);
        }
    }
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

void ImageEffectCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
        KPPixmapObject * obj = dynamic_cast<KPPixmapObject*>( objects.at(i) );
        if ( obj ) {
            obj->setImageEffect(oldSettings.at( i )->effect);
            obj->setIEParams(oldSettings.at( i )->param1, oldSettings.at( i )->param2,
                             oldSettings.at( i )->param3);
        }
    }
    doc->repaint( false );

    doc->updateSideBarItem( m_page );
}

KPrChangeVariableNoteText::KPrChangeVariableNoteText( const QString &name, KPresenterDoc *_doc,
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
