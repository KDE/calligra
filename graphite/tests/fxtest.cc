/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

void valueOut(const char *prefix, const FxValue &v) {
    kdDebug() << prefix << ": v: " << v.value() << " px: " << v.pxValue() << endl;
}

void rectOut(const char *prefix, const FxRect &r) {
    QRect pxr=r.pxRect();
    kdDebug() << prefix << ":" << endl
              << "     LEFT: v: " << r.left() << " px: " << pxr.left() << endl
              << "     TOP: v: " << r.top() << " px: " << pxr.top() << endl
              << "     RIGHT: v: " << r.right() << " px: " << pxr.right() << endl
              << "     BOTTOM: v: " << r.bottom() << " px: " << pxr.bottom() << endl;
}

int main(int argc, char **argv) {

    QApplication app(argc, argv); // needed for x11AppBlah

    kdDebug() << "===================== Testing FxValue, FXPoint, and FxRect =====================" << endl;
    kdDebug() << "### The real res. is commented out because it changes from machine to machine." << endl;
    kdDebug() << "### We just set some custom resolution here and use this one for the tests." << endl;
    kdDebug() << "### Real Resolution: " << GraphiteGlobal::self()->resolution() << " pixel per mm" << endl;
    GraphiteGlobal::self()->setResoltuion(96);
    kdDebug() << "Setting Resolution (96dpi): " << GraphiteGlobal::self()->resolution() << " pixel per mm" << endl << endl;

    kdDebug() << "-----> FxValue" << endl;
    kdDebug() << "Global Zoom Factor set to 2.0 (200%)" << endl;
    GraphiteGlobal::self()->setZoom(2.0);

    FxValue v1;
    valueOut("FxValue v1()", v1);
    FxValue v2(100);
    valueOut("FxValue v2(100)", v2);
    FxValue v3(100.0);
    valueOut("FxValue v3(100.0)", v3);

    GraphiteGlobal::self()->setZoom(2.5);
    kdDebug() << "GraphiteGlobal Zoom Factor set to 2.5 (250%)" << endl;
    FxValue v4(v3);
    valueOut("FxValue v4(v3)", v4);

    GraphiteGlobal::self()->setZoom(3.0);
    kdDebug() << "GraphiteGlobal Zoom Factor set to 3.0 (300%)" << endl;
    v1=v2;
    valueOut("v1=v2", v1);

    v1.setValue(10.0);
    valueOut("v1.setValue(10.0)", v1);
    v1.setPxValue(10);
    valueOut("v1.setPxValue(10)", v1);

    GraphiteGlobal::self()->setZoom(1.5);
    kdDebug() << "GraphiteGlobal Zoom Factor set to 1.5 (150%)" << endl;
    v1.recalculate();
    valueOut("v1.recalculate()", v1);

    v2.setValue(0.9);
    valueOut("v2.setValue(0.9)", v2);
    kdDebug() << "v1==v2: " << static_cast<int>(v1==v2) << endl;
    v2.setValue(1.0);
    valueOut("v2.setValue(1.0)", v2);
    kdDebug() << "v1==v2: " << static_cast<int>(v1==v2) << endl << endl;

    kdDebug() << "-----> FxPoint" << endl;
    FxPoint p1;
    kdDebug() << "FxPoint p1(): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "              y: v: " << p1.y() << " px: " << p1.pxY() << endl;

    FxPoint p2(v1, v2);
    kdDebug() << "FxPoint p2(v1, v2): x: v: " << p2.x() << " px: " << p2.pxX() << endl
              << "                    y: v: " << p2.y() << " px: " << p2.pxY() << endl;

    FxPoint p3(QPoint(100, 200));
    kdDebug() << "FxPoint p3(QPoint(100, 200)): x: v: " << p3.x() << " px: " << p3.pxX() << endl
              << "                              y: v: " << p3.y() << " px: " << p3.pxY() << endl;

    FxPoint p4(100, 200);
    kdDebug() << "FxPoint p4(100, 200): x: v: " << p4.x() << " px: " << p4.pxX() << endl
              << "                      y: v: " << p4.y() << " px: " << p4.pxY() << endl;

    kdDebug() << "Global Zoom Factor set to 2.0 (200%)" << endl;
    GraphiteGlobal::self()->setZoom(2.0);

    FxPoint p5(10.0, 20.0);
    kdDebug() << "FxPoint p5(10.0, 20.0): x: v: " << p5.x() << " px: " << p5.pxX() << endl
              << "                        y: v: " << p5.y() << " px: " << p5.pxY() << endl;

    p1=p5;
    kdDebug() << "p1=p5: x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "       y: v: " << p1.y() << " px: " << p1.pxY() << endl;

    p1.setPoint(20.0, 10.0);
    kdDebug() << "p1.setPoint(20.0, 10.0): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "                         y: v: " << p1.y() << " px: " << p1.pxY() << endl;

    QPoint p=p1.pxPoint();
    kdDebug() << "p1.pxPoint(): x: " << p.x() << " y: " << p.y() << endl;

    p1.setPxPoint(200, 100);
    kdDebug() << "p1.setPxPoint(200, 100): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "                         y: v: " << p1.y() << " px: " << p1.pxY() << endl;

    p5.setPxPoint(200, 100);
    kdDebug() << "p5.setPxPoint(200, 100): x: v: " << p5.x() << " px: " << p5.pxX() << endl
              << "                         y: v: " << p5.y() << " px: " << p5.pxY() << endl;
    kdDebug() << "p5==p1: " << static_cast<int>(p5==p1) << endl;

    p5.setPoint(26.5, 13.2);
    kdDebug() << "p5.setPoint(26.5, 13.4): x: v: " << p5.x() << " px: " << p5.pxX() << endl
              << "                         y: v: " << p5.y() << " px: " << p5.pxY() << endl;
    kdDebug() << "p5==p1: " << static_cast<int>(p5==p1) << endl;

    p5.setPoint(26.5, 13.4);
    kdDebug() << "p5.setPoint(26.5, 13.4): x: v: " << p5.x() << " px: " << p5.pxX() << endl
              << "                         y: v: " << p5.y() << " px: " << p5.pxY() << endl;
    kdDebug() << "p5==p1: " << static_cast<int>(p5==p1) << endl;

    p5.setPoint(26.3, 13.2);
    kdDebug() << "p5.setPoint(26.5, 13.4): x: v: " << p5.x() << " px: " << p5.pxX() << endl
              << "                         y: v: " << p5.y() << " px: " << p5.pxY() << endl;
    kdDebug() << "p5==p1: " << static_cast<int>(p5==p1) << endl << endl;

    kdDebug() << "-----> FxRect" << endl;
    FxRect r1;
    rectOut("FxRect r1()", r1);
    kdDebug() << "r1.isNull(): " << static_cast<int>(r1.isNull()) << endl;
    kdDebug() << "r1.isEmpty(): " << static_cast<int>(r1.isEmpty()) << endl << endl;

    FxRect r2(p2, p4);
    rectOut("FxRect r2(p2, p4)", r2);
    kdDebug() << "r2.isNull(): " << static_cast<int>(r2.isNull()) << endl;
    kdDebug() << "r2.isEmpty(): " << static_cast<int>(r2.isEmpty()) << endl << endl;

    FxRect r3(p4, p2);
    rectOut("FxRect r3(p4, p2)", r3);
    kdDebug() << "r3.isNull(): " << static_cast<int>(r3.isNull()) << endl;
    kdDebug() << "r3.isEmpty(): " << static_cast<int>(r3.isEmpty()) << endl;
    r3.normalize();
    kdDebug() << "r3.normalize()" << endl;
    rectOut("FxRect r3(p4, p2), normalized", r3);
    kdDebug() << "r3.isNull(): " << static_cast<int>(r3.isNull()) << endl;
    kdDebug() << "r3.isEmpty(): " << static_cast<int>(r3.isEmpty()) << endl << endl;

    FxRect r4(p1, QSize(100, 100));
    rectOut("FxRect r4(p1, QSize(100, 100))", r4);

    FxRect r5(10.0, 10.0, 100.0, 100.0);
    rectOut("FxRect r5(10.0, 10.0, 100.0, 100.0)", r5);

    FxRect r6(QRect(10, 10, 100, 100));
    rectOut("FxRect r6(QRect(10, 10, 100, 100))", r6);

    p1=r6.topLeft();
    kdDebug() << "r6.topLeft(): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "             y: v: " << p1.y() << " px: " << p1.pxY() << endl;
    p1=r6.bottomRight();
    kdDebug() << "r6.bottomRight(): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "             y: v: " << p1.y() << " px: " << p1.pxY() << endl;
    p1=r6.topRight();
    kdDebug() << "r6.topRight(): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "             y: v: " << p1.y() << " px: " << p1.pxY() << endl;
    p1=r6.bottomLeft();
    kdDebug() << "r6.bottomLeft(): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "             y: v: " << p1.y() << " px: " << p1.pxY() << endl;
    p1=r6.center();
    kdDebug() << "r6.center(): x: v: " << p1.x() << " px: " << p1.pxX() << endl
              << "             y: v: " << p1.y() << " px: " << p1.pxY() << endl;

    r6.moveTopLeft(FxPoint(100.0, 200.0));
    rectOut("r6.moveTopLeft(FxPoint(100.0, 200.0))", r6);
    r6.moveBottomRight(FxPoint(100.0, 200.0));
    rectOut("r6.moveBottomRight(FxPoint(100.0, 200.0))", r6);
    r6.moveTopRight(FxPoint(100.0, 200.0));
    rectOut("r6.moveTopRight(FxPoint(100.0, 200.0))", r6);
    r6.moveBottomLeft(FxPoint(100.0, 200.0));
    rectOut("r6.moveBottomLeft(FxPoint(100.0, 200.0))", r6);
    r6.moveCenter(p1);
    rectOut("r6.moveCenter(p1)", r6);
    r6.moveBy(20.0, 10.0);
    rectOut("r6.moveBy(20.0, 10.0)", r6);

    r6.setRect(10.0, 20.0, 40.0, 30.0);
    rectOut("r6.setRect(10.0, 20.0, 40.0, 30.0)", r6);
    r6.setRect(QRect(10, 20, 40, 30));
    rectOut("r6.setRect(10, 20, 40, 30)", r6);
    r6.setCoords(10.0, 20.0, 40.0, 30.0);
    rectOut("r6.setCoords(10.0, 20.0, 40.0, 30.0)", r6);

    kdDebug() << "r6.size(): width: " << r6.size().width() << " height: " << r6.size().height() << endl;
    r6.setSize(QSize(40, 60));
    rectOut("r6.setSize(40, 60)", r6);

    r6.setWidth(10.0);
    rectOut("r6.setWidth(10.0)", r6);
    r6.setHeight(10.0);
    rectOut("r6.setHeight(10.0)", r6);
    kdDebug() << "r6.width(): " << r6.width() << " r6.height(): " << r6.height() << endl;
    r6.setWidth(100);
    rectOut("r6.setWidth(100)", r6);
    r6.setHeight(100);
    rectOut("r6.setHeight(100)", r6);
    kdDebug() << "r6.width(): " << r6.width() << " r6.height(): " << r6.height() << endl;

    r5.setCoords(15.0, 15.0, 40.0, 40.0);
    r6.setCoords(10.0, 20.0, 20.0, 30.0);
    r6|=r5;
    rectOut("r6|=r5", r6);
    r5.setCoords(15.0, 15.0, 40.0, 40.0);
    r6.setCoords(10.0, 20.0, 20.0, 30.0);
    r6&=r5;
    rectOut("r6&=r5", r6);

    kdDebug() << "r6.contains(FxPoint(20.0, 20.0)): " << static_cast<int>(r6.contains(FxPoint(20.0, 20.0))) << endl;
    kdDebug() << "r6.contains(FxPoint(20.0, 20.0), true): " << static_cast<int>(r6.contains(FxPoint(20.0, 20.0), true)) << endl;
    kdDebug() << "r6.contains(QPoint(120, 160), true): " << static_cast<int>(r6.contains(QPoint(120, 160), true)) << endl;
    kdDebug() << "r6.contains(QPoint(120, 227), true): " << static_cast<int>(r6.contains(QPoint(120, 227), true)) << endl;
    kdDebug() << "r6.contains(QPoint(0, 0), true): " << static_cast<int>(r6.contains(QPoint(0, 0), true)) << endl << endl;

    kdDebug() << "r6.intersects(r5): " << static_cast<int>(r6.intersects(r5)) << endl;
    kdDebug() << "r6.intersects(QRect(0, 0, 100, 100)): " << static_cast<int>(r6.intersects(QRect(0, 0, 100, 100))) << endl;
    kdDebug() << "r6.intersects(QRect(120, 200, 100, 100)): " << static_cast<int>(r6.intersects(QRect(120, 200, 100, 100))) << endl;

    r1.setCoords(50.0, 50.0, 100.0, 100.0);
    r2.setCoords(70.0, 70.0, 120.0, 150.0);
    rectOut("r1 | r2", r1 | r2);
    rectOut("r1 & r2", r1 & r2);
    r2.setCoords(50.0, 50.0, 100.0, 100.1);
    rectOut("r2.setCoords(50.0, 50.0, 100.0, 100.1)", r2);
    kdDebug() << "r1==r2: " << static_cast<int>(r1==r2) << endl;
    kdDebug() << "r1!=r2: " << static_cast<int>(r1!=r2) << endl;
    r2.setCoords(50.0, 50.0, 100.0, 99.95);
    rectOut("r2.setCoords(50.0, 50.0, 100.0, 99.95)", r2);
    kdDebug() << "r1==r2: " << static_cast<int>(r1==r2) << endl;
    kdDebug() << "r1!=r2: " << static_cast<int>(r1!=r2) << endl;

    kdDebug() << "===================================== Done =====================================" << endl;
    return 0;
}
