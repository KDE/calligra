/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef __kpresenter_dlg_config__
#define __kpresenter_dlg_config__

#include <kdialogbase.h>

class KPresenterView;
class KIntNumInput;
class KColorButton;
class KConfig;
class KSpellConfig;
class QCheckBox;

class configureInterfacePage : public QWidget
{
    Q_OBJECT
public:
    configureInterfacePage( KPresenterView *_view, QWidget *parent = 0, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPresenterView* m_pView;
    KIntNumInput* eRastX,*eRastY;
    KIntNumInput* autoSave;
    KConfig* config;
    int oldAutoSaveValue;
    unsigned int oldRastX;
    unsigned int oldRastY;
    int oldNbRecentFiles;
    KDoubleNumInput* indent;
    KIntNumInput* recentFiles;
    QCheckBox *showRuler;
};

class configureColorBackground : public QWidget
{
    Q_OBJECT
public:
    configureColorBackground( KPresenterView* _view, QWidget *parent = 0, char *name = 0 );
    void apply();
    void slotDefault();
protected:
    KPresenterView* m_pView;
    KColorButton* bgColor;
    KConfig* config;
    QColor oldBgColor;
} ;

class ConfigureSpellPage : public QObject
{
    Q_OBJECT
public:
    ConfigureSpellPage( KPresenterView *_view, QVBox *box, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPresenterView* m_pView;
    KSpellConfig *_spellConfig;
    QCheckBox *_dontCheckUpperWord;
    QCheckBox *_dontCheckTilteCase;
    KConfig* config;
};

class ConfigureMiscPage : public QObject
{
    Q_OBJECT
public:
    ConfigureMiscPage( KPresenterView  *_view, QVBox *box, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPresenterView* m_pView;
    KConfig* config;
    KIntNumInput* m_undoRedoLimit;
    int m_oldNbRedo;
};

class KPConfig : public KDialogBase
{
    Q_OBJECT
public:
    enum { KP_INTERFACE = 1, KP_COLOR=2, KP_KSPELL=4,KP_MISC=8 };
    KPConfig( KPresenterView* parent );
public slots:
    void slotApply();
    void slotDefault();
    void openPage(int flags);
private:
    configureInterfacePage *_interfacePage;
    configureColorBackground *_colorBackground;
    ConfigureSpellPage *_spellPage;
    ConfigureMiscPage *_miscPage;
};

#endif
