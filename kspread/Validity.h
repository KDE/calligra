/* This file is part of the KDE project
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002,2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Stefan Hetzl <shetzl@chello.at>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 David Faure <faure@kde.org>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1998-1999 Torben Weis <weis@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_VALIDITY
#define KSPREAD_VALIDITY

#include "Condition.h"
#include "Global.h"

namespace KSpread
{

class Validity
{
public:
  Validity()
  {
    valMin = 0.0;
    valMax = 0.0;
    m_cond = Conditional::None;
    m_action = Action::Stop;
    m_restriction = Restriction::None;
    displayMessage = true;
    allowEmptyCell = false;
    displayValidationInformation = false;
  };

  QString message;
  QString title;
  QString titleInfo;
  QString messageInfo;
  double valMin;
  double valMax;
  Conditional::Type m_cond;
  Action::Type m_action;
  Restriction::Type m_restriction;
  QTime  timeMin;
  QTime  timeMax;
  QDate  dateMin;
  QDate  dateMax;
  bool displayMessage;
  bool allowEmptyCell;
  bool displayValidationInformation;
  QStringList listValidity;
};

}; // namespace KSpread

#endif // KSPREAD_VALIDITY
