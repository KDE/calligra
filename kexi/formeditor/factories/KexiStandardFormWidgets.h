/* This file is part of the KDE project
   Copyright (C) 2003 by Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXISTANDARDFORMWIDGETS_H
#define KEXISTANDARDFORMWIDGETS_H

#include <QFrame>
#include <QPixmap>
#include <QLabel>
#include <QMenu>

#include "widgetfactory.h"
#include "container.h"
#include "FormWidgetInterface.h"

class QTreeWidgetItem;
class QTreeWidget;
class KPropertySet;
class KexiStandardFormWidgetsFactory;

//! A picture label widget for use within forms
class KexiPictureLabel : public QLabel, public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT

public:
    explicit KexiPictureLabel(const QPixmap &pix, QWidget *parent = 0);
    virtual ~KexiPictureLabel();

    virtual bool setProperty(const char *name, const QVariant &value);
};

//! A line widget for use within forms
class Line : public QFrame, public KFormDesigner::FormWidgetInterface
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

public:
    explicit Line(Qt::Orientation orient, QWidget *parent = 0);
    virtual ~Line();

    void setOrientation(Qt::Orientation orient);
    Qt::Orientation orientation() const;
};

//! Internal action of editing rich text for a label or text editor
//! Keeps context expressed using container and receiver widget
class EditRichTextAction : public QAction
{
    Q_OBJECT
public:
    EditRichTextAction(KFormDesigner::Container *container,
                       QWidget *receiver, QObject *parent,
                       KexiStandardFormWidgetsFactory *factory);
protected slots:
    void slotTriggered();
private:
    KFormDesigner::Container *m_container;
    QWidget *m_receiver;
    KexiStandardFormWidgetsFactory *m_factory;
};

#endif
