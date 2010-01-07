/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "HildonMenu.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyle>
#include <QAction>
#include <QGridLayout>
#include <QX11Info>
#include <QPushButton>
#include <QMenuBar>
#include <QMainWindow>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

HildonMenu::HildonMenu(QMainWindow* parent)
        : QDialog(parent), m_row(0), m_col(0)
{
    m_layout = new QGridLayout(this);

    // larger top margin to better match gtk fremantle menu's
    int left, top, right, bottom;
    m_layout->getContentsMargins(&left, &top, &right, &bottom);
    top *= 2;
    m_layout->setContentsMargins(left, top, right, bottom);

    Atom window_type = XInternAtom(QX11Info::display(),
                                   "_NET_WM_WINDOW_TYPE",
                                   False);
    Atom hildonwinType = XInternAtom(QX11Info::display(),
                                     "_HILDON_WM_WINDOW_TYPE_APP_MENU",
                                     False);
    XChangeProperty(QX11Info::display(), winId(), window_type, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &hildonwinType, 1);

    QMenuBar* menu = parent->menuBar();
    foreach(QAction* action, menu->actions()) {
        addMenuItem(action);
    }
}

void HildonMenu::addMenuItem(QAction* action)
{
    if (m_col == 0) {
        m_layout->setRowMinimumHeight(m_row, 64);
    }
    QPushButton *b = new QPushButton(action->text(), this);
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(b, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(b, SIGNAL(clicked(bool)), action, SIGNAL(triggered()));
    m_layout->addWidget(b, m_row, m_col);
    m_col++;
    if (m_col > 1) {
        m_row++;
        m_col = 0;
    }
}

void HildonMenu::paintEvent(QPaintEvent* event)
{
    QDialog::paintEvent(event);
    QPainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect.adjust(0, -100, 0, 0);
    style()->drawPrimitive(QStyle::PE_PanelMenu, &opt, &painter, this);
}
