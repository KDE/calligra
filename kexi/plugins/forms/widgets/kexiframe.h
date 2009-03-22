/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiFrame_H
#define KexiFrame_H

#include <QFrame>
#include <kexi_export.h>

//! @short Frame widget for Kexi forms
class KEXIFORMUTILS_EXPORT KexiFrame : public QFrame
{
    Q_OBJECT
//todo Q_ENUMS( Shape Shadow )
    Q_PROPERTY(QColor frameColor READ frameColor WRITE setFrameColor)
//todo Q_PROPERTY( Shape frameShape READ frameShape WRITE setFrameShape )
//todo Q_PROPERTY( Shadow frameShadow READ frameShadow WRITE setFrameShadow )

public:
    KexiFrame(QWidget * parent, Qt::WindowFlags f = 0);
    virtual ~KexiFrame();

    virtual const QColor& frameColor() const;

#if 0
//! @todo more options
    enum Shadow {
        NoShadow = QFrame::Plain,
        Raised = QFrame::Raised,
        Sunken = QFrame::Sunken
    };
//! @todo more options
    enum Shape { NoFrame = QFrame::NoFrame, //!< no frame
                 Box = QFrame::Box,                  //!< rectangular box
                 Panel = QFrame::Panel,              //!< rectangular panel
                 StyledPanel = QFrame::StyledPanel,  //!< rectangular panel depending on the GUI style
                 GroupBoxPanel = QFrame::GroupBoxPanel //!< rectangular group-box-like panel depending on the GUI style
               };
    Shape frameShape() const;
    void setFrameShape(KexiFrame::Shape shape);
    Shadow frameShadow() const;
    void setFrameShadow(KexiFrame::Shadow shadow);
#endif

    //! Used to emit handleDragMoveEvent() signal needed to control dragging over the container's surface
    virtual void dragMoveEvent(QDragMoveEvent *e);

    //! Used to emit handleDropEvent() signal needed to control dropping on the container's surface
    virtual void dropEvent(QDropEvent *e);

public slots:
    virtual void setPalette(const QPalette &pal);
    virtual void setFrameColor(const QColor& color);

signals:
    //! Needed to control dragging over the container's surface
    void handleDragMoveEvent(QDragMoveEvent *e);

    //! Needed to control dropping on the container's surface
    void handleDropEvent(QDropEvent *e);

protected:
    virtual void drawFrame(QPainter *);

    class Private;
    Private * const d;
};

#endif
