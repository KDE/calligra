/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2001 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ConditionalDialog.h"

#include <KComboBox>
#include <KLocalizedString>
#include <KMessageBox>
#include <klineedit.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

using namespace Calligra::Sheets;

ConditionalWidget::ConditionalWidget(QWidget *parent, const char * /*name*/, Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
    QGridLayout *Form1Layout = new QGridLayout(this);

    QGroupBox *groupBox1_3 = new QGroupBox(this);
    QGridLayout *groupBox1_3Layout = new QGridLayout(groupBox1_3);
    groupBox1_3Layout->setAlignment(Qt::AlignTop);

    QLabel *textLabel1_3 = new QLabel(groupBox1_3);
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

    QLabel *textLabel2_3 = new QLabel(groupBox1_3);
    groupBox1_3Layout->addWidget(textLabel2_3, 1, 0);

    QSpacerItem *spacer = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_3Layout->addItem(spacer, 1, 2);
    QSpacerItem *spacer_2 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_3Layout->addItem(spacer_2, 1, 3);

    Form1Layout->addWidget(groupBox1_3, 2, 0);

    QGroupBox *groupBox1_2 = new QGroupBox(this);

    QGridLayout *groupBox1_2Layout = new QGridLayout(groupBox1_2);
    groupBox1_2Layout->setAlignment(Qt::AlignTop);

    QLabel *textLabel1_2 = new QLabel(groupBox1_2);
    groupBox1_2Layout->addWidget(textLabel1_2, 0, 0);

    QLabel *textLabel2_2 = new QLabel(groupBox1_2);
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

    QSpacerItem *spacer_3 = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_2Layout->addItem(spacer_3, 1, 2);
    QSpacerItem *spacer_4 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_2Layout->addItem(spacer_4, 1, 3);
    Form1Layout->addWidget(groupBox1_2, 1, 0);

    QGroupBox *groupBox1_1 = new QGroupBox(this);

    QGridLayout *groupBox1_1Layout = new QGridLayout(groupBox1_1);
    groupBox1_1Layout->setAlignment(Qt::AlignTop);

    QLabel *textLabel1_1 = new QLabel(groupBox1_1);
    groupBox1_1Layout->addWidget(textLabel1_1, 0, 0);

    QLabel *textLabel2_1 = new QLabel(groupBox1_1);
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

    QSpacerItem *spacer_5 = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_1Layout->addItem(spacer_5, 1, 2);
    QSpacerItem *spacer_6 = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBox1_1Layout->addItem(spacer_6, 1, 3);

    Form1Layout->addWidget(groupBox1_1, 0, 0);
    resize(QSize(702, 380).expandedTo(minimumSizeHint()));

    QStringList list;
    list += i18n("<none>"); // krazy:exclude=i18ncheckarg
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

    connect(m_condition_1, &QComboBox::textHighlighted, this, &ConditionalWidget::slotTextChanged1);
    connect(m_condition_2, &QComboBox::textHighlighted, this, &ConditionalWidget::slotTextChanged2);
    connect(m_condition_3, &QComboBox::textHighlighted, this, &ConditionalWidget::slotTextChanged3);
}

ConditionalWidget::~ConditionalWidget() = default;

void ConditionalWidget::slotTextChanged1(const QString &text)
{
    if (text == i18n("<none>")) { // krazy:exclude=i18ncheckarg
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

void ConditionalWidget::slotTextChanged2(const QString &text)
{
    if (text == i18n("<none>")) { // krazy:exclude=i18ncheckarg
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

void ConditionalWidget::slotTextChanged3(const QString &text)
{
    if (text == i18n("<none>")) { // krazy:exclude=i18ncheckarg
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
ConditionalDialog::ConditionalDialog(QWidget *parent)
    : ActionDialog(parent)
    , m_dlg(new ConditionalWidget(this))
{
    setCaption(i18n("Conditional Styles"));

    setMainWidget(m_dlg);

    init();
}

void ConditionalDialog::setStyleNames(const QStringList &list)
{
    m_dlg->m_style_1->clear();
    m_dlg->m_style_2->clear();
    m_dlg->m_style_3->clear();
    m_dlg->m_style_1->insertItems(0, list);
    m_dlg->m_style_2->insertItems(0, list);
    m_dlg->m_style_3->insertItems(0, list);
}

void ConditionalDialog::init()
{
    m_dlg->m_condition_2->setEnabled(false);
    m_dlg->m_condition_3->setEnabled(false);

    m_dlg->m_style_1->setEnabled(false);
    m_dlg->m_style_2->setEnabled(false);
    m_dlg->m_style_3->setEnabled(false);
}

void ConditionalDialog::clear()
{
    init();
    for (int i = 1; i <= 3; ++i)
        setValueRow(i, Validity::None, QString(), QString(), QString());
}

void ConditionalDialog::setValueRow(int id, Validity::Type type, const QString &val1, const QString &val2, const QString &style)
{
    KComboBox *cb = nullptr;
    KComboBox *sb = nullptr;
    KLineEdit *kl1 = nullptr;
    KLineEdit *kl2 = nullptr;

    switch (id) {
    case 1:
        cb = m_dlg->m_condition_1;
        sb = m_dlg->m_style_1;
        kl1 = m_dlg->m_firstValue_1;
        kl2 = m_dlg->m_secondValue_1;
        break;
    case 2:
        cb = m_dlg->m_condition_2;
        sb = m_dlg->m_style_2;
        kl1 = m_dlg->m_firstValue_2;
        kl2 = m_dlg->m_secondValue_2;
        break;
    case 3:
        cb = m_dlg->m_condition_3;
        sb = m_dlg->m_style_3;
        kl1 = m_dlg->m_firstValue_3;
        kl2 = m_dlg->m_secondValue_3;
        break;
    default:
        return;
    }

    if (!style.isEmpty()) {
        sb->setCurrentIndex(sb->findText(style));
        sb->setEnabled(true);
    }

    switch (type) {
    case Validity::None:
    case Validity::IsTrueFormula: // was unhandled
        cb->setCurrentIndex(0);
        break;

    case Validity::Equal:
        cb->setCurrentIndex(1);
        break;

    case Validity::Superior:
        cb->setCurrentIndex(2);
        break;

    case Validity::Inferior:
        cb->setCurrentIndex(3);
        break;

    case Validity::SuperiorEqual:
        cb->setCurrentIndex(4);
        break;

    case Validity::InferiorEqual:
        cb->setCurrentIndex(5);
        break;

    case Validity::Between:
        cb->setCurrentIndex(6);
        kl2->setText(val2);
        break;

    case Validity::Different:
        cb->setCurrentIndex(7);
        kl2->setText(val2);
        break;
    case Validity::DifferentTo:
        cb->setCurrentIndex(8);
        break;
    }

    if (type != Validity::None) {
        cb->setEnabled(true);
        kl1->setEnabled(true);
        kl1->setText(val1);
    }

    cb->setEnabled(true);
    // Enable the next condition too
    if (id == 1)
        m_dlg->m_condition_2->setEnabled(true);
    if (id == 2)
        m_dlg->m_condition_3->setEnabled(true);
}

Validity::Type ConditionalDialog::typeOfCondition(KComboBox const *const cb) const
{
    switch (cb->currentIndex()) {
    case 0:
        return Validity::None;
    case 1:
        return Validity::Equal;
    case 2:
        return Validity::Superior;
    case 3:
        return Validity::Inferior;
    case 4:
        return Validity::SuperiorEqual;
    case 5:
        return Validity::InferiorEqual;
    case 6:
        return Validity::Between;
    case 7:
        return Validity::Different;
    case 8:
        return Validity::DifferentTo;
    default:
        break;
    }
    return Validity::None;
}

bool ConditionalDialog::checkInputData(KLineEdit const *const edit1, KLineEdit const *const edit2)
{
    bool b1 = false;
    bool b2 = false;

    if (!edit2->isEnabled())
        return true;

    edit1->text().toDouble(&b1);
    edit2->text().toDouble(&b2);

    if (b1 != b2) {
        if (b1)
            KMessageBox::error(nullptr, i18n("If the first value is a number, the second value also has to be a number."));
        else
            KMessageBox::error(nullptr, i18n("If the first value is a string, the second value also has to be a string."));
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

Validity::Type ConditionalDialog::getType(int id)
{
    if (id == 1)
        return typeOfCondition(m_dlg->m_condition_1);
    if (id == 2)
        return typeOfCondition(m_dlg->m_condition_2);
    if (id == 3)
        return typeOfCondition(m_dlg->m_condition_3);
    return Validity::None;
}

QString ConditionalDialog::getValue1(int id)
{
    if (id == 1)
        return m_dlg->m_firstValue_1->text();
    if (id == 2)
        return m_dlg->m_firstValue_2->text();
    if (id == 3)
        return m_dlg->m_firstValue_3->text();
    return QString();
}

QString ConditionalDialog::getValue2(int id)
{
    if (id == 1)
        return m_dlg->m_secondValue_1->text();
    if (id == 2)
        return m_dlg->m_secondValue_2->text();
    if (id == 3)
        return m_dlg->m_secondValue_3->text();
    return QString();
}

QString ConditionalDialog::getStyleName(int id)
{
    if (id == 1)
        return m_dlg->m_style_1->currentText();
    if (id == 2)
        return m_dlg->m_style_2->currentText();
    if (id == 3)
        return m_dlg->m_style_3->currentText();
    return QString();
}

void ConditionalDialog::onApply()
{
    if (!checkInputData())
        return;

    Q_EMIT applyCondition();
}
