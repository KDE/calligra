/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

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

#include <kapplication.h>
#include <kglobal.h>
#include <KAboutData>

#include <tableview/kexitableview.h>

int main(int argc, char* argv[])
{
    KApplication app(argc, argv, "tv_test");
    KGlobal::iconLoader()->addAppDir("kexi");

    KexiTableView tv;

    KDbTableViewData data;
    KDbField f1("id", KDbField::Integer),
    f2("name", KDbField::Text),
    f3("age", KDbField::Integer);
    data.addColumn(new KDbTableViewColumn(f1));
    data.addColumn(new KDbTableViewColumn(f2));
    data.addColumn(new KDbTableViewColumn(f3));

    tv.setData(&data, false);

    app.setMainWidget(&tv);
    tv.show();

    return app.exec();
}
