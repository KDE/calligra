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
/* Module: Lower/Raise Command (header)                           */
/******************************************************************/

#ifndef lowraicmd_h
#define lowraicmd_h

#include <qlist.h>

#include "command.h"

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: LowerRaiseCmd                                           */
/******************************************************************/

class LowerRaiseCmd : public Command
{
public:
    LowerRaiseCmd( QString _name, QList<KPObject> *_old, QList<KPObject> *__new, KPresenterDoc *_doc );
    ~LowerRaiseCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    LowerRaiseCmd()
    {; }

    QList<KPObject> *old, *_new;
    KPresenterDoc *doc;

};

#endif
