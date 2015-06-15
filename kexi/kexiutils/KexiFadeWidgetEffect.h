/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef KEXIFADEWIDGETEFFECT_H
#define KEXIFADEWIDGETEFFECT_H

#include <kexiutils_export.h>
#include <QWidget>

class KexiFadeWidgetEffectPrivate;

/** \class KexiFadeWidgetEffect KexiFadeWidgetEffect.h KexiFadeWidgetEffect
 * \brief Animates changes fading the new UI over the old look.
 *
 * This widget will put itself above the widget that will change and show a fading transition from
 * the old to the new UI. It will delete itself after the animation is finished.
 * Example:
 * \code
 * KexiFadeWidgetEffect *animation = new KexiFadeWidgetEffect(widgetThatWillChange);
 * // do changes on widgetThatWillChange
 * // ...
 * animation->start();
 * \endcode
 *
 * \note The widget that changes needs to have a parent widget. KexiFadeWidgetEffect does not work
 * for toplevel widgets (windows).
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \since 4.1
 */
class KEXIUTILS_EXPORT KexiFadeWidgetEffect : public QWidget
{
    Q_OBJECT

public:
    /**
     * Create the animation widget. Takes a snapshot of the \p destWidget to use as old image
     * that gets faded out.
     *
     * \param destWidget The widget that will change and should fade to the new look.
     */
    KexiFadeWidgetEffect(QWidget *destWidget);

    /**
     * Destructor.
     *
     * \warning KexiFadeWidgetEffect deletes itself after the animation is finished.
     */
    ~KexiFadeWidgetEffect();

public Q_SLOTS:
    /**
     * Starts the animation.
     *
     * Call this function after all visual changes are done.
     *
     * \param duration The duration of the animation in milliseconds.
     */
    void start(int duration = 250);

protected:
    /**
     * \internal
     */
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

    /**
     * \internal
     */
    KexiFadeWidgetEffectPrivate *const d;

private Q_SLOTS:
    void finished();
};

#endif
