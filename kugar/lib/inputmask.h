/***************************************************************************
*   Copyright (C) 2005 by Adam Treat                                      *
*   treat@kde.org                                                         *
*                                                                         *
*   Copyright (C) 2000 Trolltech AS.  All rights reserved.                *
*   info@trolltech.com                                                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
***************************************************************************/

#ifndef INPUTMASK_H
#define INPUTMASK_H

#include <qobject.h>

namespace Kugar
{

class InputMask : public QObject
{
    Q_OBJECT
public:
    InputMask( QObject *parent = 0, const char *name = 0 );

    ~InputMask();

    QString mask() const;
    void setMask( const QString &mask );

    QString formatText( const QString &txt );

private:
    void parseInputMask( const QString &maskFields );
    bool isValidInput( QChar key, QChar mask ) const;
    QString maskString( uint pos, const QString &str, bool clear = false ) const;
    QString clearString( uint pos, uint len ) const;
    QString stripString( const QString &str ) const;
    int findInMask( int pos, bool forward, bool findSeparator, QChar searchChar = QChar() ) const;

private:
    QString m_text;
    int m_maxLength;
    QChar m_blank;
    QString m_mask;
    struct MaskInputData
    {
        enum Casemode { NoCaseMode, Upper, Lower };
        QChar maskChar; // either the separator char or the inputmask
        bool separator;
        Casemode caseMode;
    };
    MaskInputData *m_maskData;
};

}

#endif
