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
/* Module: Change Clipart (header)                                */
/******************************************************************/

#ifndef chgclipcmd_h
#define chgclipcmd_h

#include <qstring.h>

#include "command.h"
#include "kpclipartcollection.h"

class KPresenterDoc;
class KPClipartObject;

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

class ChgClipCmd : public Command
{
public:
    ChgClipCmd( QString _name, KPClipartObject *_object, KPClipartCollection::Key _oldName, 
                KPClipartCollection::Key _newName, KPresenterDoc *_doc );
    ~ChgClipCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    ChgClipCmd()
    {; }

    KPClipartObject *object;
    KPresenterDoc *doc;
    KPClipartCollection::Key oldKey, newKey;

};

#endif
