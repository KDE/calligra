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

#include "kformdesigner_export.h"

#include <QWidget>

namespace KFormDesigner
{

class FormPrivate;
class Form;

//! Base interface for all main form widgets
/*! You need to inherit this class, and implement the drawing functions. This is necessary
 because you cannot inherit QWidget twice, and we want form widgets to be any widget. */
class KFORMEDITOR_EXPORT FormWidget
{
public:
    FormWidget();
    virtual ~FormWidget();

    Form *form() const;
    void setForm(Form* f);

protected:
    class Private;

    Private* const d;
    friend class Form;
};

}

#endif
