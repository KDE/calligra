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
using namespace Qt3;
#include <kdebug.h>
#include "kprvariable.h"


/******************************************************************/
/* Class: ShadowCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
ShadowCmd::ShadowCmd( QString _name, QPtrList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
                      QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KCommand( _name ), oldShadow( _oldShadow ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldShadow.setAutoDelete( false );
    doc = _doc;
    newShadow = _newShadow;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
ShadowCmd::~ShadowCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldShadow.setAutoDelete( true );
    oldShadow.clear();
}

/*====================== execute =================================*/
void ShadowCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        objects.at( i )->setShadowParameter(newShadow.shadowDistance,newShadow.shadowDirection,newShadow.shadowColor);
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
SetOptionsCmd::SetOptionsCmd( QString _name, QPtrList<QPoint> &_diffs, QPtrList<KPObject> &_objects,
                              int _rastX, int _rastY, int _orastX, int _orastY,
                              QColor _txtBackCol, QColor _otxtBackCol, KPresenterDoc *_doc )
    : KCommand( _name ), diffs( _diffs ), objects( _objects ), txtBackCol( _txtBackCol ), otxtBackCol( _otxtBackCol )
{
    rastX = _rastX;
    rastY = _rastY;
    orastX = _orastX;
    orastY = _orastY;
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
SetOptionsCmd::~SetOptionsCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
}

/*====================== execute =================================*/
void SetOptionsCmd::execute()
{
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
        objects.at( i )->moveBy( -diffs.at( i )->x(), -diffs.at( i )->y() );

    doc->setRasters( orastX, orastY, false );
    doc->setTxtBackCol( otxtBackCol );
    doc->repaint( false );
}

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
SetBackCmd::SetBackCmd( QString _name, QColor _backColor1, QColor _backColor2, BCType _bcType,
			bool _backUnbalanced, int _backXFactor, int _backYFactor,
			const KoImageKey & _backPix, const KPClipartKey & _backClip,
                        BackView _backView, BackType _backType,
			QColor _oldBackColor1, QColor _oldBackColor2, BCType _oldBcType,
			bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
			const KPClipartKey & _oldBackPix, const KPClipartKey & _oldBackClip,
                        BackView _oldBackView, BackType _oldBackType,
			bool _takeGlobal, KPresenterDoc *_doc, KPrPage *_page )
    : KCommand( _name ), backColor1( _backColor1 ), backColor2( _backColor2 ), unbalanced( _backUnbalanced ),
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
	unsigned int i = 0;
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
RotateCmd::RotateCmd( QString _name, QPtrList<RotateValues> &_oldRotate, float _newAngle,
                      QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KCommand( _name ), oldRotate( _oldRotate ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldRotate.setAutoDelete( false );
    doc = _doc;
    newAngle = _newAngle;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
RotateCmd::~RotateCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldRotate.setAutoDelete( true );
    oldRotate.clear();
}

/*====================== execute =================================*/
void RotateCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->rotate( newAngle );

    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RotateCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->rotate( oldRotate.at( i )->angle );

    doc->repaint( false );
}

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

/*======================== constructor ===========================*/
ChgClipCmd::ChgClipCmd( QString _name, KPClipartObject *_object, KPClipartCollection::Key _oldKey,
                        KPClipartCollection::Key _newKey, KPresenterDoc *_doc )
    : KCommand( _name ), oldKey( _oldKey ), newKey( _newKey )
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
ChgPixCmd::ChgPixCmd( QString _name, KPPixmapObject *_oldObject, KPPixmapObject *_newObject,
                      KPresenterDoc *_doc, KPrPage *_page)
    : KCommand( _name )
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
DeleteCmd::DeleteCmd( QString _name, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, KPrPage *_page )
    : KCommand( _name ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
DeleteCmd::~DeleteCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
}

/*======================== execute ===============================*/
void DeleteCmd::execute()
{
    QRect oldRect;

    QPtrList<KPObject> list (m_page->objectList());
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
	oldRect = objects.at( i )->getBoundingRect();
	if ( list.findRef( objects.at( i ) ) != -1 )
	{
            m_page->takeObject(objects.at(i));
	    objects.at( i )->removeFromObjList();
	}
	doc->repaint( oldRect );
	doc->repaint( objects.at( i ) );
    }
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
EffectCmd::EffectCmd( QString _name, const QPtrList<KPObject> &_objs,
		      const QValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect )
    : KCommand( _name ), oldEffects( _oldEffects ),
      newEffect( _newEffect ), objs( _objs )
{
    for ( unsigned int i = 0; i < objs.count(); ++i )
        objs.at( i )->incCmdRef();
}

/*================================================================*/
EffectCmd::~EffectCmd()
{
    for ( unsigned int i = 0; i < objs.count(); ++i )
        objs.at( i )->decCmdRef();
}

/*================================================================*/
void EffectCmd::execute()
{
    KPObject *object = 0;
    for ( unsigned int i = 0; i < objs.count(); ++i ) {
	object = objs.at( i );

	object->setPresNum( newEffect.presNum );
	object->setEffect( newEffect.effect );
	object->setEffect2( newEffect.effect2 );
	object->setDisappear( newEffect.disappear );
	object->setEffect3( newEffect.effect3 );
	object->setDisappearNum( newEffect.disappearNum );
	object->setAppearTimer( newEffect.appearTimer );
	object->setDisappearTimer( newEffect.disappearTimer );
        object->setAppearSoundEffect( newEffect.appearSoundEffect );
        object->setDisappearSoundEffect( newEffect.disappearSoundEffect );
        object->setAppearSoundEffectFileName( newEffect.a_fileName );
        object->setDisappearSoundEffectFileName( newEffect.d_fileName );
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
    : KCommand( _name ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
    grpObj = new KPGroupObject( objects );
    grpObj->incCmdRef();
}

/*==============================================================*/
GroupObjCmd::~GroupObjCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
    grpObj->decCmdRef();
}

/*==============================================================*/
void GroupObjCmd::execute()
{
    QRect r = objects.first()->getBoundingRect( );
    KPObject *obj = 0;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
        obj->setOrigPointInGroup( obj->getOrig() );
        obj->setOrigSizeInGroup( obj->getSize() );
	obj->setSelected( false );
        m_page->takeObject(obj);
	obj->removeFromObjList();
	r = r.unite( obj->getBoundingRect( ) );
    }

    grpObj->setUpdateObjects( false );
    grpObj->setOrigPointInGroup( QPoint( r.x(), r.y() ) );
    grpObj->setOrigSizeInGroup( QSize( r.width(), r.height() ) );
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
    KPObject *obj = 0;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	m_page->appendObject( obj );
	obj->addToObjList();
	obj->setSelected( true );
    }

    m_page->takeObject(grpObj);
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
    : KCommand( _name ), objects( grpObj_->getObjects() )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
    grpObj = grpObj_;
    grpObj->incCmdRef();
}

/*==============================================================*/
UnGroupObjCmd::~UnGroupObjCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
    grpObj->decCmdRef();
}

/*==============================================================*/
void UnGroupObjCmd::execute()
{
    KPObject *obj = 0;
    grpObj->setUpdateObjects( false );

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	m_page->appendObject( obj );
	obj->addToObjList();
	obj->setSelected( true );
    }

    m_page->takeObject(grpObj);
    grpObj->removeFromObjList();

    doc->repaint( false );
}

/*==============================================================*/
void UnGroupObjCmd::unexecute()
{
    QRect r = objects.first()->getBoundingRect(  );

    KPObject *obj = 0;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	obj = objects.at( i );
	obj->setSelected( false );
	m_page->takeObject(obj);
	obj->removeFromObjList();
	r = r.unite( obj->getBoundingRect( ) );
    }

    grpObj->setUpdateObjects( false );
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
InsertCmd::InsertCmd( QString _name, KPObject *_object, KPresenterDoc *_doc, KPrPage *_page )
    : KCommand( _name )
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
	( (KPTextObject*)object )->recalcPageNum( doc );
    doc->repaint( object );
}

/*====================== unexecute ===============================*/
void InsertCmd::unexecute()
{
    QRect oldRect = object->getBoundingRect(  );
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
LowerRaiseCmd::LowerRaiseCmd( QString _name, QPtrList<KPObject> _oldList, QPtrList<KPObject> _newList, KPresenterDoc *_doc, KPrPage *_page )
    : KCommand( _name )
{
    oldList = _oldList;
    newList = _newList;
    m_page=_page;
    m_executed = false;
    oldList.setAutoDelete( false );
    newList.setAutoDelete( false );
    doc = _doc;

    for ( unsigned int i = 0; i < oldList.count(); i++ )
        oldList.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
LowerRaiseCmd::~LowerRaiseCmd()
{
    for ( unsigned int i = 0; i < oldList.count(); i++ )
        oldList.at( i )->decCmdRef();
}

/*====================== execute =================================*/
void LowerRaiseCmd::execute()
{
    m_page->setObjectList( newList );
    doc->repaint( false );
    m_executed = true;
}

/*====================== unexecute ===============================*/
void LowerRaiseCmd::unexecute()
{
    m_page->setObjectList( oldList );
    doc->repaint( false );
    m_executed = false;
}

/******************************************************************/
/* Class: MoveByCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd::MoveByCmd( QString _name, QPoint _diff, QPtrList<KPObject> &_objects, KPresenterDoc *_doc,KPrPage *_page )
    : KCommand( _name ), diff( _diff ), objects( _objects )
{
    objects.setAutoDelete( false );
    doc = _doc;
    m_page=_page;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	if ( objects.at( i )->getType() == OT_TEXT ) {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
	    doc->repaint( objects.at( i ) );
	}
	objects.at( i )->incCmdRef();
    }
}

/*======================== destructor ============================*/
MoveByCmd::~MoveByCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
}

/*====================== execute =================================*/
void MoveByCmd::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = objects.at( i )->getBoundingRect(  );
	objects.at( i )->moveBy( diff );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
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
	oldRect = objects.at( i )->getBoundingRect( );
	objects.at( i )->moveBy( -diff.x(), -diff.y() );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
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
MoveByCmd2::MoveByCmd2( QString _name, QPtrList<QPoint> &_diffs,
			QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KCommand( _name ), diffs( _diffs ), objects( _objects )
{
    objects.setAutoDelete( false );
    diffs.setAutoDelete( true );
    doc = _doc;
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	if ( objects.at( i )->getType() == OT_TEXT ) {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
            if(objects.at(i)->isSelected())
                doc->updateRuler();
	    doc->repaint( objects.at( i ) );
	}
	objects.at( i )->incCmdRef();
    }
}

/*======================== destructor ============================*/
MoveByCmd2::~MoveByCmd2()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();

    diffs.clear();
}

/*====================== execute =================================*/
void MoveByCmd2::execute()
{
    QRect oldRect;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	oldRect = objects.at( i )->getBoundingRect(  );
	objects.at( i )->moveBy( *diffs.at( i ) );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
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
	oldRect = objects.at( i )->getBoundingRect(  );
	objects.at( i )->moveBy( -diffs.at( i )->x(), -diffs.at( i )->y() );
	if ( objects.at( i )->getType() == OT_TEXT )
        {
	    ( (KPTextObject*)objects.at( i ) )->recalcPageNum( doc );
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
PenBrushCmd::PenBrushCmd( QString _name, QPtrList<Pen> &_oldPen, QPtrList<Brush> &_oldBrush,
			  Pen _newPen, Brush _newBrush, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags )
    : KCommand( _name ), oldPen( _oldPen ), oldBrush( _oldBrush ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldPen.setAutoDelete( false );
    oldBrush.setAutoDelete( false );
    doc = _doc;
    newPen = _newPen;
    newBrush = _newBrush;
    flags = _flags;

    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
PenBrushCmd::~PenBrushCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
	objects.at( i )->decCmdRef();
    oldPen.setAutoDelete( true );
    oldPen.clear();
    oldBrush.setAutoDelete( true );
    oldBrush.clear();
}

/*====================== execute =================================*/
void PenBrushCmd::execute()
{
    KPObject *kpobject = 0L;
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

	kpobject = objects.at( i );
	switch ( kpobject->getType() ) {
	case OT_LINE:
	    dynamic_cast<KPLineObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPLineObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPLineObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    doc->repaint( kpobject );
	    break;
	case OT_RECT:
	    dynamic_cast<KPRectObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPRectObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPRectObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPRectObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPRectObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPRectObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPRectObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPRectObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPRectObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_ELLIPSE:
	    dynamic_cast<KPEllipseObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPEllipseObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_AUTOFORM:
	    dynamic_cast<KPAutoformObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPAutoformObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_PIE:
	    dynamic_cast<KPPieObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPPieObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPPieObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPPieObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    dynamic_cast<KPPieObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPPieObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPPieObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPPieObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPPieObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPPieObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPPieObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_PART:
	    dynamic_cast<KPPartObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPPartObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPPartObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPPartObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPPartObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPPartObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPPartObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPPartObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPPartObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_TEXT:
	    dynamic_cast<KPTextObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPTextObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPTextObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPTextObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPTextObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPTextObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPTextObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPTextObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPTextObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_PICTURE:
	    dynamic_cast<KPPixmapObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPPixmapObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
	case OT_CLIPART:
	    dynamic_cast<KPClipartObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPClipartObject*>( kpobject )->setBrush( newBrush.brush );
	    dynamic_cast<KPClipartObject*>( kpobject )->setFillType( newBrush.fillType );
	    dynamic_cast<KPClipartObject*>( kpobject )->setGColor1( newBrush.gColor1 );
	    dynamic_cast<KPClipartObject*>( kpobject )->setGColor2( newBrush.gColor2 );
	    dynamic_cast<KPClipartObject*>( kpobject )->setGType( newBrush.gType );
	    dynamic_cast<KPClipartObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
	    dynamic_cast<KPClipartObject*>( kpobject )->setGXFactor( newBrush.xfactor );
	    dynamic_cast<KPClipartObject*>( kpobject )->setGYFactor( newBrush.yfactor );
	    doc->repaint( kpobject );
	    break;
        case OT_FREEHAND:
	    dynamic_cast<KPFreehandObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPFreehandObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPFreehandObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    doc->repaint( kpobject );
	    break;
        case OT_POLYLINE:
	    dynamic_cast<KPPolylineObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPPolylineObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPPolylineObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    doc->repaint( kpobject );
	    break;
        case OT_QUADRICBEZIERCURVE:
	    dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    doc->repaint( kpobject );
	    break;
        case OT_CUBICBEZIERCURVE:
	    dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->setPen( newPen.pen );
	    dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->setLineBegin( newPen.lineBegin );
	    dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->setLineEnd( newPen.lineEnd );
	    doc->repaint( kpobject );
	    break;
        case OT_POLYGON:
            dynamic_cast<KPPolygonObject*>( kpobject )->setPen( newPen.pen );
            dynamic_cast<KPPolygonObject*>( kpobject )->setBrush( newBrush.brush );
            dynamic_cast<KPPolygonObject*>( kpobject )->setFillType( newBrush.fillType );
            dynamic_cast<KPPolygonObject*>( kpobject )->setGColor1( newBrush.gColor1 );
            dynamic_cast<KPPolygonObject*>( kpobject )->setGColor2( newBrush.gColor2 );
            dynamic_cast<KPPolygonObject*>( kpobject )->setGType( newBrush.gType );
            dynamic_cast<KPPolygonObject*>( kpobject )->setGUnbalanced( newBrush.unbalanced );
            dynamic_cast<KPPolygonObject*>( kpobject )->setGXFactor( newBrush.xfactor );
            dynamic_cast<KPPolygonObject*>( kpobject )->setGYFactor( newBrush.yfactor );
            doc->repaint( kpobject );
	    break;
	default: break;
	}
    }

    newPen = tmpPen;
    newBrush = tmpBrush;
}

/*====================== unexecute ===============================*/
void PenBrushCmd::unexecute()
{
    KPObject *kpobject = 0L;

    for ( unsigned int i = 0; i < objects.count(); i++ ) {
	kpobject = objects.at( i );
	switch ( kpobject->getType() ) {
	case OT_LINE: {
	    if ( oldPen.count() > i ) {
		dynamic_cast<KPLineObject*>( kpobject )->setPen( oldPen.at( i )->pen );
		dynamic_cast<KPLineObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPLineObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
		doc->repaint( kpobject );
	    }
	} break;
	case OT_RECT: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPRectObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPRectObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPRectObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPRectObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPRectObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPRectObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPRectObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPRectObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPRectObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_ELLIPSE: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPEllipseObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPEllipseObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPEllipseObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPEllipseObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPEllipseObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPEllipseObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPEllipseObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPEllipseObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPEllipseObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_AUTOFORM: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPAutoformObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i )
		dynamic_cast<KPAutoformObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
	    if ( oldPen.count() > i ) {
		dynamic_cast<KPAutoformObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPAutoformObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
	    }
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPAutoformObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPAutoformObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPAutoformObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPAutoformObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPAutoformObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPAutoformObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPAutoformObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_PIE: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPPieObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPPieObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPPieObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPPieObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
		dynamic_cast<KPPieObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPPieObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPPieObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPPieObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPPieObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPPieObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPPieObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_PART: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPPartObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPPartObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPPartObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPPartObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPPartObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPPartObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPPartObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPPartObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPPartObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_TEXT: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPTextObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPTextObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPTextObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPTextObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPTextObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPTextObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPTextObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPTextObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPTextObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_PICTURE: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPPixmapObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPPixmapObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPPixmapObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPPixmapObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPPixmapObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPPixmapObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPPixmapObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPPixmapObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPPixmapObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
	case OT_CLIPART: {
	    if ( oldPen.count() > i )
		dynamic_cast<KPClipartObject*>( kpobject )->setPen( oldPen.at( i )->pen );
	    if ( oldBrush.count() > i ) {
		dynamic_cast<KPClipartObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
		dynamic_cast<KPClipartObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
		dynamic_cast<KPClipartObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
		dynamic_cast<KPClipartObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
		dynamic_cast<KPClipartObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
		dynamic_cast<KPClipartObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
		dynamic_cast<KPClipartObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
		dynamic_cast<KPClipartObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
	    }
	    doc->repaint( kpobject );
	} break;
        case OT_FREEHAND: {
	    if ( oldPen.count() > i ) {
		dynamic_cast<KPFreehandObject*>( kpobject )->setPen( oldPen.at( i )->pen );
		dynamic_cast<KPFreehandObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPFreehandObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
		doc->repaint( kpobject );
	    }
	} break;
        case OT_POLYLINE: {
	    if ( oldPen.count() > i ) {
		dynamic_cast<KPPolylineObject*>( kpobject )->setPen( oldPen.at( i )->pen );
		dynamic_cast<KPPolylineObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPPolylineObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
		doc->repaint( kpobject );
	    }
	} break;
        case OT_QUADRICBEZIERCURVE: {
	    if ( oldPen.count() > i ) {
		dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->setPen( oldPen.at( i )->pen );
		dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
		doc->repaint( kpobject );
	    }
	} break;
        case OT_CUBICBEZIERCURVE: {
	    if ( oldPen.count() > i ) {
		dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->setPen( oldPen.at( i )->pen );
		dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->setLineBegin( oldPen.at( i )->lineBegin );
		dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->setLineEnd( oldPen.at( i )->lineEnd );
		doc->repaint( kpobject );
	    }
	} break;
        case OT_POLYGON: {
            if ( oldPen.count() > i )
                dynamic_cast<KPPolygonObject*>( kpobject )->setPen( oldPen.at( i )->pen );
            if ( oldBrush.count() > i ) {
                dynamic_cast<KPPolygonObject*>( kpobject )->setBrush( oldBrush.at( i )->brush );
                dynamic_cast<KPPolygonObject*>( kpobject )->setFillType( oldBrush.at( i )->fillType );
                dynamic_cast<KPPolygonObject*>( kpobject )->setGColor1( oldBrush.at( i )->gColor1 );
                dynamic_cast<KPPolygonObject*>( kpobject )->setGColor2( oldBrush.at( i )->gColor2 );
                dynamic_cast<KPPolygonObject*>( kpobject )->setGType( oldBrush.at( i )->gType );
                dynamic_cast<KPPolygonObject*>( kpobject )->setGUnbalanced( oldBrush.at( i )->unbalanced );
                dynamic_cast<KPPolygonObject*>( kpobject )->setGXFactor( oldBrush.at( i )->xfactor );
                dynamic_cast<KPPolygonObject*>( kpobject )->setGYFactor( oldBrush.at( i )->yfactor );
            }
            doc->repaint( kpobject );
        } break;
	default: break;
	}
    }
}


/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

/*================================================================*/
PgConfCmd::PgConfCmd( QString _name, bool _manualSwitch, bool _infinitLoop,
                      PageEffect _pageEffect, PresSpeed _presSpeed, int _pageTimer,
                      bool _soundEffect, QString _fileName,
                      bool _oldManualSwitch, bool _oldInfinitLoop,
                      PageEffect _oldPageEffect, PresSpeed _oldPresSpeed, int _oldPageTimer,
                      bool _oldSoundEffect, QString _oldFileName,
                      KPresenterDoc *_doc, KPrPage *_page )
    : KCommand( _name )
{
    manualSwitch = _manualSwitch;
    infinitLoop = _infinitLoop;
    pageEffect = _pageEffect;
    presSpeed = _presSpeed;
    pageTimer = _pageTimer;
    soundEffect = _soundEffect;
    fileName = _fileName;
    oldManualSwitch = _oldManualSwitch;
    oldInfinitLoop = _oldInfinitLoop;
    oldPageEffect = _oldPageEffect;
    oldPresSpeed = _oldPresSpeed;
    oldPageTimer = _oldPageTimer;
    oldSoundEffect = _oldSoundEffect;
    oldFileName = _oldFileName;
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
PgLayoutCmd::PgLayoutCmd( QString _name, KoPageLayout _layout, KoPageLayout _oldLayout,
                          KPresenterView *_view )
    : KCommand( _name )
{
    layout = _layout;
    oldLayout = _oldLayout;
    view = _view;
}

/*====================== execute =================================*/
void PgLayoutCmd::execute()
{
    view->kPresenterDoc()->setPageLayout( layout );
    view->getHRuler()->setPageLayout( layout );
    view->getVRuler()->setPageLayout( layout );
    view->setRanges();
}

/*====================== unexecute ===============================*/
void PgLayoutCmd::unexecute()
{
    view->kPresenterDoc()->setPageLayout( oldLayout );
    view->getHRuler()->setPageLayout( oldLayout );
    view->getVRuler()->setPageLayout( oldLayout );
    view->setRanges();
}


/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

/*======================== constructor ===========================*/
PieValueCmd::PieValueCmd( QString _name, QPtrList<PieValues> &_oldValues, PieValues _newValues,
                          QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KCommand( _name ), oldValues( _oldValues ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldValues.setAutoDelete( false );
    doc = _doc;
    newValues = _newValues;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
PieValueCmd::~PieValueCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldValues.setAutoDelete( true );
    oldValues.clear();
}

/*====================== execute =================================*/
void PieValueCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieType( newValues.pieType );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieAngle( newValues.pieAngle );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieLength( newValues.pieLength );
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void PieValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieType( oldValues.at( i )->pieType );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieAngle( oldValues.at( i )->pieAngle );
        dynamic_cast<KPPieObject*>( objects.at( i ) )->setPieLength( oldValues.at( i )->pieLength );
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: PolygonSettingCmd                                       */
/******************************************************************/

/*======================== constructor ===========================*/
PolygonSettingCmd::PolygonSettingCmd( const QString &_name, QPtrList<PolygonSettings> &_oldSettings,
                                      PolygonSettings _newSettings, QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KCommand( _name ), oldSettings( _oldSettings ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldSettings.setAutoDelete( false );
    doc = _doc;
    newSettings = _newSettings;

    for ( unsigned int i = 0; i < objects.count(); ++i )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
PolygonSettingCmd::~PolygonSettingCmd()
{
    for ( unsigned int i = 0; i < objects.count(); ++i )
        objects.at( i )->decCmdRef();
    oldSettings.setAutoDelete( true );
    oldSettings.clear();
}

/*====================== execute =================================*/
void PolygonSettingCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i )
        dynamic_cast<KPPolygonObject*>( objects.at( i ) )->setPolygonSettings( newSettings.checkConcavePolygon,
                                                                               newSettings.cornersValue,
                                                                               newSettings.sharpnessValue );

    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void PolygonSettingCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); ++i )
        dynamic_cast<KPPolygonObject*>( objects.at( i ) )->setPolygonSettings( oldSettings.at( i )->checkConcavePolygon,
                                                                               oldSettings.at( i )->cornersValue,
                                                                               oldSettings.at( i )->sharpnessValue );

    doc->repaint( false );
}

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

/*======================== constructor ===========================*/
RectValueCmd::RectValueCmd( QString _name, QPtrList<RectValues> &_oldValues, RectValues _newValues,
                            QPtrList<KPObject> &_objects, KPresenterDoc *_doc )
    : KCommand( _name ), oldValues( _oldValues ), objects( _objects )
{
    objects.setAutoDelete( false );
    oldValues.setAutoDelete( false );
    doc = _doc;
    newValues = _newValues;

    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
RectValueCmd::~RectValueCmd()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->decCmdRef();
    oldValues.setAutoDelete( true );
    oldValues.clear();
}

/*====================== execute =================================*/
void RectValueCmd::execute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        dynamic_cast<KPRectObject*>( objects.at( i ) )->setRnds( newValues.xRnd, newValues.yRnd );

    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RectValueCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        dynamic_cast<KPRectObject*>( objects.at( i ) )->setRnds( oldValues.at( i )->xRnd, oldValues.at( i )->yRnd );

    doc->repaint( false );
}

/******************************************************************/
/* Class: ResizeCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
ResizeCmd::ResizeCmd( QString _name, QPoint _m_diff, QSize _r_diff, KPObject *_object, KPresenterDoc *_doc )
    : KCommand( _name ), m_diff( _m_diff ), r_diff( _r_diff )
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

    oldRect = object->getBoundingRect( );
    object->moveBy( m_diff );
    object->resizeBy( r_diff );
    if ( object->getType() == OT_TEXT )
    {
	( (KPTextObject*)object )->recalcPageNum( doc );
        if(object->isSelected())
            doc->updateRuler();
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

    oldRect = object->getBoundingRect( );
    object->moveBy( -m_diff.x(), -m_diff.y() );
    object->resizeBy( -r_diff.width(), -r_diff.height() );
    if ( object->getType() == OT_TEXT )
    {
	( (KPTextObject*)object )->recalcPageNum( doc );
        if(object->isSelected())
            doc->updateRuler();
    }

    if ( _repaint ) {
	doc->repaint( oldRect );
	doc->repaint( object );
    }
}


KPrPasteTextCommand::KPrPasteTextCommand( KoTextDocument *d, int parag, int idx,
                                const QCString & data )
    : QTextCommand( d ), m_parag( parag ), m_idx( idx ), m_data( data )
{
}

QTextCursor * KPrPasteTextCommand::execute( QTextCursor *c )
{
    Qt3::QTextParag *firstParag = doc->paragAt( m_parag );
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

            KoParagLayout paragLayout = textdoc->textObject()->loadParagLayout(paragElem);
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


QTextCursor * KPrPasteTextCommand::unexecute( QTextCursor *c )
{
    Qt3::QTextParag *firstParag = doc->paragAt( m_parag );
    if ( !firstParag ) {
        qWarning( "can't locate parag at %d, last parag: %d", m_parag, doc->lastParag()->paragId() );
        return 0;
    }
    cursor.setParag( firstParag );
    cursor.setIndex( m_idx );
    doc->setSelectionStart( KoTextDocument::Temp, &cursor );

    Qt3::QTextParag *lastParag = doc->paragAt( m_lastParag );
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


KPrChangeVariableSettingCommand::KPrChangeVariableSettingCommand( const QString &name, KPresenterDoc *_doc, int _oldVarOffset, int _newVarOffset):
    KCommand(name),
    m_doc(_doc),
    oldVarOffset(_oldVarOffset),
    newVarOffset(_newVarOffset)
{
}

void KPrChangeVariableSettingCommand::execute()
{
    m_doc->getVariableCollection()->variableSetting()->setNumberOffset(newVarOffset);
    m_doc->recalcVariables( VT_PGNUM );
}

void KPrChangeVariableSettingCommand::unexecute()
{
    m_doc->getVariableCollection()->variableSetting()->setNumberOffset(oldVarOffset);
    m_doc->recalcVariables( VT_PGNUM );
}
