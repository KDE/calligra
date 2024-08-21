/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 1999-2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ValidityDialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <KComboBox>
#include <KMessageBox>
#include <KPageWidget>
#include <klineedit.h>
#include <ktextedit.h>

#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "engine/ValueParser.h"

using namespace Calligra::Sheets;

Q_DECLARE_METATYPE(Validity::Type)
Q_DECLARE_METATYPE(Validity::Action)
Q_DECLARE_METATYPE(Validity::Restriction)

ValidityDialog::ValidityDialog(QWidget *parent, CalculationSettings *settings, ValueParser *parser)
    : ActionDialog(parent, Default)
{
    m_settings = settings;
    m_parser = parser;
    setWindowTitle(i18n("Validity"));
    setButtonText(Default, i18n("Clear &All"));

    KPageWidget *main = new KPageWidget();
    setMainWidget(main);
    main->setFaceType(KPageWidget::List);

    QFrame *page1 = new QFrame();
    main->addPage(page1, i18n("Criteria"));

    QGridLayout *tmpGridLayout = new QGridLayout(page1);

    QLabel *tmpQLabel = new QLabel(page1);
    tmpQLabel->setText(i18n("Allow:"));
    tmpGridLayout->addWidget(tmpQLabel, 0, 0);

    chooseType = new KComboBox(page1);
    tmpGridLayout->addWidget(chooseType, 0, 1);
    chooseType->addItem(i18n("All"), QVariant::fromValue(Validity::NoRestriction));
    chooseType->addItem(i18n("Number"), QVariant::fromValue(Validity::Number));
    chooseType->addItem(i18n("Integer"), QVariant::fromValue(Validity::Integer));
    chooseType->addItem(i18n("Text"), QVariant::fromValue(Validity::Text));
    chooseType->addItem(i18n("Date"), QVariant::fromValue(Validity::Date));
    chooseType->addItem(i18n("Time"), QVariant::fromValue(Validity::Time));
    chooseType->addItem(i18n("Text Length"), QVariant::fromValue(Validity::TextLength));
    chooseType->addItem(i18n("List"), QVariant::fromValue(Validity::List));
    chooseType->setCurrentIndex(0);

    allowEmptyCell = new QCheckBox(i18n("Allow blanks"), page1);
    tmpGridLayout->addWidget(allowEmptyCell, 1, 0, 1, 2);

    chooseLabel = new QLabel(page1);
    chooseLabel->setText(i18n("Data:"));
    tmpGridLayout->addWidget(chooseLabel, 2, 0);

    choose = new KComboBox(page1);
    tmpGridLayout->addWidget(choose, 2, 1);
    choose->addItem(i18n("equal to"), QVariant::fromValue(Validity::Equal));
    choose->addItem(i18n("greater than"), QVariant::fromValue(Validity::Superior));
    choose->addItem(i18n("less than"), QVariant::fromValue(Validity::Inferior));
    choose->addItem(i18n("equal to or greater than"), QVariant::fromValue(Validity::SuperiorEqual));
    choose->addItem(i18n("equal to or less than"), QVariant::fromValue(Validity::InferiorEqual));
    choose->addItem(i18n("between"), QVariant::fromValue(Validity::Between));
    choose->addItem(i18n("different from"), QVariant::fromValue(Validity::Different));
    choose->addItem(i18n("different to"), QVariant::fromValue(Validity::DifferentTo));
    choose->setCurrentIndex(0);

    edit1 = new QLabel(page1);
    edit1->setText(i18n("Minimum:"));
    tmpGridLayout->addWidget(edit1, 3, 0);

    val_min = new KLineEdit(page1);
    tmpGridLayout->addWidget(val_min, 3, 1);
    val_min->setValidator(new QDoubleValidator(val_min));

    edit2 = new QLabel(page1);
    edit2->setText(i18n("Maximum:"));
    tmpGridLayout->addWidget(edit2, 4, 0);

    val_max = new KLineEdit(page1);
    tmpGridLayout->addWidget(val_max, 4, 1);
    val_max->setValidator(new QDoubleValidator(val_max));

    // Apply minimum width of column1 to avoid horizontal move when changing option
    // A bit ugly to apply text always, but I couldn't get a label->QFontMetrix.boundingRect("text").width()
    // to give mew the correct results - Philipp
    edit2->setText(i18n("Date:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Date minimum:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Date maximum:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Time:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Time minimum:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Time maximum:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Minimum:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Maximum:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);
    edit2->setText(i18n("Number:"));
    tmpGridLayout->addItem(new QSpacerItem(edit2->width(), 0), 0, 0);

    validityList = new KTextEdit(page1);
    tmpGridLayout->addWidget(validityList, 2, 1, 3, 1);

    validityLabelList = new QLabel(page1);
    validityLabelList->setText(i18n("Entries:"));
    tmpGridLayout->addWidget(validityLabelList, 2, 0, Qt::AlignTop);

    tmpGridLayout->setRowStretch(5, 1);

    QFrame *page2 = new QFrame();
    main->addPage(page2, i18n("Error Alert"));

    tmpGridLayout = new QGridLayout(page2);

    displayMessage = new QCheckBox(i18n("Show error message when invalid values are entered"), page2);
    displayMessage->setChecked(true);
    tmpGridLayout->addWidget(displayMessage, 0, 0, 1, 2);

    tmpQLabel = new QLabel(page2);
    tmpQLabel->setText(i18n("Action:"));
    tmpGridLayout->addWidget(tmpQLabel, 1, 0);

    chooseAction = new KComboBox(page2);
    tmpGridLayout->addWidget(chooseAction, 1, 1);
    chooseAction->addItem(i18n("Stop"), QVariant::fromValue(Validity::Stop));
    chooseAction->addItem(i18n("Warning"), QVariant::fromValue(Validity::Warning));
    chooseAction->addItem(i18n("Information"), QVariant::fromValue(Validity::Information));
    chooseAction->setCurrentIndex(0);

    tmpQLabel = new QLabel(page2);
    tmpQLabel->setText(i18nc("Title of message", "Title:"));
    tmpGridLayout->addWidget(tmpQLabel, 2, 0);

    title = new KLineEdit(page2);
    tmpGridLayout->addWidget(title, 2, 1);

    tmpQLabel = new QLabel(page2);
    tmpQLabel->setText(i18n("Message:"));
    tmpGridLayout->addWidget(tmpQLabel, 3, 0, Qt::AlignTop);

    message = new KTextEdit(page2);
    tmpGridLayout->addWidget(message, 3, 1);

    QFrame *page3 = new QFrame();
    main->addPage(page3, i18n("Input Help"));

    tmpGridLayout = new QGridLayout(page3);

    displayHelp = new QCheckBox(i18n("Show input help when cell is selected"), page3);
    displayMessage->setChecked(false);
    tmpGridLayout->addWidget(displayHelp, 0, 0, 1, 2);

    tmpQLabel = new QLabel(page3);
    tmpQLabel->setText(i18nc("Title of message", "Title:"));
    tmpGridLayout->addWidget(tmpQLabel, 1, 0);

    titleHelp = new KLineEdit(page3);
    tmpGridLayout->addWidget(titleHelp, 1, 1);

    tmpQLabel = new QLabel(page3);
    tmpQLabel->setText(i18n("Message:"));
    tmpGridLayout->addWidget(tmpQLabel, 2, 0, Qt::AlignTop);

    messageHelp = new KTextEdit(page3);
    tmpGridLayout->addWidget(messageHelp, 2, 1);

    connect(choose, QOverload<int>::of(&KComboBox::activated), this, &ValidityDialog::changeIndexCond);
    connect(chooseType, QOverload<int>::of(&KComboBox::activated), this, &ValidityDialog::changeIndexType);
    connect(this, &KoDialog::defaultClicked, this, &ValidityDialog::clearAllPressed);

    changeIndexType(chooseType->currentIndex());
    changeIndexCond(choose->currentIndex());
}

void ValidityDialog::displayOrNotListOfValidity(bool _displayList)
{
    if (_displayList) {
        validityList->show();
        validityLabelList->show();
        chooseLabel->hide();
        choose->hide();
        edit1->hide();
        val_min->hide();
        edit2->hide();
        val_max->hide();
        static_cast<QGridLayout *>(validityList->parentWidget()->layout())->setRowStretch(5, 0);
    } else {
        validityList->hide();
        validityLabelList->hide();
        chooseLabel->show();
        choose->show();
        edit1->show();
        val_min->show();
        edit2->show();
        val_max->show();
        static_cast<QGridLayout *>(validityList->parentWidget()->layout())->setRowStretch(5, 1);
    }
}

void ValidityDialog::changeIndexType(int _index)
{
    bool activate = (_index != 0);
    allowEmptyCell->setEnabled(activate);
    message->setEnabled(activate);
    title->setEnabled(activate);
    chooseAction->setEnabled(activate);
    displayMessage->setEnabled(activate);
    displayHelp->setEnabled(activate);
    messageHelp->setEnabled(activate);
    titleHelp->setEnabled(activate);
    if (_index == 7)
        displayOrNotListOfValidity(true);
    else
        displayOrNotListOfValidity(false);

    switch (_index) {
    case 0:
        edit1->setText("");
        edit2->setText("");
        val_max->setEnabled(false);
        val_min->setEnabled(false);
        choose->setEnabled(false);
        break;
    case 1:
        val_min->setEnabled(true);
        choose->setEnabled(true);
        val_min->setValidator(new QDoubleValidator(val_min));
        val_max->setValidator(new QDoubleValidator(val_max));
        if (choose->currentIndex() <= 4) {
            edit1->setText(i18n("Number:"));
            edit2->setText("");
            val_max->setEnabled(false);
        } else {
            edit1->setText(i18n("Minimum:"));
            edit2->setText(i18n("Maximum:"));
            val_max->setEnabled(true);
        }
        break;
    case 2:
    case 6:
        val_min->setEnabled(true);
        choose->setEnabled(true);
        val_min->setValidator(new QIntValidator(val_min));
        val_max->setValidator(new QIntValidator(val_max));
        if (choose->currentIndex() <= 4) {
            edit1->setText(i18n("Number:"));
            edit2->setText("");
            val_max->setEnabled(false);
        } else {
            edit1->setText(i18n("Minimum:"));
            edit2->setText(i18n("Maximum:"));
            val_max->setEnabled(true);
        }
        break;

    case 3:
        edit1->setText("");
        edit2->setText("");
        val_max->setEnabled(false);
        val_min->setEnabled(false);
        choose->setEnabled(false);
        break;
    case 4:
        edit1->setText(i18n("Date:"));
        edit2->setText("");
        val_min->setEnabled(true);
        choose->setEnabled(true);

        val_min->setValidator(nullptr);
        val_max->setValidator(nullptr);
        if (choose->currentIndex() <= 4) {
            edit1->setText(i18n("Date:"));
            edit2->setText("");
            val_max->setEnabled(false);
        } else {
            edit1->setText(i18n("Date minimum:"));
            edit2->setText(i18n("Date maximum:"));
            val_max->setEnabled(true);
        }
        break;
    case 5:
        val_min->setEnabled(true);
        choose->setEnabled(true);
        val_min->setValidator(nullptr);
        val_max->setValidator(nullptr);
        if (choose->currentIndex() <= 4) {
            edit1->setText(i18n("Time:"));
            edit2->setText("");
            val_max->setEnabled(false);
        } else {
            edit1->setText(i18n("Time minimum:"));
            edit2->setText(i18n("Time maximum:"));
            val_max->setEnabled(true);
        }
        break;
    }
    if (width() < sizeHint().width())
        resize(sizeHint());
}

void ValidityDialog::changeIndexCond(int _index)
{
    switch (_index) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        val_max->setEnabled(false);
        if (chooseType->currentIndex() == 1 || chooseType->currentIndex() == 2 || chooseType->currentIndex() == 6)
            edit1->setText(i18n("Number:"));
        else if (chooseType->currentIndex() == 3)
            edit1->setText("");
        else if (chooseType->currentIndex() == 4)
            edit1->setText(i18n("Date:"));
        else if (chooseType->currentIndex() == 5)
            edit1->setText(i18n("Time:"));
        edit2->setText("");
        break;
    case 5:
    case 6:
        val_max->setEnabled(true);
        if (chooseType->currentIndex() == 1 || chooseType->currentIndex() == 2 || chooseType->currentIndex() == 6) {
            edit1->setText(i18n("Minimum:"));
            edit2->setText(i18n("Maximum:"));
        } else if (chooseType->currentIndex() == 3) {
            edit1->setText("");
            edit2->setText("");
        } else if (chooseType->currentIndex() == 4) {
            edit1->setText(i18n("Date minimum:"));
            edit2->setText(i18n("Date maximum:"));
        } else if (chooseType->currentIndex() == 5) {
            edit1->setText(i18n("Time minimum:"));
            edit2->setText(i18n("Time maximum:"));
        }
        break;
    }
}

void ValidityDialog::setValidity(Validity validity)
{
    const Localization *locale = m_settings->locale();

    message->setPlainText(validity.message());
    title->setText(validity.title());
    QString tmp;
    switch (validity.restriction()) {
    case Validity::NoRestriction:
        chooseType->setCurrentIndex(0);
        break;
    case Validity::Number:
        chooseType->setCurrentIndex(1);
        if (validity.condition() >= 5)
            val_max->setText(tmp.setNum((double)numToDouble(validity.maximumValue().asFloat())));
        val_min->setText(tmp.setNum((double)numToDouble(validity.minimumValue().asFloat())));
        break;
    case Validity::Integer:
        chooseType->setCurrentIndex(2);
        if (validity.condition() >= 5)
            val_max->setText(tmp.setNum((double)numToDouble(validity.maximumValue().asFloat())));
        val_min->setText(tmp.setNum((double)numToDouble(validity.minimumValue().asFloat())));
        break;
    case Validity::TextLength:
        chooseType->setCurrentIndex(6);
        if (validity.condition() >= 5)
            val_max->setText(tmp.setNum((double)numToDouble(validity.maximumValue().asFloat())));
        val_min->setText(tmp.setNum((double)numToDouble(validity.minimumValue().asFloat())));
        break;
    case Validity::Text:
        chooseType->setCurrentIndex(3);
        break;
    case Validity::Date:
        chooseType->setCurrentIndex(4);
        val_min->setText(locale->formatDate(validity.minimumValue().asDate(m_settings), false));
        if (validity.condition() >= 5)
            val_max->setText(locale->formatDate(validity.maximumValue().asDate(m_settings), false));
        break;
    case Validity::Time:
        chooseType->setCurrentIndex(5);
        val_min->setText(locale->formatTime(validity.minimumValue().asTime(), true));
        if (validity.condition() >= 5)
            val_max->setText(locale->formatTime(validity.maximumValue().asTime(), true));
        break;
    case Validity::List: {
        chooseType->setCurrentIndex(7);
        const QStringList lst = validity.validityList();
        QString tmp;
        for (QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it) {
            tmp += (*it) + '\n';
        }
        validityList->setText(tmp);
    } break;
    default:
        chooseType->setCurrentIndex(0);
        break;
    }
    chooseAction->setCurrentIndex(chooseAction->findData(QVariant::fromValue(validity.action())));
    choose->setCurrentIndex(choose->findData(QVariant::fromValue(validity.condition())));
    displayMessage->setChecked(validity.displayMessage());
    allowEmptyCell->setChecked(validity.allowEmptyCell());
    titleHelp->setText(validity.titleInfo());
    messageHelp->setPlainText(validity.messageInfo());
    displayHelp->setChecked(validity.displayValidationInformation());

    changeIndexType(chooseType->currentIndex());
    changeIndexCond(choose->currentIndex());
}

void ValidityDialog::clearAllPressed()
{
    val_min->setText("");
    val_max->setText("");
    message->setPlainText("");
    title->setText("");
    displayMessage->setChecked(true);
    allowEmptyCell->setChecked(false);
    choose->setCurrentIndex(0);
    chooseType->setCurrentIndex(0);
    chooseAction->setCurrentIndex(0);
    changeIndexType(0);
    changeIndexCond(0);
    messageHelp->setPlainText("");
    titleHelp->setText("");
    validityList->setText("");
    displayHelp->setChecked(false);
}

void ValidityDialog::onApply()
{
    // Validate the data.
    int idx = chooseType->currentIndex();
    bool ok = false;
    if (idx == 1) {
        minval = val_min->text().toDouble(&ok);
        if (!ok) {
            KMessageBox::error(this, i18n("This is not a valid value."), i18n("Error"));
            return;
        }
        maxval = val_max->text().toDouble(&ok);
        if (!ok && choose->currentIndex() >= 5 && choose->currentIndex() < 7) {
            KMessageBox::error(this, i18n("This is not a valid value."), i18n("Error"));
            return;
        }
    } else if (idx == 2 || idx == 6) {
        minval = val_min->text().toInt(&ok);
        if (!ok) {
            KMessageBox::error(this, i18n("This is not a valid value."), i18n("Error"));
            return;
        }
        maxval = val_max->text().toInt(&ok);
        if (!ok && choose->currentIndex() >= 5 && choose->currentIndex() < 7) {
            KMessageBox::error(this, i18n("This is not a valid value."), i18n("Error"));
            return;
        }
    } else if (idx == 5) {
        mintime = m_parser->tryParseTime(val_min->text(), &ok).asTime().toQTime(); // FIXME Time or QTime?
        if (!ok) {
            KMessageBox::error(this, i18n("This is not a valid time."), i18n("Error"));
            return;
        }
        maxtime = m_parser->tryParseTime(val_max->text(), &ok).asTime().toQTime(); // FIXME Time or QTime?
        if ((!ok) && choose->currentIndex() >= 5) {
            KMessageBox::error(this, i18n("This is not a valid time."), i18n("Error"));
            return;
        }
    } else if (idx == 4) {
        mindate = m_parser->tryParseDate(val_min->text(), &ok).asDate(m_settings);
        if (!ok) {
            KMessageBox::error(this, i18n("This is not a valid date."), i18n("Error"));
            return;
        }
        maxdate = m_parser->tryParseDate(val_max->text(), &ok).asDate(m_settings);
        if ((!ok) && choose->currentIndex() >= 5) {
            KMessageBox::error(this, i18n("This is not a valid date."), i18n("Error"));
            return;
        }
    } else if (idx == 7) {
        // Nothing
    }

    Validity validity = getValidity();
    Q_EMIT applyValidity(validity);
}

Validity ValidityDialog::getValidity()
{
    Validity validity;

    int idx = chooseType->currentIndex();
    if (idx == 0) { // no validity
        validity.setRestriction(Validity::NoRestriction);
        validity.setAction(Validity::Stop);
        validity.setCondition(Validity::Equal);
        validity.setMessage(message->toPlainText());
        validity.setTitle(title->text());
        validity.setMinimumValue(Value());
        validity.setMaximumValue(Value());
    } else {
        validity.setRestriction(chooseType->itemData(chooseType->currentIndex()).value<Validity::Restriction>());
        validity.setAction(chooseAction->itemData(chooseAction->currentIndex()).value<Validity::Action>());
        validity.setCondition(choose->itemData(choose->currentIndex()).value<Validity::Type>());
        validity.setMessage(message->toPlainText());
        validity.setTitle(title->text());
        validity.setMinimumValue(Value());
        validity.setMaximumValue(Value());

        if (idx == 1) {
            if (choose->currentIndex() < 5) {
                validity.setMinimumValue(Value(minval));
            } else {
                validity.setMinimumValue(Value(qMin(minval, maxval)));
                validity.setMaximumValue(Value(qMax(minval, maxval)));
            }
        } else if (idx == 2 || idx == 6) {
            if (choose->currentIndex() < 5) {
                validity.setMinimumValue(Value((int)minval));
            } else {
                validity.setMinimumValue(Value(qMin((int)minval, (int)maxval)));
                validity.setMaximumValue(Value(qMax((int)minval, (int)maxval)));
            }
        } else if (idx == 4) {
            Value minDate = Value(mindate, m_settings);
            Value maxDate = Value(maxdate, m_settings);
            if (choose->currentIndex() < 5) {
                validity.setMinimumValue(minDate);
            } else {
                if (minDate.less(maxDate)) {
                    validity.setMinimumValue(minDate);
                    validity.setMaximumValue(maxDate);
                } else {
                    validity.setMinimumValue(maxDate);
                    validity.setMaximumValue(minDate);
                }
            }
        } else if (idx == 5) {
            Value minTime = Value(Time(mintime));
            Value maxTime = Value(Time(maxtime));
            if (choose->currentIndex() < 5) {
                validity.setMinimumValue(minTime);
            } else {
                if (minTime.less(maxTime)) {
                    validity.setMaximumValue(maxTime);
                    validity.setMinimumValue(minTime);
                } else {
                    validity.setMaximumValue(minTime);
                    validity.setMinimumValue(maxTime);
                }
            }
        } else if (idx == 7) {
            validity.setValidityList(validityList->toPlainText().split('\n', Qt::SkipEmptyParts));
        }
    }
    validity.setDisplayMessage(displayMessage->isChecked());
    validity.setAllowEmptyCell(allowEmptyCell->isChecked());
    validity.setDisplayValidationInformation(displayHelp->isChecked());
    validity.setMessageInfo(messageHelp->toPlainText());
    validity.setTitleInfo(titleHelp->text());

    return validity;
}
