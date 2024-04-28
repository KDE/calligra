/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
 * SPDX-FileCopyrightText: 2001-2005, 2007 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2002, 2004-2005 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2002 Benoit Vautrin <benoit.vautrin@free.fr>
 * SPDX-FileCopyrightText: 2004-2005, 2007 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2004, 2006 Peter Simonsson <psn@linux.se>
 * SPDX-FileCopyrightText: 2004-2005 Fredrik Edemar <f_edemar@linux.se>
 * SPDX-FileCopyrightText: 2005-2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2005-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2006, 2012 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <t.zachmann@zagge.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    void openTemplate(const QUrl &url) override;
};

#endif
