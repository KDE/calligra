/* This file is part of the KDE project
   Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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

#include "ConfigWorkVacationPanel.h"

#include "calligraplansettings.h"


#include <kactioncollection.h>

#ifdef HAVE_KHOLIDAYS
#include <KHolidays/HolidayRegion>
#endif

#include <QFileDialog>

namespace KPlato
{

ConfigWorkVacationPanel::ConfigWorkVacationPanel( QWidget *parent )
    : ConfigWorkVacationPanelImpl( parent )
{
}

//-----------------------------
ConfigWorkVacationPanelImpl::ConfigWorkVacationPanelImpl(QWidget *p )
    : QWidget(p)
{
    setupUi(this);
    kcfg_Region->hide();
#ifdef HAVE_KHOLIDAYS
    int idx = 0;
    const QString regionCode = kcfg_Region->text();

    region->addItem(i18n("Default"), "Default");
    for (const QString &s : KHolidays::HolidayRegion::regionCodes()) {
        region->addItem(KHolidays::HolidayRegion::name(s), s);
        int row = region->count() - 1;
        region->setItemData(row, KHolidays::HolidayRegion::description(s), Qt::ToolTipRole);
        if (s == regionCode) {
            idx = row;
        }
    }
    connect(region, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRegionChanged(int)));
    connect(kcfg_Region, SIGNAL(textChanged(const QString&)), this, SLOT(slotRegionCodeChanged(const QString&)));
    region->setCurrentIndex(idx);
#else
    holidaysWidget->hide();
#endif
}

#ifdef HAVE_KHOLIDAYS
void ConfigWorkVacationPanelImpl::slotRegionChanged(int idx)
{
    QString r = region->itemData(idx).toString();
    if (r != kcfg_Region->text()) {
        kcfg_Region->setText(r);
    }
}

void ConfigWorkVacationPanelImpl::slotRegionCodeChanged(const QString &code)
{
    QString r = region->itemData(region->currentIndex()).toString();
    if (r != code) {
        for (int idx = 0; idx < region->count(); ++idx) {
            if (region->itemData(idx).toString() == code) {
                region->setCurrentIndex(idx);
                break;
            }
        }
    }
}
#endif

}  //KPlato namespace
