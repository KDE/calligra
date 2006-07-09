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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kwconfig__
#define __kwconfig__

#include <kdialogbase.h>

namespace KFormula {
    class ConfigurePage;
}

class KWView;
class QCheckBox;
class KIntNumInput;
class KoUnitDoubleSpinBox;
class KConfig;
class QComboBox;
class KIntNumInput;
class KCommand;
class KWDocument;
class KListView;
class QVGroupBox;
class QLabel;

namespace KSpell2 {
    class ConfigWidget;
}

class ConfigureSpellPage : public QObject
{
    Q_OBJECT
public:
    ConfigureSpellPage( KWView *view, QVBox *box, char *name = 0 );
    void apply();
    void slotDefault();
private:
    KWView* m_pView;
    KConfig* config;
    KSpell2::ConfigWidget *m_spellConfigWidget;
};

class ConfigureInterfacePage : public QObject
{
    Q_OBJECT
public:
    ConfigureInterfacePage( KWView *view, QVBox *box, char *name = 0 );
    void apply();
    void slotDefault();
    void setUnit( KoUnit::Unit unit );

signals:
    // For KWConfig
    void unitChanged( int );
private:
    KWView* m_pView;
    KConfig* config;
    QComboBox *m_unitCombo;
    KoUnitDoubleSpinBox* gridX,*gridY;
    KoUnitDoubleSpinBox* indent;
    KIntNumInput* recentFiles;
    QCheckBox *showStatusBar, *showScrollBar, *pgUpDownMovesCaret;
    int oldNbRecentFiles;
    KIntNumInput *m_nbPagePerRow;
};

class ConfigureMiscPage : public QObject
{
    Q_OBJECT
public:
    ConfigureMiscPage( KWView *view, QVBox *box, char *name = 0 );
    KCommand* apply();
    void slotDefault();
    void setUnit( KoUnit::Unit unit );
private:
    KWView* m_pView;
    KConfig* config;
    KIntNumInput* m_undoRedoLimit;
    int m_oldNbRedo;
    bool m_oldFormattingEndParag, m_oldFormattingSpace, m_oldFormattingTabs, m_oldFormattingBreak;
    QCheckBox* m_displayLink, *m_displayComment, *m_underlineLink, *m_displayFieldCode;
    QCheckBox* m_cbViewFormattingEndParag, *m_cbViewFormattingSpace;
    QCheckBox* m_cbViewFormattingTabs, *m_cbViewFormattingBreak;
};

class ConfigureDefaultDocPage : public QObject
{
    Q_OBJECT
public:
    ConfigureDefaultDocPage( KWView *view, QVBox *box, char *name = 0 );
    ~ConfigureDefaultDocPage();
    KCommand* apply();
    void slotDefault();
    void setUnit( KoUnit::Unit unit );
public slots:
    void selectNewDefaultFont();
private:
    KWView* m_pView;
    KConfig* config;
    QFont *font;
    QLabel *fontName;
    QLabel *tabStop;

    KoUnitDoubleSpinBox* m_columnSpacing;

    KIntNumInput* autoSave;
    int oldAutoSaveValue;
    double m_oldTabStopWidth;

    int m_oldStartingPage;
    bool m_oldBackupFile;
    KIntNumInput* m_variableNumberOffset;
    KoUnitDoubleSpinBox *m_tabStopWidth;
    QCheckBox *m_cursorInProtectedArea;
    QCheckBox *m_createBackupFile;
//     QCheckBox *m_directInsertCursor;
    QCheckBox *m_autoHyphenation;
    QComboBox *m_globalLanguage;
    QString m_oldLanguage;
    bool m_oldHyphenation;
};

class ConfigurePathPage : public QObject
{
    Q_OBJECT
public:
    ConfigurePathPage( KWView *view, QVBox *box, char *name = 0 );
    void slotDefault();
    void apply();
private slots:
    void slotModifyPath();
    void slotSelectionChanged(QListViewItem *);
private:
    KWView* m_pView;
    KConfig* config;
    KListView* m_pPathView;
    QPushButton *m_modifyPath;
};

class ConfigureTTSPage : public QObject
{
    Q_OBJECT
public:
    ConfigureTTSPage( KWView *view, QVBox *box, char *name = 0 );
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

class KWConfig : public KDialogBase
{
    Q_OBJECT
public:
    enum { KW_KSPELL=1,KP_INTERFACE=2,KP_MISC=4, KP_DOCUMENT=8, KP_FORMULA=16, KP_PATH = 32};
    KWConfig( KWView* parent );
    void openPage(int flags);
public slots:
    void slotApply();
    void slotDefault();
    void unitChanged( int );
private:
    ConfigureSpellPage *m_spellPage;
    ConfigureInterfacePage *m_interfacePage;
    ConfigureMiscPage *m_miscPage;
    ConfigureDefaultDocPage *m_defaultDocPage;
    KFormula::ConfigurePage *m_formulaPage;
    ConfigurePathPage *m_pathPage;
    ConfigureTTSPage *m_ttsPage;
    KWDocument *m_doc;
};



#endif
