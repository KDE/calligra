/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __kwconfig__
#define __kwconfig__

#include <kdialogbase.h>
#include <kconfig.h>
#include <kspell.h>
#include <knuminput.h>

class KWView;
class QCheckBox;

class configureSpellPage : public QWidget
{
  Q_OBJECT
public:
  configureSpellPage( KWView *_view,QWidget *parent = 0, char *name = 0 );
  void apply();
private:
  KWView* m_pView;
  KSpellConfig *_spellConfig;
  KConfig* config;
};

class configureInterfacePage : public QWidget
{
  Q_OBJECT
public:
  configureInterfacePage( KWView *_view,QWidget *parent = 0, char *name = 0 );
  void apply();
  void slotDefault();
private:
  KWView* m_pView;
  KIntNumInput *gridX,*gridY;
  KIntNumInput *indent;
  KIntNumInput * recentFiles;
  KIntNumInput*autoSave;
  QCheckBox *showRuler;
  int oldNbRecentFiles;
  KConfig* config;
  int oldAutoSaveValue;
};

class KWConfig : public KDialogBase
{
  Q_OBJECT
public:
  KWConfig(  KWView* parent, const char* name=0);
public slots:
  void slotApply();
  void slotDefault();
private :
 configureSpellPage *_spellPage;
 configureInterfacePage *_interfacePage;
};



#endif
