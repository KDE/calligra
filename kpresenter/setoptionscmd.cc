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
/* Module: Set Options Command                                    */
/******************************************************************/

#include "kpresenter_doc.h"
#include "setoptionscmd.h"

#include <qrect.h>
#include <qsize.h>

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
SetOptionsCmd::SetOptionsCmd( QString _name, QList<QPoint> &_diffs, QList<KPObject> &_objects,
                              int _rastX, int _rastY, int _orastX, int _orastY,
                              QColor _txtBackCol, QColor _otxtBackCol, KPresenterDoc *_doc )
    : Command( _name ), diffs( _diffs ), objects( _objects ), txtBackCol( _txtBackCol ), otxtBackCol( _otxtBackCol )
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
