/*
 *  Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 *  Copyright (c) 2012 Cyrille Berger <cberger@cberger.net>
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

#include "kis_color_selector_surface.h"

#include "kis_color_selector_ring.h"
#include "kis_color_selector_triangle.h"
#include "kis_color_selector_simple.h"
#include "kis_color_selector_wheel.h"
#include <cmath>

KisColorSelectorSurface::KisColorSelectorSurface(KisColorSelectorInterface* colorSelectorInterface) :
                                       m_ring(0),
                                       m_triangle(0),
                                       m_slider(0),
                                       m_square(0),
                                       m_wheel(0),
                                       m_mainComponent(0),
                                       m_subComponent(0),
                                       m_grabbingComponent(0)
{
    m_ring = new KisColorSelectorRing(0, colorSelectorInterface);
    m_triangle = new KisColorSelectorTriangle(0, colorSelectorInterface);
    m_slider = new KisColorSelectorSimple(0, colorSelectorInterface);
    m_square = new KisColorSelectorSimple(0, colorSelectorInterface);
    m_wheel = new KisColorSelectorWheel(0, colorSelectorInterface);
}

KisColorSelectorSurface::~KisColorSelectorSurface()
{
    delete m_ring;
    delete m_triangle;
    delete m_slider;
    delete m_square;
    delete m_wheel;
}

const KisColorSelectorSurface::Configuration& KisColorSelectorSurface::configuration() const
{
    return m_configuration;
}

KisColorSelectorComponent* KisColorSelectorSurface::grabbingComponent()
{
    return m_grabbingComponent;
}

KisColorSelectorComponent* KisColorSelectorSurface::mainComponent()
{
    return m_mainComponent;
}

void KisColorSelectorSurface::setConfiguration(KisColorSelectorSurface::Configuration conf)
{
    m_configuration = conf;

    if(m_mainComponent!=0) {
        Q_ASSERT(m_subComponent!=0);
        m_mainComponent->setGeometry(0, 0, 0, 0);
        m_subComponent->setGeometry(0, 0, 0, 0);

        m_mainComponent->disconnect();
        m_subComponent->disconnect();
    }

    switch (m_configuration.mainType) {
    case Square:
        m_mainComponent=m_square;
        break;
    case Wheel:
        m_mainComponent=m_wheel;
        break;
    case Triangle:
        m_mainComponent=m_triangle;
        break;
    default:
        Q_ASSERT(false);
    }

    switch (m_configuration.subType) {
    case Ring:
        m_subComponent=m_ring;
        break;
    case Slider:
        m_subComponent=m_slider;
        break;
    default:
        Q_ASSERT(false);
    }

    QObject::connect(m_mainComponent, SIGNAL(paramChanged(qreal,qreal,qreal,qreal,qreal)),
            m_subComponent,  SLOT(setParam(qreal,qreal,qreal,qreal,qreal)), Qt::UniqueConnection);
    QObject::connect(m_subComponent,  SIGNAL(paramChanged(qreal,qreal,qreal,qreal,qreal)),
            m_mainComponent, SLOT(setParam(qreal,qreal,qreal,qreal, qreal)), Qt::UniqueConnection);
    
    m_mainComponent->setConfiguration(m_configuration.mainTypeParameter, m_configuration.mainType);
    m_subComponent->setConfiguration(m_configuration.subTypeParameter, m_configuration.subType);
}

KisColorSelectorComponent* KisColorSelectorSurface::subComponent()
{
    return m_subComponent;
}

void KisColorSelectorSurface::paint(QPainter* p)
{
    m_mainComponent->paintEvent(p);
    m_subComponent->paintEvent(p);
}

void KisColorSelectorSurface::resize(int width, int height, int squareMargin)
{
    if(m_configuration.subType==Ring) {
        m_ring->setGeometry(0,0,width, height);
        if(m_configuration.mainType==Triangle) {
            m_triangle->setGeometry(width/2-m_ring->innerRadius(),
                                    height/2-m_ring->innerRadius(),
                                    m_ring->innerRadius()*2,
                                    m_ring->innerRadius()*2);
        }
        else {
            int size = m_ring->innerRadius()*2/std::sqrt(2.);
            m_square->setGeometry(width/2-size/2,
                                  height/2-size/2,
                                  size,
                                  size);
        }
    }
    else {
        // type wheel and square
        if(m_configuration.mainType==Wheel) {
            m_mainComponent->setGeometry(0, height*0.1, width, height*0.9);
            m_subComponent->setGeometry( 0, 0,            width, height*0.1);
        }
        else {
            if(height>width) {
                int selectorHeight=height-squareMargin;
                m_mainComponent->setGeometry(0, squareMargin+selectorHeight*0.1, width, selectorHeight*0.9);
                m_subComponent->setGeometry( 0, squareMargin,                    width, selectorHeight*0.1);
            }
            else {
                int selectorWidth=width-squareMargin;
                m_mainComponent->setGeometry(squareMargin, height*0.1, selectorWidth, height*0.9);
                m_subComponent->setGeometry( squareMargin, 0,            selectorWidth, height*0.1);
            }
        }
    }

}

void KisColorSelectorSurface::processMousePress(int x, int y, Qt::MouseButtons _buttons)
{
    if(m_mainComponent->wantsGrab(x, y))
        m_grabbingComponent = m_mainComponent;
    else if(m_subComponent->wantsGrab(x, y))
        m_grabbingComponent = m_subComponent;
    processMouseMove(x, y, _buttons);
}

void KisColorSelectorSurface::processMouseMove(int x, int y, Qt::MouseButtons _buttons)
{
    if(m_grabbingComponent && (_buttons&Qt::LeftButton || _buttons&Qt::RightButton))
    {
        m_grabbingComponent->mouseEvent(x, y);

        m_currentColor = m_mainComponent->currentColor();
    }

}

void KisColorSelectorSurface::processMouseRelease(int x, int y, Qt::MouseButtons _buttons)
{
    m_grabbingComponent = 0;
}

const QColor& KisColorSelectorSurface::currentColor() const
{
    return m_currentColor;
}

void KisColorSelectorSurface::setColor(const QColor& color)
{
    m_mainComponent->setColor(color);
    m_subComponent->setColor(color);
}

