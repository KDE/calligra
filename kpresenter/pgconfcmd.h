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
/* Module: Set PageConfig Command (header)                        */
/******************************************************************/

#ifndef pgconfcmd_h
#define pgconfcmd_h

#include "command.h"
#include "global.h"

#include <qmap.h>

class KPresenterDoc;

/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

class PgConfCmd : public Command
{
public:
    PgConfCmd( QString _name, bool _manualSwitch, bool _infinitLoop,
               PageEffect _pageEffect, PresSpeed _presSpeed,
               PresentSlides _presSlides, const QMap<int,bool> &_selectedSlides,
               bool _oldManualSwitch, bool _oldInfinitLoop,
               PageEffect _oldPageEffect, PresSpeed _oldPresSpeed,
               PresentSlides _oldPresSlides, const QMap<int,bool> &_oldSelectedSlides,
               KPresenterDoc *_doc, int _pgNum );

    virtual void execute();
    virtual void unexecute();

protected:
    PgConfCmd()
    {; }

    bool manualSwitch, oldManualSwitch;
    bool infinitLoop, oldInfinitLoop;
    PageEffect pageEffect, oldPageEffect;
    PresSpeed presSpeed, oldPresSpeed;
    int pgNum;
    KPresenterDoc *doc;
    PresentSlides presSlides, oldPresSlides;
    QMap<int,bool> selectedSlides, oldSelectedSlides;
    
};

#endif
