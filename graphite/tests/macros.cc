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
#include <qdom.h>
#include <kdebug.h>
#include <graphiteglobal.h>

void gradientOut(const char *prefix, const Gradient &g) {
    kdDebug() << prefix << ":" << endl;
    kdDebug() << "   ca: " << g.ca.name() << " cb: " << g.cb.name() << " type: " << static_cast<int>(g.type)
              << " xfactor: " << g.xfactor << " yfactor: " << g.yfactor << endl;
}

void bordersOut(const char *prefix, const Graphite::PageBorders &b) {
    kdDebug() << prefix << ":" << endl;
    kdDebug() << "   left: " << b.left << " top: " << b.top << " right: " << b.right << " bottom: " << b.bottom << endl;
}

void pageOut(const char *prefix, const Graphite::PageLayout &p) {
    kdDebug() << prefix << ":" << endl;
    kdDebug() << "   orientation: " << static_cast<int>(p.orientation) << " layout: " << static_cast<int>(p.layout)
              << " size: " << static_cast<int>(p.size) << endl << "   customWidth: " << p.customWidth
              << " customHeight: " << p.customHeight << endl
              << "   border: left: " << p.borders.left << " top: " << p.borders.top << " right: " << p.borders.right << " bottom: " << p.borders.bottom << endl;
}

int main() {

    kdDebug() << "=============================== Testing 'Macros' ===============================" << endl << endl
              << "------> General macros:" << endl;
    kdDebug() << "max(3, 4): " << Graphite::max(3, 4) << ", max(3.2, -7.9): " << Graphite::max(3.2, -7.9) << endl;
    kdDebug() << "min(3, 4): " << Graphite::min(3, 4) << ", min(3.2, -7.9): " << Graphite::min(3.2, -7.9) << endl;
    kdDebug() << "abs(-7): " << Graphite::abs(-7) << ", abs(2.3): " << Graphite::abs(2.3) << endl;
    kdDebug() << "mm2inch(inch2mm(1.0)): " << Graphite::mm2inch(Graphite::inch2mm(1.0)) << endl;
    kdDebug() << "mm2pt(pt2mm(1.0)): " << Graphite::mm2pt(Graphite::pt2mm(1.0)) << endl;
    kdDebug() << "inch2pt(pt2inch(1.0)): " << Graphite::inch2pt(Graphite::pt2inch(1.0)) << endl;
    kdDebug() << "inch2mm(mm2inch(1.0)): " << Graphite::inch2mm(Graphite::mm2inch(1.0)) << endl;
    kdDebug() << "pt2inch(inch2pt(1.0)): " << Graphite::pt2inch(Graphite::inch2pt(1.0)) << endl;
    kdDebug() << "pt2mm(mm2pt(1.0)): " << Graphite::pt2mm(Graphite::mm2pt(1.0)) << endl;
    kdDebug() << "qRound(1.49): " << qRound(1.49) << endl;
    kdDebug() << "qRound(1.50): " << qRound(1.50) << endl;
    kdDebug() << "qRound(-1.49): " << qRound(-1.49) << endl;
    kdDebug() << "qRound(-1.50): " << qRound(-1.50) << endl;
    kdDebug() << "rad2deg(1.0): " << Graphite::rad2deg(1.0) << endl;
    kdDebug() << "deg2rad(90.0): " << Graphite::deg2rad(90.0) << endl;
    kdDebug() << "normalizeRad(400.0): " << Graphite::normalizeRad(400.0) << endl;
    kdDebug() << "normalizeRad(-400.0): " << Graphite::normalizeRad(-400.) << endl;
    kdDebug() << "normalizeDeg(12345.0): " << Graphite::normalizeDeg(12345.0) << endl;
    kdDebug() << "normalizeDeg(-12345.0): " << Graphite::normalizeDeg(-12345.0) << endl;
    int x1=20, y1=20;
    Graphite::rotatePoint(x1, y1, 1.0, QPoint(10, 10));
    kdDebug() << "rotatePoint(20(x), 20(y), 1.0, QPoint(10, 10)): x: " << x1 << " y: " << y1 << endl;
    double x2=10.0, y2=10.0;
    Graphite::rotatePoint(x2, y2, 1.0, -2.0, -2.0);
    kdDebug() << "rotatePoint(10.0(x), 10.0(y), 1.0, -2.0, -2.0): x: " << x2 << " y: " << y2 << endl;
    x1=20;
    y1=20;
    Graphite::scalePoint(x1, y1, 2.0, 3.0, QPoint(10, 10));
    kdDebug() << "scalePoint(20(x), 20(y), 2.0, 3.0, QPoint(10, 10)): x: " << x1 << " y: " << y1 << endl;
    x2=20.0;
    y2=20.0;
    Graphite::scalePoint(x2, y2, 2.0, 3.0, 10.0, 10.0);
    kdDebug() << "scalePoint(20.0(x), 20.0(y), 2.0, 3.0, 10.0, 10.0): x: " << x2 << " y: " << y2 << endl << endl;

    kdDebug() << "------> Page Size:" << endl;
    QString tmp=QString::number(Graphite::pageWidth[0]);
    tmp+=QChar(' ');
    for(int i=1; i<=KPrinter::NPageSize; ++i) {
        tmp+=QString::number(Graphite::pageWidth[i]);
        tmp+=QChar(' ');
    }
    kdDebug() << "pageWidth[]: " << tmp << endl;

    tmp=QString::number(Graphite::pageHeight[0]);
    tmp+=QChar(' ');
    for(int i=1; i<=KPrinter::NPageSize; ++i) {
        tmp+=QString::number(Graphite::pageHeight[i]);
        tmp+=QChar(' ');
    }
    kdDebug() << "pageHeight[]: " << tmp << endl << endl;

    kdDebug() << "------> Gradient (struct):" << endl;
    Gradient g1;
    gradientOut("Gradient g1", g1);
    g1.ca=QColor(Qt::red);
    g1.cb=QColor(Qt::green);
    g1.type=KImageEffect::HorizontalGradient;
    g1.xfactor=2;
    g1.yfactor=3;
    gradientOut("Gradient g1, changed", g1);
    Gradient g2(g1);
    gradientOut("Gradient g2(g1)", g2);
    Gradient g3;
    g3.ca=QColor(Qt::blue);
    gradientOut("Gradient g3", g3);
    kdDebug() << "g1==g3: " << static_cast<int>(g1==g3) << endl;
    kdDebug() << "g1!=g3: " << static_cast<int>(g1!=g3) << endl;
    g3=g1;
    gradientOut("Gradient g3 (after copying)", g3);
    kdDebug() << "g1==g3: " << static_cast<int>(g1==g3) << endl;
    kdDebug() << "g1!=g3: " << static_cast<int>(g1!=g3) << endl;

    kdDebug() << "------> PageBorders (struct):" << endl;
    Graphite::PageBorders b1;
    bordersOut("PageBorders b1()", b1);
    b1.left=1234.5;
    Graphite::PageBorders b2(b1);
    bordersOut("PageBorders b2(b1)", b2);
    Graphite::PageBorders b3;
    b3.left=0.0;
    b3.right=1.2;
    bordersOut("PageBorders b3...", b3);
    kdDebug() << "b1==b3: " << static_cast<int>(b1==b3) << endl;
    kdDebug() << "b1!=b3: " << static_cast<int>(b1!=b3) << endl;
    b3=b1;
    bordersOut("b3=b1", b3);
    kdDebug() << "b1==b3: " << static_cast<int>(b1==b3) << endl;
    kdDebug() << "b1!=b3: " << static_cast<int>(b1!=b3) << endl << endl;

    kdDebug() << "------> PageLayout (struct):" << endl;
    Graphite::PageLayout p1;
    pageOut("PageLayout p1", p1);
    p1.size=KPrinter::B5;
    p1.orientation=KPrinter::Landscape;
    p1.customWidth=100.0;
    p1.borders=b3;
    pageOut("PageLayout p1, changed", p1);

    Graphite::PageLayout p2(p1);
    pageOut("PageLayout p2(p1))", p2);
    kdDebug() << "p1==p2: " << static_cast<int>(p1==p2) << endl;
    kdDebug() << "p1!=p2: " << static_cast<int>(p1!=p2) << endl;
    p2.size=KPrinter::A0;
    kdDebug() << "changing..." << endl;
    kdDebug() << "p1==p2: " << static_cast<int>(p1==p2) << endl;
    kdDebug() << "p1!=p2: " << static_cast<int>(p1!=p2) << endl << endl;

    kdDebug() << "------> GraphiteGlobal, QDom stuff:" << endl;
    QDomDocument doc(QString::fromLatin1("test"));
    QDomElement elem1=GraphiteGlobal::self()->createElement(QString::fromLatin1("pen"),
                                                           QPen(Qt::blue, 3, Qt::DashLine), doc);
    doc.appendChild(elem1);
    QDomElement elem2=GraphiteGlobal::self()->createElement(QString::fromLatin1("qrect"),
                                               QRect(10, 10, 100, 100), doc);
    doc.appendChild(elem2);
    QDomElement elem3=GraphiteGlobal::self()->createElement(QString::fromLatin1("fxrect"),
                                               FxRect(11.1, 22.2, 33.3, 44.4), doc);
    doc.appendChild(elem3);
    kdDebug() << doc.toString() << endl;
    QPen pen=GraphiteGlobal::self()->toPen(elem1);
    kdDebug() << "QPen: color: " << pen.color().name() << " width: " << pen.width() << " style: " << static_cast<int>(pen.style()) << endl;
    QRect rect=GraphiteGlobal::self()->toQRect(elem2);
    kdDebug() << "QRect: left: " << rect.left() << " top: " << rect.top() << " width: " << rect.width() << " height: " << rect.height() << endl;
    FxRect fxrect=GraphiteGlobal::self()->toFxRect(elem3);
    kdDebug() << "FxRect: left: " << fxrect.left() << " top: " << fxrect.top() << " width: " << fxrect.width() << " height: " << fxrect.height() << endl;

    kdDebug() << "===================================== Done =====================================" << endl;
    return 0;
}
