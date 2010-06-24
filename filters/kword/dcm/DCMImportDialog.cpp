/* This file is part of the KOffice project
   Copyright (C) 2010 Arjun Asthana <arjun.kde@iiitd.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "DCMImportDialog.h"
#include "ui_DCMImportDialog.h"

DCMImportDialog::DCMImportDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::DCMImportDialog)
{
    ui->setupUi(this);
}

DCMImportDialog::~DCMImportDialog()
{
    delete ui;
}

void DCMImportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool DCMImportDialog::getDigitalSignature()
{
    return ui->readDigitalSignatures->checkState();
}

bool DCMImportDialog::getIgnoreRelationshipConstraints()
{
    return ui->readIgnoreRelationshipConstraints->checkState();
}

bool DCMImportDialog::getIgnoreContentItemErrors()
{
    return ui->readIgnoreContentItemErrors->checkState();
}

bool DCMImportDialog::getSkipInvalidSubtree()
{
    return ui->readSkipInvlaidSubtree->checkState();
}
