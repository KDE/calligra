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
/* Module: Shadow Command                                         */
/******************************************************************/

#include "kpresenter_doc.h"
#include "shadowcmd.h"

/******************************************************************/
/* Class: ShadowCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
ShadowCmd::ShadowCmd( QString _name, QList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
                      QList<KPObject> &_objects, KPresenterDoc *_doc )
    : Command( _name ), oldShadow( _oldShadow ), objects( _objects )
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
        objects.at( i )->setShadowDistance( newShadow.shadowDistance );
        objects.at( i )->setShadowDirection( newShadow.shadowDirection );
        objects.at( i )->setShadowColor( newShadow.shadowColor );
    }
    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void ShadowCmd::unexecute()
{
    for ( unsigned int i = 0; i < objects.count(); i++ )
    {
        objects.at( i )->setShadowDistance( oldShadow.at( i )->shadowDistance );
        objects.at( i )->setShadowDirection( oldShadow.at( i )->shadowDirection );
        objects.at( i )->setShadowColor( oldShadow.at( i )->shadowColor );
    }
    doc->repaint( false );
}
