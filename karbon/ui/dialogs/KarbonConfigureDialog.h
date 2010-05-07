/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>

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

#ifndef KARBONCONFIGUREDIALOG_H
#define KARBONCONFIGUREDIALOG_H

#include <KoUnit.h>

#include <kpagedialog.h>
#include <kvbox.h>
#include <ksharedconfig.h>


class KarbonView;
class ConfigInterfacePage;
class KoConfigMiscPage;
class KoConfigGridPage;
class KoConfigDocumentPage;
class KIntNumInput;
class KColorButton;
class QCheckBox;
class QComboBox;

class KarbonConfigureDialog : public KPageDialog
{
    Q_OBJECT

public:
    KarbonConfigureDialog(KarbonView* parent);

public slots:
    void slotApply();
    void slotDefault();

private:
    ConfigInterfacePage* m_interfacePage;
    KoConfigMiscPage* m_miscPage;
    KoConfigGridPage* m_gridPage;
    KoConfigDocumentPage* m_defaultDocPage;
};

class ConfigInterfacePage : public KVBox
{
    Q_OBJECT

public:
    explicit ConfigInterfacePage(KarbonView* view, char* name = 0L);

    void apply();

public slots:
    void slotDefault();

private:
    KarbonView* m_view;
    KSharedConfigPtr m_config;

    KIntNumInput* m_recentFiles;
    int m_oldRecentFiles;

    QCheckBox* m_showStatusBar;

    KIntNumInput* m_dockerFontSize;
    int m_oldDockerFontSize;

    KColorButton* m_canvasColor;
    QColor m_oldCanvasColor;
};


#endif // KARBONCONFIGUREDIALOG_H

