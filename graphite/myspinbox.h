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

#ifndef myspinbox_h
#define myspinbox_h

#include <knuminput.h>
#include <graphiteglobal.h>

class MySpinBox : public KIntSpinBox {

    Q_OBJECT

public:
    MySpinBox(QWidget *parent=0, const char *name=0);
    virtual ~MySpinBox() {}

    Graphite::Unit defaultUnit() const { return m_default; }
    void setDefaultUnit(Graphite::Unit unit) { m_default=unit; }

    double currentValue() const;
    void setValue(const double &value);

    void fixSpinbox();

public slots:
    virtual void stepUp();
    virtual void stepDown();

protected:
    virtual QString mapValueToText(int value);
    virtual int mapTextToValue(bool* ok);

private:
    int convert(const double &value);
    Graphite::Unit m_default;
};

#endif
