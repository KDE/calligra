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
    : d( new Private )
{
  d->valMin = 0.0;
  d->valMax = 0.0;
  d->cond = Conditional::None;
  d->action = Stop;
  d->restriction = None;
  d->displayMessage = true;
  d->allowEmptyCell = false;
  d->displayValidationInformation = false;
}

bool Validity::isEmpty() const
{
    return d->restriction == None;
}

bool Validity::loadXML( Cell* const cell, const KoXmlElement& validityElement )
{
    bool ok = false;
    KoXmlElement param = validityElement.namedItem( "param" ).toElement();
    if ( !param.isNull() )
    {
        if ( param.hasAttribute( "cond" ) )
        {
            d->cond = (Conditional::Type) param.attribute("cond").toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "action" ) )
        {
            d->action = (Action) param.attribute("action").toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "allow" ) )
        {
            d->restriction = (Restriction) param.attribute("allow").toInt( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "valmin" ) )
        {
            d->valMin = param.attribute("valmin").toDouble( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "valmax" ) )
        {
            d->valMax = param.attribute("valmax").toDouble( &ok );
            if ( !ok )
                return false;
        }
        if ( param.hasAttribute( "displaymessage" ) )
        {
            d->displayMessage = ( bool )param.attribute("displaymessage").toInt();
        }
        if ( param.hasAttribute( "displayvalidationinformation" ) )
        {
            d->displayValidationInformation = ( bool )param.attribute("displayvalidationinformation").toInt();
        }
        if ( param.hasAttribute( "allowemptycell" ) )
        {
            d->allowEmptyCell = ( bool )param.attribute("allowemptycell").toInt();
        }
        if ( param.hasAttribute("listvalidity") )
        {
            d->listValidity = param.attribute("listvalidity").split(';', QString::SkipEmptyParts );
        }
    }
    KoXmlElement inputTitle = validityElement.namedItem( "inputtitle" ).toElement();
    if (!inputTitle.isNull())
    {
        d->titleInfo = inputTitle.text();
    }
    KoXmlElement inputMessage = validityElement.namedItem( "inputmessage" ).toElement();
    if (!inputMessage.isNull())
    {
        d->messageInfo = inputMessage.text();
    }

    KoXmlElement titleElement = validityElement.namedItem( "title" ).toElement();
    if (!titleElement.isNull())
    {
        d->title = titleElement.text();
    }
    KoXmlElement messageElement = validityElement.namedItem( "message" ).toElement();
    if (!messageElement.isNull())
    {
        d->message = messageElement.text();
    }
    KoXmlElement timeMinElement = validityElement.namedItem( "timemin" ).toElement();
    if ( !timeMinElement.isNull()  )
    {
        d->timeMin = cell->toTime(timeMinElement);
    }
    KoXmlElement timeMaxElement = validityElement.namedItem( "timemax" ).toElement();
    if ( !timeMaxElement.isNull()  )
    {
        d->timeMax = cell->toTime(timeMaxElement);
    }
    KoXmlElement dateMinElement = validityElement.namedItem( "datemin" ).toElement();
    if ( !dateMinElement.isNull() )
    {
        d->dateMin = cell->toDate(dateMinElement);
    }
    KoXmlElement dateMaxElement = validityElement.namedItem( "datemax" ).toElement();
    if ( !dateMaxElement.isNull() )
    {
        d->dateMax = cell->toDate(dateMaxElement);
    }
    return true;
}

QDomElement Validity::saveXML( QDomDocument& doc ) const
{
    QDomElement validityElement = doc.createElement("validity");

    QDomElement param=doc.createElement("param");
    param.setAttribute("cond",(int)d->cond);
    param.setAttribute("action",(int)d->action);
    param.setAttribute("allow",(int)d->restriction);
    param.setAttribute("valmin",d->valMin);
    param.setAttribute("valmax",d->valMax);
    param.setAttribute("displaymessage",d->displayMessage);
    param.setAttribute("displayvalidationinformation",d->displayValidationInformation);
    param.setAttribute("allowemptycell",d->allowEmptyCell);
    if ( !d->listValidity.isEmpty() )
        param.setAttribute( "listvalidity", d->listValidity.join( ";" ) );
    validityElement.appendChild(param);
    QDomElement titleElement = doc.createElement( "title" );
    titleElement.appendChild( doc.createTextNode( d->title ) );
    validityElement.appendChild( titleElement );
    QDomElement messageElement = doc.createElement( "message" );
    messageElement.appendChild( doc.createCDATASection( d->message ) );
    validityElement.appendChild( messageElement );

    QDomElement inputTitle = doc.createElement( "inputtitle" );
    inputTitle.appendChild( doc.createTextNode( d->titleInfo ) );
    validityElement.appendChild( inputTitle );

    QDomElement inputMessage = doc.createElement( "inputmessage" );
    inputMessage.appendChild( doc.createTextNode( d->messageInfo ) );
    validityElement.appendChild( inputMessage );



    QString tmp;
    if ( d->timeMin.isValid() )
    {
        QDomElement timeMinElement = doc.createElement( "timemin" );
        tmp=d->timeMin.toString();
        timeMinElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( timeMinElement );
    }
    if ( d->timeMax.isValid() )
    {
        QDomElement timeMaxElement = doc.createElement( "timemax" );
        tmp=d->timeMax.toString();
        timeMaxElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( timeMaxElement );
    }

    if ( d->dateMin.isValid() )
    {
        QDomElement dateMinElement = doc.createElement( "datemin" );
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(d->dateMin.year()).arg(d->dateMin.month()).arg(d->dateMin.day());
        dateMinElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( dateMinElement );
    }
    if ( d->dateMax.isValid() )
    {
        QDomElement dateMaxElement = doc.createElement( "datemax" );
        QString tmp("%1/%2/%3");
        tmp = tmp.arg(d->dateMax.year()).arg(d->dateMax.month()).arg(d->dateMax.day());
        dateMaxElement.appendChild( doc.createTextNode( tmp ) );
        validityElement.appendChild( dateMaxElement );
    }
    return validityElement;
}

Validity::Action Validity::action() const
{
    return d->action;
}

bool Validity::allowEmptyCell() const
{
    return d->allowEmptyCell;
}

Conditional::Type Validity::condition() const
{
    return d->cond;
}

bool Validity::displayMessage() const
{
    return d->displayMessage;
}

bool Validity::displayValidationInformation() const
{
    return d->displayValidationInformation;
}

const QString& Validity::messageInfo() const
{
    return d->messageInfo;
}

const QDate& Validity::maximumDate() const
{
    return d->dateMax;
}

const QTime& Validity::maximumTime() const
{
    return d->timeMax;
}

double Validity::maximumValue() const
{
    return d->valMax;
}

const QString& Validity::message() const
{
    return d->message;
}

const QDate& Validity::minimumDate() const
{
    return d->dateMin;
}

const QTime& Validity::minimumTime() const
{
    return d->timeMin;
}

double Validity::minimumValue() const
{
    return d->valMin;
}

Validity::Restriction Validity::restriction() const
{
    return d->restriction;
}

const QString& Validity::title() const
{
    return d->title;
}

const QString& Validity::titleInfo() const
{
    return d->titleInfo;
}

const QStringList& Validity::validityList() const
{
    return d->listValidity;
}

void Validity::setAction( Action action )
{
    d->action = action;
}

void Validity::setAllowEmptyCell( bool allow )
{
    d->allowEmptyCell = allow;
}

void Validity::setCondition( Conditional::Type condition )
{
    d->cond = condition;
}

void Validity::setDisplayMessage( bool display )
{
    d->displayMessage = display;
}

void Validity::setDisplayValidationInformation( bool display )
{
    d->displayValidationInformation = display;
}

void Validity::setMaximumDate( const QDate& date )
{
    d->dateMax = date;
}

void Validity::setMaximumTime( const QTime& time )
{
    d->timeMax = time;
}

void Validity::setMaximumValue( double value )
{
    d->valMax = value;
}

void Validity::setMessage( const QString& msg )
{
    d->message = msg;
}

void Validity::setMessageInfo( const QString& info )
{
    d->messageInfo = info;
}

void Validity::setMinimumDate( const QDate& date )
{
    d->dateMin = date;
}

void Validity::setMinimumTime( const QTime& time )
{
    d->timeMin = time;
}

void Validity::setMinimumValue( double value )
{
    d->valMin = value;
}

void Validity::setRestriction( Restriction restriction )
{
    d->restriction = restriction;
}

void Validity::setTitle( const QString& t )
{
    d->title = t;
}

void Validity::setTitleInfo( const QString& info )
{
    d->titleInfo = info;
}

void Validity::setValidityList( const QStringList& list )
{
    d->listValidity = list;
}

bool Validity::testValidity( const Cell* cell ) const
{
    bool valid = false;
    if ( d->restriction != None )
    {
        //fixme
        if ( d->allowEmptyCell && cell->text().isEmpty() )
            return true;

        if ( cell->value().isNumber() &&
            (d->restriction == Number ||
             (d->restriction == Integer &&
              cell->value().asFloat() == ceil(cell->value().asFloat()))))
        {
            switch( d->cond)
            {
              case Conditional::Equal:
                valid = ( cell->value().asFloat() - d->valMin < DBL_EPSILON
                          && cell->value().asFloat() - d->valMin >
                          (0.0 - DBL_EPSILON));
                break;
              case Conditional::DifferentTo:
                valid = !(  ( cell->value().asFloat() - d->valMin < DBL_EPSILON
                              && cell->value().asFloat() - d->valMin >
                              (0.0 - DBL_EPSILON)) );
                break;
              case Conditional::Superior:
                valid = ( cell->value().asFloat() > d->valMin);
                break;
              case Conditional::Inferior:
                valid = ( cell->value().asFloat()  <d->valMin);
                break;
              case Conditional::SuperiorEqual:
                valid = ( cell->value().asFloat() >= d->valMin);
                break;
              case Conditional::InferiorEqual:
                valid = (cell->value().asFloat() <= d->valMin);
                break;
              case Conditional::Between:
                valid = ( cell->value().asFloat() >= d->valMin &&
                          cell->value().asFloat() <= d->valMax);
                break;
              case Conditional::Different:
                valid = (cell->value().asFloat() < d->valMin ||
                         cell->value().asFloat() > d->valMax);
                break;
            default :
                break;
            }
        }
        else if(d->restriction==Text)
        {
            valid = cell->value().isString();
        }
        else if ( d->restriction == List )
        {
            //test int value
            if ( cell->value().isString() && d->listValidity.contains( cell->value().asString() ) )
                valid = true;
        }
        else if(d->restriction==TextLength)
        {
            if( cell->value().isString() )
            {
                int len = cell->strOutText().length();
                switch( d->cond)
                {
                  case Conditional::Equal:
                    if (len == d->valMin)
                        valid = true;
                    break;
                  case Conditional::DifferentTo:
                    if (len != d->valMin)
                        valid = true;
                    break;
                  case Conditional::Superior:
                    if(len > d->valMin)
                        valid = true;
                    break;
                  case Conditional::Inferior:
                    if(len < d->valMin)
                        valid = true;
                    break;
                  case Conditional::SuperiorEqual:
                    if(len >= d->valMin)
                        valid = true;
                    break;
                  case Conditional::InferiorEqual:
                    if(len <= d->valMin)
                        valid = true;
                    break;
                  case Conditional::Between:
                    if(len >= d->valMin && len <= d->valMax)
                        valid = true;
                    break;
                  case Conditional::Different:
                    if(len <d->valMin || len >d->valMax)
                        valid = true;
                    break;
                default :
                    break;
                }
            }
        }
        else if(d->restriction == Time && cell->isTime())
        {
            switch( d->cond)
            {
              case Conditional::Equal:
                valid = (cell->value().asTime( cell->sheet()->doc() ) == d->timeMin);
                break;
              case Conditional::DifferentTo:
                valid = (cell->value().asTime( cell->sheet()->doc() ) != d->timeMin);
                break;
              case Conditional::Superior:
                valid = (cell->value().asTime( cell->sheet()->doc() ) > d->timeMin);
                break;
              case Conditional::Inferior:
                valid = (cell->value().asTime( cell->sheet()->doc() ) < d->timeMin);
                break;
              case Conditional::SuperiorEqual:
                valid = (cell->value().asTime( cell->sheet()->doc() ) >= d->timeMin);
                break;
              case Conditional::InferiorEqual:
                valid = (cell->value().asTime( cell->sheet()->doc() ) <= d->timeMin);
                break;
              case Conditional::Between:
                valid = (cell->value().asTime( cell->sheet()->doc() ) >= d->timeMin &&
                         cell->value().asTime( cell->sheet()->doc() ) <= d->timeMax);
                break;
              case Conditional::Different:
                valid = (cell->value().asTime( cell->sheet()->doc() ) < d->timeMin ||
                         cell->value().asTime( cell->sheet()->doc() ) > d->timeMax);
                break;
            default :
                break;

            }
        }
        else if(d->restriction == Date && cell->isDate())
        {
            switch( d->cond)
            {
              case Conditional::Equal:
                valid = (cell->value().asDate( cell->sheet()->doc() ) == d->dateMin);
                break;
              case Conditional::DifferentTo:
                valid = (cell->value().asDate( cell->sheet()->doc() ) != d->dateMin);
                break;
              case Conditional::Superior:
                valid = (cell->value().asDate( cell->sheet()->doc() ) > d->dateMin);
                break;
              case Conditional::Inferior:
                valid = (cell->value().asDate( cell->sheet()->doc() ) < d->dateMin);
                break;
              case Conditional::SuperiorEqual:
                valid = (cell->value().asDate( cell->sheet()->doc() ) >= d->dateMin);
                break;
              case Conditional::InferiorEqual:
                valid = (cell->value().asDate( cell->sheet()->doc() ) <= d->dateMin);
                break;
              case Conditional::Between:
                valid = (cell->value().asDate( cell->sheet()->doc() ) >= d->dateMin &&
                         cell->value().asDate( cell->sheet()->doc() ) <= d->dateMax);
                break;
              case Conditional::Different:
                valid = (cell->value().asDate( cell->sheet()->doc() ) < d->dateMin ||
                         cell->value().asDate( cell->sheet()->doc() ) > d->dateMax);
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

    if ( !valid && d->displayMessage )
    {
        switch (d->action )
        {
          case Stop:
            KMessageBox::error((QWidget*)0, d->message,
                               d->title);
            break;
          case Warning:
            KMessageBox::warningYesNo((QWidget*)0, d->message,
                                      d->title);
            break;
          case Information:
            KMessageBox::information((QWidget*)0, d->message,
                                     d->title);
            break;
        }
    }
    return (valid || d->action != Stop);
}

bool Validity::operator==( const Validity& other ) const
{
  if ( d->message == other.d->message &&
       d->title == other.d->title &&
       d->titleInfo == other.d->titleInfo &&
       d->messageInfo == other.d->messageInfo &&
       d->valMin == other.d->valMin &&
       d->valMax == other.d->valMax &&
       d->cond == other.d->cond &&
       d->action == other.d->action &&
       d->restriction == other.d->restriction &&
       d->timeMin == other.d->timeMin &&
       d->timeMax == other.d->timeMax &&
       d->dateMin == other.d->dateMin &&
       d->dateMax == other.d->dateMax &&
       d->displayMessage == other.d->displayMessage &&
       d->allowEmptyCell == other.d->allowEmptyCell &&
       d->displayValidationInformation == other.d->displayValidationInformation &&
       d->listValidity == other.d->listValidity )
  {
    return true;
  }
  return false;
}
