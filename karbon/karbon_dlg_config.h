/* This file is part of the KDE project
   Copyright (C) 2002, Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef __karbon_dlg_config__
#define __karbon_dlg_config__

#include <kdialogbase.h>

class KarbonView;
class KIntNumInput;
class KColorButton;
class KConfig;
class KSpellConfig;
class QCheckBox;
class KLineEdit;

class configureInterfacePage : public QWidget
{
    Q_OBJECT
public:
    configureInterfacePage( KarbonView *_view, QWidget *parent = 0, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KarbonView* m_pView;
    KConfig* config;
    int oldNbRecentFiles;
    KIntNumInput* recentFiles;
    QCheckBox *showStatusBar;
};

class ConfigureMiscPage : public QObject
{
    Q_OBJECT
public:
    ConfigureMiscPage( KarbonView  *_view, QVBox *box, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KarbonView* m_pView;
    KConfig* config;
    KIntNumInput* m_undoRedoLimit;
    int m_oldNbRedo;
};

class ConfigureDefaultDocPage : public QObject
{
    Q_OBJECT
public:
    ConfigureDefaultDocPage( KarbonView  *_view, QVBox *box, char *name = 0 );
    void apply();
    void slotDefault();
private:
    int oldAutoSaveValue;
    KarbonView* m_pView;
    KConfig* config;
    KIntNumInput* autoSave;
};


class KarbonConfig : public KDialogBase
{
    Q_OBJECT
public:
    KarbonConfig( KarbonView* parent );
public slots:
    void slotApply();
    void slotDefault();
private:
    configureInterfacePage *_interfacePage;
    ConfigureMiscPage *_miscPage;
    ConfigureDefaultDocPage *_defaultDocPage;
};

#endif
