/*
 *  opts_line_dlg.h - part of Krayon
 *
 *  Copyright (c) 2001 John Califf 
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

#ifndef __filloptionsdialog_h__
#define __filloptionsdialog_h__

#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <kdialog.h>

typedef struct _fillOptsConfig
{
    int _opacity;
    int _redtolerance;
    int _greentolerance;
    int _bluetolerance;
    int _huetolerance;
    int _saturationtolerance;
    int _valuetolerance;
} fillOptsConfig;


class FillOptionsDialog : public KDialog
{
  Q_OBJECT

public:

    FillOptionsDialog( int _opacity = 255, bool _pattern = false,
        int _redtolerance = 0, int _greentolerance = 0, int _bluetolerance = 0,
        QWidget *parent = 0, const char *name = 0 );

    int opacity()           { return mpOpacity->value(); };
    bool usePattern()       { return mpUsePattern->isChecked(); }; 
    //bool useGradient()    { return mpUseGradient->isChecked(); };         
    int ToleranceRed()      { return mpToleranceRed->value(); };
    int ToleranceGreen()    { return mpToleranceGreen->value(); };
    int ToleranceBlue()     { return mpToleranceBlue->value(); };

private:
  
    QSpinBox  *mpOpacity;

    QSpinBox  *mpToleranceRed;
    QSpinBox  *mpToleranceGreen;
    QSpinBox  *mpToleranceBlue;
    
    QCheckBox *mpUsePattern;
    //QCheckBox *mpUseGradient;
};

#endif // __filloptionsdialog.h__
