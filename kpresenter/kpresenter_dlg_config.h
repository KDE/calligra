// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
class KPresenterDoc;
class KIntNumInput;
class KColorButton;
class KConfig;
class QCheckBox;
class KLineEdit;
class KDoubleNumInput;
class KoSpellConfigWidget;
class PenStyleWidget;
class PieProperty;

namespace KSpell2 {
    class ConfigWidget;
}

class configureInterfacePage : public QWidget
{
    Q_OBJECT
public:
    configureInterfacePage( KPresenterView *_view, QWidget *parent = 0, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPresenterView* m_pView;
    KConfig* config;
    int oldNbRecentFiles;
    KDoubleNumInput* indent;
    KIntNumInput* recentFiles;
    QCheckBox *showRuler, *showStatusBar;
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
    KColorButton* gridColor;
    KConfig* config;
    QColor oldBgColor;
    QColor oldGridColor;
} ;

class configureSpellPage : public QWidget
{
    Q_OBJECT
public:
    configureSpellPage( KPresenterView *_view, QWidget *parent, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPresenterView* m_pView;
    KSpell2::ConfigWidget *m_spellConfigWidget;
    KConfig* config;
};

class configureMiscPage : public QWidget
{
    Q_OBJECT
public:
    configureMiscPage( KPresenterView  *_view, QWidget *parent, char *name = 0 );
    KCommand *apply();
    void slotDefault();
private:
    KPresenterView* m_pView;
    KConfig* config;
    KIntNumInput* m_undoRedoLimit;
    int m_oldNbRedo;
    bool m_printNotes;
    QCheckBox* m_displayLink, *m_displayComment, *m_underlineLink, *m_displayFieldCode, *m_cbPrintNotes;
    KDoubleNumInput * resolutionY, * resolutionX;
    KDoubleNumInput *m_rotation;
};

class configureDefaultDocPage : public QWidget
{
    Q_OBJECT
public:
    configureDefaultDocPage( KPresenterView  *_view, QWidget *parent, char *name = 0 );
    ~configureDefaultDocPage();
    KCommand *apply();
    void slotDefault();
public slots:
    void selectNewDefaultFont();
private:
    int oldAutoSaveValue;
    int m_oldStartingPage;
    double m_oldTabStopWidth;
    bool m_oldBackupFile;
    QString m_oldLanguage;
    KPresenterView* m_pView;
    KConfig* config;
    QFont *font;
    QLabel *fontName;
    KIntNumInput* autoSave;
    KIntNumInput* m_variableNumberOffset;
    KDoubleNumInput *m_tabStopWidth;
    QCheckBox *m_cursorInProtectedArea;
    QCheckBox *m_createBackupFile;
    QCheckBox *m_directInsertCursor;
    QComboBox *m_globalLanguage;
    QCheckBox *m_autoHyphenation;
    bool m_oldHyphenation;
};

class configureToolsPage : public QWidget
{
    Q_OBJECT
public:
    configureToolsPage( KPresenterView  *_view, QWidget *parent, char *name = 0 );
    ~configureToolsPage();
    void apply();
    void slotDefault();
public slots:

private:
    KPresenterView* m_pView;
    KConfig* config;

    PenStyleWidget *m_confPenDia;
    PieProperty *m_pieProperty;
    ConfRectDia *m_confRectDia;
    ConfBrushDia *m_confBrushDia;
    ConfPolygonDia *m_confPolygonDia;
};

class configurePathPage : public QWidget
{
    Q_OBJECT
public:
    configurePathPage( KPresenterView *_view, QWidget *parent, char *name = 0 );
    void slotDefault();
    void apply();
private slots:
    void slotModifyPath();
    void slotSelectionChanged(QListViewItem *);
private:
    KPresenterView* m_pView;
    KConfig* config;
    KListView* m_pPathView;
    QPushButton *m_modifyPath;
};


class KPConfig : public KDialogBase
{
    Q_OBJECT
public:
    enum { KP_INTERFACE = 1, KP_COLOR=2, KP_KSPELL=4,KP_MISC=8, KP_DOC=16, KP_TOOLS=32,KP_PATH = 64 };
    KPConfig( KPresenterView* parent );
public slots:
    void slotApply();
    void slotDefault();
    void openPage(int flags);
private:
    configureInterfacePage *_interfacePage;
    configureColorBackground *_colorBackground;
    configureSpellPage *_spellPage;
    configureMiscPage *_miscPage;
    configureDefaultDocPage *_defaultDocPage;
    configureToolsPage *_toolsPage;
    configurePathPage *m_pathPage;
    KPresenterDoc* m_doc;

};

#endif
