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

#ifndef KEXISERVERDRIVERNOTFOUNDMESSAGE_H
#define KEXISERVERDRIVERNOTFOUNDMESSAGE_H

#include "kexi_export.h"
#include <kexiutils/KexiContextMessage.h>
#include <QWidget>

//! Context message used when no server drivers are installed.
class KEXIEXTWIDGETS_EXPORT KexiServerDriverNotFoundMessage: public KexiContextMessageWidget
{
    Q_OBJECT
public:
    KexiServerDriverNotFoundMessage(QWidget *parent);
    
    virtual ~KexiServerDriverNotFoundMessage();
    
};

#endif