/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISEARCHLINEEDIT_H
#define KEXISEARCHLINEEDIT_H

#include <QPair>
#include <QModelIndex>
#include <KLineEdit>
#include <KexiSearchableModel.h>

class KexiSearchableModel;

//! @short Search line edit with advanced autocompletion
/*! It works like KLineEdit with QCompleter but uses fork of QCompleter
    for full-text-search.
    @note Qt Embedded features of QLineEdit+QCompleter
          (i.e. those marked with ifdef QT_KEYPAD_NAVIGATION) are not ported. */
class KexiSearchLineEdit : public KLineEdit
{
    Q_OBJECT
public:
    explicit KexiSearchLineEdit(QWidget *parent = 0);

    virtual ~KexiSearchLineEdit();

    /*! Add searchable model to the main window. This extends search to a new area. 
     One example is Project Navigator. */
    void addSearchableModel(KexiSearchableModel *model);

private slots:
    void slotCompletionHighlighted(const QString &newText);
    void slotCompletionHighlighted(const QModelIndex &index);
    void slotCompletionActivated(const QModelIndex &index);

protected:
    virtual void inputMethodEvent(QInputMethodEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

private:
    void connectCompleter();
    void disconnectCompleter();
    bool advanceToEnabledItem(int dir);
    void complete(int key);
    QString textBeforeSelection() const;
    QString textAfterSelection() const;
    int selectionEnd() const;
    
    QPair<QModelIndex, KexiSearchableModel*> mapCompletionIndexToSource(const QModelIndex &index) const;

    class Private;
    Private * const d;
};

#endif
