/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Footer/Header                                          */
/******************************************************************/

#ifndef FOOTER_HEADER_H
#define FOOTER_HEADER_H

#include <qvbox.h>
#include <qevent.h>

class KPresenterDoc;

class QTabWidget;

/******************************************************************/
/* Class: KPFooterHeaderEditor                                    */
/******************************************************************/

class KPFooterHeaderEditor : public QVBox
{
  Q_OBJECT
  
public:
  KPFooterHeaderEditor(KPresenterDoc *_doc);
  ~KPFooterHeaderEditor();

  void allowClose()
  { _allowClose = true; }
  
protected:
  void setupHeader();
  void setupFooter();
  
  void closeEvent(QCloseEvent *e) {
    // this dialog must not get closed!!
    if (!_allowClose) {
      hide();
      e->ignore();
    } else {
      QVBox::closeEvent(e);
    }
  }
  
  QTabWidget *tabwidget;

  KPresenterDoc *doc;
  bool _allowClose;
  
};

#endif
