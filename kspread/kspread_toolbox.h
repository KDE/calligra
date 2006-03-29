/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <qframe.h>
#include <qpoint.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QMouseEvent>
#include <QChildEvent>
#include <koffice_export.h>
class Q3VBoxLayout;
class QSpinBox;

class KoDocumentChild;

namespace KSpread
{

class KOFFICEUI_EXPORT ToolBox : public QFrame
{
    Q_OBJECT
public:
    ToolBox( QWidget* parent = 0, const char* name = 0 );

public slots:
    void setEnabled( bool enable );

protected:
    // void paintEvent( QPaintEvent* );
    void childEvent( QChildEvent* ev );
    void mousePressEvent( QMouseEvent* ev );
    void mouseMoveEvent( QMouseEvent* ev );

private:
    Q3VBoxLayout* m_layout;
    QPoint m_mousePos;
    QPoint m_startPos;
};

class KOFFICEUI_EXPORT KoTransformToolBox : public ToolBox
{
    Q_OBJECT
public:
    KoTransformToolBox( KoDocumentChild* child, QWidget* parent = 0, const char* name = 0 );

    double rotation() const;
    double scaling() const;
    double xShearing() const;
    double yShearing() const;

    void setRotation( double );
    void setScaling( double );
    void setXShearing( double );
    void setYShearing( double );

signals:
    void rotationChanged( double );
    void scalingChanged( double );
    void xShearingChanged( double );
    void yShearingChanged( double );

public slots:
    void setDocumentChild( KoDocumentChild* );

private slots:
    void slotRotationChanged( int );
    void slotScalingChanged( int );
    void slotXShearingChanged( int );
    void slotYShearingChanged( int );

private:
    QSpinBox* m_rotation;
    QSpinBox* m_scale;
    QSpinBox* m_shearX;
    QSpinBox* m_shearY;
    KoDocumentChild* m_child;
    class Private;
    Private *d;
};

} // namespace KSpread

#endif
