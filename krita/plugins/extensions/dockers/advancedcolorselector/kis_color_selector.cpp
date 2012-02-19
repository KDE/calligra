/*
 *  Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
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

#include "kis_color_selector.h"

#include <cmath>

#include <QHBoxLayout>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QPushButton>

#include <KConfig>
#include <KConfigGroup>
#include <KComponentData>
#include <KGlobal>
#include <KIcon>
#include <KDebug>

#include "kis_color_selector_container.h"
#include "kis_color_selector_component.h"
#include <KoColor.h>

KisColorSelector::KisColorSelector(KisColorSelectorSurface::Configuration conf, QWidget* parent)
                                       : KisColorSelectorBase(parent), m_surface(this)
{
    init();
    setConfiguration(conf);
}

KisColorSelector::KisColorSelector(QWidget* parent)
                                       : KisColorSelectorBase(parent), m_surface(this)
{
    init();
    updateSettings();
}

KisColorSelectorBase* KisColorSelector::createPopup() const
{
    KisColorSelectorBase* popup = new KisColorSelector(0);
    popup->setColor(m_lastColor);
    return popup;
}

void KisColorSelector::setConfiguration(KisColorSelectorSurface::Configuration conf)
{
    m_surface.setConfiguration(conf);

    QResizeEvent event(QSize(width(), height()), QSize());
    resizeEvent(&event);
}

KisColorSelectorSurface::Configuration KisColorSelector::configuration() const
{
    return m_surface.configuration();
}

void KisColorSelector::updateSettings()
{
    KisColorSelectorBase::updateSettings();
    KConfigGroup cfg = KGlobal::config()->group("advancedColorSelector");
    setConfiguration(KisColorSelectorSurface::Configuration::fromString(cfg.readEntry("colorSelectorConfiguration", KisColorSelectorSurface::Configuration().toString())));
}

void KisColorSelector::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.fillRect(0,0,width(),height(),QColor(128,128,128));
    p.setRenderHint(QPainter::Antialiasing);

    m_surface.paint(&p);
}

inline int iconSize(qreal width, qreal height) {
    qreal radius = qMin(width, height)/2.;
    qreal xm = width/2.;
    qreal ym = height/2.;
    if(xm>=2*ym || ym>=2*xm)
        return qBound<qreal>(5., radius, 32.);

    qreal a=-2;
    qreal b=2.*(xm+ym);
    qreal c=radius*radius-xm*xm-ym*ym;
    return qBound<qreal>(5., ((-b+sqrt(b*b-4*a*c))/(2*a)), 32.);
}

void KisColorSelector::resizeEvent(QResizeEvent* e)
{
    int buttonSize = 0;
    if(m_surface.configuration().subType == KisColorSelectorSurface::Ring) {
        if(displaySettingsButton()) {
            int size = iconSize(width(), height());
            m_button->setGeometry(0, 0, size, size);
        }
    }
    else {
        // type wheel and square
        if(m_surface.configuration().mainType == KisColorSelectorSurface::Wheel) {
            if(displaySettingsButton()) {
                int size = iconSize(width(), height()*0.9);
                m_button->setGeometry(0, height()*0.1, size, size);
            }
        }
        else {
            if(displaySettingsButton()) {
                buttonSize = qBound(20, int(0.1*height()), 32);
                m_button->setGeometry(0, 0, buttonSize, buttonSize);
            }
        }
    }
    m_surface.resize(width(), height(), buttonSize);
    
    KisColorSelectorBase::resizeEvent(e);
}

void KisColorSelector::mousePressEvent(QMouseEvent* e)
{
    e->setAccepted(false);
    KisColorSelectorBase::mousePressEvent(e);

    if(!e->isAccepted()) {
        m_surface.processMousePress(e->x(), e->y(), e->buttons());
        KoColor kocolor(m_surface.currentColor(), colorSpace());
        updateColorPreview(kocolor.toQColor());
    }
}

void KisColorSelector::mouseMoveEvent(QMouseEvent* e)
{
    KisColorSelectorBase::mouseMoveEvent(e);
    m_surface.processMouseMove(e->x(), e->y(), e->buttons());
    KoColor kocolor(m_surface.currentColor(), colorSpace());
    updateColorPreview(kocolor.toQColor());
}

void KisColorSelector::mouseReleaseEvent(QMouseEvent* e)
{
    KisColorSelectorBase::mouseReleaseEvent(e);
    
    m_surface.processMouseRelease(e->x(), e->y(), e->buttons());
    
    QColor currentColor = m_surface.currentColor();
    if(m_lastColor!=currentColor && currentColor.isValid())
    {
        m_lastColor=currentColor;
        ColorRole role;
        if(e->button() == Qt::LeftButton)
            role=Foreground;
        else
            role=Background;
        commitColor(KoColor(currentColor, colorSpace()), role);

        if(isPopup() && m_surface.mainComponent()->containsPoint(e->pos())) {
            hidePopup();
        }
    }
    e->accept();
}

bool KisColorSelector::displaySettingsButton()
{
    if(dynamic_cast<KisColorSelectorContainer*>(parent())!=0)
        return true;
    else
        return false;
}

void KisColorSelector::setColor(const QColor &color)
{
    m_surface.setColor(color);
    m_lastColor=color;
    update();
}

void KisColorSelector::init()
{
    setAcceptDrops(true);

    if(displaySettingsButton()) {
        m_button = new QPushButton(this);
        m_button->setIcon(KIcon("configure"));
        connect(m_button, SIGNAL(clicked()), SIGNAL(settingsButtonClicked()));
    }

    // a tablet can send many more signals, than a mouse
    // this causes many repaints, if updating after every signal.
    // a workaround with a timer can fix that.
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(1);
    m_updateTimer->setSingleShot(true);

    connect(m_updateTimer,      SIGNAL(timeout()), this,  SLOT(update()));

    QObject::connect(m_surface.mainComponent(), SIGNAL(update()), m_updateTimer,   SLOT(start()), Qt::UniqueConnection);
    QObject::connect(m_surface.subComponent(),  SIGNAL(update()), m_updateTimer,   SLOT(start()), Qt::UniqueConnection);

    setMinimumSize(40, 40);
}
