/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

// Qt
#include <QDate>
#include <QStringList>
#include <QTime>

// KSpread
#include "Condition.h"

namespace KSpread
{

/**
 * This namespace collects enumerations related to
 * cell content restrictions.
 */
namespace Restriction
{
  /**
   * The type of the restriction
   */
  enum Type
  {
    None /** No restriction */,
    Number /** Restrict to numbers */,
    Text /** Restrict to texts */,
    Time /** Restrict to times */,
    Date /** Restrict to dates */,
    Integer /** Restrict to integers  */,
    TextLength /** Restrict text length */,
    List /** Restrict to lists */
  };
} // namespace Restriction

namespace Action
{
  enum Type
  {
    Stop,
    Warning,
    Information
  };
}

class Validity
{
public:
  Validity();
  bool operator==( const Validity& other ) const;
  inline bool operator!=( const Validity& other ) const { return !operator==( other ); }

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

} // namespace KSpread

#endif // KSPREAD_VALIDITY
