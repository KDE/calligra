// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include "ImportHelpers.h"

// Rough rule for including code in this file:
//   use nothing of QT except QMap, QValueList and QString

bool AbiPropsMap::setProperty(QString newName, QString newValue)
{
    replace(newName,AbiProps(newValue));
    return true;
}

// Treat the "props" attribute of AbiWord's tags and split it in separates names and values
void AbiPropsMap::splitAndAddAbiProps(const QString& strProps)
{
    // NOTE: we assume that all AbiWord properties are in the form:
    //     property:value;
    //  If, as in CSS2, any new AbiWord property is not anymore in this form,
    //  then this method will need to be changed (perhaps with QStringList::split())
    if (strProps.isEmpty())
        return;

    QString name,value;
    bool notFinished=true;
    int position=0;
    int result;
    while (notFinished)
    {
        //Find next name and its value
        result=strProps.find(':',position);
        if (result==-1)
        {
            name=strProps.mid(position).stripWhiteSpace();
            value="";
            notFinished=false;
        }
        else
        {
            name=strProps.mid(position,result-position).stripWhiteSpace();
            position=result+1;
            result=strProps.find(';',position);
            if (result==-1)
            {
                value=strProps.mid(position).stripWhiteSpace();
                notFinished=false;
            }
            else
            {
                value=strProps.mid(position,result-position).stripWhiteSpace();
                position=result+1;
            }
        }
        kdDebug(30506) << "========== (Property :" << name << "=" << value <<":)"<<endl;
        // Now set the property
        setProperty(name,value);
    }
}

// Do not put this function inline (it's too long!)
double ValueWithLengthUnit(const QString& _str)
{
    double d;
    int pos=0;
    if ((pos=_str.find("cm"))>=0)
    {
        d=CentimetresToPoints(_str.left(pos).toDouble());
    }
    else if ((pos=_str.find("in"))>=0)
    {
        d=InchesToPoints(_str.left(pos).toDouble());
    }
    else if ((pos=_str.find("mm"))>=0)
    {
        d=MillimetresToPoints(_str.left(pos).toDouble());
    }
    else if((pos=_str.find("pt"))>=0)
    {
        d=_str.left(pos).toDouble();
    }
    else if((pos=_str.find("pi"))>=0)
    {
        d=PicaToPoints(_str.left(pos).toDouble());
    }
    else
    {
        bool b=false;
        d=_str.toDouble(&b);
        if (!b)
        {
            d=0;
            kdWarning(30506) << "Unknown value: " << _str << " (ValueWithLengthUnit)" << endl;
        }
    }
    return d;
}
