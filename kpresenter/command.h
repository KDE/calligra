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
/* Module: Command (header)                                       */
/******************************************************************/

#ifndef command_h
#define command_h

#include <qstring.h>

/******************************************************************/
/* Class: Command                                                 */
/******************************************************************/

class Command
{
public:
    Command( QString _name );
    virtual ~Command()
    {; }

    virtual void execute() = 0;
    virtual void unexecute() = 0;

    virtual QString getName()
    { return name; }

protected:
    Command() : name()
    {; }

    QString name;

};

#endif
