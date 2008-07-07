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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_PREFERENCE_DIALOG
#define KSPREAD_PREFERENCE_DIALOG

#include <QGroupBox>
#include <QLabel>

#include <kconfig.h>
#include <klineedit.h>
#include <kpagedialog.h>
#include <ksharedconfig.h>
#include <kvbox.h>

#include <KoUnit.h>

class KIntNumInput;
class KDoubleNumInput;
class KSpellConfig;
class QCheckBox;
class KComboBox;
class QPushButton;
class KColorButton;

namespace Sonnet
{
class ConfigWidget;
}

namespace KSpread
{
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


class PreferenceDialog : public KPageDialog
{
    Q_OBJECT

public:
    enum
    {
        LocalePage = 1,
        InterfacePage = 2,
        OpenSavePage = 4,
        SpellCheckerPage = 8,
        PluginPage = 16
    };

    explicit PreferenceDialog(View* view);
    ~PreferenceDialog();

    void openPage(int flags);

public Q_SLOTS:
    void slotApply();
    void slotDefault();
    void slotReset();

private Q_SLOTS:
    void unitChanged(int index);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_PREFERENCE_DIALOG
