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
/* Module: Move Command (header)                                  */
/******************************************************************/

#ifndef movecmd_h
#define movecmd_h

#include <qpoint.h>
#include <qlist.h>
#include <qpoint.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: MoveByCmd                                               */
/******************************************************************/

class MoveByCmd : public Command
{
public:
    MoveByCmd( QString _name, QPoint _diff, QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~MoveByCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    MoveByCmd()
    {; }

    QPoint diff;
    QList<KPObject> objects;
    KPresenterDoc *doc;

};

/******************************************************************/
/* Class: MoveByCmd2                                              */
/******************************************************************/

class MoveByCmd2 : public Command
{
public:
    MoveByCmd2( QString _name, QList<QPoint> &_diffs, QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~MoveByCmd2();

    virtual void execute();
    virtual void unexecute();

protected:
    MoveByCmd2()
    {; }

    QList<QPoint> diffs;
    QList<KPObject> objects;
    KPresenterDoc *doc;

};

#endif
