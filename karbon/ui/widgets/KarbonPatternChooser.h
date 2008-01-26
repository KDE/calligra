/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KARBONPATTERNCHOOSER_H
#define KARBONPATTERNCHOOSER_H

#include <KoResourceItemChooser.h>
#include <KoPattern.h>
#include <KoResourceServerAdapter.h>

#include <karbonui_export.h>

class KARBONUI_EXPORT KarbonPatternChooser : public KoResourceItemChooser
{
    Q_OBJECT
public:
    KarbonPatternChooser( QWidget *parent = 0 );
    ~KarbonPatternChooser();
private slots:
    void addPattern(KoResource* resource);
    void removePattern(KoResource* resource);
    void importPattern();
    void deletePattern();
private:
    KoResourceServerAdapter<KoPattern> * m_resourceAdapter;
    QMap<KoResource*, KoResourceItem*> m_itemMap;
};

#endif // KARBONPATTERNCHOOSER_H
