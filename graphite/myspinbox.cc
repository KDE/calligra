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

#include <myspinbox.h>

#include <qpushbutton.h>
#include <kglobal.h>
#include <klocale.h>

#include <limits.h>  // INT_MAX

MySpinBox::MySpinBox(QWidget *parent, const char *name) :
    KIntSpinBox(0, INT_MAX, 100, 0, 10, parent, name), m_default(Graphite::MM) {
}

QString MySpinBox::mapValueToText(int value) {

    int unit=(value & 0x60000000) >> 29;
    value&=0x9fffffff;
    QString ret=QString::number(static_cast<double>(value)/100.0, 'f', 2);
    if(unit==0)
        return ret+QString::fromLatin1(" mm");
    else if(unit==1)
        return ret+QString::fromLatin1(" pt");
    else if(unit==2)
        return ret+QString::fromLatin1(" in");
    else
        return ret+QString::fromLatin1(" cm");
}

int MySpinBox::mapTextToValue(bool* ok) {

    *ok=false;
    QString t=text();
    QString corrected;
    QChar dot('.');
    QChar colon(',');
    double value=0.0;
    int ret=0;
    unsigned int textLength=t.length();

    // analyze the new text and decide it we can use it
    unsigned int start=0;
    bool comma=false;
    // get rid of leading garbage
    while(!t[start].isDigit() && t[start]!=dot && t[start]!=colon &&
          t[start]!=KGlobal::locale()->decimalSymbol()[0] && start<textLength)
        ++start;

    // QString::toDouble() doesn't like colons
    if(t[start]==dot)
        comma=true;
    else if(t[start]==colon || t[start]==KGlobal::locale()->decimalSymbol()[0]) {
        t[start]=dot;
        comma=true;
    }

    // now get the number
    unsigned int end=start+1;
    while(end<textLength) {
        if(t[end].isDigit())
            ++end;
        else if(!comma && t[end]==dot) {
            ++end;
            comma=true;
        }
        else if(!comma && (t[end]==colon || t[end]==KGlobal::locale()->decimalSymbol()[0])) {
            t[end]=dot;  // QString::toDouble...
            ++end;
            comma=true;
        }
        else
            break;
    }

    // get hold of the number..
    if(start<textLength) {
        corrected=t.mid(start, end-start);
        value=corrected.toDouble(ok);

        int unit=static_cast<int>(m_default);
        if(t.find(QString::fromLatin1("mm"), end, false)!=-1)
            unit=0;
        else if(t.find(QString::fromLatin1("pt"), end, false)!=-1)
            unit=1;
        else if(t.find(QString::fromLatin1("in"), end, false)!=-1)
            unit=2;
        else if(t.find(QString::fromLatin1("cm"), end, false)!=-1)
            unit=3;
        ret=static_cast<int>(value*100.0) | (unit << 29);
    }
    return ret;
}

double MySpinBox::currentValue() const {

    int v=value();
    int unit=(v & 0x60000000) >> 29;
    v&=0x9fffffff;
    double ret=static_cast<double>(v)/100.0;
    if(unit==0)
        return ret;
    else if(unit==1)
        return Graphite::pt2mm(ret);
    else if(unit==2)
        return Graphite::inch2mm(ret);
    else
        return ret/10.0;
}

void MySpinBox::setValue(const double &value) {
    KIntSpinBox::setValue(convert(value));
}

void MySpinBox::fixSpinbox() {
    upButton()->setDown(false);
}

void MySpinBox::stepUp() {
    interpretText();
    if(value()==0 && m_default!=Graphite::MM)
        directSetValue(static_cast<int>(m_default) << 29);
    KIntSpinBox::stepUp();
}

void MySpinBox::stepDown() {
    interpretText();
    if(value()==0 && m_default!=Graphite::MM)
        directSetValue(static_cast<int>(m_default) << 29);
    KIntSpinBox::stepDown();
}

int MySpinBox::convert(const double &value) {

    QString t=text();
    int unit=static_cast<int>(m_default);
    double v=value;
    if(t.find(QString::fromLatin1("mm"), 0, false)!=-1)
        unit=0;
    else if(t.find(QString::fromLatin1("pt"), 0, false)!=-1) {
        unit=1;
        v=Graphite::mm2pt(value);
    }
    else if(t.find(QString::fromLatin1("in"), 0, false)!=-1) {
        unit=2;
        v=Graphite::mm2inch(value);
    }
    else if(t.find(QString::fromLatin1("cm"), 0, false)!=-1) {
        unit=3;
        v=value/10.0;
    }
    else {
        if(m_default==Graphite::Pt)
            v=Graphite::mm2pt(value);
        else if(m_default==Graphite::Inch)
            v=Graphite::mm2inch(value);
    }
    return static_cast<int>(v*100.0) | (unit << 29);
}

#include <myspinbox.moc>
