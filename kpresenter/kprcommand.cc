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
#include <kpclosedlineobject.h>

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

    doc->setGridValue( gridX, gridY, false );
    doc->updateRuler();
    doc->setTxtBackCol( txtBackCol );
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void SetOptionsCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
        objects.at( i )->moveBy( -(*diffs.at( i )).x(), -(*diffs.at( i )).y() );
    doc->setGridValue( oldGridX, oldGridY, false );
    doc->updateRuler();
    doc->setTxtBackCol( otxtBackCol );
    doc->repaint( false );
}

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
SetBackCmd::SetBackCmd( const QString &_name, const QColor &_backColor1, const QColor &_backColor2, BCType _bcType,
			bool _backUnbalanced, int _backXFactor, int _backYFactor,
			const KoPictureKey & _backPix,
                        BackView _backView, BackType _backType,
			const QColor &_oldBackColor1, const QColor &_oldBackColor2, BCType _oldBcType,
			bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
			const KoPictureKey & _oldBackPix,
                        BackView _oldBackView, BackType _oldBackType,
			bool _takeGlobal, KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name ), backColor1( _backColor1 ), backColor2( _backColor2 ), unbalanced( _backUnbalanced ),
      xfactor( _backXFactor ), yfactor( _backYFactor ), backPix( _backPix ),
      oldBackColor1( _oldBackColor1 ), oldBackColor2( _oldBackColor2 ), oldUnbalanced( _oldBackUnbalanced ),
      oldXFactor( _oldBackXFactor ), oldYFactor( _oldBackYFactor ), oldBackPix( _oldBackPix )
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
    if ( doc->refreshSideBar())
    {
        int pos=doc->pageList().findRef(m_page);
        doc->updateSideBarItem(pos);
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
    if ( doc->refreshSideBar())
    {

        int pos=doc->pageList().findRef(m_page);
        doc->updateSideBarItem(pos);
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
    if ( doc->refreshSideBar())
    {
        int pos=doc->pageList().findRef(m_page);
        doc->updateSideBarItem(pos);
    }
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
    if ( doc->refreshSideBar())
    {

        int pos=doc->pageList().findRef(m_page);
        doc->updateSideBarItem(pos);
    }
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
    if ( doc->refreshSideBar())
    {
        int pos=doc->pageList().findRef(m_page);
        doc->updateSideBarItem(pos);
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
    if ( doc->refreshSideBar())
    {
        int pos=doc->pageList().findRef(m_page);
        doc->updateSideBarItem(pos);
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
/* Class: PenCmd						  */
/******************************************************************/

PenCmd::PenCmd(const QString &_name, QPtrList<Pen> &_oldPen, Pen _newPen,
               QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags)
    : KNamedCommand(_name), doc(_doc), oldPen(_oldPen), objects(_objects),
      newPen(_newPen), flags(_flags)
{
    objects.setAutoDelete( false );
    oldPen.setAutoDelete( false );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
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
    Pen tmpPen = newPen;

    for ( int i = 0; i < static_cast<int>( objects.count() ); i++ )
    {
        if (!(flags & LineBegin))
            newPen.lineBegin = oldPen.at( i )->lineBegin;

        if (!(flags & LineEnd))
            newPen.lineEnd = oldPen.at( i )->lineEnd;

        if (!(flags & Color))
            if (newPen.pen != Qt::NoPen)
                newPen.pen = QPen(oldPen.at( i )->pen.color(), newPen.pen.width(), newPen.pen.style());
            else
                newPen.pen = QPen(oldPen.at( i )->pen.color(), oldPen.at( i )->pen.width(), Qt::NoPen);

        if (!(flags & Width))
            if (newPen.pen != Qt::NoPen)
                newPen.pen = QPen(newPen.pen.color(), oldPen.at( i )->pen.width(), newPen.pen.style());
            else
                newPen.pen = QPen(oldPen.at( i )->pen.color(), oldPen.at( i )->pen.width(), Qt::NoPen);

        if (!(flags & Style))
            if (newPen.pen != Qt::NoPen)
                newPen.pen = QPen(newPen.pen.color(), newPen.pen.width(), oldPen.at( i )->pen.style());
            else
                newPen.pen = QPen(oldPen.at( i )->pen.color(), oldPen.at( i )->pen.width(), Qt::NoPen);

        applyPen(objects.at( i ), &newPen);
    }
    newPen = tmpPen;
}

void PenCmd::applyPen(KPObject *kpobject, Pen *tmpPen)
{
    switch (kpobject->getType()) {
    case OT_LINE: {
        KPLineObject* obj=dynamic_cast<KPLineObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            obj->setLineBegin( tmpPen->lineBegin );
            obj->setLineEnd( tmpPen->lineEnd );
            doc->repaint( obj );
	}
    } break;
    case OT_RECT: {
        KPRectObject* obj=dynamic_cast<KPRectObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_ELLIPSE: {
        KPEllipseObject* obj=dynamic_cast<KPEllipseObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_AUTOFORM: {
        KPAutoformObject* obj=dynamic_cast<KPAutoformObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_PIE: {
        KPPieObject* obj=dynamic_cast<KPPieObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_PART: {
        KPPartObject* obj=dynamic_cast<KPPartObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_TEXT: {
        KPTextObject* obj=dynamic_cast<KPTextObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_PICTURE: {
        KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_CLIPART: {
        KPClipartObject* obj=dynamic_cast<KPClipartObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_FREEHAND: {
        KPFreehandObject *obj=dynamic_cast<KPFreehandObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            obj->setLineBegin( tmpPen->lineBegin );
            obj->setLineEnd( tmpPen->lineEnd );
            doc->repaint( obj );
	}
    } break;
    case OT_POLYLINE: {
        KPPolylineObject *obj=dynamic_cast<KPPolylineObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            obj->setLineBegin( tmpPen->lineBegin );
            obj->setLineEnd( tmpPen->lineEnd );
            doc->repaint( obj );
	}
    } break;
    case OT_QUADRICBEZIERCURVE: {
        KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            obj->setLineBegin( tmpPen->lineBegin );
            obj->setLineEnd( tmpPen->lineEnd );
            doc->repaint( obj );
	}
    } break;
    case OT_CUBICBEZIERCURVE: {
        KPCubicBezierCurveObject* obj=dynamic_cast<KPCubicBezierCurveObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            obj->setLineBegin( tmpPen->lineBegin );
            obj->setLineEnd( tmpPen->lineEnd );
            doc->repaint( obj );
	}
    } break;
    case OT_POLYGON: {
        KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
        if(obj)
	{
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    case OT_CLOSED_LINE: {
        KPClosedLineObject *obj = dynamic_cast<KPClosedLineObject*>( kpobject );
        if( obj ) {
            obj->setPen( tmpPen->pen );
            doc->repaint( obj );
	}
    } break;
    default: break;
    }
}

void PenCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        if( oldPen.count() > i )
        {
            applyPen(objects.at( i ), oldPen.at( i ));
        }
    }
}

/******************************************************************/
/* Class: BrushCmd						  */
/******************************************************************/

BrushCmd::BrushCmd(const QString &_name, QPtrList<Brush> &_oldBrush, Brush _newBrush,
                   QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags)
    : KNamedCommand(_name), doc(_doc), oldBrush(_oldBrush), objects(_objects),
      newBrush(_newBrush), flags(_flags)
{
    objects.setAutoDelete( false );
    oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->incCmdRef();
}

BrushCmd::~BrushCmd()
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
        it.current()->decCmdRef();

    oldBrush.setAutoDelete( true );
    oldBrush.clear();
}

void BrushCmd::execute()
{
    Brush tmpBrush = newBrush;

    for ( int i = 0; i < static_cast<int>( objects.count() ); i++ )
    {
        if (!(flags & BrushColor))
            if (newBrush.brush != Qt::NoBrush)
                newBrush.brush = QBrush(oldBrush.at( i )->brush.color(), newBrush.brush.style());
            else
                newBrush.brush = QBrush(oldBrush.at( i )->brush.color(), Qt::NoBrush);

        if (!(flags & BrushStyle))
            if (newBrush.brush != Qt::NoBrush)
                newBrush.brush = QBrush(newBrush.brush.color(), oldBrush.at( i )->brush.style());
            else
                newBrush.brush = QBrush(oldBrush.at( i )->brush.color(), Qt::NoBrush);

        if (!(flags & BrushGradientSelect))
            newBrush.fillType = oldBrush.at( i )->fillType;

        if (!(flags & GradientColor1))
            newBrush.gColor1 = oldBrush.at( i )->gColor1;

        if (!(flags & GradientColor2))
            newBrush.gColor2 = oldBrush.at( i )->gColor2;

        if (!(flags & GradientType))
            newBrush.gType = oldBrush.at( i )->gType;

        if (!(flags & GradientBalanced))
        {
            newBrush.unbalanced = oldBrush.at( i )->unbalanced;
            newBrush.xfactor = oldBrush.at( i )->xfactor;
            newBrush.yfactor = oldBrush.at( i )->yfactor;
        }

        applyBrush(objects.at( i ), &newBrush);
    }
    newBrush = tmpBrush;
}

void BrushCmd::applyBrush(KPObject *kpobject, Brush *tmpBrush)
{
    switch (kpobject->getType()) {
    case OT_RECT: {
        KPRectObject* obj=dynamic_cast<KPRectObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_ELLIPSE: {
        KPEllipseObject* obj=dynamic_cast<KPEllipseObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_AUTOFORM: {
        KPAutoformObject* obj=dynamic_cast<KPAutoformObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_PIE: {
        KPPieObject* obj=dynamic_cast<KPPieObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_PART: {
        KPPartObject* obj=dynamic_cast<KPPartObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_TEXT: {
        KPTextObject* obj=dynamic_cast<KPTextObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_PICTURE: {
        KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_CLIPART: {
        KPClipartObject* obj=dynamic_cast<KPClipartObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_POLYGON: {
        KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
        if(obj)
	{
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
    } break;
    case OT_CLOSED_LINE: {
        KPClosedLineObject *obj = dynamic_cast<KPClosedLineObject*>( kpobject );
        if( obj ) {
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
    } break;
    default: break;
    }
}

void BrushCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ ) {
        if( oldBrush.count() > i)
        {
            applyBrush(objects.at( i ), oldBrush.at( i ));
        }
    }
}

/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

/*================================================================*/
PgConfCmd::PgConfCmd( const QString &_name, bool _manualSwitch, bool _infiniteLoop,
                      PageEffect _pageEffect, PresSpeed _presSpeed, int _pageTimer,
                      bool _soundEffect, const QString &_fileName, bool _showPresentationDuration,
                      bool _oldManualSwitch, bool _oldInfiniteLoop,
                      PageEffect _oldPageEffect, PresSpeed _oldPresSpeed, int _oldPageTimer,
                      bool _oldSoundEffect, const QString &_oldFileName, bool _oldShowPresentationDuration,
                      KPresenterDoc *_doc, KPrPage *_page )
    : KNamedCommand( _name )
{
    manualSwitch = _manualSwitch;
    infiniteLoop = _infiniteLoop;
    pageEffect = _pageEffect;
    presSpeed = _presSpeed;
    pageTimer = _pageTimer;
    soundEffect = _soundEffect;
    fileName = _fileName;
    showPresentationDuration = _showPresentationDuration;
    oldManualSwitch = _oldManualSwitch;
    oldInfiniteLoop = _oldInfiniteLoop;
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
    doc->setInfiniteLoop( infiniteLoop );
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
    doc->setInfiniteLoop( oldInfiniteLoop );
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
                          QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags )
    : KNamedCommand( _name ), oldValues( _oldValues ), objects( _objects ), flags(_flags)
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
            if (flags & Type)
                obj->setPieType( newValues.pieType );
            if (flags & Angle)
                obj->setPieAngle( newValues.pieAngle );
            if (flags & Length)
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
                                      PolygonSettings _newSettings, QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags )
    : KNamedCommand( _name ), oldSettings( _oldSettings ), objects( _objects ), flags(_flags)
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
            if (flags & ConcaveConvex)
                obj->setCheckConcavePolygon(newSettings.checkConcavePolygon);
            if (flags & Corners)
                obj->setCornersValue(newSettings.cornersValue);
            if (flags & Sharpness)
            obj->setSharpnessValue(newSettings.sharpnessValue );
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
            obj->setCheckConcavePolygon(oldSettings.at( i )->checkConcavePolygon);
            obj->setCornersValue(oldSettings.at( i )->cornersValue);
            obj->setSharpnessValue(oldSettings.at( i )->sharpnessValue);
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
            obj->setPictureMirrorType(newSettings.mirrorType);
            obj->setPictureDepth(newSettings.depth);
            obj->setPictureSwapRGB(newSettings.swapRGB);
            obj->setPictureGrayscal(newSettings.grayscal);
            obj->setPictureBright(newSettings.bright);
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
            obj->setPictureMirrorType(oldSettings.at( i )->mirrorType);
            obj->setPictureDepth(oldSettings.at( i )->depth);
            obj->setPictureSwapRGB(oldSettings.at( i )->swapRGB);
            obj->setPictureGrayscal(oldSettings.at( i )->grayscal);
            obj->setPictureBright(oldSettings.at( i )->bright);
        }
    }
    doc->repaint( false );
}

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

/*======================== constructor ===========================*/
RectValueCmd::RectValueCmd( const QString &_name, QPtrList<RectValues> &_oldValues, RectValues _newValues,
                            QPtrList<KPObject> &_objects, KPresenterDoc *_doc, int _flags )
    : KNamedCommand( _name ), oldValues( _oldValues ), objects( _objects ), flags(_flags)
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
        {
            if (flags & XRnd)
            {
                int xtmp, ytmp;
                obj->getRnds(xtmp, ytmp);
                obj->setRnds(newValues.xRnd, ytmp);
            }

            if (flags & YRnd)
            {
                int xtmp, ytmp;
                obj->getRnds(xtmp, ytmp);
                obj->setRnds(xtmp, newValues.yRnd);
            }
        }
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
        doc->layout( object );
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
        doc->layout( object );
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
    //kdDebug(33001) << "KWPasteTextCommand::execute m_parag=" << m_parag << " m_idx=" << m_idx
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
    kdDebug(33001) << "KPrPasteTextCommand::execute Inserting text: '" << text << "'" << endl;
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
    //kdDebug(33001) << "KPrPasteTextCommand::execute starting at parag " << parag << " " << parag->paragId() << endl;

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
        QValueList<QDomElement> listVariable;
        listVariable.clear();
        int i = 0;
        if ( item == 0 && m_idx > 0 )
            i = m_idx;
        while ( !n.isNull() ) {
            if ( n.tagName() == "TEXT" ) {
                QString txt = n.firstChild().toText().data();
                KoTextFormat fm = textdoc->textObject()->loadFormat( n, parag->paragraphFormat(), textdoc->textObject()->kPresenterDocument()->defaultFont() );
                parag->setFormat( i, txt.length(), textdoc->formatCollection()->format( &fm ) );
                i += txt.length();
            }
            else if ( n.tagName() == "CUSTOM" )
            {
                listVariable.append( n );
            }
            n = n.nextSibling().toElement();
        }
        parag->format();
        parag->setChanged( TRUE );
        textdoc->textObject()->loadVariable( listVariable,parag, m_idx );
        parag = static_cast<KoTextParag *>(parag->next());
        //kdDebug(33001) << "KWPasteTextCommand::execute going to next parag: " << parag << endl;
    }
    textdoc->textObject()->textObject()->setNeedSpellCheck( true );
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


KPrChangeVariableSettingsCommand::KPrChangeVariableSettingsCommand( const QString &name, KPresenterDoc *_doc, bool _oldValue, bool _newValue, VariableProperties _type):
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
        m_doc->getVariableCollection()->variableSetting()->setDisplayFiedCode(b);
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

KPrDeletePageCmd::KPrDeletePageCmd( const QString &_name, int pos,KPrPage *_page, KPresenterDoc *_doc):
    KNamedCommand(_name),
    doc(_doc),
    m_page(_page),
    position(pos)
{
}

KPrDeletePageCmd::~KPrDeletePageCmd()
{
}

void KPrDeletePageCmd::execute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->addRemovePage( position, false );
}

void KPrDeletePageCmd::unexecute()
{
    doc->deSelectAllObj();
    doc->insertPage( m_page, position);
    doc->addRemovePage( position, true );
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
}

void KPrInsertPageCmd::execute()
{
    doc->deSelectAllObj();
    doc->insertPage( m_page, position);
    doc->addRemovePage( position, true );
    m_page->completeLoading( false, -1 );
}

void KPrInsertPageCmd::unexecute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->addRemovePage( position, false );
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
    doc->movePageTo( oldPosition, newPosition );
}

void KPrMovePageCmd::unexecute()
{
    doc->deSelectAllObj();
    doc->takePage(m_page);
    doc->insertPage(m_page,oldPosition);
    doc->movePageTo( newPosition, oldPosition );
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

KPrChangeFieldVariableSubType::KPrChangeFieldVariableSubType( const QString &name, KPresenterDoc *_doc,
                                                              short int _oldValue, short int _newValue,
                                                              KPrFieldVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangeFieldVariableSubType::~KPrChangeFieldVariableSubType()
{
}

void KPrChangeFieldVariableSubType::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_FIELD );
}

void KPrChangeFieldVariableSubType::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_FIELD );
}

KPrChangeDateVariableSubType::KPrChangeDateVariableSubType( const QString &name, KPresenterDoc *_doc,
                        short int _oldValue, short int _newValue,
                        KPrDateVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangeDateVariableSubType::~KPrChangeDateVariableSubType()
{
}

void KPrChangeDateVariableSubType::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_DATE );
}

void KPrChangeDateVariableSubType::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_DATE );
}

KPrChangeDateVariableFormat::KPrChangeDateVariableFormat( const QString &name, KPresenterDoc *_doc,
                        const QString _oldValue, const QString _newValue,
                        KPrDateVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangeDateVariableFormat::~KPrChangeDateVariableFormat()
{
}

void KPrChangeDateVariableFormat::execute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableDateFormat *>(m_var->variableFormat())->m_strFormat = newValue;
    m_doc->recalcVariables( VT_DATE );
}

void KPrChangeDateVariableFormat::unexecute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableDateFormat *>(m_var->variableFormat())->m_strFormat = oldValue;
    m_doc->recalcVariables( VT_DATE );
}

KPrChangeTimeVariableSubType::KPrChangeTimeVariableSubType( const QString &name, KPresenterDoc *_doc,
                        short int _oldValue, short int _newValue,
                        KPrTimeVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangeTimeVariableSubType::~KPrChangeTimeVariableSubType()
{
}

void KPrChangeTimeVariableSubType::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_TIME );
}

void KPrChangeTimeVariableSubType::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_TIME );
}

KPrChangeTimeVariableFormat::KPrChangeTimeVariableFormat( const QString &name, KPresenterDoc *_doc,
                        const QString _oldValue, const QString _newValue,
                        KPrTimeVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangeTimeVariableFormat::~KPrChangeTimeVariableFormat()
{
}

void KPrChangeTimeVariableFormat::execute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableTimeFormat *>(m_var->variableFormat())->m_strFormat = newValue;
    m_doc->recalcVariables( VT_TIME );
}

void KPrChangeTimeVariableFormat::unexecute()
{
    Q_ASSERT(m_var);
    dynamic_cast<KoVariableTimeFormat *>(m_var->variableFormat())->m_strFormat = oldValue;
    m_doc->recalcVariables( VT_TIME );
}

KPrChangePgNumVariableValue::KPrChangePgNumVariableValue( const QString &name, KPresenterDoc *_doc,
                        short int _oldValue, short int _newValue,
                        KPrPgNumVariable *var):
    KNamedCommand(name),
    m_doc(_doc),
    newValue(_newValue),
    oldValue(_oldValue),
    m_var(var)
{
}

KPrChangePgNumVariableValue::~KPrChangePgNumVariableValue()
{
}

void KPrChangePgNumVariableValue::execute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(newValue);
    m_doc->recalcVariables( VT_PGNUM );
}

void KPrChangePgNumVariableValue::unexecute()
{
    Q_ASSERT(m_var);
    m_var->setVariableSubType(oldValue);
    m_doc->recalcVariables( VT_PGNUM );
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
        kdDebug(33001)<<"Error in void KPrHideShowHeaderFooter::execute()\n";
}

void KPrHideShowHeaderFooter::unexecute()
{
    if( m_textObject==m_doc->footer())
        m_doc->setFooter( !newValue );
    else if( m_textObject==m_doc->header())
        m_doc->setHeader( !newValue );
    else
        kdDebug(33001)<<"Error in void KPrHideShowHeaderFooter::unexecute()\n";

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

KPrGeometryPropertiesCommand::KPrGeometryPropertiesCommand( const QString &_name, QValueList<bool> &_lst, QPtrList<KPObject> &_objects, bool _newValue,  KPresenterDoc *_doc, KgpType _type):
    KNamedCommand( _name ),
    list( _lst),
    objects( _objects ),
    newValue( _newValue ),
    doc(_doc),
    m_type( _type )
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
        if ( m_type == ProtectSize )
            it.current()->setProtect( newValue );
        else if ( m_type == KeepRatio)
            it.current()->setKeepRatio( newValue );
    }
}

void KPrGeometryPropertiesCommand::unexecute()
{
    KPObject *obj = 0;
    for ( unsigned int i = 0; i < objects.count(); ++i ) {
	obj = objects.at( i );
        if ( m_type == ProtectSize )
            obj->setProtect( *list.at(i) );
        else if ( m_type == KeepRatio)
            obj->setKeepRatio( *list.at(i) );
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


KPrChangeMarginCommand::KPrChangeMarginCommand( const QString &name, KPTextObject *_obj, MarginsStruct _MarginsBegin, MarginsStruct _MarginsEnd ) :
    KNamedCommand(name),
    m_obj( _obj ),
    m_marginsBegin(_MarginsBegin),
    m_marginsEnd(_MarginsEnd)
{
}

void KPrChangeMarginCommand::execute()
{
    m_obj->setTextMargins( m_marginsEnd.leftMargin, m_marginsEnd.topMargin, m_marginsEnd.rightMargin, m_marginsEnd.bottomMargin);
    m_obj->resizeTextDocument();
    m_obj->kPresenterDocument()->layout(m_obj);
    m_obj->kPresenterDocument()->repaint(m_obj);
}

void KPrChangeMarginCommand::unexecute()
{
    m_obj->setTextMargins( m_marginsBegin.leftMargin, m_marginsBegin.topMargin, m_marginsBegin.rightMargin, m_marginsBegin.bottomMargin);
    m_obj->resizeTextDocument();
    m_obj->kPresenterDocument()->layout(m_obj);
    m_obj->kPresenterDocument()->repaint(m_obj);
}


KPrChangeVerticalAlignmentCommand::KPrChangeVerticalAlignmentCommand( const QString &name, KPTextObject *_obj, VerticalAlignmentType _oldAlign, VerticalAlignmentType _newAlign) :
    KNamedCommand(name),
    m_obj( _obj ),
    m_oldAlign(_oldAlign),
    m_newAlign(_newAlign)
{
}

void KPrChangeVerticalAlignmentCommand::execute()
{
    m_obj->setVerticalAligment( m_newAlign );
    m_obj->kPresenterDocument()->layout(m_obj);
    m_obj->kPresenterDocument()->repaint(m_obj);
}

void KPrChangeVerticalAlignmentCommand::unexecute()
{
    m_obj->setVerticalAligment( m_oldAlign );
    m_obj->kPresenterDocument()->layout(m_obj);
    m_obj->kPresenterDocument()->repaint(m_obj);
}


KPrChangeTabStopValueCommand::KPrChangeTabStopValueCommand( const QString &name, double _oldValue, double _newValue, KPresenterDoc *_doc):
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
