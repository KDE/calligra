/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Rotate Command                                         */
/******************************************************************/

#include "kpresenter_doc.h"
#include "rotatecmd.h"

/******************************************************************/
/* Class: RotateCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
RotateCmd::RotateCmd( QString _name, QList<RotateValues> &_oldRotate, float _newAngle,
                      QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), oldRotate( _oldRotate ), objects( _objects )
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

