/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <qtimer.h>
//Added by qt3to4:
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

    void setResizingEnabled(bool enabled) {
        m_enableResizing = enabled;
    }
    void setRecordNavigatorVisible(bool visible);

    void setOuterAreaIndicatorVisible(bool visible)  {
        m_outerAreaVisible = visible;
    }

    void refreshContentsSizeLater(bool horizontal, bool vertical);
    void updateNavPanelGeometry();

    KexiRecordNavigator* recordNavigator() const;

    inline bool preview() const {
        return m_preview;
    }

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

    bool m_resizing;
    bool m_enableResizing;
    QWidget *m_widget;

    int m_gridSize;
    QFont m_helpFont;
    QColor m_helpColor;
    QTimer m_delayedResize;
    //! for refreshContentsSizeLater()
    Q3ScrollView::ScrollBarMode m_vsmode, m_hsmode;
bool m_snapToGrid : 1;
bool m_preview : 1;
bool m_smodeSet : 1;
bool m_outerAreaVisible : 1;
    KexiRecordNavigator* m_scrollViewNavPanel;
};

#endif

