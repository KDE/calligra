/* This file is part of the KDE project
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
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

#ifndef KWINSERTINLINENOTEDIALOG_H
#define KWINSERTINLINENOTEDIALOG_H

#include <kdialog.h>

#include <ui_KWInsertInlineNoteDialog.h>

class KWDocument;
class KWView;

/// A dialog for showing and altering frame properties
class KWInsertInlineNoteDialog : public KDialog
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent a parent widget for the purpose of centering the dialog
     */
    KWInsertInlineNoteDialog(KWDocument *document, KWView *view);
    ~KWInsertInlineNoteDialog();

private slots:
    // actually adds/inserts the page
    void doIt();

private:
    Ui::KWInsertInlineNoteDialog m_widget;
    KWDocument *m_document;
    KWView *m_view;
};

#endif
