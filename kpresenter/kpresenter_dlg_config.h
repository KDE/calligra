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

class KPConfig : public KDialogBase
{
    Q_OBJECT
public:
    KPConfig( KPresenterView* parent );
public slots:
    void slotApply();
    void slotDefault();
private:
    configureInterfacePage *_interfacePage;
    configureColorBackground *_colorBackground;
};

#endif
