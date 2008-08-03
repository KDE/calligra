/* This file is part of the KDE project
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qapplication.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qobject.h>
#include <qpainter.h>
#include <qstyle.h>
//Added by qt3to4:
#include <QPixmap>
#include <QEvent>
#include <QPaintEvent>
#include <QChildEvent>

#include <kimageeffect.h>
#include <kpixmap.h>

#include "kexigradientwidget.h"

KexiGradientWidget::KexiGradientWidget(QWidget *parent, const char *name, Qt::WFlags f)
        : QWidget(parent, name, f), p_displayMode(NoGradient),
        p_gradientType(VerticalGradient),
        p_color1(Qt::white), p_color2(Qt::blue), p_currentChild(0),
        p_opacity(0.5), p_cacheDirty(true)
{
    p_rebuildDelayTimer.setSingleShot(true);
    p_customBackgroundWidgets.setAutoDelete(false);
    p_knownWidgets.setAutoDelete(false);

    p_backgroundColor = QWidget::paletteBackgroundColor();

    connect(&p_rebuildDelayTimer, SIGNAL(timeout()), this, SLOT(setCacheDirty()));

    installEventFilter(this);
}

KexiGradientWidget::~KexiGradientWidget()
{
}

bool KexiGradientWidget::isValidChildWidget(QObject* child)
{
    const QWidget* wgt = dynamic_cast<QWidget*>(child);

    if (wgt == 0L)
        return false;

    if (wgt->inherits("QScrollView"))
        return false;
    if (wgt->inherits("QComboBox"))
        return false;
    if (wgt->inherits("QLineEdit"))
        return false;
    if (wgt->inherits("KexiDBForm"))
        return false;

    return true;
}

void KexiGradientWidget::buildChildrenList(WidgetList& list, QWidget* p)
{
    QObjectList* objects = p->queryList("QWidget", 0, false, false);

    for (QObjectList::Iterator it = objects->begin(); it != objects->end(); ++it) {
        if (isValidChildWidget((*it)) == false)
            continue;
        list.append(dynamic_cast<QWidget*>((*it)));
        buildChildrenList(list, dynamic_cast<QWidget*>((*it)));
    }

    delete objects;
}

void KexiGradientWidget::rebuildCache(void)
{
    WidgetList childWidgetList;
    buildChildrenList(childWidgetList, this);

    /**
    Disable the effect and behave like a normal QWidget.
    */
    if (p_displayMode == NoGradient) {
//  if ( p_backgroundPixmap.isNull() ) {
        //unsetPalette();
        //} else {
        QWidget::setPaletteBackgroundPixmap(p_backgroundPixmap);
        //}
        QWidget::setPaletteBackgroundColor(p_backgroundColor);

        for (WidgetList::Iterator it = childWidgetList.begin();
                it != childWidgetList.end(); ++it) {

            if (p_customBackgroundWidgets.contains((*it)) == false) {
                (*it)->unsetPalette();
            }
        }
        /**
        The cache is now in a current state.
        */
        p_cacheDirty = false;
        return;
    }

    KPixmap tempPixmap;
    QImage gradientImage;
    QImage bgImage;

    /**
    Draw the gradient
    */
    gradientImage = KImageEffect::gradient(size(), p_color1, p_color2,
                                           (KImageEffect::GradientType)p_gradientType);

    /**
    Draw the widget-background in a pixmap and fade it with the gradient.
    */
    if (p_displayMode == FadedGradient) {
        tempPixmap.resize(size());
        QPainter p(&tempPixmap, this);

        if (p_backgroundPixmap.isNull()) {
            /*
            Need to unset the palette, otherwise the old gradient
            will be used as a background, not the widget's default bg.
            */
            unsetPalette();
            p.fillRect(0, 0, width(), height(), palette().brush(
                           isEnabled() ? QPalette::Active : QPalette::Disabled,
                           QColorGroup::Background));
        } else {
            p.drawTiledPixmap(0, 0, width(), height(), p_backgroundPixmap);
        }

        p.end();

        bgImage = tempPixmap;

        KImageEffect::blend(gradientImage, bgImage, (float)p_opacity);

        tempPixmap.convertFromImage(bgImage);
    } else if (p_displayMode == SimpleGradient) {
        /**
        Use the gradient as the final background-pixmap
        if displaymode is set to SimpleGradient.
        */
        tempPixmap.convertFromImage(gradientImage);
    }

    /**
    All children need to have our background set.
    */
    KPixmap partPixmap;
    QRect area;
    QWidget* childWidget = 0;
    const QPoint topLeft(0, 0);

    for (WidgetList::Iterator it = childWidgetList.begin();
            it != childWidgetList.end(); ++it) {

        childWidget = (*it);

        /**
        Exclude widgets with a custom palette.
        */
        if (p_customBackgroundWidgets.contains(childWidget)) {
            continue;
        }

        partPixmap.resize(childWidget->size());
        /**
        Get the part of the tempPixmap that is
        under the current child-widget.
        */
        if (childWidget->parent() == this) {
            area = childWidget->geometry();
        } else {
            area.setTopLeft(childWidget->mapTo(this,
                                               childWidget->clipRegion().boundingRect().topLeft()));
            area.setSize(childWidget->size());
        }
        bitBlt(&partPixmap, topLeft, &tempPixmap, area);

        p_currentChild = childWidget;
        childWidget->setPaletteBackgroundPixmap(partPixmap);
    }

    QWidget::setPaletteBackgroundPixmap(tempPixmap);
    /**
    Unset the dirty-flag at the end of the method.
    QWidget::setPaletteBackgroundPixmap() causes this
    to get set to true again, so set it to false
    right after setting the pixmap.
    */
    p_cacheDirty = false;
}

void KexiGradientWidget::paintEvent(QPaintEvent* e)
{
    /**
    Rebuild the background-pixmap if necessary.
    */
    if (p_cacheDirty == true) {
        rebuildCache();
    }

    /**
    Draw the widget as usual
    */
    QWidget::paintEvent(e);
}

bool KexiGradientWidget::eventFilter(QObject* object, QEvent* event)
{
    QWidget* child = dynamic_cast<QWidget*>(object);

    /**
    Manage list of child-widgets.
    */
    if (object == this) {
        if (event->type() == QEvent::ChildAdded) {
            child = dynamic_cast<QWidget*>(dynamic_cast<QChildEvent*>(event)->child());
            if (isValidChildWidget(child) == false) {
                return false;
            }
            /**
            Add the new child-widget to our list of known widgets.
            */
            p_knownWidgets.append(child);
            /**
            ... and install 'this' as the child's event-filter.
            */
            child->installEventFilter(this);
        } else if (event->type() == QEvent::ChildRemoved) {
            /**
            Remove the child-widget from the list of known widgets.
            */
            p_knownWidgets.remove(dynamic_cast<QWidget*>(dynamic_cast<QChildEvent*>(event)->child()));
        }
        return false;
    }

    /**
    Manage custombackground-list.
    */
    if (event->type() == QEvent::PaletteChange) {
        /**
        p_currentChild will be == 0L, when the user
        sets it's palette manually.
        In this case, it has to be added to the customBackground-list.
        */
        if (p_currentChild == 0L && child != 0L) {
            if (p_customBackgroundWidgets.contains(child) == false) {
                p_customBackgroundWidgets.append(child);
                return false;
            }
        }
        /**
        Check if the widget whose PaletteChange-event we handle
        isn't the widget we set the background in rebuildCache().
        */
        if (child != p_currentChild && child != 0L) {
            /**
            Add the new child to the list of widgets, we don't set
            the background ourselves if it isn't in the list.
            */
            if (p_customBackgroundWidgets.contains(child) == false) {
                if (child->paletteBackgroundPixmap() != 0L) {
                    p_customBackgroundWidgets.append(child);
                }
            } else {
                /**
                If the palette is now the default-palette again,
                remove it from the "don't set background in rebuildCache()"-list
                and rebuild the cache, so it again will get the gradient background.
                */
                if (child->paletteBackgroundPixmap() == 0L) {
                    p_customBackgroundWidgets.remove(child);
                    if (p_displayMode != NoGradient) {
                        p_cacheDirty = true;
                    }
                }
            }
        }
        p_currentChild = 0;
    }

    if (event->type() == QEvent::Move) {
        if (p_customBackgroundWidgets.contains(child) == false) {
            updateChildBackground(child);
        }
    }
    return false;
}

void KexiGradientWidget::updateChildBackground(QWidget* childWidget)
{
    KPixmap partPixmap;
    KPixmap bgPixmap;
    QRect area;
    const QPoint topLeft(0, 0);

    bgPixmap = paletteBackgroundPixmap() ? (*paletteBackgroundPixmap()) : QPixmap();
    if (bgPixmap.isNull())
        return;

    /**
    Exclude widgtes that don't have a parent.
    This happens when children are removed
    which are in the knownWidgets-list.
    */
    if (childWidget->parent() == 0L)
        return;

    /**
    Exclude widgets with a custom palette.
    */
    if (p_customBackgroundWidgets.contains(childWidget)) {
        return;
    }

    partPixmap.resize(childWidget->size());
    /**
    Get the part of the tempPixmap that is
    under the current child-widget.
    */
    if (childWidget->parent() == this) {
        area = childWidget->geometry();
    } else {
        area.setTopLeft(childWidget->mapTo(this,
                                           childWidget->clipRegion().boundingRect().topLeft()));
        area.setSize(childWidget->size());
    }
    bitBlt(&partPixmap, topLeft, &bgPixmap, area);

    p_currentChild = childWidget;
    childWidget->setPaletteBackgroundPixmap(partPixmap);
}

void KexiGradientWidget::setPaletteBackgroundColor(const QColor& color)
{
    p_backgroundColor = color;
    if (p_displayMode == NoGradient) {
        QWidget::setPaletteBackgroundColor(p_backgroundColor);
    }
}

const QColor& KexiGradientWidget::paletteBackgroundColor() const
{
    return p_backgroundColor;
}

#include "kexigradientwidget.moc"
