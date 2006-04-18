/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef kwstatistics_h
#define kwstatistics_h

class KWFrameSet;
class KWDocument;
class Q3VBoxLayout;
class QTabWidget;
class QLabel;
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QLabel>
#include <Q3Frame>

#include <defs.h>
#include <kdialogbase.h>

/**
 * A class that calculates and show the statistics for a certain KWDocument
 */
class KWStatisticsDialog : public KDialogBase
{
    Q_OBJECT

public:
    KWStatisticsDialog( QWidget *parent, KWDocument *doc );
    bool wasCanceled()const { return m_canceled; }

private:
    KWDocument *m_doc;
    QWidget *m_parent;
    bool m_canceled;
    QLabel *m_resultLabelAll[7];
    QLabel *m_resultLabelSelected[7];
    QLabel *m_resultGeneralLabel[6];

    void addBox( Q3Frame *page, QLabel **resultLabel, bool calcWithFootNoteCheckbox );
    void addBoxGeneral( Q3Frame *page, QLabel **resultLabel );

    bool calcStats( QLabel **resultLabel, bool selection, bool useFootEndNote );
    void calcGeneral( QLabel **resultLabel );
    bool docHasSelection()const;
    double calcFlesch(ulong sentences, ulong words, ulong syllables);
private slots:
    void slotRefreshValue(bool);
};

#endif
