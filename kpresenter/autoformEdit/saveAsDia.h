/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
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
/* Module: save as dialog (header)                                */
/******************************************************************/

#ifndef SAVEASDIA_H
#define SAVEASDIA_H

#include <stdlib.h>

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qlistbox.h>
#include <qlined.h>
#include <qfile.h>

#include <kapp.h>

/******************************************************************/
/* class SaveAsDia                                                */
/******************************************************************/

class SaveAsDia : public QDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  SaveAsDia(QWidget* parent=0,const char* name=0);
  ~SaveAsDia();

protected:

  // get groups
  void getGroups();

  // ********** variables **********

  // dialog objects
  QStrList groupList;
  QListBox *groups;
  QLabel *grpLabel,*nameLabel;
  QPushButton *addGrp,*cancelBut,*okBut;
  QLineEdit *grpEdit,*nameEdit;

private slots:

  // save autoform as
  void saveAs();

  // add a group
  void addGroup();

signals:

  // dave autoform as
  void saveATFAs(const QString &,const QString &);

};
#endif //SAVEASDIA_H
