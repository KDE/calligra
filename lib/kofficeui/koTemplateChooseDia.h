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

#include <qdialog.h>
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
#include <qvbox.h>
#include <qradiobutton.h>
#include <qtabwidget.h>

#include <kiconloaderdialog.h>
#include <kapp.h>

class MyIconCanvas : public KIconLoaderCanvas
{
  Q_OBJECT
  
public:
  MyIconCanvas(QWidget *parent = 0,const QString &name = QString::null)
    : KIconLoaderCanvas(parent,name) {}
  
  bool isCurrentValid() { return sel_id >= 0 && sel_id < (int)name_list.count() && !getCurrent().isEmpty(); }
  
protected:
  void mousePressEvent(QMouseEvent *e) {
    KIconLoaderCanvas::mousePressEvent(e);
    if (sel_id >= 0 && sel_id < (int)name_list.count()) {
      QString s = getCurrent();
      emit currentChanged(s);
    } else {
      QString s = "";
      emit currentChanged(s);
    }
  }
  
signals:
  void currentChanged(const QString &);

};


/******************************************************************/
/* Class: KoTemplateChooseDia                                     */
/******************************************************************/

class KoTemplateChooseDia : public QDialog
{
  Q_OBJECT

public:
  enum ReturnType {Cancel,Template,File,Empty};
  
  KoTemplateChooseDia(QWidget *parent,const char *name,QString _globalTemplatePath,QString _personalTemplatePath,bool _hasCancel,bool _onlyTemplates);
  ~KoTemplateChooseDia() {;}

  static ReturnType chooseTemplate(QString _globalTemplatePath,QString _personalTemplatePath,QString &_template,bool _hasCancel,bool _onlyTemplates = true);

  QString getTemplate() { return templateName; }
  QString getFullTemplate() { return fullTemplateName; }
  ReturnType getReturnType() { return returnType; }
  
protected:
  struct Group
  {
    QFileInfo dir;
    QString name;
    QWidget *tab;
    MyIconCanvas *loadWid;
    QLabel *label;
  };

  void getGroups();
  void setupTabs();
  void resizeEvent(QResizeEvent *e) {
    QDialog::resizeEvent(e);
    back->resize(size());
  }
  
  QList<Group> groupList;
  Group *grpPtr;
  QString globalTemplatePath,personalTemplatePath;
  QString templateName,fullTemplateName;
  bool onlyTemplates;
  QVBox *back;
  QRadioButton *rbTemplates,*rbFile,*rbEmpty;
  QLabel *lFile;
  QPushButton *bFile,*ok;
  QTabWidget *tabs;
  ReturnType returnType;
    
private slots:
  void nameChanged(const QString &);
  void chosen();
  void currentChanged(const QString &);
  
  void openTemplate();
  void openFile();
  void openEmpty();
  void chooseFile();
  void tabsChanged(const QString &)
  { openTemplate(); }
  
signals:
  void templateChosen(const QString &);

};

#endif

