/*
    Copyright (c) 2006 David Faure <faure@kde.org>

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

#include <kactionselector.h>

#include <QApplication>
#include <QListWidget>

int main( int argc, char *argv[] )
{
  QApplication app( argc, argv );

  KActionSelector actionSelector(0);
  actionSelector.availableListWidget()->addItems(QStringList() << QLatin1String("A") << QLatin1String("B") << QLatin1String("C") << QLatin1String("D") << QLatin1String("E"));
  actionSelector.selectedListWidget()->addItems(QStringList() << QLatin1String("1") << QLatin1String("2"));
  actionSelector.show();

  return app.exec();
}
