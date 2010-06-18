/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000-2001 Laurent Montel <montel@kde.org>
             (C) 1999-2002 David Faure <faure@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1999 Reginald Stadlbauer <reggie@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CONSOLIDATE_DIALOG
#define KSPREAD_CONSOLIDATE_DIALOG

#include <QStringList>
#include <kdialog.h>

class KLineEdit;
class QPushButton;
class KComboBox;
class QCheckBox;
class QListWidget;

namespace KSpread
{
class Selection;
class Sheet;

/**
 * \ingroup UI
 * Dialog to consolidate cell values.
 */
class ConsolidateDialog : public KDialog
{
    Q_OBJECT
public:
    ConsolidateDialog(QWidget* parent, Selection* selection);
    virtual ~ConsolidateDialog();
    QStringList refs();

public slots:
    virtual void slotOk();
    virtual void slotCancel();
    void slotAdd();
    void slotRemove();

    void slotSelectionChanged();
    void slotReturnPressed();

protected:
    void setText(Sheet* sheet, int row, int column, const QString& text, bool asString = false);

protected:
    Selection* m_selection;
    KLineEdit* m_pRef;
    QListWidget* m_pRefs;
    KComboBox* m_pFunction;
    QPushButton* m_pAdd;
    QPushButton* m_pRemove;
    QCheckBox* m_pRow;
    QCheckBox* m_pCol;
    QCheckBox* m_pCopy;

    QString evaluate(const QString& formula, Sheet* sheet);

    enum { Sum = 0, Average, Count, Max, Min, Product, StdDev, Var };
};

} // namespace KSpread

#endif // KSPREAD_CONSOLIDATE_DIALOG
