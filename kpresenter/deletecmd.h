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
/* Module: Delete Command (header)                                */
/******************************************************************/

#ifndef deletecmd_h
#define deletecmd_h

#include <qlist.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: DeleteCmd                                               */
/******************************************************************/

class DeleteCmd : public Command
{
public:
    DeleteCmd( QString _name, QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~DeleteCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    DeleteCmd()
    {; }

    QList<KPObject> objects;
    KPresenterDoc *doc;

};

#endif
