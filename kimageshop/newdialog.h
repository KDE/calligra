/*
 *  newdialog.h - part of KImageShop
 *
 *  Copyright (c) 1999 Sven Fischer    <herpes@kawo2.rwth-aachen.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __newdialog_h__
#define __newdialog_h__

#include <kdialog.h>
#include <qspinbox.h>

class QRadioButton;

class NewDialog : public KDialog
{
    Q_OBJECT

public:

    enum {RGB=0, CMYK, LAB, GREYSCALE } ColorModel;
    enum {BACKGROUND=0, FOREGROUND, WHITE, TRANSPARENT } BgColor;

    NewDialog( QWidget *parent = 0, const char *name = 0 );

    int background();
    int colorModel();

    int newwidth() { return iwidth->value(); };
    int newheight() { return iheight->value(); };

private:
    QRadioButton* cmodel[4];
    QRadioButton* bground[4];
    QSpinBox* iwidth;
    QSpinBox* iheight;
};

#endif // __newdialog.h__
