/*  This file is part of the KDE libraries
    Copyright (C) 1999 Waldo Bastian (bastian@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 2
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// NOTE: This is copied from kmessagebox.cpp

#include "kmessagebox.h"
#include <KoIcon.h>

static QIcon themedMessageBoxIcon(QMessageBox::Icon icon)
{
    const char *icon_name = 0;

    switch (icon) {
    case QMessageBox::NoIcon:
        return QIcon();
        break;
    case QMessageBox::Information:
        icon_name = koIconNameCStr("dialog-information");
        break;
    case QMessageBox::Warning:
        icon_name = koIconNameCStr("dialog-warning");
        break;
    case QMessageBox::Critical:
        icon_name = koIconNameCStr("dialog-error");
        break;
    default:
        break;
    }

   QIcon ret = KIconLoader::global()->loadIcon(QLatin1String(icon_name), KIconLoader::NoGroup, KIconLoader::SizeHuge, KIconLoader::DefaultState, QStringList(), 0, true);

   if (ret.isNull()) {
       return QMessageBox::standardIcon(icon);
   } else {
       return ret;
   }
}

