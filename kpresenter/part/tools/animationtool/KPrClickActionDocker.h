/* This file is part of the KDE project
   Copyright (C) 2008 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KPRCLICKACTIONDOCKER_H
#define KPRCLICKACTIONDOCKER_H

#include <QWidget>
#include <QMap>
#include <QPixmap>
#include <KoCanvasObserverBase.h>

class QComboBox;
class QUndoCommand;
class KoPAViewBase;
class KoCanvasBase;
class QCheckBox;
class KPrSoundCollection;
class KoEventActionWidget;

/**
 * This is the click action docker widget that let's you choose a click action for your shapes
 */
class KPrClickActionDocker : public QWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KPrClickActionDocker( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    void setView( KoPAViewBase* view );

public slots:
    void addCommand( QUndoCommand * command );

private slots:
    /// selection has changed
    void selectionChanged();

    /// reimplemented
    virtual void setCanvas( KoCanvasBase *canvas );

private:
    KoPAViewBase *m_view;
    KPrSoundCollection *m_soundCollection;
    KoCanvasBase *m_canvas;
    QComboBox *m_cbPlaySound; // TODO remove when the embedded widgets are ok
    QMap<QString, QWidget *> m_eventActionWidgets;
};

#endif // KPRCLICKACTIONDOCKER_H

