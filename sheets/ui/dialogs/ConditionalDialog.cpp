/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2001 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "ConditionalDialog.h"

#include "engine/ValueConverter.h"
#include "engine/ValueParser.h"
#include "core/Cell.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "core/StyleManager.h"
#include "../Selection.h"

// commands
#include "../commands/ConditionCommand.h"

#include <kcombobox.h>
#include <klineedit.h>
#include <kmessagebox.h>

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>

using namespace Calligra::Sheets;

ConditionalWidget::ConditionalWidget(QWidget* parent, const char* /*name*/, Qt::WindowFlags fl)
        : QWidget(parent, fl)
{
    QGridLayout * Form1Layout = new QGridLayout(this);

    QGroupBox * groupBox1_3 = new QGroupBox(this);
    QGridLayout * groupBox1_3Layout = new QGridLayout(groupBox1_3);
    groupBox1_3Layout->setAlignment(Qt::AlignTop);

    QLabel * textLabel1_3 = new QLabel(groupBox1_3);
    groupBox1_3Layout->addWidget(textLabel1_3, 0, 0);

    m_condition_3 = new KComboBox(groupBox1_3);
    groupBox1_3Layout->addWidget(m_condition_3, 0, 1);

    m_firstValue_3 = new KLineEdit(groupBox1_3);
    m_firstValue_3->setEnabled(false);
    groupBox1_3Layout->addWidget(m_firstValue_3, 0, 2);

    m_secondValue_3 = new KLineEdit(groupBox1_3);
    m_secondValue_3->setEnabled(false);
    groupBox1_3Layout->addWidget(m_secondValue_3, 0, 3);

    m_style_3 = new KComboBox(groupBox1_3);
    m_style_3->setEnabled(false);
    groupBox1_3Layout->addWidget(m_style_3, 1, 1);

    QLabel * textLabel2_3 = new QLabel(groupBox1_3);
    groupBox1_3Layout->addWidget(textLabel2_3, 1, 0);

    QSpacerItem * spacer = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_3Layout->addItem(spacer, 1, 2);
    QSpacerItem * spacer_2 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_3Layout->addItem(spacer_2, 1, 3);

    Form1Layout->addWidget(groupBox1_3, 2, 0);

    QGroupBox * groupBox1_2 = new QGroupBox(this);

    QGridLayout * groupBox1_2Layout = new QGridLayout(groupBox1_2);
    groupBox1_2Layout->setAlignment(Qt::AlignTop);

    QLabel * textLabel1_2 = new QLabel(groupBox1_2);
    groupBox1_2Layout->addWidget(textLabel1_2, 0, 0);

    QLabel * textLabel2_2 = new QLabel(groupBox1_2);
    groupBox1_2Layout->addWidget(textLabel2_2, 1, 0);

    m_condition_2 = new KComboBox(groupBox1_2);
    groupBox1_2Layout->addWidget(m_condition_2, 0, 1);

    m_style_2 = new KComboBox(groupBox1_2);
    m_style_2->setEnabled(false);
    groupBox1_2Layout->addWidget(m_style_2, 1, 1);

    m_firstValue_2 = new KLineEdit(groupBox1_2);

    m_firstValue_2->setEnabled(false);
    groupBox1_2Layout->addWidget(m_firstValue_2, 0, 2);

    m_secondValue_2 = new KLineEdit(groupBox1_2);
    m_secondValue_2->setEnabled(false);

    groupBox1_2Layout->addWidget(m_secondValue_2, 0, 3);

    QSpacerItem * spacer_3 = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_2Layout->addItem(spacer_3, 1, 2);
    QSpacerItem * spacer_4 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_2Layout->addItem(spacer_4, 1, 3);
    Form1Layout->addWidget(groupBox1_2, 1, 0);

    QGroupBox * groupBox1_1 = new QGroupBox(this);

    QGridLayout * groupBox1_1Layout = new QGridLayout(groupBox1_1);
    groupBox1_1Layout->setAlignment(Qt::AlignTop);

    QLabel * textLabel1_1 = new QLabel(groupBox1_1);
    groupBox1_1Layout->addWidget(textLabel1_1, 0, 0);

    QLabel * textLabel2_1 = new QLabel(groupBox1_1);
    groupBox1_1Layout->addWidget(textLabel2_1, 1, 0);

    m_condition_1 = new KComboBox(groupBox1_1);
    groupBox1_1Layout->addWidget(m_condition_1, 0, 1);

    m_style_1 = new KComboBox(groupBox1_1);
    m_style_1->setEnabled(false);
    groupBox1_1Layout->addWidget(m_style_1, 1, 1);

    m_firstValue_1 = new KLineEdit(groupBox1_1);
    m_firstValue_1->setEnabled(false);
    groupBox1_1Layout->addWidget(m_firstValue_1, 0, 2);

    m_secondValue_1 = new KLineEdit(groupBox1_1);
    m_secondValue_1->setEnabled(false);
    groupBox1_1Layout->addWidget(m_secondValue_1, 0, 3);

    QSpacerItem * spacer_5 = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_1Layout->addItem(spacer_5, 1, 2);
    QSpacerItem * spacer_6 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_1Layout->addItem(spacer_6, 1, 3);

    Form1Layout->addWidget(groupBox1_1, 0, 0);
    resize(QSize(702, 380).expandedTo(minimumSizeHint()));

    QStringList list;
    list += i18n("<none>");   // krazy:exclude=i18ncheckarg
    list += i18n("equal to");
    list += i18n("greater than");
    list += i18n("less than");
    list += i18n("equal to or greater than");
    list += i18n("equal to or less than");
    list += i18n("between");
    list += i18n("outside range");
    list += i18n("different to");

    m_condition_1->clear();
    m_condition_2->clear();
    m_condition_3->clear();
    m_condition_1->insertItems(0, list);
    m_condition_2->insertItems(0, list);
    m_condition_3->insertItems(0, list);

    groupBox1_1->setTitle(i18n("First Condition"));
    groupBox1_2->setTitle(i18n("Second Condition"));
    groupBox1_3->setTitle(i18n("Third Condition"));
    textLabel1_1->setText(i18n("Cell is"));
    textLabel1_2->setText(i18n("Cell is"));
    textLabel1_3->setText(i18n("Cell is"));
    textLabel2_1->setText(i18n("Cell style"));
    textLabel2_2->setText(i18n("Cell style"));
    textLabel2_3->setText(i18n("Cell style"));

    connect(m_condition_1, QOverload<const QString &>::of(&QComboBox::highlighted), this, &ConditionalWidget::slotTextChanged1);
    connect(m_condition_2, QOverload<const QString &>::of(&QComboBox::highlighted), this, &ConditionalWidget::slotTextChanged2);
    connect(m_condition_3, QOverload<const QString &>::of(&QComboBox::highlighted), this, &ConditionalWidget::slotTextChanged3);
}

ConditionalWidget::~ConditionalWidget()
{
}

void ConditionalWidget::slotTextChanged1(const QString & text)
{
    if (text == i18n("<none>")) {   // krazy:exclude=i18ncheckarg
        m_firstValue_1->setEnabled(false);
        m_secondValue_1->setEnabled(false);
        m_style_1->setEnabled(false);
    } else {
        m_condition_2->setEnabled(true);
        m_style_1->setEnabled(true);
        if ((text == i18n("between")) || (text == i18n("different from"))) {
            m_firstValue_1->setEnabled(true);
            m_secondValue_1->setEnabled(true);
        } else {
            m_firstValue_1->setEnabled(true);
            m_secondValue_1->setEnabled(false);
        }
    }
}

void ConditionalWidget::slotTextChanged2(const QString & text)
{
    if (text == i18n("<none>")) {   // krazy:exclude=i18ncheckarg
        m_firstValue_2->setEnabled(false);
        m_secondValue_2->setEnabled(false);
        m_style_2->setEnabled(false);
    } else {
        m_condition_3->setEnabled(true);
        m_style_2->setEnabled(true);
        if ((text == i18n("between")) || (text == i18n("different from"))) {
            m_firstValue_2->setEnabled(true);
            m_secondValue_2->setEnabled(true);
        } else {
            m_firstValue_2->setEnabled(true);
            m_secondValue_2->setEnabled(false);
        }
    }
}

void ConditionalWidget::slotTextChanged3(const QString & text)
{
    if (text == i18n("<none>")) {   // krazy:exclude=i18ncheckarg
        m_firstValue_3->setEnabled(false);
        m_secondValue_3->setEnabled(false);
        m_style_3->setEnabled(false);
    } else {
        m_style_3->setEnabled(true);
        if ((text == i18n("between")) || (text == i18n("different from"))) {
            m_firstValue_3->setEnabled(true);
            m_secondValue_3->setEnabled(true);
        } else {
            m_firstValue_3->setEnabled(true);
            m_secondValue_3->setEnabled(false);
        }
    }
}

/**
 * ConditionalDialog
 * Sets conditional cell formattings.
 */
ConditionalDialog::ConditionalDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent),
        m_selection(selection),
        m_dlg(new ConditionalWidget(this))
{
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    setCaption(i18n("Conditional Styles"));

    QStringList list(m_selection->activeSheet()->fullMap()->styleManager()->styleNames());

    m_dlg->m_style_1->insertItems(0, list);
    m_dlg->m_style_2->insertItems(0, list);
    m_dlg->m_style_3->insertItems(0, list);

    setMainWidget(m_dlg);

    connect(this, &KoDialog::okClicked, this, &ConditionalDialog::slotOk);

    init();
}

void ConditionalDialog::init()
{
    QLinkedList<Conditional> conditionList;
    QLinkedList<Conditional> otherList;
    bool found;
    int numCondition;

    QLinkedList<Conditional>::iterator it1;
    QLinkedList<Conditional>::iterator it2;

    Sheet* sheet = m_selection->activeSheet();

    conditionList = Cell(sheet, m_selection->marker()).conditions().conditionList();
    /* this is the list, but only display the conditions common to all selected
       cells*/

    for (int x = m_selection->lastRange().left(); x <= m_selection->lastRange().right(); x++) {
        for (int y = m_selection->lastRange().top(); y <= m_selection->lastRange().bottom(); y++) {
            otherList = Cell(sheet, x, y).conditions().conditionList();

            it1 = conditionList.begin();
            while (it1 != conditionList.end()) {
                debugSheets << "Here";
                found = false;
                for (it2 = otherList.begin(); !found && it2 != otherList.end(); ++it2) {
                    debugSheets << "Found:" << found;
                    found = ((*it1).value1 == (*it2).value1 &&
                             (*it1).value2 == (*it2).value2 &&
                             (*it1).cond == (*it2).cond);

                    if (!found)
                        continue;

                    if ((*it1).styleName != (*it2).styleName)
                        found = false;
                }

                if (!found) {  /* if it's not here, don't display this condition */
                    it1 = conditionList.erase(it1);
                } else {
                    ++it1;
                }
            }
        }
    }

    debugSheets << "Conditions:" << conditionList.size();

    m_dlg->m_condition_2->setEnabled(false);
    m_dlg->m_condition_3->setEnabled(false);

    m_dlg->m_style_1->setEnabled(false);
    m_dlg->m_style_2->setEnabled(false);
    m_dlg->m_style_3->setEnabled(false);

    numCondition = 0;
    for (it1 = conditionList.begin(); numCondition < 3 && it1 != conditionList.end(); ++it1) {
        init(*it1, numCondition);

        ++numCondition;
    }
    // Enable the next condition too
    if (numCondition == 0) m_dlg->m_condition_1->setEnabled(true);
    if (numCondition == 1) m_dlg->m_condition_2->setEnabled(true);
    if (numCondition == 2) m_dlg->m_condition_3->setEnabled(true);
}

void ConditionalDialog::init(Conditional const & tmp, int numCondition)
{
    debugSheets << "Adding" << numCondition;
    KComboBox * cb  = 0;
    KComboBox * sb  = 0;
    KLineEdit * kl1 = 0;
    KLineEdit * kl2 = 0;
    MapBase *const map = m_selection->activeSheet()->map();
    ValueConverter *const converter = map->converter();

    switch (numCondition) {
    case 0:
        cb  = m_dlg->m_condition_1;
        sb  = m_dlg->m_style_1;
        kl1 = m_dlg->m_firstValue_1;
        kl2 = m_dlg->m_secondValue_1;
        break;
    case 1:
        cb  = m_dlg->m_condition_2;
        sb  = m_dlg->m_style_2;
        kl1 = m_dlg->m_firstValue_2;
        kl2 = m_dlg->m_secondValue_2;
        break;
    case 2:
        cb  = m_dlg->m_condition_3;
        sb  = m_dlg->m_style_3;
        kl1 = m_dlg->m_firstValue_3;
        kl2 = m_dlg->m_secondValue_3;
        break;
    default:
        return;
    }

    if (!tmp.styleName.isEmpty()) {
        sb->setCurrentIndex(sb->findText(tmp.styleName));
        cb->setEnabled(true);
        sb->setEnabled(true);
    }

    switch (tmp.cond) {
    case Validity::None :
    case Validity::IsTrueFormula: // was unhandled
        break;

    case Validity::Equal :
        cb->setCurrentIndex(1);
        break;

    case Validity::Superior :
        cb->setCurrentIndex(2);
        break;

    case Validity::Inferior :
        cb->setCurrentIndex(3);
        break;

    case Validity::SuperiorEqual :
        cb->setCurrentIndex(4);
        break;

    case Validity::InferiorEqual :
        cb->setCurrentIndex(5);
        break;

    case Validity::Between :
        cb->setCurrentIndex(6);
        kl2->setText(converter->asString(tmp.value2).asString());
        break;

    case Validity::Different :
        cb->setCurrentIndex(7);
        kl2->setText(converter->asString(tmp.value2).asString());
        break;
    case Validity::DifferentTo :
        cb->setCurrentIndex(8);
        break;
    }

    if (tmp.cond != Validity::None) {
        kl1->setEnabled(true);
        kl1->setText(converter->asString(tmp.value1).asString());
    }
}

Validity::Type ConditionalDialog::typeOfCondition(KComboBox const * const cb) const
{
    Validity::Type result = Validity::None;
    switch (cb->currentIndex()) {
    case 0 :
        result = Validity::None;
        break;
    case 1 :
        result = Validity::Equal;
        break;
    case 2 :
        result = Validity::Superior;
        break;
    case 3 :
        result = Validity::Inferior;
        break;
    case 4 :
        result = Validity::SuperiorEqual;
        break;
    case 5 :
        result = Validity::InferiorEqual;
        break;
    case 6 :
        result = Validity::Between;
        break;
    case 7 :
        result = Validity::Different;
        break;
    case 8 :
        result = Validity::DifferentTo;
        break;
    default:
        debugSheets << "Error in list";
        break;
    }

    return result;
}

bool ConditionalDialog::checkInputData(KLineEdit const * const edit1,
                                       KLineEdit const * const edit2)
{
    bool b1 = false;
    bool b2 = false;

    if (!edit2->isEnabled())
        return true;

    edit1->text().toDouble(&b1);
    edit2->text().toDouble(&b2);

    if (b1 != b2) {
        if (b1)
            KMessageBox::sorry(0, i18n("If the first value is a number, the second value also has to be a number."));
        else
            KMessageBox::sorry(0, i18n("If the first value is a string, the second value also has to be a string."));
        return false;
    }

    return true;
}

bool ConditionalDialog::checkInputData()
{
    if (m_dlg->m_firstValue_1->isEnabled() && !checkInputData(m_dlg->m_firstValue_1, m_dlg->m_secondValue_1))
        return false;
    if (m_dlg->m_firstValue_2->isEnabled() && !checkInputData(m_dlg->m_firstValue_2, m_dlg->m_secondValue_2))
        return false;
    if (m_dlg->m_firstValue_3->isEnabled() && !checkInputData(m_dlg->m_firstValue_3, m_dlg->m_secondValue_3))
        return false;

    return true;
}

bool ConditionalDialog::getCondition(Conditional & newCondition, const KComboBox * cb,
                                     const KLineEdit * edit1, const KLineEdit * edit2,
                                     const KComboBox * sb)
{
    newCondition.cond = typeOfCondition(cb);
    if (newCondition.cond == Validity::None)
        return false;

    MapBase *const map = m_selection->activeSheet()->map();
    ValueParser *const parser = map->parser();

    newCondition.value1 = parser->parse(edit1->text());
    newCondition.value2 = parser->parse(edit2->text());
    newCondition.styleName = sb->currentText();

    return true;
}

void ConditionalDialog::slotOk()
{
    debugSheets << "slotOk";

    if (!checkInputData())
        return;

    debugSheets << "Input data is valid";

    QLinkedList<Conditional> newList;

    Conditional newCondition;

    if (getCondition(newCondition, m_dlg->m_condition_1, m_dlg->m_firstValue_1,
                     m_dlg->m_secondValue_1, m_dlg->m_style_1))
        newList.append(newCondition);

    if (getCondition(newCondition, m_dlg->m_condition_2, m_dlg->m_firstValue_2,
                     m_dlg->m_secondValue_2, m_dlg->m_style_2))
        newList.append(newCondition);

    if (getCondition(newCondition, m_dlg->m_condition_3, m_dlg->m_firstValue_3,
                     m_dlg->m_secondValue_3, m_dlg->m_style_3))
        newList.append(newCondition);

    debugSheets << "Setting conditional list";
    ConditionCommand* manipulator = new ConditionCommand();
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->setConditionList(newList);
    manipulator->add(*m_selection);
    manipulator->execute(m_selection->canvas());

    accept();
}
