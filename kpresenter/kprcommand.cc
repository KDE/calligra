/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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

#include <kpresenter_doc.h>
#include <kprcommand.h>
#include <kpbackground.h>
#include <kpclipartobject.h>
#include <kpgroupobject.h>


#include <kplineobject.h>
#include <kpellipseobject.h>
#include <kpautoformobject.h>
#include <kpfreehandobject.h>
#include <kppolylineobject.h>
#include <kpquadricbeziercurveobject.h>
#include <kpcubicbeziercurveobject.h>
#include <kppolygonobject.h>

#include <kptextobject.h>
#include <kppixmapobject.h>

#include <kppartobject.h>
#include <koRuler.h>
#include <kppieobject.h>
#include <kprectobject.h>
#include <kpresenter_view.h>
#include <kotextobject.h>
#include "kprtextdocument.h"
#include <koVariable.h>
#include <kdebug.h>
#include "kprvariable.h"
#include <koRect.h>
#include <koSize.h>
#include <koPoint.h>

/******************************************************************/
/* Class: ShadowCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
ShadowCmd::ShadowCmd( const QString &_name, QPtrList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
                      QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldShadow( _oldShadow ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldShadow.setAutoDelete( false );
    doc = _doc;
    newShadow = _newShadow;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
ShadowCmd::~ShadowCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldShadow.setAutoDelete( true );
    oldShadow.clear();
}

/*====================== execute =================================*/
void ShadowCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
      {
        it.current()->setShadowParameter(newShadow.shadowDistance,newShadow.shadowDirection,newShadow.shadowColor);
      }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void ShadowCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        objects.at( i )->setShadowParameter(oldShadow.at(i)->shadowDistance,oldShadow.at(i)->shadowDirection,oldShadow.at(i)->shadowColor);
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
SetOptionsCmd::SetOptionsCmd( const QString &_name, QValueList<KoPoint> &_diffs, QPtrList<KPObject> &_objects,
                              int _rastX, int _rastY, int _orastX, int _orastY,
                              const QColor &_txtBackCol, const QColor &_otxtBackCol, KPresenterDoc *_doc )
    : KNamedCommand( _name ), diffs( _diffs ), objects( _objects ), txtBackCol( _txtBackCol ), otxtBackCol( _otxtBackCol )
{
    rastX = _rastX;
    rastY = _rastY;
    orastX = _orastX;
    orastY = _orastY;
    doc = _doc;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
SetOptionsCmd::~SetOptionsCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

/*====================== execute =================================*/
void SetOptionsCmd::execute()
{
    // ## use iterator
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( *diffs.at( i ) );

    doc->setRasters( rastX, rastY, false );
    doc->setTxtBackCol( txtBackCol );
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void SetOptionsCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( -(*diffs.at( i )).x(), -(*diffs.at( i )).y() );

    doc->setRasters( orastX, orastY, false );
    doc->setTxtBackCol( otxtBackCol );
    doc->repaint( false );
}

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
SetBackCmd::SetBackCmd( const QString &_name, const QColor &_backColor1, const QColor &_backColor2, BCType _bcType,
			bool _backUnbalanced, int _backXFactor, int _backYFactor,
			const KoPictureKey & _backPix, const KoPictureKey & _backClip,
                        BackView _backView, BackType _backType,
			const QColor &_oldBackColor1, const QColor &_oldBackColor2, BCType _oldBcType,
			bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
			const KoPictureKey & _oldBackPix, const KoPictureKey & _oldBackClip,
                        BackView _oldBackView, BackType _oldBackType,
			bool _takeGlobal, KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name ), backColor1( _backColor1 ), backColor2( _backColor2 ), unbalanced( _backUnbalanced ),
      xfactor( _backXFactor ), yfactor( _backYFactor ), backPix( _backPix ), backClip( _backClip ),
      oldBackColor1( _oldBackColor1 ), oldBackColor2( _oldBackColor2 ), oldUnbalanced( _oldBackUnbalanced ),
      oldXFactor( _oldBackXFactor ), oldYFactor( _oldBackYFactor ), oldBackPix( _oldBackPix ), oldBackClip( _oldBackClip )
{
    bcType = _bcType;
    backView = _backView;
    backType = _backType;
    oldBcType = _oldBcType;
    oldBackView = _oldBackView;
    oldBackType = _oldBackType;
    takeGlobal = _takeGlobal;
    doc = _doc;
    m_page=_page;
}

/*====================== execute =================================*/
void SetBackCmd::execute()
{

    if ( !takeGlobal ) {
	m_page->setBackColor( backColor1, backColor2, bcType,
			   unbalanced, xfactor, yfactor );
	m_page->setBackType(  backType );
	m_page->setBackView(  backView );
	m_page->setBackPixmap( backPix );
	m_page->setBackClipart( backClip );
	doc->restoreBackground( m_page );
    } else {
        QPtrListIterator<KPrPage> it( doc->getPageList() );
        for ( ; it.current() ; ++it )
        {
 	    it.current()->setBackColor(  backColor1, backColor2, bcType,
			       unbalanced, xfactor, yfactor );
	    it.current()->setBackType( backType );
	    it.current()->setBackView( backView );
	    it.current()->setBackPixmap(  backPix );
	    it.current()->setBackClipart( backClip );
            doc->restoreBackground(it.current());
        }

    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void SetBackCmd::unexecute()
{
    if ( !takeGlobal ) {
	m_page->setBackColor(  oldBackColor1, oldBackColor2, oldBcType,
			   oldUnbalanced, oldXFactor, oldYFactor );
	m_page->setBackType( oldBackType );
	m_page->setBackView(  oldBackView );
	m_page->setBackPixmap(  oldBackPix );
	m_page->setBackClipart( oldBackClip );
	doc->restoreBackground( m_page );
    } else {
        QPtrListIterator<KPrPage> it( doc->getPageList() );
        for ( ; it.current() ; ++it )
        {
 	    it.current()->setBackColor(  oldBackColor1, oldBackColor2, oldBcType,
			       oldUnbalanced, oldXFactor, oldYFactor );
	    it.current()->setBackType( oldBackType );
	    it.current()->setBackView( oldBackView );
	    it.current()->setBackPixmap(  oldBackPix );
	    it.current()->setBackClipart( oldBackClip );
            doc->restoreBackground(it.current());
        }
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: RotateCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
RotateCmd::RotateCmd( const QString &_name, QPtrList<RotateValues> &_oldRotate, float _newAngle,
                      QPtrList<KPObject> &_objects, KPresenterDoc *_doc, bool _addAngle )
    : KNamedCommand( _name ), oldRotate( _oldRotate ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldRotate.setAutoDelete( false );
    doc = _doc;
    newAngle = _newAngle;

    addAngle = _addAngle;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
RotateCmd::~RotateCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldRotate.setAutoDelete( true );
    oldRotate.clear();
}

/*====================== execute =================================*/
void RotateCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        if ( addAngle )
            it.current()->rotate( it.current()->getAngle()+newAngle );
        else
            it.current()->rotate( newAngle );
    }
    doc->updateRuler();
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RotateCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at(i)->rotate( oldRotate.at( i )->angle );
    doc->updateRuler();
    doc->repaint( false );
}

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

/*======================== constructor ===========================*/
ChgClipCmd::ChgClipCmd( const QString &_name, KPClipartObject *_object, KoPictureKey _oldKey,
                        KoPictureKey _newKey, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldKey( _oldKey ), newKey( _newKey )
{
    object = _object;
    doc = _doc;
    object->incCmdRef();
}

/*======================== destructor ============================*/
ChgClipCmd::~ChgClipCmd()
{
    object->decCmdRef();
}

/*======================== execute ===============================*/
void ChgClipCmd::execute()
{
    object->setClipart( newKey );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void ChgClipCmd::unexecute()
{
    object->setClipart( oldKey );
    doc->repaint( object );
}

/******************************************************************/
/* Class: ChgPixCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
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

/*======================== destructor ============================*/
ChgPixCmd::~ChgPixCmd()
{
    oldObject->decCmdRef();
    newObject->decCmdRef();
}

/*======================== execute ===============================*/
void ChgPixCmd::execute()
{
    m_page->insertObject(oldObject, newObject);
    doc->repaint( newObject );
}

/*====================== unexecute ===============================*/
void ChgPixCmd::unexecute()
{
    m_page->insertObject(newObject, oldObject);
    doc->repaint( oldObject );
}

/******************************************************************/
/* Class: DeleteCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
DeleteCmd::DeleteCmd( const QString &_name, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
DeleteCmd::~DeleteCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

/*======================== execute ===============================*/
void DeleteCmd::execute()
{
    QRect oldRect;
    bool textObj=false;
    QPtrList<KPObject> list (m_page->objectList());
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
	oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect(doc->zoomHandler()));
	if ( list.findRef( objects.at( i ) ) != -1 )
	{
            m_page->takeObject(objects.at(i));
	    objects.at( i )->removeFromObjList();
            if(objects.at(i)->getType()==OT_TEXT)
                textObj=true;
	}
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
    if(textObj)
        doc->updateRuler();
}

/*====================== unexecute ===============================*/
void DeleteCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
	m_page->appendObject( objects.at( i ) );
	objects.at( i )->addToObjList();
	doc->repaint( objects.at( i ) );
    }
}


/******************************************************************/
/* Class: EffectCmd                                               */
/******************************************************************/

/*================================================================*/
EffectCmd::EffectCmd( const QString &_name, const QPtrList<KPObject> &_objs,
		      const QValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect )
    : KNamedCommand( _name ), oldEffects( _oldEffects ),
      newEffect( _newEffect ), objs( _objs )
{
    QPtrListIterator<KPObject> it( objs );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*================================================================*/
EffectCmd::~EffectCmd()
{
    QPtrListIterator<KPObject> it( objs );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

/*================================================================*/
void EffectCmd::execute()
{
    QPtrListIterator<KPObject> it( objs );
    for ( ; it.current() ; ++it )
      {
	it.current()->setPresNum( newEffect.presNum );
	it.current()->setEffect( newEffect.effect );
	it.current()->setEffect2( newEffect.effect2 );
	it.current()->setDisappear( newEffect.disappear );
	it.current()->setEffect3( newEffect.effect3 );
	it.current()->setDisappearNum( newEffect.disappearNum );
	it.current()->setAppearTimer( newEffect.appearTimer );
	it.current()->setDisappearTimer( newEffect.disappearTimer );
        it.current()->setAppearSoundEffect( newEffect.appearSoundEffect );
        it.current()->setDisappearSoundEffect( newEffect.disappearSoundEffect );
        it.current()->setAppearSoundEffectFileName( newEffect.a_fileName );
        it.current()->setDisappearSoundEffectFileName( newEffect.d_fileName );
    }
}

/*================================================================*/
void EffectCmd::unexecute()
{
    KPObject *object = 0;
    for ( unsigned int i = 0; i < objs.count(); ++i ) {
	object = objs.at( i );

	object->setPresNum( oldEffects[ i ].presNum );
	object->setEffect( oldEffects[ i ].effect );
	object->setEffect2( oldEffects[ i ].effect2 );
	object->setDisappear( oldEffects[ i ].disappear );
	object->setEffect3( oldEffects[ i ].effect3 );
	object->setDisappearNum( oldEffects[ i ].disappearNum );
	object->setAppearTimer( oldEffects[ i ].appearTimer );
	object->setDisappearTimer( oldEffects[ i ].disappearTimer );
        object->setAppearSoundEffect( oldEffects[ i ].appearSoundEffect );
        object->setDisappearSoundEffect( oldEffects[ i ].disappearSoundEffect );
        object->setAppearSoundEffectFileName( oldEffects[ i ].a_fileName );
        object->setDisappearSoundEffectFileName( oldEffects[ i ].d_fileName );
    }
}

/******************************************************************/
/* Class: GroupObjCmd						  */
/******************************************************************/

/*==============================================================*/
GroupObjCmd::GroupObjCmd( const QString &_name,
			  const QPtrList<KPObject> &_objects,
			  KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
    grpObj = new KPGroupObject( objects );
    grpObj->incCmdRef();
}

/*==============================================================*/
GroupObjCmd::~GroupObjCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    grpObj->decCmdRef();
}

/*==============================================================*/
void GroupObjCmd::execute()
{
    KoRect r=KoRect();
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setOrigPointInGroup( it.current()->getOrig() );
        it.current()->setOrigSizeInGroup( it.current()->getSize() );

        it.current()->setSelected( false );
        m_page->takeObject(it.current() );
        it.current()->removeFromObjList();
        r |= it.current()->getBoundingRect( doc->zoomHandler() );

        if ( it.current()->getType() == OT_GROUP ) {
            KPGroupObject *_groupObj = static_cast<KPGroupObject*>( it.current() );
            QPtrListIterator<KPObject> it2( _groupObj->objectList() );
            for ( ; it2.current(); ++it2 ) {
                it2.current()->setOrigPointInGroup( it2.current()->getOrig() );
                it2.current()->setOrigSizeInGroup( it2.current()->getSize() );
            }
        }
    }

    grpObj->setUpdateObjects( false );
    grpObj->setOrigPointInGroup( r.topLeft() );
    grpObj->setOrigSizeInGroup( r.size() );
    grpObj->setOrig( r.x(), r.y() );
    grpObj->setSize( r.width(), r.height() );
    m_page->appendObject( grpObj );
    grpObj->addToObjList();
    grpObj->setUpdateObjects( true );
    grpObj->setSelected( true );

    doc->repaint( false );
}

/*==============================================================*/
void GroupObjCmd::unexecute()
{
    grpObj->setUpdateObjects( false );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        m_page->appendObject( it.current() );
        it.current()->addToObjList();
        it.current()->setSelected( true );
    }

    m_page->takeObject( grpObj );
    grpObj->removeFromObjList();

    doc->repaint( false );
}

/******************************************************************/
/* Class: UnGroupObjCmd						  */
/******************************************************************/

/*==============================================================*/
UnGroupObjCmd::UnGroupObjCmd( const QString &_name,
			  KPGroupObject *grpObj_,
			  KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name ), objects( grpObj_->getObjects() )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
    grpObj = grpObj_;
    grpObj->incCmdRef();
}

/*==============================================================*/
UnGroupObjCmd::~UnGroupObjCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    grpObj->decCmdRef();
}

/*==============================================================*/
void UnGroupObjCmd::execute()
{
    grpObj->setUpdateObjects( false );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        m_page->appendObject( it.current() );
        it.current()->addToObjList();
        it.current()->setSelected( true );
    }

    m_page->takeObject(grpObj);
    grpObj->removeFromObjList();

    doc->repaint( false );
}

/*==============================================================*/
void UnGroupObjCmd::unexecute()
{
    KoRect r=KoRect();

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setOrigPointInGroup( it.current()->getOrig() );
        it.current()->setOrigSizeInGroup( it.current()->getSize() );

        it.current()->setSelected( false );
        m_page->takeObject( it.current() );
        it.current()->removeFromObjList();
        r |= it.current()->getBoundingRect( doc->zoomHandler() );

        if ( it.current()->getType() == OT_GROUP ) {
            KPGroupObject *_groupObj = static_cast<KPGroupObject*>( it.current() );
            QPtrListIterator<KPObject> it2( _groupObj->objectList() );
            for ( ; it2.current(); ++it2 ) {
                it2.current()->setOrigPointInGroup( it2.current()->getOrig() );
                it2.current()->setOrigSizeInGroup( it2.current()->getSize() );
            }
        }
    }

    grpObj->setUpdateObjects( false );
    grpObj->setOrigPointInGroup( r.topLeft() );
    grpObj->setOrigSizeInGroup( r.size() );
    grpObj->setOrig( r.x(), r.y() );
    grpObj->setSize( r.width(), r.height() );
    m_page->appendObject( grpObj );
    grpObj->addToObjList();
    grpObj->setUpdateObjects( true );
    grpObj->setSelected( true );

    doc->repaint( false );
}

/******************************************************************/
/* Class: InsertCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
InsertCmd::InsertCmd( const QString &_name, KPObject *_object, KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name )
{
    object = _object;
    doc = _doc;
    m_page=_page;
    object->incCmdRef();
}

/*======================== destructor ============================*/
InsertCmd::~InsertCmd()
{
    object->decCmdRef();
}

/*====================== execute =================================*/
void InsertCmd::execute()
{
    m_page->appendObject( object );
    object->addToObjList();
    if ( object->getType() == OT_TEXT )
	( (KPTextObject*)object )->recalcPageNum( doc,m_page );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void InsertCmd::unexecute()
{
    QRect oldRect = doc->zoomHandler()->zoomRect(object->getBoundingRect( doc->zoomHandler() ));
    QPtrList<KPObject> list(m_page->objectList());
    if ( list.findRef( object ) != -1 ) {
	m_page->takeObject(  object );
	object->removeFromObjList();
        if ( object->getType() == OT_TEXT )
            doc->terminateEditing( (KPTextObject*)object );
    }
    doc->repaint( oldRect );
}

/******************************************************************/
/* Class: LowerRaiseCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
LowerRaiseCmd::LowerRaiseCmd( const QString &_name, QPtrList<KPObject> _oldList, QPtrList<KPObject> _newList, KPresenterDoc *_doc, KPrPage *_page )
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

/*======================== destructor ============================*/
LowerRaiseCmd::~LowerRaiseCmd()
{
    QPtrListIterator<KPObject> it( oldList );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

/*====================== execute =================================*/
void LowerRaiseCmd::execute()
{
    m_page->setObjectList( newList );
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void LowerRaiseCmd::unexecute()
{
    m_page->setObjectList( oldList );
    doc->repaint( false );
}

/******************************************************************/
/* Class: MoveByCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd::MoveByCmd( const QString &_name, const KoPoint &_diff, QPtrList<KPObject> &_objects, KPresenterDoc *_doc,KPrPage *_page )
    : KNamedCommand( _name ), diff( _diff ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
	if ( it.current()->getType() == OT_TEXT ) {
	    ( (KPTextObject*)it.current() )->recalcPageNum( doc,m_page );
	    doc->repaint( it.current() );
	}
	it.current()->incCmdRef();
    }
}

/*======================== destructor ============================*/
MoveByCmd::~MoveByCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
	it.current()->decCmdRef();
}

/*====================== execute =================================*/
void MoveByCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect( doc->zoomHandler() ));
	objects.at( i )->moveBy( diff );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc,m_page );
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }

}

/*====================== unexecute ===============================*/
void MoveByCmd::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect(doc->zoomHandler() ));
	objects.at( i )->moveBy( -diff.x(), -diff.y() );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc,m_page );
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
}

/******************************************************************/
/* Class: MoveByCmd2						  */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd2::MoveByCmd2( const QString &_name, QPtrList<KoPoint> &_diffs,
			QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), diffs( _diffs ), objects( _objects )
{
    objects.setAutoDelete( false );
    diffs.setAutoDelete( true );
    doc = _doc;
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
	if ( it.current()->getType() == OT_TEXT )
        {
            if(it.current()->isSelected())
                doc->updateRuler();
	    doc->repaint( it.current() );
	}
	it.current()->incCmdRef();
    }
}

/*======================== destructor ============================*/
MoveByCmd2::~MoveByCmd2()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
	it.current()->decCmdRef();

    diffs.clear();
}

/*====================== execute =================================*/
void MoveByCmd2::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = doc->zoomHandler()->zoomRect( objects.at( i )->getBoundingRect( doc->zoomHandler() ));
	objects.at( i )->moveBy( *diffs.at( i ) );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
            if(objects.at(i)->isSelected())
                doc->updateRuler();
        }

	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
}

/*====================== unexecute ===============================*/
void MoveByCmd2::unexecute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = doc->zoomHandler()->zoomRect(objects.at( i )->getBoundingRect( doc->zoomHandler() ));
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
}

/******************************************************************/
/* Class: PenBrushCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
PenBrushCmd::PenBrushCmd( const QString &_name, QPtrList<Pen> &_oldPen, QPtrList<Brush> &_oldBrush,
			  Pen _newPen, Brush _newBrush, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags )
    : KNamedCommand( _name ), oldPen( _oldPen ), oldBrush( _oldBrush ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldPen.setAutoDelete( false );
    oldBrush.setAutoDelete( false );
    doc = _doc;
    newPen = _newPen;
    newBrush = _newBrush;
    flags = _flags;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
PenBrushCmd::~PenBrushCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldPen.setAutoDelete( true );
    oldPen.clear();
    oldBrush.setAutoDelete( true );
    oldBrush.clear();
}

/*====================== execute =================================*/
void PenBrushCmd::execute()
{
  Pen tmpPen = newPen;
  Brush tmpBrush = newBrush;

  for ( int i = 0; i < static_cast<int>( objects.count() ); i++ ) {
    if ( flags & LB_ONLY ) {
      newPen.pen = oldPen.at( i )->pen;
      newPen.lineEnd = oldPen.at( i )->lineEnd;
      newBrush = *oldBrush.at( i );
    }
    if ( flags & LE_ONLY ) {
      newPen.pen = oldPen.at( i )->pen;
      newPen.lineBegin = oldPen.at( i )->lineBegin;
      newBrush = *oldBrush.at( i );
    }
    if ( flags & PEN_ONLY ) {
      newPen.lineBegin = oldPen.at( i )->lineBegin;
      newPen.lineEnd = oldPen.at( i )->lineEnd;
      if ( newPen.pen != Qt::NoPen )
	newPen.pen = QPen( newPen.pen.color(), oldPen.at( i )->pen.width(),
			   oldPen.at( i )->pen.style() != Qt::NoPen ? oldPen.at( i )->pen.style() : Qt::SolidLine );
      else
	newPen.pen = QPen( oldPen.at( i )->pen.color(), oldPen.at( i )->pen.width(), Qt::NoPen );
      newBrush = *oldBrush.at( i );
    }
    if ( flags & BRUSH_ONLY ) {
      newBrush.gColor1 = oldBrush.at( i )->gColor1;
      newBrush.gColor2 = oldBrush.at( i )->gColor2;
      newBrush.unbalanced = oldBrush.at( i )->unbalanced;
      newBrush.xfactor = oldBrush.at( i )->xfactor;
      newBrush.yfactor = oldBrush.at( i )->yfactor;
      if ( newBrush.brush != Qt::NoBrush )
	newBrush.brush = QBrush( newBrush.brush.color(),
				 oldBrush.at( i )->brush.style() != Qt::NoBrush ? oldBrush.at( i )->brush.style() : Qt::SolidPattern );
      else
	newBrush.brush = QBrush( oldBrush.at( i )->brush.color(), Qt::NoBrush );
      newBrush.gType = oldBrush.at( i )->gType;
      newPen = *oldPen.at( i );
    }
    applyPenBrush(objects.at( i ),&newPen,&newBrush );
  }
  newPen = tmpPen;
  newBrush = tmpBrush;
}


void PenBrushCmd::applyPenBrush(KPObject *kpobject,Pen *tmpPen,Brush *tmpBrush )
{

    switch ( kpobject->getType() ) {
    case OT_LINE:
      {
      KPLineObject* obj=dynamic_cast<KPLineObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_RECT:
      {
      KPRectObject* obj=dynamic_cast<KPRectObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_ELLIPSE:
      {
      KPEllipseObject* obj=dynamic_cast<KPEllipseObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_AUTOFORM:
      {
      KPAutoformObject* obj=dynamic_cast<KPAutoformObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_PIE:
      {
      KPPieObject* obj=dynamic_cast<KPPieObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_PART:
      {
      KPPartObject* obj=dynamic_cast<KPPartObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_TEXT:
      {
      KPTextObject* obj=dynamic_cast<KPTextObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_PICTURE:
      {
      KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
      }
      break;
    case OT_CLIPART:
    {
        KPClipartObject* obj=dynamic_cast<KPClipartObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
    }
      break;
    case OT_FREEHAND:
    {
        KPFreehandObject *obj=dynamic_cast<KPFreehandObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  doc->repaint( obj );
	}
    }
      break;
    case OT_POLYLINE:
    {
        KPPolylineObject *obj=dynamic_cast<KPPolylineObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  doc->repaint( obj );
	}
    }
      break;
    case OT_QUADRICBEZIERCURVE:
    {
        KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  doc->repaint( obj );
	}
    }
      break;
    case OT_CUBICBEZIERCURVE:
    {
        KPCubicBezierCurveObject* obj=dynamic_cast<KPCubicBezierCurveObject*>( kpobject );
      if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setLineBegin( tmpPen->lineBegin );
	  obj->setLineEnd( tmpPen->lineEnd );
	  doc->repaint( obj );
	}
    }
      break;
    case OT_POLYGON:
    {
        KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
        if(obj)
	{
	  obj->setPen( tmpPen->pen );
	  obj->setBrush( tmpBrush->brush );
	  obj->setFillType( tmpBrush->fillType );
	  obj->setGColor1( tmpBrush->gColor1 );
	  obj->setGColor2( tmpBrush->gColor2 );
	  obj->setGType( tmpBrush->gType );
	  obj->setGUnbalanced( tmpBrush->unbalanced );
	  obj->setGXFactor( tmpBrush->xfactor );
	  obj->setGYFactor( tmpBrush->yfactor );
	  doc->repaint( obj );
	}
    }
      break;
    default: break;
    }
}

/*====================== unexecute ===============================*/
void PenBrushCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        if( oldPen.count() > i && oldBrush.count() > i)
        {
            applyPenBrush(objects.at( i ),oldPen.at( i ),oldBrush.at( i ));
        }
    }
}


/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

/*================================================================*/
PgConfCmd::PgConfCmd( const QString &_name, bool _manualSwitch, bool _infinitLoop,
                      PageEffect _pageEffect, PresSpeed _presSpeed, int _pageTimer,
                      bool _soundEffect, const QString &_fileName, bool _showPresentationDuration,
                      bool _oldManualSwitch, bool _oldInfinitLoop,
                      PageEffect _oldPageEffect, PresSpeed _oldPresSpeed, int _oldPageTimer,
                      bool _oldSoundEffect, const QString &_oldFileName, bool _oldShowPresentationDuration,
                      KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name )
{
    manualSwitch = _manualSwitch;
    infinitLoop = _infinitLoop;
    pageEffect = _pageEffect;
    presSpeed = _presSpeed;
    pageTimer = _pageTimer;
    soundEffect = _soundEffect;
    fileName = _fileName;
    showPresentationDuration = _showPresentationDuration;
    oldManualSwitch = _oldManualSwitch;
    oldInfinitLoop = _oldInfinitLoop;
    oldPageEffect = _oldPageEffect;
    oldPresSpeed = _oldPresSpeed;
    oldPageTimer = _oldPageTimer;
    oldSoundEffect = _oldSoundEffect;
    oldFileName = _oldFileName;
    oldShowPresentationDuration = _oldShowPresentationDuration;
    doc = _doc;
    m_page=_page;
}

/*================================================================*/
void PgConfCmd::execute()
{
    doc->setManualSwitch( manualSwitch );
    doc->setInfinitLoop( infinitLoop );
    m_page->setPageEffect( pageEffect );
    doc->setPresSpeed( presSpeed );
    m_page->setPageTimer(  pageTimer );
    m_page->setPageSoundEffect(  soundEffect );
    m_page->setPageSoundFileName(  fileName );
    doc->setPresentationDuration( showPresentationDuration );
}

/*================================================================*/
void PgConfCmd::unexecute()
{
    doc->setManualSwitch( oldManualSwitch );
    doc->setInfinitLoop( oldInfinitLoop );
    m_page->setPageEffect( oldPageEffect );
    doc->setPresSpeed( oldPresSpeed );
    m_page->setPageTimer(  oldPageTimer );
    m_page->setPageSoundEffect(  oldSoundEffect );
    m_page->setPageSoundFileName(  oldFileName );
}

/******************************************************************/
/* Class: PgLayoutCmd                                             */
/******************************************************************/

/*======================== constructor ===========================*/
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

/*====================== execute =================================*/
void PgLayoutCmd::execute()
{
    m_doc->setUnit( unit );
    m_doc->setPageLayout( layout );
    m_doc->updateHeaderFooterPosition();
    m_doc->updateRuler();
    m_doc->updateRulerPageLayout();
}

/*====================== unexecute ===============================*/
void PgLayoutCmd::unexecute()
{
    m_doc->setUnit( oldUnit );
    m_doc->setPageLayout( oldLayout );
    m_doc->updateHeaderFooterPosition();
    m_doc->updateRuler();
    m_doc->updateRulerPageLayout();
}


/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

/*======================== constructor ===========================*/
PieValueCmd::PieValueCmd( const QString &_name, QPtrList<PieValues> &_oldValues, PieValues _newValues,
                          QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldValues( _oldValues ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldValues.setAutoDelete( false );
    doc = _doc;
    newValues = _newValues;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
PieValueCmd::~PieValueCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldValues.setAutoDelete( true );
    oldValues.clear();
}

/*====================== execute =================================*/
void PieValueCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        KPPieObject* obj=dynamic_cast<KPPieObject*>( it.current() );
        if(obj)
	{
	  obj->setPieType( newValues.pieType );
	  obj->setPieAngle( newValues.pieAngle );
	  obj->setPieLength( newValues.pieLength );
	}
    }
  doc->repaint( false );
}

/*====================== unexecute ===============================*/
void PieValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        KPPieObject* obj=dynamic_cast<KPPieObject*>( objects.at( i ) );
        if(obj)
	{
            obj->setPieType( oldValues.at( i )->pieType );
            obj->setPieAngle( oldValues.at( i )->pieAngle );
            obj->setPieLength( oldValues.at( i )->pieLength );
	}
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: PolygonSettingCmd                                       */
/******************************************************************/

/*======================== constructor ===========================*/
PolygonSettingCmd::PolygonSettingCmd( const QString &_name, QPtrList<PolygonSettings> &_oldSettings,
                                      PolygonSettings _newSettings, QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldSettings( _oldSettings ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldSettings.setAutoDelete( false );
    doc = _doc;
    newSettings = _newSettings;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
PolygonSettingCmd::~PolygonSettingCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldSettings.setAutoDelete( true );
    oldSettings.clear();
}

/*====================== execute =================================*/
void PolygonSettingCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        KPPolygonObject * obj=dynamic_cast<KPPolygonObject*>( it.current() );
        if(obj)
	{
            obj->setPolygonSettings( newSettings.checkConcavePolygon,
                                     newSettings.cornersValue,
                                     newSettings.sharpnessValue );
	}
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void PolygonSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i )
    {
        KPPolygonObject * obj=dynamic_cast<KPPolygonObject*>( objects.at(i) );
        if(obj)
	{
            obj->setPolygonSettings( oldSettings.at( i )->checkConcavePolygon,
                                     oldSettings.at( i )->cornersValue,
                                     oldSettings.at( i )->sharpnessValue );
	}
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: PictureSettingCmd                                       */
/******************************************************************/

/*======================== constructor ===========================*/
PictureSettingCmd::PictureSettingCmd( const QString &_name, QPtrList<PictureSettings> &_oldSettings,
                                      PictureSettings _newSettings, QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldSettings( _oldSettings ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldSettings.setAutoDelete( false );
    doc = _doc;
    newSettings = _newSettings;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

/*======================== destructor ============================*/
PictureSettingCmd::~PictureSettingCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
    oldSettings.setAutoDelete( true );
    oldSettings.clear();
}

/*====================== execute =================================*/
void PictureSettingCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it ) {
        KPPixmapObject * obj = dynamic_cast<KPPixmapObject*>( it.current() );
        if ( obj ) {
            obj->setPictureSettings( newSettings.mirrorType,
                                     newSettings.depth,
                                     newSettings.swapRGB,
                                     newSettings.grayscal,
                                     newSettings.bright );
	}
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void PictureSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
        KPPixmapObject * obj = dynamic_cast<KPPixmapObject*>( objects.at(i) );
        if ( obj ) {
            obj->setPictureSettings( oldSettings.at( i )->mirrorType,
                                     oldSettings.at( i )->depth,
                                     oldSettings.at( i )->swapRGB,
                                     oldSettings.at( i )->grayscal,
                                     oldSettings.at( i )->bright );
	}
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

/*======================== constructor ===========================*/
RectValueCmd::RectValueCmd( const QString &_name, QPtrList<RectValues> &_oldValues, RectValues _newValues,
                            QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KNamedCommand( _name ), oldValues( _oldValues ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldValues.setAutoDelete( false );
    doc = _doc;
    newValues = _newValues;

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();

}

/*======================== destructor ============================*/
RectValueCmd::~RectValueCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldValues.setAutoDelete( true );
    oldValues.clear();
}

/*====================== execute =================================*/
void RectValueCmd::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        KPRectObject *obj=dynamic_cast<KPRectObject*>(it.current() );
        if(obj)
            obj->setRnds( newValues.xRnd, newValues.yRnd );
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RectValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        KPRectObject *obj=dynamic_cast<KPRectObject*>( objects.at(i));

        if(obj)
	{
            obj->setRnds( oldValues.at( i )->xRnd, oldValues.at( i )->yRnd );
	}
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: ResizeCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
ResizeCmd::ResizeCmd( const QString &_name, const KoPoint &_m_diff, const KoSize &_r_diff, KPObject *_object, KPresenterDoc *_doc )
    : KNamedCommand( _name ), m_diff( _m_diff ), r_diff( _r_diff )
{
    object = _object;
    doc = _doc;
    object->incCmdRef();
}

/*======================== destructor ============================*/
ResizeCmd::~ResizeCmd()
{
    object->decCmdRef();
}

/*====================== execute =================================*/
void ResizeCmd::execute()
{
    QRect oldRect;

    oldRect = doc->zoomHandler()->zoomRect( object->getBoundingRect(doc->zoomHandler() ));
    object->moveBy( m_diff );
    object->resizeBy( r_diff );
    if ( object->getType() == OT_TEXT )
    {
        if(object->isSelected())
            doc->updateRuler();
    }
    if ( object->isSelected())
    {
        doc->updateObjectStatusBarItem();
    }
    doc->repaint( oldRect );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void ResizeCmd::unexecute()
{
    unexecute(true);
}

/*====================== unexecute ===============================*/
void ResizeCmd::unexecute( bool _repaint )
{
    QRect oldRect;

    oldRect = doc->zoomHandler()->zoomRect(object->getBoundingRect(doc->zoomHandler() ));
    object->moveBy( -m_diff.x(), -m_diff.y() );
    object->resizeBy( -r_diff.width(), -r_diff.height() );
    if ( object->getType() == OT_TEXT )
    {
        if(object->isSelected())
            doc->updateRuler();
    }
    if ( object->isSelected())
    {
        doc->updateObjectStatusBarItem();
    }

    if ( _repaint ) {
	doc->repaint( oldRect );
	doc->repaint( object );
    }
}


KPrPasteTextCommand::KPrPasteTextCommand( KoTextDocument *d, int parag, int idx,
                                const QCString & data )
    : KoTextDocCommand( d ), m_parag( parag ), m_idx( idx ), m_data( data )
{
}

KoTextCursor * KPrPasteTextCommand::execute( KoTextCursor *c )
{
    KoTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    //kdDebug() << "KWPasteTextCommand::execute m_parag=" << m_parag << " m_idx=" << m_idx
    //          << " firstParag=" << firstParag << " " << firstParag->paragId() << endl;
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    QDomDocument domDoc;
    domDoc.setContent( m_data );
    QDomElement elem = domDoc.documentElement();
    // We iterate twice over the list of paragraphs.
    // First time to gather the text,
    // second time to apply the character & paragraph formatting
    QString text;

    QValueList<QDomElement> listParagraphs;
    QDomElement paragElem = elem.firstChild().toElement();
    bool first = true;
    for ( ; !paragElem.isNull() ; paragElem = paragElem.nextSibling().toElement() )
    {
        if ( paragElem.tagName() == "P" )
        {
            QDomElement n = paragElem.firstChild().toElement();
            while ( !n.isNull() ) {
                if ( n.tagName() == "TEXT" )
                    text += n.firstChild().toText().data();
                n = n.nextSibling().toElement();
            }
            listParagraphs.append( paragElem );
            if ( !first )
                text += '\n';
            else
                first = false;
        }
    }
    kdDebug() << "KPrPasteTextCommand::execute Inserting text: '" << text << "'" << endl;
    KPrTextDocument * textdoc = static_cast<KPrTextDocument *>(c->parag()->document());

    cursor.insert( text, true );

    // Move cursor to the end
    c->setParag( firstParag );
    c->setIndex( m_idx );
    for ( int i = 0; i < (int)text.length(); ++i )
        c->gotoRight();
    // Redo the parag lookup because if firstParag was empty, insert() has
    // shifted it down (side effect of splitAndInsertEmptyParag)
    firstParag = doc->paragAt( m_parag );
    KoTextParag * parag = static_cast<KoTextParag *>(firstParag);
    //kdDebug() << "KPrPasteTextCommand::execute starting at parag " << parag << " " << parag->paragId() << endl;

    //uint count = listParagraphs.count();
    QValueList<QDomElement>::ConstIterator it = listParagraphs.begin();
    QValueList<QDomElement>::ConstIterator end = listParagraphs.end();
    for ( uint item = 0 ; it != end ; ++it, ++item )
    {
        if (!parag)
        {
            kdWarning() << "KPrPasteTextCommand: parag==0L ! KPresenter bug, please report." << endl;
            break;
        }
        paragElem = *it;
        // First line (if appending to non-empty line) : apply offset to formatting, don't apply parag layout
        if ( item == 0 && m_idx > 0 ) { }
        else
        {
            if ( item == 0 ) // This paragraph existed, store its parag layout
                m_oldParagLayout = parag->paragLayout();

            KoParagLayout paragLayout = textdoc->textObject()->loadParagLayout(paragElem, textdoc->textObject()->kPresenterDocument(), true);
            parag->setParagLayout( paragLayout );
        }
        // Now load (parse) and apply the character formatting
        QDomElement n = paragElem.firstChild().toElement();
        int i = 0;
        if ( item == 0 && m_idx > 0 )
            i = m_idx;
        while ( !n.isNull() ) {
            if ( n.tagName() == "TEXT" ) {
                QString txt = n.firstChild().toText().data();
                KoTextFormat fm = textdoc->textObject()->loadFormat( n );
                parag->setFormat( i, txt.length(), textdoc->formatCollection()->format( &fm ) );
                i += txt.length();
            }
            n = n.nextSibling().toElement();
        }
        parag->format();
        parag->setChanged( TRUE );
        parag = static_cast<KoTextParag *>(parag->next());
        //kdDebug() << "KWPasteTextCommand::execute going to next parag: " << parag << endl;
    }
    m_lastParag = c->parag()->paragId();
    m_lastIndex = c->index();
    return c;
}


KoTextCursor * KPrPasteTextCommand::unexecute( KoTextCursor *c )
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
    cursor.setParag( lastParag );
    cursor.setIndex( m_lastIndex );
    doc->setSelectionEnd( KoTextDocument::Temp, &cursor );
    // Delete all custom items

    doc->removeSelectedText( KoTextDocument::Temp, c /* sets c to the correct position */ );
    if ( m_idx == 0 )
        static_cast<KoTextParag *>( firstParag )->setParagLayout( m_oldParagLayout );
    return c;
}


KPrChangeStartingPageCommand::KPrChangeStartingPageCommand( const QString &name, KPresenterDoc *_doc, int _oldStartingPage, int _newStartingPage):
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

KPrChangeDisplayLinkCommand::KPrChangeDisplayLinkCommand( const QString &name, KPresenterDoc *_doc, bool _oldDisplay,bool _newDisplay):
    KNamedCommand(name),
    m_doc(_doc),
    m_bOldDisplay(_oldDisplay),
    m_bNewDisplay(_newDisplay)
{
}

void KPrChangeDisplayLinkCommand::execute()
{
    m_doc->getVariableCollection()->variableSetting()->setDisplayLink(m_bNewDisplay);
    m_doc->recalcVariables( VT_LINK );
}

void KPrChangeDisplayLinkCommand::unexecute()
{
    m_doc->getVariableCollection()->variableSetting()->setDisplayLink(m_bOldDisplay);
    m_doc->recalcVariables( VT_LINK );
}

KPrDeletePageCmd::KPrDeletePageCmd( const QString &_name, int pos,KPrPage *_page, KPresenterDoc *_doc):
    KNamedCommand(_name),
    doc(_doc),
    m_page(_page),
    position(pos)
{
}

KPrDeletePageCmd::~KPrDeletePageCmd()
{
    //todo
    m_page->deletePage();
    delete m_page;
}

void KPrDeletePageCmd::execute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->AddRemovePage();
}

void KPrDeletePageCmd::unexecute()
{
    doc->deSelectAllObj();
    doc->insertPage( m_page, position);
    doc->AddRemovePage();
}

KPrInsertPageCmd::KPrInsertPageCmd( const QString &_name,int _pos, KPrPage *_page, KPresenterDoc *_doc ) :
    KNamedCommand(_name),
    doc(_doc),
    m_page(_page),
    position(_pos)
{
}

KPrInsertPageCmd::~KPrInsertPageCmd()
{
    m_page->deletePage();
}

void KPrInsertPageCmd::execute()
{
    doc->deSelectAllObj();
    doc->insertPage( m_page, position);
    doc->AddRemovePage();
    m_page->completeLoading( false, -1 );
}

void KPrInsertPageCmd::unexecute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->AddRemovePage();
}

KPrMovePageCmd::KPrMovePageCmd( const QString &_name,int _oldpos,int _newpos, KPrPage *_page, KPresenterDoc *_doc ) :
    KNamedCommand(_name),
    doc(_doc),
    m_page(_page),
    oldPosition(_oldpos),
    newPosition(_newpos)
{
}

KPrMovePageCmd::~KPrMovePageCmd()
{
}

void KPrMovePageCmd::execute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->insertPage( m_page, newPosition);
    doc->AddRemovePage();
}

void KPrMovePageCmd::unexecute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->insertPage(m_page,oldPosition);
    doc->AddRemovePage();
}


KPrChangeTitlePageNameCommand::KPrChangeTitlePageNameCommand( const QString &_name,KPresenterDoc *_doc, const QString &_oldPageName, const QString &_newPageName, KPrPage *_page ) :
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
    int pos=m_doc->pageList().findRef(m_page);
    m_doc->updateSideBarItem(pos);
    m_doc->recalcVariables( VT_PGNUM );
}

void KPrChangeTitlePageNameCommand::unexecute()
{
    m_page->insertManualTitle(oldPageName);
    int pos=m_doc->pageList().findRef(m_page);
    m_doc->updateSideBarItem(pos);
    m_doc->recalcVariables( VT_PGNUM );
}

KPrChangeCustomVariableValue::KPrChangeCustomVariableValue( const QString &name, KPresenterDoc *_doc,const QString & _oldValue, const QString & _newValue,KoCustomVariable *var):
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


KPrChangeLinkVariable::KPrChangeLinkVariable( const QString &name, KPresenterDoc *_doc,const QString & _oldHref, const QString & _newHref, const QString & _oldLink,const QString &_newLink, KoLinkVariable *var):
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

KPrStickyObjCommand::KPrStickyObjCommand( const QString &_name, QPtrList<KPObject> &_objects,bool sticky, KPrPage*_page, KPresenterDoc *_doc )
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
}

void KPrStickyObjCommand::stickObj(KPObject *_obj)
{
    m_page->takeObject(_obj);
    m_doc->stickyPage()->appendObject(_obj);
    _obj->setSticky(true);
}

void KPrStickyObjCommand::unstickObj(KPObject *_obj)
{
    m_doc->stickyPage()->takeObject(_obj);
    m_page->appendObject(_obj);
    _obj->setSticky(false);
}

KPrHideShowHeaderFooter::KPrHideShowHeaderFooter( const QString &name, KPresenterDoc *_doc, bool _newValue, KPTextObject *_textObject):
    KNamedCommand(name),
    m_doc(_doc),
    m_textObject(_textObject),
    newValue(_newValue)
{
}


void KPrHideShowHeaderFooter::execute()
{
    if( m_textObject==m_doc->footer())
        m_doc->setFooter( newValue );
    else if( m_textObject==m_doc->header())
        m_doc->setHeader( newValue );
    else
        kdDebug()<<"Error in void KPrHideShowHeaderFooter::execute()\n";
}

void KPrHideShowHeaderFooter::unexecute()
{
    if( m_textObject==m_doc->footer())
        m_doc->setFooter( !newValue );
    else if( m_textObject==m_doc->header())
        m_doc->setHeader( !newValue );
    else
        kdDebug()<<"Error in void KPrHideShowHeaderFooter::unexecute()\n";

}


KPrFlipObjectCommand::KPrFlipObjectCommand( const QString &name, KPresenterDoc *_doc, bool _horizontal , KPObject *_obj):
    KNamedCommand(name),
    m_doc(_doc),
    m_object(_obj),
    horizontal(_horizontal)
{
}

void KPrFlipObjectCommand::execute()
{
    flipObject();
}

void KPrFlipObjectCommand::unexecute()
{
    flipObject();
}

void KPrFlipObjectCommand::flipObject()
{
    if ( m_object->getType() == OT_LINE)
    {
        KPLineObject *obj=dynamic_cast<KPLineObject *>(m_object);
        if ( obj)
        {
            obj->flip(horizontal );
            m_doc->repaint( obj );
        }
    }
    else if ( m_object->getType() == OT_POLYLINE)
    {
        KPPolylineObject *obj=dynamic_cast<KPPolylineObject *>(m_object);
        if ( obj)
        {
            obj->flip(horizontal);
            m_doc->repaint( obj );
        }
    }
    else if ( m_object->getType() == OT_CUBICBEZIERCURVE)
    {
        KPCubicBezierCurveObject *obj=dynamic_cast<KPCubicBezierCurveObject *>(m_object);
        if ( obj)
        {
            obj->flip(horizontal);
            m_doc->repaint( obj );
        }
    }
    else if ( m_object->getType() == OT_QUADRICBEZIERCURVE)
    {
        KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject *>(m_object);
        if ( obj)
        {
            obj->flip(horizontal);
            m_doc->repaint( obj );
        }
    }
    else if ( m_object->getType() == OT_FREEHAND )
    {
        KPFreehandObject *obj=dynamic_cast<KPFreehandObject *>(m_object);
        if ( obj)
        {
            obj->flip(horizontal);
            m_doc->repaint( obj );
        }
    }
    else if ( m_object->getType() == OT_PIE )
    {
        KPPieObject *obj=dynamic_cast<KPPieObject *>(m_object);
        if ( obj)
        {
            obj->flip(horizontal);
            m_doc->repaint( obj );
        }
    }

}

KPrGeometryPropertiesCommand::KPrGeometryPropertiesCommand( const QString &_name, QValueList<bool> &_protect, QValueList<bool> &_ratio, QPtrList<KPObject> &_objects, bool _newProtect, bool _newRatio, KPresenterDoc *_doc ):
    KNamedCommand( _name ),
    protect( _protect ),
    ratio(_ratio),
    objects( _objects ),
    newProtect( _newProtect ),
    newRatio( _newRatio ),
    doc(_doc)
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

KPrGeometryPropertiesCommand::~KPrGeometryPropertiesCommand()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();
}

void KPrGeometryPropertiesCommand::execute()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        it.current()->setProtect( newProtect );
        it.current()->setKeepRatio( newRatio );
    }
}

void KPrGeometryPropertiesCommand::unexecute()
{
    KPObject *obj = 0;
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
	obj = objects.at( i );
        obj->setProtect( *protect.at(i) );
        obj->setKeepRatio( *ratio.at(i) );
    }
}


KPrProtectContentCommand::KPrProtectContentCommand( const QString &_name, bool _protectContent, KPTextObject *_obj, KPresenterDoc *_doc )
    : KNamedCommand( _name ),
      protectContent( _protectContent ),
      objects( _obj ),
      doc(_doc)
{
}

KPrProtectContentCommand::~KPrProtectContentCommand()
{
}

void KPrProtectContentCommand::execute()
{
    objects->setProtectContent( protectContent );
    doc->updateObjectSelected();
}

void KPrProtectContentCommand::unexecute()
{
    objects->setProtectContent( !protectContent );
    doc->updateObjectSelected();
}

KPrCloseObjectCommand::KPrCloseObjectCommand( const QString &_name, KPObject *_obj, KPresenterDoc *_doc )
    : KNamedCommand( _name ),
      objects( _obj ),
      doc(_doc)
{
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
    if ( objects->getType()==OT_POLYLINE )
    {
        KPPolylineObject * obj = dynamic_cast<KPPolylineObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
    else if ( objects->getType()==OT_FREEHAND )
    {
        KPFreehandObject * obj = dynamic_cast<KPFreehandObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
    else if ( objects->getType()==OT_QUADRICBEZIERCURVE )
    {
        KPQuadricBezierCurveObject * obj = dynamic_cast<KPQuadricBezierCurveObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
    else if ( objects->getType()==OT_CUBICBEZIERCURVE )
    {
        KPCubicBezierCurveObject * obj = dynamic_cast<KPCubicBezierCurveObject *>(objects);
        if ( obj )
        {
            obj->closeObject( close );
            doc->repaint( obj );
        }
    }
}
