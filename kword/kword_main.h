/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Main (header)                                          */
/******************************************************************/

#ifndef kword_main_h
#define kword_main_h

class PartShell_impl;
class ImageApp;
class ImageDocument_impl;

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "kword_doc.h"
#include "kword_shell.h"

/******************************************************************/
/* Class: KWordApp                                                */
/******************************************************************/

class KWordApp : public OPApplication
{
  Q_OBJECT

public:
  KWordApp(int argc,char** argv);
  ~KWordApp();
  
  virtual void start();
};

#endif
