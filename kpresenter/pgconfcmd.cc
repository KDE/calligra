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
/* Module: Set PageConfig Command                                 */
/******************************************************************/

#include "kpresenter_doc.h"
#include "pgconfcmd.h"
#include "kpbackground.h"

/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

/*================================================================*/
PgConfCmd::PgConfCmd( QString _name, bool _manualSwitch, bool _infinitLoop,
                      PageEffect _pageEffect, PresSpeed _presSpeed,
                      PresentSlides _presSlides, const QMap<int,bool> &_selectedSlides,
                      bool _oldManualSwitch, bool _oldInfinitLoop,
                      PageEffect _oldPageEffect, PresSpeed _oldPresSpeed,
                      PresentSlides _oldPresSlides, const QMap<int,bool> &_oldSelectedSlides,
                      KPresenterDoc *_doc, int _pgNum )
    : Command( _name )
{
    manualSwitch = _manualSwitch;
    infinitLoop = _infinitLoop;
    pageEffect = _pageEffect;
    presSpeed = _presSpeed;
    oldManualSwitch = _oldManualSwitch;
    oldInfinitLoop = _oldInfinitLoop;
    oldPageEffect = _oldPageEffect;
    oldPresSpeed = _oldPresSpeed;
    doc = _doc;
    pgNum = _pgNum;
    presSlides = _presSlides;
    oldPresSlides = _oldPresSlides;
    selectedSlides = QMap<int,bool>( _selectedSlides );
    oldSelectedSlides = QMap<int,bool>( _oldSelectedSlides );
}

/*================================================================*/
void PgConfCmd::execute()
{
    doc->setManualSwitch( manualSwitch );
    doc->setInfinitLoop( infinitLoop );
    doc->setPageEffect( pgNum, pageEffect );
    doc->setPresSpeed( presSpeed );
    doc->setPresentSlides( presSlides );
    doc->setSelectedSlides( selectedSlides );
}

/*================================================================*/
void PgConfCmd::unexecute()
{
    doc->setManualSwitch( oldManualSwitch );
    doc->setInfinitLoop( oldInfinitLoop );
    doc->setPageEffect( pgNum, oldPageEffect );
    doc->setPresSpeed( oldPresSpeed );
    doc->setPresentSlides( oldPresSlides );
    doc->setSelectedSlides( oldSelectedSlides );
}

