/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIANIMATEDLAYOUT_H
#define KEXIANIMATEDLAYOUT_H

#include <QStackedLayout>

#include "kexiutils_export.h"

//! A tool for animated switching between widgets in a given stacked layout.
/*! Animation is performed if the graphic effects level is set at least
 at "simple" level, i.e. when
 (KGlobalSettings::self()->graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects)
 is true. */
class KEXIUTILS_EXPORT KexiAnimatedLayout : public QStackedLayout
{
    Q_OBJECT
public:
    explicit KexiAnimatedLayout(QWidget* parent = 0);

    ~KexiAnimatedLayout();

public slots:
    //! Sets the current widget to be the specified widget.
    /*! Animation is performed while switching the widgets
        (assuming animations are enabled in KGlobalSettings (see the explanation
        for @ref KexiAnimatedStackedLayout).
        The new current widget must already be contained in this stacked layout. 
        Because of the animation, changing current widget is asynchronous, i.e.
        after this methods returns, current widget is not changed. 
        Connect to signal QStackedLayout::currentChanged(int index) to be notified
        about actual change of the current widget when animation finishes.
        @note this method is not virtual, so when calling it, make sure
              the pointer is KexiAnimatedStackedLayout, not parent class QStackedLayout.
        @see setCurrentIndex() currentWidget() */
    void setCurrentWidget(QWidget* widget);

    //! Sets the current widget to be the specified index.
    /*! Animation is performed as for setCurrentWidget(). */
    void setCurrentIndex(int index);
    
private:
    class Private;
    Private* const d;
};

#endif
