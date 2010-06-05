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

#include <kcombobox.h>
#include <kconfig.h>
#include <kicon.h>
#include <kstatusbar.h>
#include <knuminput.h>
#include <kmessagebox.h>

#include <ksharedconfig.h>
#include <KStandardDirs>
#include <sonnet/configwidget.h>

#include "CalculationSettings.h"
#include "Localization.h"
#include "Map.h"
#include "Selection.h"
#include "Sheet.h"

using namespace KSpread;

class DocumentSettingsDialog::Private
{
public:
    KPageWidgetItem* page1;

    // Locale Options
    parameterLocale* localePage;
};


DocumentSettingsDialog::DocumentSettingsDialog(Selection* selection, QWidget* parent)
        : KPageDialog(parent)
        , d(new Private)
{
    setObjectName("DocumentSettingsDialog");
    setCaption(i18n("Document Settings"));
//     setFaceType(List);
    setButtons(Ok | Cancel/* | Default | Reset*/);
    setDefaultButton(Ok);

    connect(this, SIGNAL(okClicked()), this, SLOT(slotApply()));
//     connect(this, SIGNAL(defaultClicked()), this,SLOT(slotDefault()));
//     connect(this, SIGNAL(resetClicked()), this, SLOT(slotReset()));

    KVBox *page2 = new KVBox();
    d->page1 = addPage(page2, i18n("Locale Settings"));
    d->page1->setHeader(i18n("Document's Locale Settings"));
    d->page1->setIcon(KIcon("preferences-desktop-locale"));
    d->localePage = new parameterLocale(selection, page2);
}

DocumentSettingsDialog::~DocumentSettingsDialog()
{
    delete d;
}

void DocumentSettingsDialog::slotApply()
{
    d->localePage->apply();
}

void DocumentSettingsDialog::slotDefault()
{
}

void DocumentSettingsDialog::slotReset()
{
}


parameterLocale::parameterLocale(Selection* selection, KVBox *box , char * /*name*/)
        : QObject(box->parent())
{
    m_selection = selection;
    m_bUpdateLocale = false;
//   QGroupBox* tmpQGroupBox = new QGroupBox( i18n("Settings"), box );
    KVBox* tmpQGroupBox = box;

    KLocale* locale = selection->activeSheet()->map()->calculationSettings()->locale();

    m_language = new QLabel(tmpQGroupBox);
    m_number = new QLabel(tmpQGroupBox);
    m_date = new QLabel(tmpQGroupBox);
    m_shortDate = new QLabel(tmpQGroupBox);
    m_time = new QLabel(tmpQGroupBox);
    m_money = new QLabel(tmpQGroupBox);

    updateToMatchLocale(locale);

    m_updateButton = new QPushButton(i18n("&Use System's Locale Settings"), tmpQGroupBox);
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

#include "DocumentSettingsDialog.moc"
