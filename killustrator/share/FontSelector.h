/*
  $Id$

  Actually this code is a modification of the KFontDialog class, written
  by Bernd Johannes Wuebben  (wuebben@math.cornell.edu).
  
  Copyright (C) 1996 Bernd Johannes Wuebben   
  wuebben@math.cornell.edu

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef FontSelector_h_
#define FontSelector_h_

#include <qmsgbox.h>
#include <qpixmap.h>
#include <qapp.h>
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qscrbar.h>
#include <qtooltip.h>

#include <qstring.h>
#include <qfont.h>

class FontSelector : public QWidget {
    Q_OBJECT

public:
    FontSelector (QWidget *parent, const char *name = 0L,
		  const char* example = "Text",
		  const QStrList* fontlist = 0L );

    void setFont (const QFont &font);
    QFont font () { return selFont; }

signals:
	/*
	 * connect to this to monitor the font as it as selected if you are
	 * not running modal.
	 */
	void fontSelected (const QFont &font);

private slots:
      void 	family_chosen_slot (const char *);
      void      size_chosen_slot   (const char *);
      void      weight_chosen_slot (const char *);
      void      style_chosen_slot  (const char *);

      void 	family_chosen_slot (const QString&);
      void      size_chosen_slot   (const QString&);
      void      weight_chosen_slot (const QString&);
      void      style_chosen_slot  (const QString&);

      void      display_example    (const QFont &font);
      void      charset_chosen_slot(int index);
      void      setColors();

private:

    bool loadKDEInstalledFonts();
    void fill_family_combo();
    void setCombos();
   
    QGroupBox	 *box1;
    QGroupBox	 *box2;
    
    // pointer to an optinally supplied list of fonts to 
    // inserted into the fontdialog font-family combo-box
    QStrList     *fontlist; 

    QLabel	 *family_label;
    QLabel	 *size_label;
    QLabel       *weight_label;
    QLabel       *style_label;
    QLabel	 *charset_label;

    QLabel	 *actual_family_label;
    QLabel	 *actual_size_label;
    QLabel       *actual_weight_label;
    QLabel       *actual_style_label;
    QLabel	 *actual_charset_label;


    QLabel	 *actual_family_label_data;
    QLabel	 *actual_size_label_data;
    QLabel       *actual_weight_label_data;
    QLabel       *actual_style_label_data;
    QLabel	 *actual_charset_label_data;
    QComboBox    *family_combo;
    QComboBox    *size_combo;
    QComboBox    *weight_combo;
    QComboBox    *style_combo;
    QComboBox	 *charset_combo;    

    QLabel       *example_label;
    QFont         selFont;
};

#endif
