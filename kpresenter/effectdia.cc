/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Effect Dialog                                          */
/******************************************************************/

#include "effectdia.h"
#include "effectdia.moc"

/******************************************************************/
/* class EffectDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
EffectDia::EffectDia(QWidget* parent,const char* name,int _pageNum,int _objNum,KPresenterView_impl *_view)
  :QDialog(parent,name,true)
{
  pageNum = _pageNum;
  objNum = _objNum;
  view = _view;

  resize(100,100);
}

/*===================== destructor ===============================*/
EffectDia::~EffectDia()
{
}














