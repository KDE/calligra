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
/* Module: Resize Command (header)                                */
/******************************************************************/

#ifndef resizecmd_h
#define resizecmd_h

#include <qpoint.h>
#include <qsize.h>

#include "command.h"

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: ResizeCmd                                               */
/******************************************************************/

class ResizeCmd : public Command
{
public:
    ResizeCmd( QString _name, QPoint _m_diff, QSize _r_diff, KPObject *_object, KPresenterDoc *_doc );
    ~ResizeCmd();

    virtual void execute();
    virtual void unexecute();
    virtual void unexecute( bool _repaint );

protected:
    ResizeCmd()
    {; }

    QPoint m_diff;
    QSize r_diff;
    KPObject *object;
    KPresenterDoc *doc;

};

#endif
