/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KIVIO_FACTORY_H
#define KIVIO_FACTORY_H

#include <KoFactory.h>
#include <koffice_export.h>
class KInstance;
class KAboutData;

class KIVIO_EXPORT KivioFactory : public KoFactory
{ Q_OBJECT
public:
  KivioFactory( QObject* parent = 0, const char* name = 0 );
  ~KivioFactory();

  virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

  static KInstance* global();
  static KAboutData* aboutData();

private:
  static KInstance* s_global;
  static KAboutData* s_aboutData;
};

#endif
