/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2000-2003 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2001-2002 David Faure <faure@kde.org>
             (C) 2001 Werner Trobin <trobin@kde.org>
             (C) 2000 Bernd Johannes Wuebben <wuebben@kde.org>

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

#ifndef __kspread_dlg_preference__
#define __kspread_dlg_preference__

#include <kdialogbase.h>
//Added by qt3to4:
#include <QLabel>
#include <kvbox.h>

class KConfig;
class KIntNumInput;
class KDoubleNumInput;
class KSpellConfig;
class QCheckBox;
class QComboBox;
class QPushButton;
class KColorButton;

namespace KSpread
{
class Sheet;
class View;

class parameterLocale :  public QObject
{
 Q_OBJECT
public:
   parameterLocale( View* _view,KVBox *box, char *name = 0);
 void apply();
public slots:
   void updateDefaultSystemConfig();
 protected:
   /**
    * Updates the GUI widgets to match the specified locale.
    */
   void updateToMatchLocale(KLocale* locale);

   QLabel *m_shortDate,*m_time,*m_money,*m_date,*m_language,*m_number;
   QPushButton *m_updateButton;
   View* m_pView;
   bool m_bUpdateLocale;
};

class configure : public QObject
{
  Q_OBJECT
public:
  configure( View* _view,KVBox *box, char *name = 0 );
  void apply();
  void slotDefault();
protected:
  View* m_pView;
  KIntNumInput  *nbPage;
  KIntNumInput* nbRecentFile;
  KIntNumInput* autoSaveDelay;
  QCheckBox *showVScrollBar;
  QCheckBox *showHScrollBar;
  QCheckBox *showColHeader;
  QCheckBox *showRowHeader;
  QCheckBox *showTabBar;
  QCheckBox *showFormulaBar;
    QCheckBox *showStatusBar;
    QCheckBox *m_createBackupFile;
  bool m_oldBackupFile;

  KConfig* config;
  int oldRecent;
  int oldAutoSaveValue;
} ;


class miscParameters : public QObject
{
  Q_OBJECT
public:
  miscParameters( View* _view, KVBox *box, char *name = 0 );
  void apply();
  void slotDefault();

  void initComboBox();

public slots:
  void slotTextComboChanged(const QString &);

protected:
  View* m_pView;
  KDoubleNumInput  *valIndent;

  /**
   * Needed to ensure the same unit for loading and saving.
   */
  KoUnit::Unit indentUnit;
  KConfig* config;
  QComboBox *typeCompletion;
  QComboBox *typeCalc;
  QComboBox *typeOfMove;
  QCheckBox *msgError;
  KIntNumInput* m_undoRedoLimit;
  bool comboChanged;
  int m_oldNbRedo;
} ;

class colorParameters : public QObject
{
  Q_OBJECT
public:
  colorParameters( View* _view, KVBox *box, char *name = 0 );
  void apply();
  void slotDefault();
protected:
  View* m_pView;
  KColorButton* gridColor;
  KColorButton* pageBorderColor;
  KConfig* config;
} ;

class configureLayoutPage : public QObject
{
  Q_OBJECT
public:
  configureLayoutPage( View* _view,KVBox *box, char *name = 0 );
  void apply();
  void slotDefault();
  void initCombo();
protected:
  View* m_pView;
  QComboBox *defaultOrientationPage;
  QComboBox *defaultSizePage;
  QComboBox *defaultUnit;
  //store old config
  int paper;
  int orientation;
  int unit;

  KConfig* config;
} ;

class configureSpellPage : public QObject
{
  Q_OBJECT
public:
  configureSpellPage( View* _view, KVBox *box, char *name = 0 );
  void apply();
  void slotDefault();
protected:
  View * m_pView;
  KConfig * config;
    KSpellConfig *m_spellConfigWidget;
    QCheckBox *dontCheckUpperWord;
    QCheckBox *dontCheckTitleCase;
} ;

class configureTTSPage : public QObject
{
  Q_OBJECT
public:
  configureTTSPage( View *_view, KVBox *box, char *name = 0 );
  void slotDefault();
  void apply();
private slots:
  void screenReaderOptionChanged();
private:
  KConfig* config;
  QCheckBox* m_cbSpeakPointerWidget;
  QCheckBox* m_cbSpeakFocusWidget;
  QGroupBox* m_gbScreenReaderOptions;
  QCheckBox* m_cbSpeakTooltips;
  QCheckBox* m_cbSpeakWhatsThis;
  QCheckBox* m_cbSpeakDisabled;
  QCheckBox* m_cbSpeakAccelerators;
  QLabel* m_lblAcceleratorPrefix;
  QLineEdit* m_leAcceleratorPrefixWord;
  KIntNumInput* m_iniPollingInterval;
};

class PreferenceDialog : public KDialogBase
{
  Q_OBJECT
public:
  enum { KS_PREFERENCES = 1, KS_LOCALE = 2, KS_INTERFACE = 4,
         KS_MISC = 8, KS_COLOR = 16, KS_LAYOUT = 32, KS_SPELLING = 64 };
  PreferenceDialog( View* parent, const char* name);
public slots:
  void slotApply();
  void slotDefault();
  void openPage(int flags);
private :
  View* m_pView;
  configure * _configure;
  miscParameters *_miscParameter;
  colorParameters *_colorParameter;
  configureLayoutPage *_layoutPage;
  configureSpellPage *_spellPage;
  parameterLocale *_localePage;
};

} // namespace KSpread

#endif
