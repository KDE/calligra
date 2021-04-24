/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008, 2010 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * SPDX-FileCopyrightText: 2009 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTABLECELLSTYLE_P_H
#define KOTABLECELLSTYLE_P_H

#include "Styles_p.h"

class KoTableCellStylePrivate
{
public:
    KoTableCellStylePrivate();
    virtual ~KoTableCellStylePrivate();

    void setProperty(int key, const QVariant &value);

    QString name;
    KoParagraphStyle *paragraphStyle;
    KoTableCellStyle *parentStyle;
    int next;
    StylePrivate stylesPrivate;
};

#endif // KOTABLECELLSTYLE_P_H
