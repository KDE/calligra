/*
 * localemon.cpp
 *
 * Copyright (c) 1999-2003 Hans Petter Bieker <bieker@kde.org>
 * Copyright (c) 2009, 2012 Dag Andersen <danders@get2net.dk>
 * Copyright (C) 2016 Dag Andersen <danders@get2net.dk>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

//#include "toplevel.h"
#include "localemon.h"

#include "kptlocale.h"
#include "kptcommand.h"
#include "kptdebug.h"

#include <QCheckBox>
#include <QComboBox>
#include <QStandardPaths>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <KSharedConfig>

namespace KPlato {

LocaleConfigMoney::LocaleConfigMoney(Locale *locale,
                                       QWidget *parent)
  : QWidget(parent),
    m_locale(locale)
{
  setupUi(this);

  // Money
  m_labMonCurSym->setObjectName( I18N_NOOP("Currency symbol:") );
  m_labMonFraDig->setObjectName( I18N_NOOP("Fract digits:") );

  connect(m_edMonCurSym,SIGNAL(textChanged(QString)),SLOT(slotMonCurSymChanged(QString)));

  connect(m_inMonFraDig,SIGNAL(valueChanged(int)),SLOT(slotMonFraDigChanged(int)));

  m_inMonFraDig->setRange(0, 10);
  m_inMonFraDig->setSingleStep(1);

  slotTranslate();
  slotLocaleChanged();
}

LocaleConfigMoney::~LocaleConfigMoney()
{
}

void LocaleConfigMoney::slotLocaleChanged()
{
  m_edMonCurSym->setText( m_locale->currencySymbolExplicit());
  m_inMonFraDig->setValue(m_locale->monetaryDecimalPlaces());
}

void LocaleConfigMoney::slotMonCurSymChanged(const QString &/*t*/)
{
  emit localeChanged();
}

void LocaleConfigMoney::slotMonFraDigChanged(int /*value*/)
{
  emit localeChanged();
}

void LocaleConfigMoney::slotMonPosPreCurSymChanged()
{
  emit localeChanged();
}

void LocaleConfigMoney::slotMonNegPreCurSymChanged()
{
  emit localeChanged();
}

void LocaleConfigMoney::slotMonPosMonSignPosChanged(int /*i*/)
{
  emit localeChanged();
}

void LocaleConfigMoney::slotMonNegMonSignPosChanged(int /*i*/)
{
  emit localeChanged();
}


void LocaleConfigMoney::slotTranslate()
{
  QString str;

  str = i18n( "Here you can enter your usual currency "
               "symbol, e.g. $ or €." );
  m_labMonCurSym->setWhatsThis( str );
  m_edMonCurSym->setWhatsThis( str );
}

MacroCommand *LocaleConfigMoney::buildCommand()
{
    MacroCommand *m = new MacroCommand();
    if ( m_locale->currencySymbolExplicit() != m_edMonCurSym->text() ) {
        m->addCommand( new ModifyCurrencySymolCmd( m_locale, m_edMonCurSym->text() ) );
    }
    if (m_locale->monetaryDecimalPlaces() != m_inMonFraDig->value()) {
        m->addCommand( new ModifyCurrencyFractionalDigitsCmd( m_locale, m_inMonFraDig->value() ) );
    }
    debugPlan<<"buildCommand:"<<m->isEmpty();
    if ( m->isEmpty() ) {
        delete m;
        return 0;
    }
    return m;
}

} // namespace KPlato
