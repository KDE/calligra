/* This file is part of the KDE project
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2000-2005 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2001-2002 David Faure <faure@kde.org>
             (C) 2001 Werner Trobin <trobin@kde.org>
             (C) 2000 Bernd Johannes Wuebben <wuebben@kde.org>

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

// Local
#include "DocumentSettingsDialog.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QComboBox>
#include <QSpinBox>

#include <KoVBox.h>

#include <sonnet/configwidget.h>

#include <KoIcon.h>
#include "CalculationSettings.h"
#include "Localization.h"
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

class DocumentSettingsDialog::Private
{
public:
    KPageWidgetItem *page1, *page2;
    // Calculation Settings
    calcSettings* calcPage;
    // Locale Options
    parameterLocale* localePage;
};


DocumentSettingsDialog::DocumentSettingsDialog(Selection* selection, QWidget* parent)
        : KPageDialog(parent)
        , d(new Private)
{
    setObjectName(QLatin1String("DocumentSettingsDialog"));
    setWindowTitle(i18n("Document Settings"));
//     setFaceType(List);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel/* | QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Reset*/);
    button(QDialogButtonBox::Ok)->setDefault(true);

    connect(this, SIGNAL(accepted()), this, SLOT(slotApply()));
//     connect(button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked(bool)), this, SLOT(slotDefault()));
//     connect(button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(slotReset()));

    KoVBox *p1 = new KoVBox();
    d->page1 = addPage(p1, i18n("Calculation"));
    d->page1->setHeader(QString(""));
    d->page1->setIcon(koIcon("application-vnd.oasis.opendocument.spreadsheet"));
    d->calcPage = new calcSettings(selection, p1);

    KoVBox *p2 = new KoVBox();
    d->page2 = addPage(p2, i18n("Locale"));
    d->page2->setHeader(QString(""));
    d->page2->setIcon(koIcon("preferences-desktop-locale"));
    d->localePage = new parameterLocale(selection, p2);
}

DocumentSettingsDialog::~DocumentSettingsDialog()
{
    delete d;
}

void DocumentSettingsDialog::slotApply()
{
    d->calcPage->apply();
    d->localePage->apply();
}

void DocumentSettingsDialog::slotDefault()
{
}

void DocumentSettingsDialog::slotReset()
{
}


calcSettings::calcSettings(Selection* selection, KoVBox *box)
        : QObject(box->parent())
{
    m_cs = selection->activeSheet()->map()->calculationSettings();

    m_caseSensitiveCheckbox = new QCheckBox(i18n("Case sensitive"), box);
    m_caseSensitiveCheckbox->setChecked(m_cs->caseSensitiveComparisons() == Qt::CaseSensitive);

    m_precisionAsShownCheckbox = new QCheckBox(i18n("Precision as shown"), box);
    m_precisionAsShownCheckbox->setChecked(m_cs->precisionAsShown());

    m_searchCriteriaMustApplyToWholeCellCheckbox = new QCheckBox(i18n("Search criteria must apply to whole cell"), box);
    m_searchCriteriaMustApplyToWholeCellCheckbox->setChecked(m_cs->wholeCellSearchCriteria());

    m_automaticFindLabelsCheckbox = new QCheckBox(i18n("Automatic find labels"), box);
    m_automaticFindLabelsCheckbox->setChecked(m_cs->automaticFindLabels());

    QHBoxLayout *matchModeLayout = new QHBoxLayout();
    matchModeLayout->setMargin(0);
    box->layout()->addItem(matchModeLayout);
    QLabel *matchModeLabel = new QLabel(i18n("String comparison:"), box);
    matchModeLayout->addWidget(matchModeLabel);
    m_matchModeCombobox = new QComboBox(box);
    matchModeLayout->addWidget(m_matchModeCombobox);
    matchModeLabel->setBuddy(m_matchModeCombobox);
    m_matchModeCombobox->setEditable(false);
    m_matchModeCombobox->addItems(QStringList() << i18n("None") << i18n("Wildcards") << i18n("Regular Expressions"));
    m_matchModeCombobox->setCurrentIndex(m_cs->useWildcards() ? 1 : m_cs->useRegularExpressions() ? 2 : 0 );

    QHBoxLayout *m_nullYearLayout = new QHBoxLayout();
    m_nullYearLayout->setMargin(0);
    box->layout()->addItem(m_nullYearLayout);
    QLabel *m_nullYearLabel = new QLabel(i18n("Null Year:"), box);
    m_nullYearLayout->addWidget(m_nullYearLabel);
    m_nullYearEdit = new QSpinBox(box);
    m_nullYearLayout->addWidget(m_nullYearEdit);
    m_nullYearLabel->setBuddy(m_nullYearEdit);
    m_nullYearEdit->setRange(0, 32767);
    m_nullYearEdit->setValue(m_cs->referenceYear());

    box->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void calcSettings::apply()
{
    m_cs->setCaseSensitiveComparisons(m_caseSensitiveCheckbox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
    m_cs->setPrecisionAsShown(m_precisionAsShownCheckbox->isChecked());
    m_cs->setWholeCellSearchCriteria(m_searchCriteriaMustApplyToWholeCellCheckbox->isChecked());
    m_cs->setAutomaticFindLabels(m_automaticFindLabelsCheckbox->isChecked());
    m_cs->setUseWildcards(m_matchModeCombobox->currentIndex() == 1);
    m_cs->setUseRegularExpressions(m_matchModeCombobox->currentIndex() == 2);
    m_cs->setReferenceYear(m_nullYearEdit->value());
}

parameterLocale::parameterLocale(Selection* selection, KoVBox *box)
        : QObject(box->parent())
{
    m_selection = selection;
    m_bUpdateLocale = false;

    KLocale* locale = selection->activeSheet()->map()->calculationSettings()->locale();

    m_language = new QLabel(box);
    m_number = new QLabel(box);
    m_date = new QLabel(box);
    m_shortDate = new QLabel(box);
    m_time = new QLabel(box);
    m_money = new QLabel(box);

    updateToMatchLocale(locale);

    m_updateButton = new QPushButton(i18n("&Use System's Locale Settings"), box);
    connect(m_updateButton, SIGNAL(clicked()), this, SLOT(updateDefaultSystemConfig()));

    box->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void parameterLocale::apply()
{
    if (m_bUpdateLocale) {
        const QList<Sheet*> sheets = m_selection->activeSheet()->map()->sheetList();
        foreach(Sheet* sheet, sheets) {
            sheet->updateLocale();
        }
    }
}

void parameterLocale::updateDefaultSystemConfig()
{
    m_bUpdateLocale = true;
    KLocale* const locale = m_selection->activeSheet()->map()->calculationSettings()->locale();
    static_cast<Localization*>(locale)->defaultSystemConfig();
    updateToMatchLocale(locale);
}

void parameterLocale::updateToMatchLocale(KLocale* locale)
{
    m_language->setText(i18n("Language: %1", locale->language()));
    m_number->setText(i18n("Default number format: %1", locale->formatNumber(12.55)));   // krazy:exclude=i18ncheckarg
    m_date->setText(i18n("Long date format: %1", locale->formatDate(QDate::currentDate())));
    m_shortDate->setText(i18n("Short date format: %1", locale->formatDate(QDate::currentDate() , KLocale::ShortDate)));
    m_time->setText(i18n("Time format: %1", locale->formatTime(QTime::currentTime())));
    m_money->setText(i18n("Currency format: %1", locale->formatMoney(12.55)));
}
