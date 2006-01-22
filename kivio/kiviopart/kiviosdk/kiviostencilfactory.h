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
#ifndef KIVIO_STENCIL_FACTORY_H
#define KIVIO_STENCIL_FACTORY_H

class KivioStencil;
class KivioStencilSpawnerInfo;
class QPixmap;

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <koffice_export.h>
class KIVIO_EXPORT KivioStencilFactory : public QObject
{
  Q_OBJECT
  public:
    KivioStencilFactory(QObject *parent=0, const char*name=0, const QStringList& args = QStringList())
      : QObject(parent, name) { Q_UNUSED(args) }
    virtual KivioStencil *NewStencil(const QString& name) =0;
    virtual KivioStencil *NewStencil()=0;
    virtual QPixmap *GetIcon()=0;
    virtual KivioStencilSpawnerInfo *GetSpawnerInfo()=0;
};
#endif
