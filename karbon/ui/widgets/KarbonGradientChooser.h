/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KARBONGRADIENTCHOOSER_H
#define KARBONGRADIENTCHOOSER_H

#include <KoResourceItemChooser.h>
#include <KoAbstractGradient.h>
#include <KoResourceServerAdapter.h>
#include <KoCheckerBoardPainter.h>

class QResizeEvent;

class KarbonGradientChooser : public KoResourceItemChooser
{
    Q_OBJECT
public:
    KarbonGradientChooser( QWidget *parent = 0 );
    ~KarbonGradientChooser();
private slots:
    void addGradient(KoResource* resource);
    void removeGradient(KoResource* resource);
    void importGradient();
    void deleteGradient();
private:
    virtual void resizeEvent ( QResizeEvent * event );

    KoResourceServerAdapter<KoAbstractGradient> * m_resourceAdapter;
    QMap<KoResource*, KoResourceItem*> m_itemMap;
    KoCheckerBoardPainter m_checkerPainter;
};

#endif // KARBONGRADIENTCHOOSER_H
