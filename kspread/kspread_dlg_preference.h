/* This file is part of the KDE project
   Copyright (C) 1999, 2000, 2001 Montel Laurent <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_dlg_preference__
#define __kspread_dlg_preference__

#include <qdialog.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qrect.h>
#include <kdialogbase.h>
#include <knuminput.h>
#include <kconfig.h>
#include <kcolorbutton.h>
#include <kspell.h>

class KSpreadView;
class KSpreadTable;

class preference : public QWidget
{
  Q_OBJECT
public:
  preference( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();
  void slotDefault();

protected:
  QCheckBox *m_pFormula;
  QCheckBox *m_pAutoCalc;
  QCheckBox *m_pGrid;
  QCheckBox *m_pColumn;
  QCheckBox *m_pLcMode;
  QCheckBox *m_pHideZero;
  QCheckBox *m_pFirstLetterUpper;
  KSpreadView* m_pView;
} ;

class parameterLocale : public QWidget
{
public:
   parameterLocale( KSpreadView* _view,QWidget *parent = 0, char *name = 0);
};

class configure : public QWidget
{
  Q_OBJECT
public:
  configure( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();
  void slotDefault();
protected:
  KSpreadView* m_pView;
  KIntNumInput  *nbPage;

  QCheckBox *showVScrollBar;
  QCheckBox *showHScrollBar;
  QCheckBox *showColHeader;
  QCheckBox *showRowHeader;
  QCheckBox *showTabBar;
  QCheckBox *showFormulaBar;
  KConfig* config;
} ;


class miscParameters : public QWidget
{
  Q_OBJECT
public:
  miscParameters( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();
  void slotDefault();

  void initComboBox();
protected:
  KSpreadView* m_pView;
  KIntNumInput  *valIndent;
  KConfig* config;
  QComboBox *typeCompletion;
  QComboBox *typeCalc;
  QComboBox *typeOfMove;
  QCheckBox *msgError;
  QCheckBox *commentIndicator;
} ;

class colorParameters : public QWidget
{
  Q_OBJECT
public:
  colorParameters( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();
  void slotDefault();
protected:
  KSpreadView* m_pView;
  KColorButton* gridColor;
  KConfig* config;
} ;

class configureLayoutPage : public QWidget
{
  Q_OBJECT
public:
  configureLayoutPage( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();
  void slotDefault();
  void initCombo();
protected:
  KSpreadView* m_pView;
  QComboBox *defaultOrientationPage;
  QComboBox *defaultSizePage;
  
  //store old config
  int paper;
  int orientation;
  
  KConfig* config;
} ;

class configureSpellPage : public QWidget
{
  Q_OBJECT
public:
  configureSpellPage( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();
protected:
  KSpreadView* m_pView;
  KSpellConfig * _spellConfig;
  KConfig* config;
} ;

class KSpreadpreference : public KDialogBase
{
  Q_OBJECT
public:
  KSpreadpreference( KSpreadView* parent, const char* name);
public slots:
  void slotApply();
  void slotDefault();
private :
  KSpreadView* m_pView;
  preference *_preferenceConfig;
  configure * _configure;
  miscParameters *_miscParameter;
  colorParameters *_colorParameter;
  configureLayoutPage *_layoutPage;
  configureSpellPage *_spellPage;
};



#endif
