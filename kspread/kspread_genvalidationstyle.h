/* This file is part of the KDE project

   Copyright 2004 Laurent Montel <montel@kde.org>

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


#ifndef KSPREAD_GENVALIDATIONSTYLE
#define KSPREAD_GENVALIDATIONSTYLE

#include <qdict.h>
#include <qmap.h>

class KoXmlWriter;
class KSpreadValidity;
class KSpreadGenValidationStyle;

class KSpreadGenValidationStyles
{
public:
    KSpreadGenValidationStyles();
    ~KSpreadGenValidationStyles();
    QString lookup( const KSpreadGenValidationStyle& style );

    typedef QMap<KSpreadGenValidationStyle, QString> StyleMap;
    void writeStyle( KoXmlWriter& writer );

private:
    QString makeUniqueName( const QString& base ) const;

    /// style definition -> name
    StyleMap m_styles;
    /// name -> style   (used to check for name uniqueness)
    typedef QMap<QString, bool> NameMap;
    NameMap m_names;

};

class KSpreadGenValidationStyle
{
public:
    explicit KSpreadGenValidationStyle(KSpreadValidity *_val=0) { initVal( _val ); }


    bool operator<( const KSpreadGenValidationStyle &other ) const {
        if ( allowEmptyCell != other.allowEmptyCell ) return ( allowEmptyCell < other.allowEmptyCell );
        if ( condition != other.condition ) return ( condition < other.condition );
        if ( titleInfo != other.titleInfo ) return ( titleInfo < other.titleInfo );
        if ( displayValidationInformation != other.displayValidationInformation ) return ( displayValidationInformation < other.displayValidationInformation );
        if ( messageInfo != other.messageInfo ) return ( messageInfo < other.messageInfo );
        if ( messageType != other.messageType ) return ( messageType < other.messageType );
        if ( displayMessage != other.displayMessage ) return ( displayMessage < other.displayMessage );
        if ( message != other.message ) return ( message < other.message );
        if ( title != other.title ) return ( title < other.title );

        return false;
    }
private:
    QString createValidationCondition( KSpreadValidity* _val );
    QString createTextValidationCondition( KSpreadValidity* _val );
    QString createTimeValidationCondition( KSpreadValidity* _val );
    QString createDateValidationCondition( KSpreadValidity* _val );
    QString createNumberValidationCondition( KSpreadValidity* _val );
    QString createListValidationCondition( KSpreadValidity* _val );

    void initVal( KSpreadValidity *_val );

    QString allowEmptyCell;
    QString condition;
    QString titleInfo;
    QString displayValidationInformation;
    QString messageInfo;
    QString messageType;
    QString displayMessage;
    QString message;
    QString title;
    friend class KSpreadGenValidationStyles;
};


#endif
