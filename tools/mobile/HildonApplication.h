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

#ifndef HILDONAPPLICATION_H
#define HILDONAPPLICATION_H

#include <QApplication>

/*!
 * \brief Class inherited from QApplication, to handle and display
 * application menu.
 */
class HildonApplication : public QApplication
{
    Q_OBJECT
public:
    HildonApplication(int& argc, char** argv);
    bool x11EventFilter(XEvent* event);
signals:
    void showApplicationMenu();
    void openDocument(const QString &fileName);
};

#endif // HILDONAPPLICATION_H
