/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KPLATO_LOCALECONFIGMONEYDIALOG_H
#define KPLATO_LOCALECONFIGMONEYDIALOG_H

#include "kplatoui_export.h"

#include <kdialog.h>

class KLocale;
class QUndoCommand;

namespace KPlato
{

class LocaleConfigMoney;
class Project;

class KPLATOUI_EXPORT LocaleConfigMoneyDialog : public KDialog {
    Q_OBJECT
public:
    LocaleConfigMoneyDialog( KLocale *locale, QWidget *parent=0);

    QUndoCommand *buildCommand( Project &project );

protected slots:
    void slotChanged();

private:
    LocaleConfigMoney *m_panel;

};

} //KPlato namespace

#endif // KPLATO_LOCALECONFIGMONEYDIALOG_H
