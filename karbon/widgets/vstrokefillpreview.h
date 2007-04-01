/* This file is part of the KDE project
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2002-2003 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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

#ifndef __VSTROKEFILLPREVIEW_H__
#define __VSTROKEFILLPREVIEW_H__

#include <QFrame>

class QEvent;
class QPaintEvent;
class QBrush;
class KoShapeBorderModel;

/// A widget to preview stroke and fill of a shape
class VStrokeFillPreview : public QFrame
{
    Q_OBJECT

public:
    /// Constructs preview widget with given parent
    explicit VStrokeFillPreview( QWidget* parent = 0L );

    /// Destroys the preview widget
    ~VStrokeFillPreview();

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual QSizePolicy sizePolicy() const;

    /**
     * Updates the preview with the given fill and stroke
     * @param stroke the stroke to preview
     * @param fill the fill to preview
     */
    void update( const KoShapeBorderModel * stroke, const QBrush * fill );

    virtual bool eventFilter( QObject* object, QEvent* event );

    /**
     * Returns whether the stroke (true) or the fill (false) is selected.
     */
    bool strokeIsSelected() const { return m_strokeWidget; }

signals:
    /// Is emitted as soon as the stroke was changed
    void strokeChanged( const KoShapeBorderModel & );
    /// Is emitted as soon as the fill was changed
    void fillChanged( const QBrush& );
    /// Is emitted as soon as the fill is selected
    void fillSelected();
    /// Is emitted as soon as the stroke is selected
    void strokeSelected();

protected:
    virtual void paintEvent( QPaintEvent* event );

private:
    void drawFill( QPainter & painter, const QBrush* );
    void drawStroke( QPainter & painter, const KoShapeBorderModel* );

    bool m_strokeWidget; ///< shows if stroke or fill is selected
    const QBrush * m_fill; ///< the fill to preview
    const KoShapeBorderModel * m_stroke; ///< the stroke to preview
    QRectF m_strokeRect;
    QRectF m_fillRect;
};

#endif

