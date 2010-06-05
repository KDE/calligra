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

#include "GoalSeekDialog.h"

#include "CalculationSettings.h"
#include "ui/Editors.h"
#include "Map.h"
#include "Selection.h"
#include "Sheet.h"
#include "Util.h"

// commands
#include "commands/DataManipulators.h"

#include <kdebug.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <KStandardGuiItem>
#include <ktextedit.h>
#include <kpushbutton.h>

#include <QFrame>
#include <QLabel>
#include <QApplication>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCloseEvent>

#include <math.h>

using namespace KSpread;

GoalSeekDialog::GoalSeekDialog(QWidget* parent, Selection* selection)
        : KDialog(parent),
        m_selection(selection),
        m_maxIter(1000),
        m_restored(true)
{
    //setWFlags( Qt::WDestructiveClose );
    setButtons(0);
    setModal(false);

    setObjectName("GoalSeekDialog");

    resize(458, 153);
    setWindowTitle(i18n("Goal Seek"));
    setSizeGripEnabled(true);

    QWidget* mainWidget = new QWidget(this);
    setMainWidget(mainWidget);

    GoalSeekDialogLayout = new QGridLayout(mainWidget);
    GoalSeekDialogLayout->setMargin(KDialog::marginHint());
    GoalSeekDialogLayout->setSpacing(KDialog::spacingHint());

    m_startFrame = new QFrame(this);
    m_startFrame->setFrameShape(QFrame::StyledPanel);
    m_startFrame->setFrameShadow(QFrame::Raised);
    m_startFrameLayout = new QGridLayout(m_startFrame);
    m_startFrameLayout->setMargin(KDialog::marginHint());
    m_startFrameLayout->setSpacing(KDialog::spacingHint());

    QLabel* label1 = new QLabel(i18n("Set cell:"), m_startFrame);
    m_startFrameLayout->addWidget(label1, 0, 0);

    m_selector1 = new RegionSelector(m_startFrame);
    m_selector1->setSelection(selection);
    m_selector1->setDialog(this);
    m_selector1->setSelectionMode(RegionSelector::SingleCell);
    m_startFrameLayout->addWidget(m_selector1, 0, 1);

    QLabel* label2 = new QLabel(i18n("To value:"), m_startFrame);
    m_startFrameLayout->addWidget(label2, 1, 0);

    m_selector2 = new RegionSelector(m_startFrame);
    m_selector2->setSelection(selection);
    m_selector2->setDialog(this);
    m_selector2->setSelectionMode(RegionSelector::SingleCell);
    m_startFrameLayout->addWidget(m_selector2, 1, 1);

    QLabel* label3 = new QLabel(i18n("By changing cell:"), m_startFrame);
    m_startFrameLayout->addWidget(label3, 2, 0);

    m_selector3 = new RegionSelector(m_startFrame);
    m_selector3->setSelection(selection);
    m_selector3->setDialog(this);
    m_selector3->setSelectionMode(RegionSelector::SingleCell);
    m_startFrameLayout->addWidget(m_selector3, 2, 1);

    GoalSeekDialogLayout->addWidget(m_startFrame, 0, 0);

    QVBoxLayout * Layout5 = new QVBoxLayout();
    Layout5->setMargin(0);
    Layout5->setSpacing(6);

    m_buttonOk = new QPushButton(this);
    m_buttonOk->setText(i18n("&Start"));
    m_buttonOk->setAutoDefault(true);
    m_buttonOk->setDefault(true);
    Layout5->addWidget(m_buttonOk);

    m_buttonCancel = new KPushButton(KStandardGuiItem::cancel(), this);
    m_buttonCancel->setAutoDefault(true);
    Layout5->addWidget(m_buttonCancel);
    QSpacerItem* spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    Layout5->addItem(spacer);

    GoalSeekDialogLayout->addLayout(Layout5, 0, 1);

    m_resultFrame = new QFrame(this);
    m_resultFrame->setFrameShape(QFrame::StyledPanel);
    m_resultFrame->setFrameShadow(QFrame::Raised);
    m_resultFrame->setMinimumWidth(350);
    m_resultFrameLayout = new QGridLayout(m_resultFrame);
    m_resultFrameLayout->setMargin(KDialog::marginHint());
    m_resultFrameLayout->setSpacing(KDialog::spacingHint());

    m_currentValueLabel = new QLabel(m_resultFrame);
    m_currentValueLabel->setText(i18n("Current value:"));

    m_resultFrameLayout->addWidget(m_currentValueLabel, 2, 0);

    m_newValueDesc = new QLabel(m_resultFrame);
    m_newValueDesc->setText(i18n("New value:"));

    m_resultFrameLayout->addWidget(m_newValueDesc, 1, 0);

    m_newValue = new QLabel(m_resultFrame);
    m_newValue->setText("m_targetValueEdit");

    m_resultFrameLayout->addWidget(m_newValue, 1, 1);

    m_currentValue = new QLabel(m_resultFrame);
    m_currentValue->setText("m_currentValue");

    m_resultFrameLayout->addWidget(m_currentValue, 2, 1);

    m_resultText = new QLabel(m_resultFrame);
    m_resultText->setText(i18n("Goal seeking with cell <cell> found <a | no> solution:"));
    m_resultText->setAlignment(Qt::AlignVCenter);
    m_resultText->setWordWrap(true);

    m_resultFrameLayout->addWidget(m_resultText, 0, 0, 0, 1);

    //  GoalSeekDialogLayout->addWidget( m_resultFrame, 1, 0 );

    m_resultFrame->hide();

    m_sheetName = m_selection->activeSheet()->sheetName();

    // Allow the user to select cells on the spreadsheet.
//   m_selection->canvasWidget()->startChoose();

    qApp->installEventFilter(this);

    // signals and slots connections
    connect(m_buttonOk, SIGNAL(clicked()), this, SLOT(buttonOkClicked()));
    connect(m_buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(this, SIGNAL(rejected()), this, SLOT(buttonCancelClicked()));

    //connect( m_selection, SIGNAL(changed(const Region&)),
    //         this, SLOT(slotSelectionChanged()));

    // tab order
    setTabOrder(m_selector1,  m_selector2);
    setTabOrder(m_selector2,  m_selector3);
    setTabOrder(m_selector3,  m_buttonOk);
    setTabOrder(m_buttonOk, m_buttonCancel);
}

GoalSeekDialog::~GoalSeekDialog()
{
    kDebug() << "~GoalSeekDialog";

    if (m_selection->activeSheet()) {
        chooseCleanup();
        if (!m_restored) {
            m_sourceCell.setValue(Value(m_oldSource));
            m_selection->emitModified();
        }
    }
}

void GoalSeekDialog::closeEvent(QCloseEvent * e)
{
    e->accept();
    deleteLater();
}

void GoalSeekDialog::buttonOkClicked()
{
    if (m_maxIter > 0) {
        Sheet * sheet = m_selection->activeSheet();

        const Region source(m_selector3->textEdit()->toPlainText(), sheet->map(), sheet);
        if (!source.isValid() || !source.isSingular()) {
            KMessageBox::error(this, i18n("Cell reference is invalid."));
            m_selector3->textEdit()->selectAll();
            m_selector3->textEdit()->setFocus();

            m_selection->emitModified();
            return;
        }

        const Region target(m_selector1->textEdit()->toPlainText(), sheet->map(), sheet);
        if (!target.isValid() || !source.isSingular()) {
            KMessageBox::error(this, i18n("Cell reference is invalid."));
            m_selector1->textEdit()->selectAll();
            m_selector1->textEdit()->setFocus();

            m_selection->emitModified();
            return;
        }

        bool ok = false;
        double goal = m_selection->activeSheet()->map()->calculationSettings()->locale()->readNumber(m_selector2->textEdit()->toPlainText(), &ok);
        if (!ok) {
            KMessageBox::error(this, i18n("Target value is invalid."));
            m_selector2->textEdit()->selectAll();
            m_selector2->textEdit()->setFocus();

            m_selection->emitModified();
            return;
        }

        m_sourceCell = Cell(source.firstSheet(), source.firstRange().topLeft());
        m_targetCell = Cell(target.firstSheet(), target.firstRange().topLeft());

        if (!m_sourceCell.value().isNumber()) {
            KMessageBox::error(this, i18n("Source cell must contain a numeric value."));
            m_selector3->textEdit()->selectAll();
            m_selector3->textEdit()->setFocus();

            m_selection->emitModified();
            return;
        }

        if (!m_targetCell.isFormula()) {
            KMessageBox::error(this, i18n("Target cell must contain a formula."));
            m_selector1->textEdit()->selectAll();
            m_selector1->textEdit()->setFocus();

            m_selection->emitModified();
            return;
        }

        m_buttonOk->setText(i18n("&OK"));
        m_buttonOk->setEnabled(false);
        m_buttonCancel->setEnabled(false);
        GoalSeekDialogLayout->addWidget(m_resultFrame, 0, 0);
        m_startFrame->hide();
        m_resultFrame->show();
        if (m_startFrame->width() > 350)
            m_resultFrame->setMinimumWidth(m_startFrame->width());

        m_restored = false;

        startCalc(numToDouble(m_sourceCell.value().asFloat()), goal);
        m_selection->emitModified();

        return;
    } else {
        m_restored = true;
    }

    m_selection->emitModified();
    accept();
    deleteLater();
}

void GoalSeekDialog::buttonCancelClicked()
{
    if (!m_restored) {
        m_sourceCell.setValue(Value(m_oldSource));
        m_restored = true;
        m_selection->emitModified();
    }

    deleteLater();
}

void GoalSeekDialog::chooseCleanup()
{
//   m_selection->canvasWidget()->endChoose();

    Sheet * sheet = 0;

    // Switch back to the old sheet
    Q_ASSERT(m_selection->activeSheet());
    if (m_selection->activeSheet()->sheetName() !=  m_sheetName) {
        sheet = m_selection->activeSheet()->map()->findSheet(m_sheetName);
        if (sheet)
            m_selection->setActiveSheet(sheet);
    } else
        sheet = m_selection->activeSheet();
}


void GoalSeekDialog::startCalc(double _start, double _goal)
{
    m_resultText->setText(i18n("Starting..."));
    m_newValueDesc->setText(i18n("Iteration:"));

    // lets be optimistic
    bool ok = true;

    // TODO: make this configurable
    double eps = 0.0000001;

    double startA = 0.0, startB;
    double resultA, resultB;

    // save old value
    m_oldSource = numToDouble(m_sourceCell.value().asFloat());
    resultA = _goal;

    // initialize start value
    startB = _start;
    double x = startB + 0.5;

    // while the result is not close enough to zero
    // or while the max number of iterations is not reached...
    while (fabs(resultA) > eps && (m_maxIter >= 0)) {
        startA = startB;
        startB = x;

        m_sourceCell.setValue(Value(startA));
        resultA = numToDouble(m_targetCell.value().asFloat()) - _goal;
        //    kDebug() <<"Target A:" << m_targetCell.value().asFloat() <<"," << m_targetCell.userInput() <<" Calc:" << resultA;

        m_sourceCell.setValue(Value(startB));
        resultB = numToDouble(m_targetCell.value().asFloat()) - _goal;
        /*
          kDebug() <<"Target B:" << m_targetCell.value().asFloat() <<"," << m_targetCell.userInput() <<" Calc:" << resultB;

          kDebug() <<"Iteration:" << m_maxIter <<", StartA:" << startA
                  << ", ResultA: " << resultA << " (eps: " << eps << "), StartB: "
                  << startB << ", ResultB: " << resultB << endl;
        */

        // find zero with secant method (rough implementation was provided by Franz-Xaver Meier):
        // if the function returns the same for two different
        // values we have something like a horizontal line
        // => can't get zero.
        if (resultB == resultA) {
            //      kDebug() <<" resultA == resultB";
            if (fabs(resultA) < eps) {
                ok = true;
                break;
            }

            ok = false;
            break;
        }

        // Point of intersection of secant with x-axis
        x = (startA * resultB - startB * resultA) / (resultB - resultA);

        if (fabs(x) > 100000000) {
            //      kDebug() <<"fabs(x) > 100000000:" << x;
            ok = false;
            break;
        }

        //    kDebug() <<"X:" << x <<", fabs (resultA):" << fabs(resultA) <<", Real start:" << startA <<", Real result:" << resultA <<", Iteration:" << m_maxIter;

        --m_maxIter;
        if (m_maxIter % 20 == 0)
            m_newValue->setText(QString::number(m_maxIter));
    }

    m_newValueDesc->setText(i18n("New value:"));
    if (ok) {
        m_sourceCell.setValue(Value(startA));

        m_resultText->setText(i18n("Goal seeking with cell %1 found a solution:",
                                   m_selector3->textEdit()->toPlainText()));
        m_newValue->setText(m_selection->activeSheet()->map()->calculationSettings()->locale()->formatNumber(startA));
        m_currentValue->setText(m_selection->activeSheet()->map()->calculationSettings()->locale()->formatNumber(m_oldSource));
        m_restored = false;
    } else {
        // restore the old value
        m_sourceCell.setValue(Value(m_oldSource));
        m_resultText->setText(i18n("Goal seeking with cell %1 has found NO solution.",
                                   m_selector3->textEdit()->toPlainText()));
        m_newValue->setText("");
        m_currentValue->setText(m_selection->activeSheet()->map()->calculationSettings()->locale()->formatNumber(m_oldSource));
        m_restored = true;
    }

    m_buttonOk->setEnabled(true);
    m_buttonCancel->setEnabled(true);
    m_maxIter = 0;
}

#include "GoalSeekDialog.moc"

