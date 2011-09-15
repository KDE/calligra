/* This file is part of the Calligra project
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


#ifndef DCMIMPORTDIALOG_H
#define DCMIMPORTDIALOG_H

#include <QDialog>

namespace Ui
{
class DCMImportDialog;
}

class DCMImportDialog : public QDialog
{
    Q_OBJECT
public:
    DCMImportDialog(QWidget *parent = 0);
    ~DCMImportDialog();

    bool getDigitalSignature();
    bool getIgnoreRelationshipConstraints();
    bool getIgnoreContentItemErrors();
    bool getSkipInvalidSubtree();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DCMImportDialog *ui;
};

#endif // DCMIMPORTDIALOG_H
