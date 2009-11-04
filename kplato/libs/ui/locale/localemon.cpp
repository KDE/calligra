/*
 * localemon.cpp
 *
 * Copyright (c) 1999-2003 Hans Petter Bieker <bieker@kde.org>
 * Copyright (c) 2009 Dag Andersen <danders@get2net.dk>
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

#include "kptcommand.h"

#include <kdebug.h>
#include <QCheckBox>
#include <QComboBox>



#include <KConfig>
#include <KConfigGroup>
#include <KStandardDirs>


namespace KPlato {

LocaleConfigMoney::LocaleConfigMoney(KLocale *locale,
                                       QWidget *parent)
  : QWidget(parent),
    m_locale(locale)
{
  setupUi(this);

  // Money
  m_labMonCurSym->setObjectName( I18N_NOOP("Currency symbol:") );
/*  m_labMonDecSym->setObjectName( I18N_NOOP("Decimal symbol:") );
  m_labMonThoSep->setObjectName( I18N_NOOP("Thousands separator:") );*/
  m_labMonFraDig->setObjectName( I18N_NOOP("Fract digits:") );
  m_positiveGB->setObjectName( I18N_NOOP("Positive") );
  m_chMonPosPreCurSym->setObjectName(I18N_NOOP("Prefix currency symbol"));
  m_labMonPosMonSignPos->setObjectName( I18N_NOOP("Sign position:") );
  m_negativeGB->setObjectName( I18N_NOOP("Negative") );
  m_chMonNegPreCurSym->setObjectName(I18N_NOOP("Prefix currency symbol"));
  m_labMonNegMonSignPos->setObjectName( I18N_NOOP("Sign position:") );
//  m_labMonDigSet->setObjectName( I18N_NOOP("Digit set:") );

  connect( m_edMonCurSym, SIGNAL( textChanged(const QString &) ),
           SLOT( slotMonCurSymChanged(const QString &) ) );

//   connect( m_edMonDecSym, SIGNAL( textChanged(const QString &) ),
//            SLOT( slotMonDecSymChanged(const QString &) ) );
// 
//   connect( m_edMonThoSep, SIGNAL( textChanged(const QString &) ),
//            SLOT( slotMonThoSepChanged(const QString &) ) );
// 
   connect( m_inMonFraDig, SIGNAL( valueChanged(int) ),
            SLOT( slotMonFraDigChanged(int) ) );

  connect( m_chMonPosPreCurSym, SIGNAL( clicked() ),
           SLOT( slotMonPosPreCurSymChanged() ) );

  connect( m_cmbMonPosMonSignPos, SIGNAL( activated(int) ),
           SLOT( slotMonPosMonSignPosChanged(int) ) );

  connect( m_chMonNegPreCurSym, SIGNAL( clicked() ),
           SLOT( slotMonNegPreCurSymChanged() ) );

  connect( m_cmbMonNegMonSignPos, SIGNAL( activated(int) ),
           SLOT( slotMonNegMonSignPosChanged(int) ) );

//   connect( m_cmbMonDigSet, SIGNAL( activated(int) ),
//            SLOT( slotMonDigSetChanged(int) ) );

  m_inMonFraDig->setRange(0, 10, 1);
  m_inMonFraDig->setSliderEnabled(false);
  
  slotTranslate();
  slotLocaleChanged();
}

LocaleConfigMoney::~LocaleConfigMoney()
{
}

void LocaleConfigMoney::save()
{
  KSharedConfig::Ptr config = KGlobal::config();
  KConfigGroup group(config, "Locale");
  KConfig ent(KStandardDirs::locate("locale",
                           QString::fromLatin1("l10n/%1/entry.desktop")
                           .arg(m_locale->country())));
  KConfigGroup entGrp = ent.group("KCM Locale");

  QString str;
  int i;
  bool b;

  str = entGrp.readEntry("CurrencySymbol", QString::fromLatin1("$"));
  group.deleteEntry("CurrencySymbol", KConfig::Persistent | KConfig::Global);
  if (str != m_locale->currencySymbol())
    group.writeEntry("CurrencySymbol",
                       m_locale->currencySymbol(), KConfig::Persistent|KConfig::Global);

  str = entGrp.readEntry("MonetaryDecimalSymbol", QString::fromLatin1("."));
  group.deleteEntry("MonetaryDecimalSymbol", KConfig::Persistent | KConfig::Global);
  if (str != m_locale->monetaryDecimalSymbol())
    group.writeEntry("MonetaryDecimalSymbol",
                       m_locale->monetaryDecimalSymbol(), KConfig::Persistent|KConfig::Global);

  str = entGrp.readEntry("MonetaryThousandsSeparator", QString::fromLatin1(","));
  str.remove(QString::fromLatin1("$0"));
  group.deleteEntry("MonetaryThousandsSeparator", KConfig::Persistent | KConfig::Global);
  if (str != m_locale->monetaryThousandsSeparator())
    group.writeEntry("MonetaryThousandsSeparator",
                       QString::fromLatin1("$0%1$0")
                       .arg(m_locale->monetaryThousandsSeparator()),
                       KConfig::Persistent|KConfig::Global);

  i = entGrp.readEntry("FracDigits", 2);
  group.deleteEntry("FracDigits", KConfig::Persistent | KConfig::Global);
  if (i != m_locale->fracDigits())
    group.writeEntry("FracDigits", m_locale->fracDigits(), KConfig::Persistent|KConfig::Global);

  b = entGrp.readEntry("PositivePrefixCurrencySymbol", true);
  group.deleteEntry("PositivePrefixCurrencySymbol", KConfig::Persistent | KConfig::Global);
  if (b != m_locale->positivePrefixCurrencySymbol())
    group.writeEntry("PositivePrefixCurrencySymbol",
                       m_locale->positivePrefixCurrencySymbol(), KConfig::Persistent|KConfig::Global);

  b = entGrp.readEntry("NegativePrefixCurrencySymbol", true);
  group.deleteEntry("NegativePrefixCurrencySymbol", KConfig::Persistent | KConfig::Global);
  if (b != m_locale->negativePrefixCurrencySymbol())
    group.writeEntry("NegativePrefixCurrencySymbol",
                       m_locale->negativePrefixCurrencySymbol(), KConfig::Persistent|KConfig::Global);

  i = entGrp.readEntry("PositiveMonetarySignPosition",
                       (int)KLocale::BeforeQuantityMoney);
  group.deleteEntry("PositiveMonetarySignPosition", KConfig::Persistent | KConfig::Global);
  if (i != m_locale->positiveMonetarySignPosition())
    group.writeEntry("PositiveMonetarySignPosition",
                       (int)m_locale->positiveMonetarySignPosition(),
                       KConfig::Persistent|KConfig::Global);

  i = entGrp.readEntry("NegativeMonetarySignPosition",
                       (int)KLocale::ParensAround);
  group.deleteEntry("NegativeMonetarySignPosition", KConfig::Persistent | KConfig::Global);
  if (i != m_locale->negativeMonetarySignPosition())
    group.writeEntry("NegativeMonetarySignPosition",
                       (int)m_locale->negativeMonetarySignPosition(),
                       KConfig::Persistent|KConfig::Global);

//   i = entGrp.readEntry("MonetaryDigitSet", (int)KLocale::ArabicDigits);
//   group.deleteEntry("MonetaryDigitSet", KConfig::Persistent | KConfig::Global);
//   if (i != m_locale->monetaryDigitSet())
//     group.writeEntry("MonetaryDigitSet",
//                      (int)m_locale->monetaryDigitSet(),
//                      KConfig::Persistent|KConfig::Global);

  group.sync();
}

void LocaleConfigMoney::slotLocaleChanged()
{
  m_edMonCurSym->setText( m_locale->currencySymbol() );
/*  m_edMonDecSym->setText( m_locale->monetaryDecimalSymbol() );
  m_edMonThoSep->setText( m_locale->monetaryThousandsSeparator() );*/
  m_inMonFraDig->setValue( m_locale->fracDigits() );

  m_chMonPosPreCurSym->setChecked( m_locale->positivePrefixCurrencySymbol() );
  m_chMonNegPreCurSym->setChecked( m_locale->negativePrefixCurrencySymbol() );
  m_cmbMonPosMonSignPos->setCurrentIndex( m_locale->positiveMonetarySignPosition() );
  m_cmbMonNegMonSignPos->setCurrentIndex( m_locale->negativeMonetarySignPosition() );

//  m_cmbMonDigSet->setCurrentIndex( m_locale->monetaryDigitSet() );
}

void LocaleConfigMoney::slotMonCurSymChanged(const QString &t)
{
  //m_locale->setCurrencySymbol(t);
  emit localeChanged();
}

// void LocaleConfigMoney::slotMonDecSymChanged(const QString &t)
// {
//   m_locale->setMonetaryDecimalSymbol(t);
//   emit localeChanged();
// }

// void LocaleConfigMoney::slotMonThoSepChanged(const QString &t)
// {
//   m_locale->setMonetaryThousandsSeparator(t);
//   emit localeChanged();
// }

void LocaleConfigMoney::slotMonFraDigChanged(int value)
{
  //m_locale->setFracDigits(value);
  emit localeChanged();
}

void LocaleConfigMoney::slotMonPosPreCurSymChanged()
{
  //m_locale->setPositivePrefixCurrencySymbol(m_chMonPosPreCurSym->isChecked());
  emit localeChanged();
}

void LocaleConfigMoney::slotMonNegPreCurSymChanged()
{
  //m_locale->setNegativePrefixCurrencySymbol(m_chMonNegPreCurSym->isChecked());
  emit localeChanged();
}

void LocaleConfigMoney::slotMonPosMonSignPosChanged(int i)
{
  //m_locale->setPositiveMonetarySignPosition((KLocale::SignPosition)i);
  emit localeChanged();
}

void LocaleConfigMoney::slotMonNegMonSignPosChanged(int i)
{
  //m_locale->setNegativeMonetarySignPosition((KLocale::SignPosition)i);
  emit localeChanged();
}

// void LocaleConfigMoney::slotMonDigSetChanged(int i)
// {
//   m_locale->setMonetaryDigitSet((KLocale::DigitSet)i);
//   emit localeChanged();
// }

void LocaleConfigMoney::slotTranslate()
{
  QList<QComboBox*> list;
  list.append(m_cmbMonPosMonSignPos);
  list.append(m_cmbMonNegMonSignPos);

  foreach (QComboBox* wc, list)
  {
    wc->setItemText(0, i18n("Parentheses Around"));
    wc->setItemText(1, i18n("Before Quantity Money"));
    wc->setItemText(2, i18n("After Quantity Money"));
    wc->setItemText(3, i18n("Before Money"));
    wc->setItemText(4, i18n("After Money"));
  }

//   QList<KLocale::DigitSet> digitSets = m_locale->allDigitSetsList();
//   qSort(digitSets);
//   m_cmbMonDigSet->clear();
//   foreach (KLocale::DigitSet ds, digitSets)
//   {
//     m_cmbMonDigSet->addItem(m_locale->digitSetToName(ds, true));
//   }
//   m_cmbMonDigSet->setCurrentIndex(m_locale->monetaryDigitSet());

  QString str;

  str = i18n( "Here you can enter your usual currency "
               "symbol, e.g. $ or €." );
  m_labMonCurSym->setWhatsThis( str );
  m_edMonCurSym->setWhatsThis( str );
//   str = i18n( "<p>Here you can define the decimal separator used "
//                "to display monetary values.</p>"
//                "<p>Note that the decimal separator used to "
//                "display other numbers has to be defined "
//                "separately (see the 'Numbers' tab).</p>" );
//   m_labMonDecSym->setWhatsThis( str );
//   m_edMonDecSym->setWhatsThis( str );

//   str = i18n( "<p>Here you can define the thousands separator "
//                "used to display monetary values.</p>"
//                "<p>Note that the thousands separator used to "
//                "display other numbers has to be defined "
//                "separately (see the 'Numbers' tab).</p>" );
//   m_labMonThoSep->setWhatsThis( str );
//   m_edMonThoSep->setWhatsThis( str );

  str = i18n( "This determines the number of fract digits for "
               "monetary values, i.e. the number of digits you "
               "find <em>behind</em> the decimal separator. "
               "Correct value is 2 for almost all people." );
  m_labMonFraDig->setWhatsThis( str );
  m_inMonFraDig->setWhatsThis( str );

  str = i18n( "If this option is checked, the currency sign "
               "will be prefixed (i.e. to the left of the "
               "value) for all positive monetary values. If "
               "not, it will be postfixed (i.e. to the right)." );
  m_chMonPosPreCurSym->setWhatsThis( str );

  str = i18n( "If this option is checked, the currency sign "
               "will be prefixed (i.e. to the left of the "
               "value) for all negative monetary values. If "
               "not, it will be postfixed (i.e. to the right)." );
  m_chMonNegPreCurSym->setWhatsThis( str );

  str = i18n( "Here you can select how a positive sign will be "
               "positioned. This only affects monetary values." );
  m_labMonPosMonSignPos->setWhatsThis( str );
  m_cmbMonPosMonSignPos->setWhatsThis( str );

  str = i18n( "Here you can select how a negative sign will "
               "be positioned. This only affects monetary "
               "values." );
  m_labMonNegMonSignPos->setWhatsThis( str );
  m_cmbMonNegMonSignPos->setWhatsThis( str );

//   str = i18n( "<p>Here you can define the set of digits "
//                "used to display monetary values. "
//                "If digits other than Arabic are selected, "
//                "they will appear only if used in the language "
//                "of the application or the piece of text "
//                "where the number is shown.</p>"
//                "<p>Note that the set of digits used to "
//                "display other numbers has to be defined "
//                "separately (see the 'Numbers' tab).</p>" );
//   m_labMonDigSet->setWhatsThis( str );
//   m_cmbMonDigSet->setWhatsThis( str );

}

MacroCommand *LocaleConfigMoney::buildCommand()
{
    MacroCommand *m = new MacroCommand();
    if ( m_locale->currencySymbol() != m_edMonCurSym->text() ) {
        m->addCommand( new ModifyCurrencySymolCmd( m_locale, m_edMonCurSym->text() ) );
    }
    if ( m_locale->fracDigits() != m_inMonFraDig->value() ) {
        m->addCommand( new ModifyCurrencyFractionalDigitsCmd( m_locale, m_inMonFraDig->value() ) );
    }
    if ( m_locale->positivePrefixCurrencySymbol() != m_chMonPosPreCurSym->isChecked() ) {
        m->addCommand( new ModifyPositivePrefixCurrencySymolCmd( m_locale, m_chMonPosPreCurSym->isChecked() ) );
    }
    if ( m_locale->negativePrefixCurrencySymbol() != m_chMonNegPreCurSym->isChecked() ) {
        m->addCommand( new ModifyNegativePrefixCurrencySymolCmd( m_locale, m_chMonNegPreCurSym->isChecked() ) );
    }
    if ( m_locale->positiveMonetarySignPosition() != m_cmbMonPosMonSignPos->currentIndex() ) {
        m->addCommand( new ModifyPositiveMonetarySignPositionCmd( m_locale, m_cmbMonPosMonSignPos->currentIndex() ) );
    }
    if ( m_locale->negativeMonetarySignPosition() != m_cmbMonNegMonSignPos->currentIndex() ) {
        m->addCommand( new ModifyNegativeMonetarySignPositionCmd( m_locale, m_cmbMonNegMonSignPos->currentIndex() ) );
    }
    qDebug()<<"buildCommand:"<<m->isEmpty();
    if ( m->isEmpty() ) {
        delete m;
        return 0;
    }
    return m;
}

} // namespace KPlato

#include "localemon.moc"
