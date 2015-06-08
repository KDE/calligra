/* This file is part of the KDE libraries
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFE_RESIZEHANDLER_H
#define KFE_RESIZEHANDLER_H

#include "kformdesigner_export.h"

#include <QHash>
#include <QWidget>

namespace KFormDesigner
{

class Form;
class ResizeHandleSet;

/**
* a set of resize handles (for resizing widgets)
* @author Joseph Wenninger
*/
class KFORMEDITOR_EXPORT ResizeHandleSet: public QObject
{
    Q_OBJECT

public:
    typedef QHash<QString, ResizeHandleSet*> Hash;

    ResizeHandleSet(QWidget *modify, Form *form);

    ~ResizeHandleSet();

    QWidget *widget() const;

    void setWidget(QWidget *modify);

    void raise();

    void setEditingMode(bool editing);

    Form *form() const;

Q_SIGNALS:
    void geometryChangeStarted();
    void geometryChanged(const QRect &newGeometry);

protected:
    void resizeStarted();
    void resizeFinished();
private:
    class Private;

    Private* const d;
    friend class ResizeHandle;
};

}

#endif
