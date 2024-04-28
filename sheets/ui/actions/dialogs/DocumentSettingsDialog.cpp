/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2000-2005 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2001-2002 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000 Bernd Johannes Wuebben <wuebben@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "DocumentSettingsDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>

#include <KPageWidget>

#include <KoIcon.h>
#include <KoVBox.h>

// #include <sonnet/configwidget.h>

#include "core/Sheet.h"
#include "engine/CS_Time.h"
#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

class DocumentSettingsDialog::Private
{
public:
    KPageWidget *pages;
    KPageWidgetItem *page1, *page2;
    // Calculation Settings
    calcSettings *calcPage;
    // Locale Options
    parameterLocale *localePage;
};

DocumentSettingsDialog::DocumentSettingsDialog(MapBase *map, QWidget *parent)
    : ActionDialog(parent)
    , d(new Private)
{
    setObjectName(QLatin1String("DocumentSettingsDialog"));
    setWindowTitle(i18n("Document Settings"));

    d->pages = new KPageWidget();
    setMainWidget(d->pages);
    d->pages->setFaceType(KPageWidget::List);

    KoVBox *p1 = new KoVBox();
    d->page1 = d->pages->addPage(p1, i18n("Calculation"));
    d->page1->setHeader(QString(""));
    d->page1->setIcon(koIcon("application-vnd.oasis.opendocument.spreadsheet"));
    d->calcPage = new calcSettings(map, p1);

    KoVBox *p2 = new KoVBox();
    d->page2 = d->pages->addPage(p2, i18n("Locale"));
    d->page2->setHeader(QString(""));
    d->page2->setIcon(koIcon("preferences-desktop-locale"));
    d->localePage = new parameterLocale(map, p2);
}

DocumentSettingsDialog::~DocumentSettingsDialog()
{
    delete d;
}

void DocumentSettingsDialog::onApply()
{
    d->calcPage->apply();
    d->localePage->apply();
}

calcSettings::calcSettings(MapBase *map, KoVBox *box)
    : QObject(box->parent())
{
    m_cs = map->calculationSettings();

    m_caseSensitiveCheckbox = new QCheckBox(i18n("Case sensitive"), box);
    m_caseSensitiveCheckbox->setChecked(m_cs->caseSensitiveComparisons() == Qt::CaseSensitive);

    m_precisionAsShownCheckbox = new QCheckBox(i18n("Precision as shown"), box);
    m_precisionAsShownCheckbox->setChecked(m_cs->precisionAsShown());

    m_searchCriteriaMustApplyToWholeCellCheckbox = new QCheckBox(i18n("Search criteria must apply to whole cell"), box);
    m_searchCriteriaMustApplyToWholeCellCheckbox->setChecked(m_cs->wholeCellSearchCriteria());

    m_automaticFindLabelsCheckbox = new QCheckBox(i18n("Automatic find labels"), box);
    m_automaticFindLabelsCheckbox->setChecked(m_cs->automaticFindLabels());

    QHBoxLayout *matchModeLayout = new QHBoxLayout();
    matchModeLayout->setContentsMargins({});
    box->layout()->addItem(matchModeLayout);
    QLabel *matchModeLabel = new QLabel(i18n("String comparison:"), box);
    matchModeLayout->addWidget(matchModeLabel);
    m_matchModeCombobox = new QComboBox(box);
    matchModeLayout->addWidget(m_matchModeCombobox);
    matchModeLabel->setBuddy(m_matchModeCombobox);
    m_matchModeCombobox->setEditable(false);
    m_matchModeCombobox->addItems(QStringList() << i18n("None") << i18n("Wildcards") << i18n("Regular Expressions"));
    m_matchModeCombobox->setCurrentIndex(m_cs->useWildcards() ? 1 : m_cs->useRegularExpressions() ? 2 : 0);

    QHBoxLayout *m_nullYearLayout = new QHBoxLayout();
    m_nullYearLayout->setContentsMargins({});
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

parameterLocale::parameterLocale(MapBase *map, KoVBox *box)
    : QObject(box->parent())
{
    m_map = map;
    m_bUpdateLocale = false;

    Localization *locale = map->calculationSettings()->locale();

    m_language = new QLabel(box);
    m_number = new QLabel(box);
    m_date = new QLabel(box);
    m_shortDate = new QLabel(box);
    m_time = new QLabel(box);
    m_money = new QLabel(box);

    updateToMatchLocale(locale);

    m_updateButton = new QPushButton(i18n("&Use System's Locale Settings"), box);
    connect(m_updateButton, &QAbstractButton::clicked, this, &parameterLocale::updateDefaultSystemConfig);

    box->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void parameterLocale::apply()
{
    if (m_bUpdateLocale) {
        const QList<SheetBase *> sheets = m_map->sheetList();
        for (SheetBase *sheet : sheets) {
            Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
            if (fullSheet)
                fullSheet->updateLocale();
        }
    }
}

void parameterLocale::updateDefaultSystemConfig()
{
    m_bUpdateLocale = true;
    Localization *const locale = m_map->calculationSettings()->locale();
    static_cast<Localization *>(locale)->setDefaultLocale();
    updateToMatchLocale(locale);
}

void parameterLocale::updateToMatchLocale(Localization *locale)
{
    m_language->setText(i18n("Language: %1 (%2)", locale->languageName(true), locale->languageName(false)));
    m_number->setText(i18n("Default number format: %1", locale->formatNumber(12.55))); // krazy:exclude=i18ncheckarg
    m_date->setText(i18n("Long date format: %1", locale->formatDate(QDate::currentDate())));
    m_shortDate->setText(i18n("Short date format: %1", locale->formatDate(QDate::currentDate(), false)));
    m_time->setText(i18n("Time format: %1", locale->formatTime(Time::currentTime())));
    m_money->setText(i18n("Currency format: %1", locale->formatCurrency(12.55, locale->currencySymbol())));
}
