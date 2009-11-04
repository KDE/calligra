/*
 * localemon.h
 *
 * Copyright (c) 1999-2003 Hans Petter Bieker <bieker@kde.org>
 * Copyright (c) 2007 Dag Andersen <danders@get2net.dk>
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

#ifndef KPLATO_LOCALEMON_H
#define KPLATO_LOCALEMON_H

#include "kplatoui_export.h"

#include "ui_localemon.h"

class KLocale;

namespace KPlato
{

class Part;
class MacroCommand;

class KPLATOUI_EXPORT LocaleConfigMoney : public QWidget, Ui::LocaleConfigMoney
{
  Q_OBJECT

public:
  LocaleConfigMoney(KLocale *locale, QWidget *parent);
  virtual ~LocaleConfigMoney();

  void save();

  MacroCommand *buildCommand();

public Q_SLOTS:
  /**
   * Loads all settings from the current locale into the current widget.
   */
  void slotLocaleChanged();
  /**
   * Retranslate all objects owned by this object using the current locale.
   */
  void slotTranslate();

Q_SIGNALS:
  void localeChanged();

private Q_SLOTS:
  // Money
  void slotMonCurSymChanged(const QString &t);
/*  void slotMonDecSymChanged(const QString &t);
  void slotMonThoSepChanged(const QString &t);*/
  void slotMonFraDigChanged(int value);
  void slotMonPosPreCurSymChanged();
  void slotMonNegPreCurSymChanged();
  void slotMonPosMonSignPosChanged(int i);
  void slotMonNegMonSignPosChanged(int i);
//  void slotMonDigSetChanged(int i);

private:
  KLocale *m_locale;
};

} // namespace KPlato

#endif // LOCALEMON_H
