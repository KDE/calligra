/* This file is part of the KDE project
 * Copyright (C) 2010 Christoph Goerlich <chgoerlich@gmx.de>
 * Copyright (C) 2012 Shreya Pandit <shreya@shreyapandit.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CHANGECASEMENU_H
#define CHANGECASEMENU_H

#include <QObject>
#include <QPair>

#include <kmenu.h>
#include <KActionMenu>
#include <sonnet/speller.h>
#include <Changecase.h>

class KActionMenu;
class KAction;
class QSignalMapper;
class Changecase;

class ChangeCaseMenu : public QObject
{
    Q_OBJECT
public:
    explicit ChangeCaseMenu(Changecase* changecase);
    ~ChangeCaseMenu();
    KAction* lowercaseAction;
    KAction* uppercaseAction;
    KAction* initialcapsAction;
    KAction* togglecaseAction ;
    KAction* sentencecaseAction ;

    void setEnabled(bool b);
    void setVisible(bool b);
    QPair<QString, KAction*> menuAction();

private slots:    
    void createCaseMenu();

private:
    KMenu* optionsMenu;
    KActionMenu *m_changeCaseMenuAction;
    KActionMenu* actionMenu;
};

#endif // CHANGECASEMENU_H
