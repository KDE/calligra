/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef configfootnotedia_h
#define configfootnotedia_h

#include <kdialogbase.h>

class KWDocument;
class QWidget;
class KoCounterStyleWidget;
class QRadioButton;
class KIntNumInput;
class QComboBox;
class KoUnitDoubleSpinBox;

class KWConfigFootNoteDia : public KDialogBase
{
    Q_OBJECT
public:
    KWConfigFootNoteDia( QWidget *parent, const char *name, KWDocument *_doc );
protected:
    void setupTab1();
    void setupTab2();
    void setupTab3();
private:
    KoCounterStyleWidget *m_footNoteConfig;
    KoCounterStyleWidget *m_endNoteConfig;
    KWDocument *m_doc;
    QRadioButton *rbPosLeft;
    QRadioButton *rbPosCentered;
    QRadioButton *rbPosRight;
    KIntNumInput *spLength;
    KoUnitDoubleSpinBox *spWidth;
    QComboBox *m_cbLineType;
protected slots:
    virtual void slotOk();
};

#endif


