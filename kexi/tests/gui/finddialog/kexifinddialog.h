/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIFINDDIALOG_H
#define KEXIFINDDIALOG_H

#include "kexifinddialogbase.h"

/*! @brief A Kexi-specific "Find text" dialog.

  Also used for replace.
*/
class KexiFindDialog : public KexiFindDialogBase
{
    Q_OBJECT
public:
    KexiFindDialog(bool replaceMode, QWidget* parent = 0, const char* name = 0, bool modal = FALSE);
    virtual ~KexiFindDialog();

#if 0
    TODO TODO TODO TODO TODO TODO
    /*! Sets \a columnNames list for 'look in column' combo box.
     "(All columns)" item is also prepended. */
    void setLookInColumnList(const QStringList& columnNames);

    /*! \return a list for 'look in column' combo box.
     "(All columns)" item is also prepended. */
    QStringList* lookInColumnList() const;

    /*! \return column name selected in 'look in column' combo box.
     If "(All columns)" item is selected, "*" is returned. */
    QString lookInColumn() const;

    /*! Selects \a columnName to be selected 'look in column'.
     By default "(All columns)" item is selected. To select this item, pass "*". */
    void setLookInColumn(const QString& columnName);

#endif

public slots:
    /*! Sets or clears replace mode.
     For replace mode 'prompt or replace' option is visible.
    */
    void setReplaceMode(bool set);

protected:
    bool m_replaceMode : 1;
};

#endif
