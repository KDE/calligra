/*
 *  Copyright (c) 2014 Spencer Brown <sbrown655@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KISPREVIEWRENDERERWIDGET_H
#define KISPREVIEWRENDERERWIDGET_H

#include <QGLWidget>

class KisPreviewRendererWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit KisPreviewRendererWidget(QWidget *parent = 0);

signals:

public slots:
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

};

#endif // KISPREVIEWRENDERERWIDGET_H
