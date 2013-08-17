/* This file is part of the KDE libraries
    Copyright (C) 2007 Olivier Goffart  <ogoffart at kde.org>

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

#include <qapplication.h>
#include <knewpassworddialog.h>
#include <iostream>

int main( int argc, char *argv[] )
{
    QApplication a(argc, argv);

    KNewPasswordDialog dlg;
    dlg.setPasswordStrengthWarningLevel(30);
    dlg.setPrompt(QApplication::translate("main", "Enter a password for the test"));

    if (dlg.exec()) {
        std::cout << "Entered password: " << qPrintable(dlg.password()) << std::endl;
        return 0;
    } else {
        std::cout << "No password" << std::endl;
        return -1;
    }

}

