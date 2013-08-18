/*
 * Copyright 2009 Matthew Woehlke <mw_triad@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KCOLORUTILSDEMO_H
#define KCOLORUTILSDEMO_H

#include "ui_kcolorutilsdemo.h"

class KColorUtilsDemo: public QWidget, Ui::form
{
    Q_OBJECT
public:
    KColorUtilsDemo(QWidget* parent = 0);
    virtual ~KColorUtilsDemo() {}

public Q_SLOTS:
    void inputChanged();
    void lumaChanged();
    void mixChanged();
    void shadeChanged();

    void inputSpinChanged();
    void inputSwatchChanged(const QColor&);

    void targetSpinChanged();
    void targetSwatchChanged(const QColor&);

protected:
    QImage _leOutImg, _mtMixOutImg, _mtTintOutImg;
    bool _noUpdate;
};

#endif
// kate: hl C++; indent-width 4; replace-tabs on;
