/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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

class KSpreadView;
class KSpreadTable;

class preference : public QWidget
{
  Q_OBJECT
public:
  preference( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  void apply();

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

  void initComboBox();
protected:
  KSpreadView* m_pView;
  KIntNumInput  *nbPage;
  QCheckBox *showVScrollBar;
  QCheckBox *showHScrollBar;
  KConfig* config;
  QComboBox *typeCompletion;
} ;


class KSpreadpreference : public KDialogBase
{
  Q_OBJECT
public:
  KSpreadpreference( KSpreadView* parent, const char* name);
public slots:
  void slotApply();
private :
  preference *_preferenceConfig;
  configure * _configure;
};



#endif
