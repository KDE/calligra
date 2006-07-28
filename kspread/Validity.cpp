/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "Validity.h"

using namespace KSpread;

Validity::Validity()
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

bool Validity::operator==( const Validity& other ) const
{
  if ( message == other.message &&
       title == other.title &&
       titleInfo == other.titleInfo &&
       messageInfo == other.messageInfo &&
       valMin == other.valMin &&
       valMax == other.valMax &&
       m_cond == other.m_cond &&
       m_action == other.m_action &&
       m_restriction == other.m_restriction &&
       timeMin == other.timeMin &&
       timeMax == other.timeMax &&
       dateMin == other.dateMin &&
       dateMax == other.dateMax &&
       displayMessage == other.displayMessage &&
       allowEmptyCell == other.allowEmptyCell &&
       displayValidationInformation == other.displayValidationInformation &&
       listValidity == other.listValidity )
  {
    return true;
  }
  return false;
}
