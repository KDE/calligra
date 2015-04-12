/*
 *  Copyright (c) 2015 Scott Petrovic <scottpetrovic@gmail.com>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_WDG_IMAGE_FROM_TEMPLATE
#define KIS_WDG_IMAGE_FROM_TEMPLATE

#include "kis_global.h"
#include  <ui_wdgnewimagefromtemplate.h>


class WdgImageFromTemplate : public QWidget, public Ui::Wdgnewimagefromtemplate
{
    Q_OBJECT

    public:
        WdgImageFromTemplate(QWidget *parent) :QWidget(parent) {
            setupUi(this);
        }
};


class DlgImageFromTemplate: public KDialog
{
    Q_OBJECT

public:
    DlgImageFromTemplate(QWidget *parent);
    ~DlgImageFromTemplate();

public Q_SLOTS:
    void slotButtonClicked(int button);

private:
    WdgImageFromTemplate *m_ui;



};




#endif


