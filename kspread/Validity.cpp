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
  m_displayMessage = true;
  m_allowEmptyCell = false;
  m_displayValidationInformation = false;
}

bool Validity::loadXML( Cell* const cell, const KoXmlElement& validityElement )
{
    bool ok = false;
    KoXmlElement param = validityElement.namedItem( "param" ).toElement();
    if ( !param.isNull() )
    {
        if ( param.hasAttribute( "cond" ) )
        {
            m_cond = (Conditional::Type) param.attribute("cond").toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "action" ) )
        {
            m_action = (Action::Type) param.attribute("action").toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "allow" ) )
        {
            m_restriction = (Restriction::Type) param.attribute("allow").toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "valmin" ) )
        {
            valMin = param.attribute("valmin").toDouble( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "valmax" ) )
        {
            valMax = param.attribute("valmax").toDouble( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "displaymessage" ) )
        {
            m_displayMessage = ( bool )param.attribute("displaymessage").toInt();
        }
        if ( param.hasAttribute( "displayvalidationinformation" ) )
        {
            m_displayValidationInformation = ( bool )param.attribute("displayvalidationinformation").toInt();
        }
        if ( param.hasAttribute( "allowemptycell" ) )
        {
            m_allowEmptyCell = ( bool )param.attribute("allowemptycell").toInt();
        }
        if ( param.hasAttribute("listvalidity") )
        {
            m_listValidity = param.attribute("listvalidity").split(';', QString::SkipEmptyParts );
        }
    }
    KoXmlElement inputTitle = validityElement.namedItem( "inputtitle" ).toElement();
    if (!inputTitle.isNull())
    {
        m_titleInfo = inputTitle.text();
    }
    KoXmlElement inputMessage = validityElement.namedItem( "inputmessage" ).toElement();
    if (!inputMessage.isNull())
    {
        m_messageInfo = inputMessage.text();
    }

    KoXmlElement titleElement = validityElement.namedItem( "title" ).toElement();
    if (!titleElement.isNull())
    {
        m_title = titleElement.text();
    }
    KoXmlElement messageElement = validityElement.namedItem( "message" ).toElement();
    if (!messageElement.isNull())
    {
        m_message = messageElement.text();
    }
    KoXmlElement timeMinElement = validityElement.namedItem( "timemin" ).toElement();
    if ( !timeMinElement.isNull()  )
    {
        timeMin = cell->toTime(timeMinElement);
    }
    KoXmlElement timeMaxElement = validityElement.namedItem( "timemax" ).toElement();
    if ( !timeMaxElement.isNull()  )
    {
        timeMax = cell->toTime(timeMaxElement);
    }
    KoXmlElement dateMinElement = validityElement.namedItem( "datemin" ).toElement();
    if ( !dateMinElement.isNull() )
    {
        dateMin = cell->toDate(dateMinElement);
    }
    KoXmlElement dateMaxElement = validityElement.namedItem( "datemax" ).toElement();
    if ( !dateMaxElement.isNull() )
    {
        dateMax = cell->toDate(dateMaxElement);
    }
    return true;
}

QDomElement Validity::saveXML( QDomDocument& doc ) const
{
    QDomElement validityElement = doc.createElement("validity");

    QDomElement param=doc.createElement("param");
    param.setAttribute("cond",(int)m_cond);
    param.setAttribute("action",(int)m_action);
    param.setAttribute("allow",(int)m_restriction);
    param.setAttribute("valmin",valMin);
    param.setAttribute("valmax",valMax);
    param.setAttribute("displaymessage",m_displayMessage);
    param.setAttribute("displayvalidationinformation",m_displayValidationInformation);
    param.setAttribute("allowemptycell",m_allowEmptyCell);
    if ( !m_listValidity.isEmpty() )
        param.setAttribute( "listvalidity", m_listValidity.join( ";" ) );
    validityElement.appendChild(param);
    QDomElement titleElement = doc.createElement( "title" );
    titleElement.appendChild( doc.createTextNode( m_title ) );
    validityElement.appendChild( titleElement );
    QDomElement messageElement = doc.createElement( "message" );
    messageElement.appendChild( doc.createCDATASection( m_message ) );
    validityElement.appendChild( messageElement );

    QDomElement inputTitle = doc.createElement( "inputtitle" );
    inputTitle.appendChild( doc.createTextNode( m_titleInfo ) );
    validityElement.appendChild( inputTitle );

    QDomElement inputMessage = doc.createElement( "inputmessage" );
    inputMessage.appendChild( doc.createTextNode( m_messageInfo ) );
    validityElement.appendChild( inputMessage );



    QString tmp;
    if ( timeMin.isValid() )
    {
        QDomElement timeMinElement = doc.createElement( "timemin" );
        tmp=timeMin.toString();
        timeMinElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( timeMinElement );
    }
    if ( timeMax.isValid() )
    {
        QDomElement timeMaxElement = doc.createElement( "timemax" );
        tmp=timeMax.toString();
        timeMaxElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( timeMaxElement );
    }

    if ( dateMin.isValid() )
    {
        QDomElement dateMinElement = doc.createElement( "datemin" );
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(dateMin.year()).arg(dateMin.month()).arg(dateMin.day());
        dateMinElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( dateMinElement );
    }
    if ( dateMax.isValid() )
    {
        QDomElement dateMaxElement = doc.createElement( "datemax" );
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(dateMax.year()).arg(dateMax.month()).arg(dateMax.day());
        dateMaxElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( dateMaxElement );
    }
    return validityElement;
}

Action::Type Validity::action() const
{
    return m_action;
}

bool Validity::allowEmptyCell() const
{
    return m_allowEmptyCell;
}

Conditional::Type Validity::condition() const
{
    return m_cond;
}

bool Validity::displayMessage() const
{
    return m_displayMessage;
}

bool Validity::displayValidationInformation() const
{
    return m_displayValidationInformation;
}

const QString& Validity::messageInfo() const
{
    return m_messageInfo;
}

const QDate& Validity::maximumDate() const
{
    return dateMax;
}

const QTime& Validity::maximumTime() const
{
    return timeMax;
}

double Validity::maximumValue() const
{
    return valMax;
}

const QString& Validity::message() const
{
    return m_message;
}

const QDate& Validity::minimumDate() const
{
    return dateMin;
}

const QTime& Validity::minimumTime() const
{
    return timeMin;
}

double Validity::minimumValue() const
{
    return valMin;
}

Restriction::Type Validity::restriction() const
{
    return m_restriction;
}

const QString& Validity::title() const
{
    return m_title;
}

const QString& Validity::titleInfo() const
{
    return m_titleInfo;
}

const QStringList& Validity::validityList() const
{
    return m_listValidity;
}

void Validity::setAction( Action::Type action )
{
    m_action = action;
}

void Validity::setAllowEmptyCell( bool allow )
{
    m_allowEmptyCell = allow;
}

void Validity::setCondition( Conditional::Type condition )
{
    m_cond = condition;
}

void Validity::setDisplayMessage( bool display )
{
    m_displayMessage = display;
}

void Validity::setDisplayValidationInformation( bool display )
{
    m_displayValidationInformation = display;
}

void Validity::setMaximumDate( const QDate& date )
{
    dateMax = date;
}

void Validity::setMaximumTime( const QTime& time )
{
    timeMax = time;
}

void Validity::setMaximumValue( double value )
{
    valMax = value;
}

void Validity::setMessage( const QString& msg )
{
    m_message = msg;
}

void Validity::setMessageInfo( const QString& info )
{
    m_messageInfo = info;
}

void Validity::setMinimumDate( const QDate& date )
{
    dateMin = date;
}

void Validity::setMinimumTime( const QTime& time )
{
    timeMin = time;
}

void Validity::setMinimumValue( double value )
{
    valMin = value;
}

void Validity::setRestriction( Restriction::Type restriction )
{
    m_restriction = restriction;
}

void Validity::setTitle( const QString& t )
{
    m_title = t;
}

void Validity::setTitleInfo( const QString& info )
{
    m_titleInfo = info;
}

void Validity::setValidityList( const QStringList& list )
{
    m_listValidity = list;
}

bool Validity::testValidity( const Cell* cell ) const
{
    bool valid = false;
    if ( m_restriction != Restriction::None )
    {
        //fixme
        if ( m_allowEmptyCell && cell->text().isEmpty() )
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
            if ( cell->value().isString() && m_listValidity.contains( cell->value().asString() ) )
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

    if ( !valid && m_displayMessage )
    {
        switch (m_action )
        {
          case Action::Stop:
            KMessageBox::error((QWidget*)0, m_message,
                               m_title);
            break;
          case Action::Warning:
            KMessageBox::warningYesNo((QWidget*)0, m_message,
                                      m_title);
            break;
          case Action::Information:
            KMessageBox::information((QWidget*)0, m_message,
                                     m_title);
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
  if ( m_message == other.m_message &&
       m_title == other.m_title &&
       m_titleInfo == other.m_titleInfo &&
       m_messageInfo == other.m_messageInfo &&
       valMin == other.valMin &&
       valMax == other.valMax &&
       m_cond == other.m_cond &&
       m_action == other.m_action &&
       m_restriction == other.m_restriction &&
       timeMin == other.timeMin &&
       timeMax == other.timeMax &&
       dateMin == other.dateMin &&
       dateMax == other.dateMax &&
       m_displayMessage == other.m_displayMessage &&
       m_allowEmptyCell == other.m_allowEmptyCell &&
       m_displayValidationInformation == other.m_displayValidationInformation &&
       m_listValidity == other.m_listValidity )
  {
    return true;
  }
  return false;
}
