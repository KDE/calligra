 /* This file is part of the KDE project
   Copyright (C) 2012 Dimitrios T. Tanis <dimitrios.tanis@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiServerDriverNotFoundMessage.h"
#include <klocale.h>

KexiServerDriverNotFoundMessage::KexiServerDriverNotFoundMessage(QWidget *parent):
    KexiContextMessageWidget(parent, 0, 0,
            KexiContextMessage (
                i18nc("@info",
                "<title>No database server drivers found</title> \
                <para>Connection to a database server requires an appropriate driver which you do not have installed.</para> \
                <subtitle>Details</subtitle> \
                <para>To obtain database drivers please use your Package Manager to search and install packages named <filename> \
                calligra-kexi-xxxx-driver</filename>. Please note that your package names could vary slightly according to the distribution you use.</para>"))
            )
{
        setMessageType(KMessageWidget::Warning);
        setCalloutPointerDirection(KMessageWidget::NoPointer);
        setWordWrap(true);
        setClickClosesMessage(false);
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);        
}

KexiServerDriverNotFoundMessage::~KexiServerDriverNotFoundMessage()
{
}

#include "KexiServerDriverNotFoundMessage.moc"
