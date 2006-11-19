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

#include <float.h>

// KDE
#include <kmessagebox.h>

// KSpread
#include "Cell.h"
#include "Sheet.h"
#include "Value.h"

// Local
#include "Validity.h"

using namespace KSpread;

Validity::Validity()
    : QSharedData()
{
  valMin = 0.0;
  valMax = 0.0;
  m_cond = Conditional::None;
  m_action = Action::Stop;
  m_restriction = Restriction::None;
  displayMessage = true;
  allowEmptyCell = false;
  displayValidationInformation = false;
}

bool Validity::testValidity( const Cell* cell ) const
{
    bool valid = false;
    if ( m_restriction != Restriction::None )
    {
        //fixme
        if ( allowEmptyCell && cell->text().isEmpty() )
            return true;

        if ( cell->value().isNumber() &&
            (m_restriction == Restriction::Number ||
             (m_restriction == Restriction::Integer &&
              cell->value().asFloat() == ceil(cell->value().asFloat()))))
        {
            switch( m_cond)
            {
              case Conditional::Equal:
                valid = ( cell->value().asFloat() - valMin < DBL_EPSILON
                          && cell->value().asFloat() - valMin >
                          (0.0 - DBL_EPSILON));
                break;
              case Conditional::DifferentTo:
                valid = !(  ( cell->value().asFloat() - valMin < DBL_EPSILON
                              && cell->value().asFloat() - valMin >
                              (0.0 - DBL_EPSILON)) );
                break;
              case Conditional::Superior:
                valid = ( cell->value().asFloat() > valMin);
                break;
              case Conditional::Inferior:
                valid = ( cell->value().asFloat()  <valMin);
                break;
              case Conditional::SuperiorEqual:
                valid = ( cell->value().asFloat() >= valMin);
                break;
              case Conditional::InferiorEqual:
                valid = (cell->value().asFloat() <= valMin);
                break;
              case Conditional::Between:
                valid = ( cell->value().asFloat() >= valMin &&
                          cell->value().asFloat() <= valMax);
                break;
              case Conditional::Different:
                valid = (cell->value().asFloat() < valMin ||
                         cell->value().asFloat() > valMax);
                break;
            default :
                break;
            }
        }
        else if(m_restriction==Restriction::Text)
        {
            valid = cell->value().isString();
        }
        else if ( m_restriction == Restriction::List )
        {
            //test int value
            if ( cell->value().isString() && listValidity.contains( cell->value().asString() ) )
                valid = true;
        }
        else if(m_restriction==Restriction::TextLength)
        {
            if( cell->value().isString() )
            {
                int len = cell->strOutText().length();
                switch( m_cond)
                {
                  case Conditional::Equal:
                    if (len == valMin)
                        valid = true;
                    break;
                  case Conditional::DifferentTo:
                    if (len != valMin)
                        valid = true;
                    break;
                  case Conditional::Superior:
                    if(len > valMin)
                        valid = true;
                    break;
                  case Conditional::Inferior:
                    if(len < valMin)
                        valid = true;
                    break;
                  case Conditional::SuperiorEqual:
                    if(len >= valMin)
                        valid = true;
                    break;
                  case Conditional::InferiorEqual:
                    if(len <= valMin)
                        valid = true;
                    break;
                  case Conditional::Between:
                    if(len >= valMin && len <= valMax)
                        valid = true;
                    break;
                  case Conditional::Different:
                    if(len <valMin || len >valMax)
                        valid = true;
                    break;
                default :
                    break;
                }
            }
        }
        else if(m_restriction == Restriction::Time && cell->isTime())
        {
            switch( m_cond)
            {
              case Conditional::Equal:
                valid = (cell->value().asTime( cell->sheet()->doc() ) == timeMin);
                break;
              case Conditional::DifferentTo:
                valid = (cell->value().asTime( cell->sheet()->doc() ) != timeMin);
                break;
              case Conditional::Superior:
                valid = (cell->value().asTime( cell->sheet()->doc() ) > timeMin);
                break;
              case Conditional::Inferior:
                valid = (cell->value().asTime( cell->sheet()->doc() ) < timeMin);
                break;
              case Conditional::SuperiorEqual:
                valid = (cell->value().asTime( cell->sheet()->doc() ) >= timeMin);
                break;
              case Conditional::InferiorEqual:
                valid = (cell->value().asTime( cell->sheet()->doc() ) <= timeMin);
                break;
              case Conditional::Between:
                valid = (cell->value().asTime( cell->sheet()->doc() ) >= timeMin &&
                         cell->value().asTime( cell->sheet()->doc() ) <= timeMax);
                break;
              case Conditional::Different:
                valid = (cell->value().asTime( cell->sheet()->doc() ) < timeMin ||
                         cell->value().asTime( cell->sheet()->doc() ) > timeMax);
                break;
            default :
                break;

            }
        }
        else if(m_restriction == Restriction::Date && cell->isDate())
        {
            switch( m_cond)
            {
              case Conditional::Equal:
                valid = (cell->value().asDate( cell->sheet()->doc() ) == dateMin);
                break;
              case Conditional::DifferentTo:
                valid = (cell->value().asDate( cell->sheet()->doc() ) != dateMin);
                break;
              case Conditional::Superior:
                valid = (cell->value().asDate( cell->sheet()->doc() ) > dateMin);
                break;
              case Conditional::Inferior:
                valid = (cell->value().asDate( cell->sheet()->doc() ) < dateMin);
                break;
              case Conditional::SuperiorEqual:
                valid = (cell->value().asDate( cell->sheet()->doc() ) >= dateMin);
                break;
              case Conditional::InferiorEqual:
                valid = (cell->value().asDate( cell->sheet()->doc() ) <= dateMin);
                break;
              case Conditional::Between:
                valid = (cell->value().asDate( cell->sheet()->doc() ) >= dateMin &&
                         cell->value().asDate( cell->sheet()->doc() ) <= dateMax);
                break;
              case Conditional::Different:
                valid = (cell->value().asDate( cell->sheet()->doc() ) < dateMin ||
                         cell->value().asDate( cell->sheet()->doc() ) > dateMax);
                break;
            default :
                break;

            }
        }
    }
    else
    {
        valid= true;
    }

    if ( !valid && displayMessage )
    {
        switch (m_action )
        {
          case Action::Stop:
            KMessageBox::error((QWidget*)0, message,
                               title);
            break;
          case Action::Warning:
            KMessageBox::warningYesNo((QWidget*)0, message,
                                      title);
            break;
          case Action::Information:
            KMessageBox::information((QWidget*)0, message,
                                     title);
            break;
        }
    }
    return (valid || m_action != Action::Stop);
}

void Validity::operator=( const Validity& other ) const
{
    // FIXME KSPREAD_NEW_STYLE_STORAGE
    return;
}

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
