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

#include "kis_image_from_template_widget.h"
#include  <ui_wdgnewimagefromtemplate.h>
#include <QDebug>
#include <KDialog>



DlgImageFromTemplate::DlgImageFromTemplate(QWidget *parent):KDialog(parent),
    m_ui(new WdgImageFromTemplate(parent)) {
    setMainWidget(m_ui);
    setMinimumSize(m_ui->page->sizeHint());
    this->show();
}


DlgImageFromTemplate::~DlgImageFromTemplate() {

}


void DlgImageFromTemplate::slotButtonClicked(int button) {


    if ( button == KDialog::Ok)    {
        qDebug() << "ok clicked";
    }

    if ( button == KDialog::Cancel)    {
         qDebug() << "cancel clicked";
    }

}
