// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

#ifndef NOTEBAR_H
#define NOTEBAR_H

#include <QWidget>

class QPainter;
class QLabel;

class KPrDocument;
class KPrView;
class KPrinter;
class KTextEdit;

class KPrNoteBar : public QWidget
{
    Q_OBJECT

public:
    KPrNoteBar( QWidget *_parent, KPrView *_view );
    ~KPrNoteBar();

    void setCurrentNoteText( const QString &_text );

    // print
    void printNotes( QPainter *_painter, KPrinter *_printer, QValueList<int> );

    QString getNotesTextForPrinting(QValueList<int>) const;

private slots:
    void slotTextChanged();
    void slotSelectionChanged();
    void slotCopyAvailable( bool );
    void slotUndoAvailable( bool );
    void slotRedoAvailable( bool );

private:
    ::KTextEdit *textEdit;
    QLabel *label;

    KPrView *view;

    bool initialize;
};

#endif
