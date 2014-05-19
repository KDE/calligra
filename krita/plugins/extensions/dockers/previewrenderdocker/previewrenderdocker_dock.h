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

#ifndef _PREVIEWRENDERDOCKER_DOCK_H_
#define _PREVIEWRENDERDOCKER_DOCK_H_

#include <QDockWidget>
#include <QGLWidget>
#include <KoCanvasObserverBase.h>

class KisPaintOpPresetsChooserPopup;
class KisCanvas2;

class PreviewRenderDockerDock : public QDockWidget  {
    Q_OBJECT
public:
    PreviewRenderDockerDock( );
public slots:
private slots:
private:
    QGLWidget* m_previewRenderer; // note: probably want to subclass this for the real thing
};


#endif
