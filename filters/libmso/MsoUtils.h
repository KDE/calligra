/*
 * This file is part of Office 2007 Filters for Calligra
 * SPDX-FileCopyrightText: 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
 * SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef MSOUTILS_H
#define MSOUTILS_H

#include <QColor>
#include <QLocale>

namespace MSO
{

//! @return QColor value for DefaultIndexColor
QColor defaultIndexedColor(int index);

//! @return QLocale for the give language id
QLocale localeForLangId(int langid);

}; // namespace MSO

#endif /* MSOUTILS_H */
