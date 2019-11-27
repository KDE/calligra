/* This file is part of the KDE project
 * Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
 * Copyright (C) 2001-2005,2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2002,2004-2005 Laurent Montel <montel@kde.org>
 * Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * Copyright (C) 2004-2005,2007 David Faure <faure@kde.org>
 * Copyright (C) 2004,2006 Peter Simonsson <psn@linux.se>
 * Copyright (C) 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2005-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2006,2012 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KARBON_PART_H
#define KARBON_PART_H

#include <KoPart.h>

#include "karbonui_export.h"

class KoView;
class KoDocument;

class KARBONUI_EXPORT KarbonPart : public KoPart
{
    Q_OBJECT

public:
    explicit KarbonPart(QObject *parent);

    ~KarbonPart() override;

    /// reimplemented
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override;
    /// reimplemented
    KoMainWindow *createMainWindow() override;

protected Q_SLOTS:
    /// reimplemented
    void openTemplate(const QUrl& url) override;
};

#endif
