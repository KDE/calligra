/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KODOCUMENTBASE_H
#define KODOCUMENTBASE_H

#include "komvc_export.h"
#include <QObject>

/**
 * @brief The KodocumentBase class is the base class for classes that can load, save and export
 * documents in one or more mime types. The KoDocumentBase class does not provide any gui.
 */
class KOMVC_EXPORT KodocumentBase : public QObject
{
    Q_OBJECT
public:
    explicit KodocumentBase(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // KODOCUMENTBASE_H
