/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWFACTORY_H
#define KWFACTORY_H

#include <kpluginfactory.h>
#include "words_export.h"

class KoComponentData;

/**
 * Factory for the Words application.
 * Will use the Words library to create a new instance of the Words doc.
 */
class WORDS_EXPORT KWFactory : public KPluginFactory
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    explicit KWFactory();
    ~KWFactory() override;

    /// overwritten method from superclass
    QObject* create(const char* iface, QWidget* parentWidget, QObject *parent, const QVariantList& args, const QString& keyword) override;

    static const KoComponentData &componentData();

private:
    static KoComponentData *s_componentData;
};

#endif
