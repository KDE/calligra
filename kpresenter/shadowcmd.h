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
/* Module: Shadow Command (header)                                */
/******************************************************************/

#ifndef shadowcmd_h
#define shadowcmd_h

#include <qlist.h>
#include <qcolor.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: ShadowCmd                                               */
/******************************************************************/

class ShadowCmd : public Command
{
public:
    struct ShadowValues
    {
        int shadowDistance;
        ShadowDirection shadowDirection;
        QColor shadowColor;
    };

    ShadowCmd( QString _name, QList<ShadowValues> &_oldShadow, ShadowValues _newShadow,
               QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~ShadowCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    ShadowCmd()
    {; }

    KPresenterDoc *doc;
    QList<ShadowValues> oldShadow;
    QList<KPObject> objects;
    ShadowValues newShadow;

};

#endif
