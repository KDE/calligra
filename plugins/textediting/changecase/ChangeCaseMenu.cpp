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

#include <ChangeCaseMenu.h>

#include <klocale.h>


ChangeCaseMenu::ChangeCaseMenu(Changecase* changecase ): QObject(changecase),
	optionsMenu(0)
{
    m_changeCaseMenuAction = new KActionMenu(i18n("AABBCC"), changecase);
    optionsMenu = m_changeCaseMenuAction->menu();
    lowercaseAction = new KAction(i18n("lowercase"),m_changeCaseMenuAction);
    uppercaseAction = new KAction("UPPER case" ,m_changeCaseMenuAction);
    initialcapsAction = new KAction("Initial Caps" ,m_changeCaseMenuAction);
    togglecaseAction = new KAction("tOggLe case" ,m_changeCaseMenuAction);
    sentencecaseAction = new KAction("Sentence case" ,m_changeCaseMenuAction);
    optionsMenu->addAction(lowercaseAction);
    optionsMenu->addAction(uppercaseAction);
    optionsMenu->addAction(initialcapsAction);
    optionsMenu->addAction(togglecaseAction);
}

ChangeCaseMenu::~ChangeCaseMenu()
{
    delete this;
}

void ChangeCaseMenu::setEnabled(bool b)
{
    optionsMenu->setEnabled(b);
}

void ChangeCaseMenu::setVisible(bool b)
{
    optionsMenu->setVisible(b);
}

QPair< QString, KAction* > ChangeCaseMenu::menuAction()
{
    return QPair<QString, KAction*>("changecase_suggestions", m_changeCaseMenuAction);
}

void ChangeCaseMenu::createCaseMenu()
{

}
