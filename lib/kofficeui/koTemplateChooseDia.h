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
/* Module: Template Choose Dialog (header)                        */
/******************************************************************/

#ifndef koTemplateChooseDia_h
#define koTemplateChooseDia_h

#include <qtabdlg.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qstring.h>
#include <qpushbt.h>
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
/* Class: KoTemplateChooseDia                                     */
/******************************************************************/

class KoTemplateChooseDia : public QTabDialog
{
  Q_OBJECT

public:
  KoTemplateChooseDia(QWidget *parent,const char *name,QString _globalTemplatePath,QString _personalTemplatePath,bool _hasCancel); 
  ~KoTemplateChooseDia() {;}

  static bool chooseTemplate(QString _globalTemplatePath,QString _personalTemplatePath,QString &_template,bool _hasCancel);

  QString getTemplate() { return templateName; }
  QString getFullTemplate() { return fullTemplateName; }

protected:
  struct Group
  {
    QFileInfo dir;
    QString name;
    QWidget *tab;
    KIconLoaderCanvas *loadWid;
    QLabel *label;
  };

  void getGroups();
  void setupTabs();
  void resizeEvent(QResizeEvent *);

  QList<Group> groupList;
  Group *grpPtr;
  QString globalTemplatePath,personalTemplatePath;
  QString templateName,fullTemplateName;

private slots:
  void nameChanged(const char*);
  void chosen();

signals:
  void templateChosen(const char*);

};

#endif

