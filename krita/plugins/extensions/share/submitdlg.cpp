/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "submitdlg.h"

class SubmitDlg::Private {
public:
    Private()
        : ui(new Ui::Dialog)
    {}
    Ui::Dialog *ui;
};

SubmitDlg::SubmitDlg(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , d(new Private)
{
    d->ui->setupUi(this);
}

SubmitDlg::~SubmitDlg()
{
    delete d;
}

Ui::Dialog* SubmitDlg::submitDlg()
{
    return d->ui;
}


#include "submitdlg.moc"
