/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qcolor.h>
#include <qapplication.h>
#include <kdebug.h>

#include <graphiteglobal.h>

int main(int argc, char **argv) {

    QApplication app(argc, argv); // needed for x11AppBlah

    kdDebug() << "===================== Testing FxValue, FXPoint, and FxRect =====================" << endl;
    kdDebug() << "+++ Global Zoom Factor set to 2.0 (200%), Resolution: " << GraphiteGlobal::self()->resolution()
              << " pixel per mm" << endl;
    kdDebug() << "### This is a comment which is ignored" << endl;
    GraphiteGlobal::self()->setZoom(2.0);
    FxValue v1;
    kdDebug() << "+++ FxValue v1(): v: " << v1.value() << " px: " << v1.pxValue() << endl;
    FxValue v2(100);
    kdDebug() << "+++ FxValue v2(100): v: " << v2.value() << " px: " << v2.pxValue() << endl;
    FxValue v3(100.0);
    kdDebug() << "+++ FxValue v3(100.0): v: " << v3.value() << " px: " << v3.pxValue() << endl;

    GraphiteGlobal::self()->setZoom(2.5);
    kdDebug() << "+++ GraphiteGlobal Zoom Factor set to 2.5 (250%)" << endl;
    FxValue v4(v3);
    kdDebug() << "+++ FxValue v4(v3): v: " << v4.value() << " px: " << v4.pxValue() << endl;

    GraphiteGlobal::self()->setZoom(3.0);
    kdDebug() << "+++ GraphiteGlobal Zoom Factor set to 3.0 (300%)" << endl;
    v1=v2;
    kdDebug() << "+++ v1=v2: v: " << v1.value() << " px: " << v1.pxValue() << endl;

    kdDebug() << "===================================== Done =====================================" << endl;
    return 0;
}
