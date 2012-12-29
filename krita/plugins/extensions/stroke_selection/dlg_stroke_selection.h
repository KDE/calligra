/*
 *  Copyright (c) 2012 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or stroke
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef DLG_STROKE_SELECTION_H
#define DLG_STROKE_SELECTION_H

#include <kdialog.h>

#include "ui_wdg_stroke_selection.h"

class WdgStrokeSelection : public QWidget, public Ui::WdgStrokeSelection
{
    Q_OBJECT

public:
    WdgStrokeSelection(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }
};

class DlgStrokeSelection: public KDialog
{

    Q_OBJECT

public:

    DlgStrokeSelection(QWidget * parent = 0, const char* name = 0);
    ~DlgStrokeSelection();

private slots:

    void okClicked();

private:

    WdgStrokeSelection * m_page;
};

#endif // DLG_STROKE_SELECTION_H
