/*
   KOffice Library - (c) by Reginald Stadlbauer 1998              
   Version: 1.0                                                   
   Author: Reginald Stadlbauer                                    
   E-Mail: reggie@kde.org                                         
   Homepage: http://boch35.kfunigraz.ac.at/~rs                    
   needs c++ library Qt (http://www.troll.no)                     
   written for KDE (http://www.kde.org)                           
   License: GNU GPL                                               

   Module: About Dialog (header)                                  
*/

#ifndef KOABOUTDIA_H
#define KOABOUTDIA_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qmsgbox.h>
#include <qlayout.h>

#include <kbuttonbox.h>
#include <kapp.h>

/**
 * class KoAboutDia
 */
class KoAboutDia : public QDialog
{
  Q_OBJECT

public:

  // enums
  enum KoApplication {KPresenter,KSpread,KCharts,KImage,KAutoformEdit,KOffice,KDE};

  // constructor - destructor
  KoAboutDia(QWidget* parent=0,const char* name=0,KoApplication koapp=KDE,QString version = QString::null);
  ~KoAboutDia();

  // show about dialog
  static void about(KoApplication koapp,QString version = QString::null);

protected:

  // dialog objects
  QPixmap pLogo;
  QLabel *lLogo,*lInfo;
  QString author,email,add;
  QGridLayout *grid;
  QPushButton *bOk;
  KButtonBox *bbox;

};

#endif
