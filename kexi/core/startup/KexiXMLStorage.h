/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#ifndef KEXIXMLSTORAGE_H
#define KEXIXMLSTORAGE_H

#include <kexidb/connectiondata.h>

#include "core/kexiprojectdata.h"
#include "KexiProjectSet.h"

class QIODevice;

/*! \return a new KexiConnectionData object loaded from xml contents on \a dev device
	or NULL on error (then i18n'd error message is filled in \a error).
*/
KEXICORE_EXPORT KexiProjectData* 
loadKexiConnectionDataXML(QIODevice *dev, QString &error);

/*! Saves \a data of KexiDB::ConnectionData object at xml on \a dev device;
	\return false on error (then i18n'd error message is filled in \a error).
*/
KEXICORE_EXPORT bool 
saveKexiConnectionDataXML(QIODevice *dev, const KexiDB::ConnectionData &data, QString &error);

/*! \return a new KexiProjectData object loaded from xml contents on \a dev device
	or NULL on error (then i18n'd error message is filled in \a error).
*/
KEXICORE_EXPORT KexiProjectData* 
loadKexiProjectDataXML(QIODevice *dev, QString &error);

/*! Saves \a data of KexiProjectData object at xml on \a dev device;
	\return false on error (then i18n'd error message is filled in \a error).
*/
KEXICORE_EXPORT bool 
saveKexiProjectDataXML(QIODevice *dev, const KexiProjectData &data, QString &error);

/*! \return a new KexiProjectSet object loaded from xml contents on \a dev device
	or NULL on error (then i18n'd error message is filled in \a error).
*/
KEXICORE_EXPORT KexiProjectData* 
loadKexiProjectSetXML(QIODevice *dev, QString &error);

/*! Saves \a data of KexiProjectSet object at xml on \a dev device;
	\return false on error (then i18n'd error message is filled in \a error).
*/
KEXICORE_EXPORT bool 
saveKexiProjectSetXML(QIODevice *dev, const KexiProjectSet &pset, QString &error);

#endif

