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
/* Module: Autoform Choose Dialog (header)                        */
/******************************************************************/

#ifndef AFCHOOSE_H
#define AFCHOOSE_H

#include <qtabdlg.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qstring.h>
#include <qpushbt.h>
#include <qlistbox.h>
#include <qlist.h>
#include <qfileinf.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qevent.h>
#include <qfile.h>
#include <qcolor.h>

#include <kiconloaderdialog.h>
#include <kapp.h>

/******************************************************************/
/* class AFChoose                                                 */
/******************************************************************/

class AFChoose : public QTabDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  AFChoose(QWidget *parent=0,const char *name=0);
  ~AFChoose();

protected:

  // structure of a group
  struct Group
  {
    QFileInfo dir;
    QString name;
    QWidget *tab;
    KIconLoaderCanvas *loadWid;
    QLabel *label;
  };

  // set groups
  void getGroups();

  // setup tabs
  void setupTabs();

  // resize event
  void resizeEvent(QResizeEvent *);

  // ********** variables **********

  // list of groups and a pointer to a group
  QList<Group> groupList;
  Group *grpPtr;

private slots:

  // name changed
  void nameChanged(const QString &);

  // autoform chosen
  void chosen();

signals:

  //autoform chosen
  void formChosen(const QString &);

};
#endif //AFCHOOSE_H

