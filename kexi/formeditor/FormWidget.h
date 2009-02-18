/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMDESIGNERFORMWIDGET_H
#define KFORMDESIGNERFORMWIDGET_H

#include <QList>
#include <QRect>
#include <QWidget>
#include <kexi_export.h>

namespace KFormDesigner
{

class Container;
class WidgetLibrary;
class FormManager;
class FormPrivate;
class Form;

//! Base (virtual) class for all form widgets
/*! You need to inherit this class, and implement the drawing functions. This is necessary
 because you cannot inherit QWidget twice, and we want form widgets to be any widget.
 See FormWidgetBase in test/kfd_part.cpp and just copy functions there. */
class KFORMEDITOR_EXPORT FormWidget
{
public:
    FormWidget();
    virtual ~FormWidget();

    /*! This function draws the rects in the \a list  in the Form, above of all widgets,
     using double-buffering. \a type can be 1 (selection rect)
     or 2 (insert rect, dotted). */

    virtual void drawRects(const QList<QRect> &list, int type) = 0;

    virtual void drawRect(const QRect &r, int type) = 0;

    /*! This function inits the buffer used for double-buffering. Called before drawing rect. */
    virtual void initBuffer() = 0;

    /*! Clears the form, ie pastes the whole buffer to repaint the Form. */
    virtual void clearForm() = 0;

    /*! This function highlights two widgets (to is optional), which are
    sender and receiver, and draws a link between them. */
    virtual void highlightWidgets(QWidget *from, QWidget *to) = 0;

    Form *form() const { return m_form; }

protected:
    Form *m_form;

    friend class Form;
};

}

#endif
