/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "tablehandler.h"

#include <functor.h>
#include <functordata.h>
#include <word97_generated.h>

#include <kdebug.h>

#include <algorithm>

KWordTableHandler::KWordTableHandler()
{
}

void KWordTableHandler::tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> /*tap*/ )
{
    kdDebug() << k_funcinfo << endl;
}

void KWordTableHandler::tableRowEnd()
{
    kdDebug() << k_funcinfo << endl;
}

void KWordTableHandler::tableCellStart()
{
    kdDebug() << k_funcinfo << endl;
}

void KWordTableHandler::tableCellEnd()
{
    kdDebug() << k_funcinfo << endl;
}
