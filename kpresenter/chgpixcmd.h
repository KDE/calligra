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
/* Module: Change Pixmap (header)                                 */
/******************************************************************/

#ifndef chgpixcmd_h
#define chgpixcmd_h

#include <qstring.h>

#include "command.h"
#include "kppixmapobject.h"

class KPresenterDocument_impl;

/******************************************************************/
/* Class: ChgPixCmd                                               */
/******************************************************************/

class ChgPixCmd : public Command
{
  Q_OBJECT

public:
  ChgPixCmd(QString _name,KPPixmapObject *_oldObject,KPPixmapObject *_newObject,
	    KPresenterDocument_impl *_doc);
  ~ChgPixCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  ChgPixCmd()
    {;}

  KPPixmapObject *oldObject,*newObject;
  KPresenterDocument_impl *doc;

};

#endif
