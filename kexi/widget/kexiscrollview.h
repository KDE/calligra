/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXISCROLLVIEW_H
#define KEXISCROLLVIEW_H

#include <kexi_export.h>

#include <Q3ScrollView>
#include <QTimer>
#include <QMouseEvent>
#include <QPixmap>
#include <QEvent>

class QColor;
class QFont;
class KexiRecordNavigator;

//! The scrollview which includes KexiDBForm
/*! It allows to resize its m_widget, following snapToGrid setting.
 Its contents is resized so the widget can always be resized. */
class KEXIEXTWIDGETS_EXPORT KexiScrollView : public Q3ScrollView
{
    Q_OBJECT

public:
    KexiScrollView(QWidget *parent, bool preview);
    virtual ~KexiScrollView();

    void setWidget(QWidget *w);
    void setSnapToGrid(bool enable, int gridSize = 10);

    void setResizingEnabled(bool enabled);
    void setRecordNavigatorVisible(bool visible);

    void setOuterAreaIndicatorVisible(bool visible);

    void refreshContentsSizeLater(bool horizontal, bool vertical);
    void updateNavPanelGeometry();

    KexiRecordNavigator* recordNavigator() const;

    bool isPreviewing() const;

public slots:
    /*! Make sure there is a 300px margin around the form contents to allow resizing. */
    virtual void refreshContentsSize();

signals:
    void outerAreaClicked();
    void resizingStarted();
    void resizingEnded();

protected:
    virtual void contentsMousePressEvent(QMouseEvent * ev);
    virtual void contentsMouseReleaseEvent(QMouseEvent * ev);
    virtual void contentsMouseMoveEvent(QMouseEvent * ev);
    virtual void drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph);
    virtual void leaveEvent(QEvent *e);
    virtual void setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h);
    void setupPixmapBuffer(QPixmap& pixmap, const QString& text, int lines);

    const QTimer *delayedResizeTimer() const;
    QWidget *widget() const;

private:
    class Private;
    Private * const d;
};

#endif

