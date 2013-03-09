/*  This file is part of the KDE project

    Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>

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

#ifndef CACANVASITEM_H
#define CACANVASITEM_H

#include <QDeclarativeItem>

class QGraphicsWidget;
class KoCanvasBase;

class CACanvasItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY (bool editable READ editable WRITE setEditable NOTIFY editableChanged)
public:
    explicit CACanvasItem(QDeclarativeItem* parent = 0);
    virtual ~CACanvasItem();

    KoCanvasBase *koCanvas();
    void setKoCanvas(KoCanvasBase *koCanvas);
    bool editable() const;
    void setEditable(bool value);
    void updateDocumentSize(QSize sz, bool recalculateCenter);

signals:
    void koCanvasChanged();
    void editableChanged();

protected:
    virtual bool eventFilter(QObject* o, QEvent* e);

private slots:
    void resetShouldIgnoreGeometryChange();
    void resizeToCanvas();

private:
    KoCanvasBase *m_koCanvas;
    QGraphicsWidget *m_koCanvasGraphicsWidget;
    bool m_shouldIgnoreGeometryChange;
    bool m_editable;
};

#endif // CACANVASITEM_H
