/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRSOUNDEVENTACTIONWIDGET_H
#define KPRSOUNDEVENTACTIONWIDGET_H

#include <KPrEventActionWidget.h>

class QComboBox;
class QString;
class KoShape;
class KoEventAction;
class KoEventActionData;
class KPrEventActionData;
class KPrSoundCollection;

class KPrSoundEventActionWidget : public KPrEventActionWidget
{
    Q_OBJECT
public:
    explicit KPrSoundEventActionWidget( QWidget * parent = 0 );
    virtual ~KPrSoundEventActionWidget();

public slots:
    void setData( KPrEventActionData *eventActionData );

private slots:
    void soundComboChanged();

private:
    void updateCombo( const QString & title );

    KoShape * m_shape;
    KoEventAction * m_eventAction;
    KPrSoundCollection * m_soundCollection;
    QComboBox * m_soundCombo;
};

#endif /* KPRSOUNDEVENTACTIONWIDGET_H */
