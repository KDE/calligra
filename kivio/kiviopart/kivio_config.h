/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_CONFIG_H
#define KIVIO_CONFIG_H

#include <ksimpleconfig.h>
#include <qcolor.h>
#include <qstring.h>
#include <qpixmap.h>

class KivioConfig : public KSimpleConfig
{
    Q_OBJECT
public:
    typedef enum {
        sbgtColor=0,
        sbgtPixmap,
    } StencilBGType;

protected:
    KivioConfig( const QString & );
    virtual ~KivioConfig();

public:
    void writeConfig();

public:
    static bool readUserConfig();
    static bool deleteConfig();

    static KivioConfig *config() { return s_config; }

public:
    QPixmap *stencilBGPixmap() { return m_pStencilBGPixmap; }
    QColor stencilBGColor() { return m_stencilBGColor; }
    StencilBGType stencilBGType() { return m_stencilBGType; }
    QPixmap *connectorTargetPixmap() { return m_pConnectorTargetPixmap; }

protected:
    static KivioConfig *s_config;


    StencilBGType m_stencilBGType;      // Holds the background type of the stencil dock
    QString m_stencilBGFile;            // Holds the bg pixmap filename of the stencil dock
    QColor m_stencilBGColor;            // Holds the color of the stencil dock bg
    QPixmap *m_pStencilBGPixmap;        // Holds the pixmap of the stencil dock bg.
    
    QPixmap *m_pConnectorTargetPixmap;  // Holds the pixmap of the KivioConnectorObject
};

#endif

