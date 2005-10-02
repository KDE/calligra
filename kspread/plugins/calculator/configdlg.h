
/*

 $Id$


 KCalc

 Copyright (C) Bernd Johannes Wuebben
               wuebben@math.cornell.edu
	       wuebben@kde.org

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

 */


#ifndef _CONFIG_DLG_H_
#define _CONFIG_DLG_H_

#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qframe.h>
#include <qspinbox.h>
#include <kcolorbutton.h>

#include "kcalc.h"



class ConfigDlg : public QDialog {

Q_OBJECT

public:

  ConfigDlg(QWidget *parent=0, const char *name=0, DefStruct *defstruct=NULL);

  ~ConfigDlg() {}

  DefStruct *defst ;
  QLineEdit *mail;
  QLineEdit *print;


private slots:

  void okButton();
  void cancelbutton();
  void set_fore_color(const QColor &);
  void set_background_color(const QColor &);
  void help();

signals:
  void color_change();

public:
  bool colors_changed;

private:


  QGroupBox *box;

  QPushButton *ok;
  QPushButton *cancel;

  QLabel *label1;
  KColorButton *button1;

  QLabel *label2;
  KColorButton *button2;

  QLabel *label3;
  QPushButton *button3;
  QLabel *label5;

  QGroupBox *gbox;
  QCheckBox *cb;
  QCheckBox *cb2;
  QSpinBox* precspin;
  QSpinBox* precspin2;
  QCheckBox *mybox;
  QCheckBox *frame3d;

  QButtonGroup *stylegroup;
  QLabel *stylelabel;
  QRadioButton *trigstyle;
  QRadioButton *statstyle;
  QRadioButton *sheetstyle;
};
#endif

