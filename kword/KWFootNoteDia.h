/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef footnotedia_h
#define footnotedia_h

#include <kdialogbase.h>
#include "defs.h" // for NoteType
#include "KWVariable.h"
class QRadioButton;
class QLineEdit;
class KWDocument;
/******************************************************************/
/* Class: KWFootNoteDia                                           */
/******************************************************************/

class KWFootNoteDia : public KDialogBase
{
    Q_OBJECT

public:
    KWFootNoteDia( NoteType _noteType, KWFootNoteVariable::Numbering _numberingType,  const QString & _manualString, QWidget *parent, KWDocument *_doc, const char *name = 0 );

    NoteType noteType() const;
    KWFootNoteVariable::Numbering numberingType()const;
    QString manualString()const;
    void appendManualFootNote( const QString & );
protected:
    bool insertFootNote();
    bool footNoteAlreadyExists( const QString & );
protected slots:
    void footLineChanged( const QString & );
    void footNoteTypeChanged();
    void slotConfigurate();
private:

    QRadioButton *m_rbAuto;
    QRadioButton *m_rbManual;

    QRadioButton *m_rbFootNote;
    QRadioButton *m_rbEndNote;
    QLineEdit *m_footLine;
    KWDocument *m_doc;
    QValueList<QString> manualFootNotes;
};

#endif


