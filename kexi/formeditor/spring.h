/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifndef SPACER_H
#define SPACER_H

#include <qwidget.h>
#include <Q3CString>

#include <kexi_export.h>

class QDomElement;
class QDomDocument;

namespace KFormDesigner
{

class ObjectTreeItem;

}

class KFORMEDITOR_EXPORT Spring : public QWidget
{
    Q_OBJECT
    Q_ENUMS(SizeType)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(SizeType sizeType READ sizeType WRITE setSizeType)

private:
    enum {HSize = 6, HMask = 0x3f, VMask = HMask << HSize, MayGrow = 1, ExpMask = 2, MayShrink = 4 };
public:
    enum SizeType {Fixed = 0, Minimum = MayGrow, Maximum = MayShrink, Preferred = MayGrow | MayShrink , MinimumExpanding = Minimum | ExpMask,
                   Expanding = MinimumExpanding | MayShrink
                  };

public:
    Spring(QWidget *parent);
    ~Spring();

    static bool isPropertyVisible(const Q3CString &name);
    static void saveSpring(KFormDesigner::ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc, bool insideGridLayout);

    void setOrientation(Qt::Orientation orient);
    Qt::Orientation orientation() const {
        return m_orient;
    }
    void setSizeType(SizeType size);
    SizeType sizeType() const;

    void  setPreviewMode() {
        m_edit = false;
    }

private:
    void paintEvent(QPaintEvent *ev);

private:
    Qt::Orientation m_orient;
    bool m_edit;
};

#endif
