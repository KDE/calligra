/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONCONFIGINTERFACEPAGE_H
#define KARBONCONFIGINTERFACEPAGE_H

#include <KSharedConfig>
#include <KoVBox.h>

class KarbonView;
class KColorButton;
class QSpinBox;
class QCheckBox;

class KarbonConfigInterfacePage : public KoVBox
{
    Q_OBJECT

public:
    explicit KarbonConfigInterfacePage(KarbonView *view, char *name = nullptr);

    void apply();

public Q_SLOTS:
    void slotDefault();

private:
    KarbonView *m_view;
    KSharedConfigPtr m_config;

    QSpinBox *m_recentFiles;
    int m_oldRecentFiles;

    QCheckBox *m_showStatusBar;

    QSpinBox *m_dockerFontSize;
    int m_oldDockerFontSize;

    KColorButton *m_canvasColor;
    QColor m_oldCanvasColor;
};

#endif // KARBONCONFIGINTERFACEPAGE_H
