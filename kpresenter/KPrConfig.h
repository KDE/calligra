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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kpresenter_dlg_config__
#define __kpresenter_dlg_config__

#include <kdialogbase.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3ListViewItem>
class KPrView;
class KPrDocument;
class KIntNumInput;
class KColorButton;
class KConfig;
class QCheckBox;
class KLineEdit;
class KDoubleNumInput;
class KoSpellConfigWidget;
class KPrPenStyleWidget;
class KPrBrushProperty;
class KPrRectProperty;
class KPrPolygonProperty;
class KPrPieProperty;
class KoUnitDoubleSpinBox;
class QVGroupBox;

namespace KSpell2 {
    class ConfigWidget;
}


class KPrConfigureInterfacePage : public QWidget
{
    Q_OBJECT
public:
    KPrConfigureInterfacePage( KPrView *_view, QWidget *parent = 0, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPrView* m_pView;
    KConfig* config;
    int oldNbRecentFiles;
    KDoubleNumInput* indent;
    KIntNumInput* recentFiles;
    QCheckBox *showRuler, *showStatusBar;
};

class KPrConfigureColorBackground : public QWidget
{
    Q_OBJECT
public:
    KPrConfigureColorBackground( KPrView* _view, QWidget *parent = 0, char *name = 0 );
    void apply();
    void slotDefault();
protected:
    KPrView* m_pView;
    KColorButton* bgColor;
    KColorButton* gridColor;
    KConfig* config;
    QColor oldBgColor;
    QColor oldGridColor;
} ;

class KPrConfigureSpellPage : public QWidget
{
    Q_OBJECT
public:
    KPrConfigureSpellPage( KPrView *_view, QWidget *parent, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KPrView* m_pView;
    KSpell2::ConfigWidget *m_spellConfigWidget;
    KConfig* config;
};

class KPrConfigureMiscPage : public QWidget
{
    Q_OBJECT
public:
    KPrConfigureMiscPage( KPrView  *_view, QWidget *parent, char *name = 0 );
    KCommand *apply();
    void slotDefault();
private:
    KPrView* m_pView;
    KConfig* config;
    KIntNumInput* m_undoRedoLimit;
    int m_oldNbRedo;
    bool m_printNotes;
    QCheckBox* m_displayLink, *m_displayComment, *m_underlineLink, *m_displayFieldCode, *m_cbPrintNotes;
    KoUnitDoubleSpinBox * resolutionY, * resolutionX;
    KDoubleNumInput *m_rotation;
};

class KPrConfigureDefaultDocPage : public QWidget
{
    Q_OBJECT
public:
    KPrConfigureDefaultDocPage( KPrView  *_view, QWidget *parent, char *name = 0 );
    ~KPrConfigureDefaultDocPage();
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
    KPrView* m_pView;
    KConfig* config;
    QFont *font;
    QLabel *fontName;
    KIntNumInput* autoSave;
    KIntNumInput* m_variableNumberOffset;
    KoUnitDoubleSpinBox *m_tabStopWidth;
    QCheckBox *m_cursorInProtectedArea;
    QCheckBox *m_createBackupFile;
    QCheckBox *m_directInsertCursor;
    QComboBox *m_globalLanguage;
    QCheckBox *m_autoHyphenation;
    bool m_oldHyphenation;
};

class KPrConfigureToolsPage : public QWidget
{
    Q_OBJECT
public:
    KPrConfigureToolsPage( KPrView  *_view, QWidget *parent, char *name = 0 );
    ~KPrConfigureToolsPage();
    void apply();
    void slotDefault();
public slots:

private:
    KPrView* m_pView;
    KConfig* config;

    KPrPenStyleWidget *m_confPenDia;
    KPrBrushProperty *m_brushProperty;
    KPrRectProperty *m_rectProperty;
    KPrPolygonProperty *m_polygonProperty;
    KPrPieProperty *m_pieProperty;
};

class KPrConfigurePathPage : public QWidget
{
    Q_OBJECT
public:
    KPrConfigurePathPage( KPrView *_view, QWidget *parent, char *name = 0 );
    void slotDefault();
    void apply();
private slots:
    void slotModifyPath();
    void slotSelectionChanged(Q3ListViewItem *);
private:
    KPrView* m_pView;
    KConfig* config;
    K3ListView* m_pPathView;
    QPushButton *m_modifyPath;
};

class KPrConfigureTTSPage : public QWidget
{
  Q_OBJECT
public:
  KPrConfigureTTSPage( KPrView *_view, QWidget *parent, char *name = 0 );
  void slotDefault();
  void apply();
private slots:
  void screenReaderOptionChanged();
private:
  KConfig* config;
  QCheckBox* m_cbSpeakPointerWidget;
  QCheckBox* m_cbSpeakFocusWidget;
  QVGroupBox* m_gbScreenReaderOptions;
  QCheckBox* m_cbSpeakTooltips;
  QCheckBox* m_cbSpeakWhatsThis;
  QCheckBox* m_cbSpeakDisabled;
  QCheckBox* m_cbSpeakAccelerators;
  QLabel* m_lblAcceleratorPrefix;
  QLineEdit* m_leAcceleratorPrefixWord;
  KIntNumInput* m_iniPollingInterval;
};


class KPrConfig : public KDialogBase
{
    Q_OBJECT
public:
    enum { KP_INTERFACE = 1, KP_COLOR=2, KP_KSPELL=4,KP_MISC=8, KP_DOC=16, KP_TOOLS=32,KP_PATH = 64 };
    KPrConfig( KPrView* parent );
public slots:
    void slotApply();
    void slotDefault();
    void openPage(int flags);
private:
    KPrConfigureInterfacePage *_interfacePage;
    KPrConfigureColorBackground *_colorBackground;
    KPrConfigureSpellPage *_spellPage;
    KPrConfigureMiscPage *_miscPage;
    KPrConfigureDefaultDocPage *_defaultDocPage;
    KPrConfigureToolsPage *_toolsPage;
    KPrConfigurePathPage *m_pathPage;
    KPrDocument* m_doc;

};

#endif
