/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2002 Harri Porten <porten@kde.org>

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

#ifndef KSPREAD_GOALSEEK_DIALOG
#define KSPREAD_GOALSEEK_DIALOG

#include <QPoint>
#include <QRect>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QCloseEvent>

#include <kdialog.h>

#include "Cell.h"

class QFrame;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace KSpread
{
class RegionSelector;
class Selection;

/**
 * \ingroup UI
 * Dialog to seek a specific value for a set of parameter values.
 */
class GoalSeekDialog : public KDialog
{
    Q_OBJECT

public:
    GoalSeekDialog(QWidget* parent, Selection* selection);
    ~GoalSeekDialog();

public slots:
    void buttonOkClicked();
    void buttonCancelClicked();

protected:
    virtual void closeEvent(QCloseEvent *);

    QGridLayout * GoalSeekDialogLayout;
    QGridLayout * m_startFrameLayout;
    QGridLayout * m_resultFrameLayout;

private:
    Selection   * m_selection;
    Cell          m_sourceCell;
    Cell          m_targetCell;
    double        m_result;
    int           m_maxIter;
    bool          m_restored;
    double        m_oldSource;

    QString       m_oldText;
    QString       m_sheetName;

    QFrame      * m_startFrame;
    QPushButton * m_buttonOk;
    QPushButton * m_buttonCancel;
    QFrame      * m_resultFrame;
    QLabel      * m_newValueDesc;
    QLabel      * m_currentValueLabel;
    QLabel      * m_newValue;
    QLabel      * m_currentValue;
    QLabel      * m_resultText;

    RegionSelector* m_selector1;
    RegionSelector* m_selector2;
    RegionSelector* m_selector3;

    void startCalc(double _start, double _goal);
    void chooseCleanup();
};

} // namespace KSpread

#endif // KSPREAD_GOALSEEK_DIALOG
