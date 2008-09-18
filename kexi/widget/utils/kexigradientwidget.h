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

#ifndef KEXIGRADIENTWIDGET_H
#define KEXIGRADIENTWIDGET_H

#include <QTimer>
#include <QWidget>
#include <QPixmap>
#include <QResizeEvent>
#include <QEvent>
#include <QPaintEvent>
#include <Q3PtrList>

#include <kimageeffect.h>
#include <kpixmap.h>
#include <kexi_export.h>

#define REBUILD_DELAY 100

//! @short A simple widget that can use different types of gradients as the background.
/*!
  @author Christian Nitschkowski
*/
class KEXIGUIUTILS_EXPORT KexiGradientWidget : public QWidget
{
    typedef QList<QWidget*> WidgetList;

    Q_OBJECT
    Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode DESIGNABLE true)
    Q_PROPERTY(GradientType gradientType READ gradientType WRITE setGradientType DESIGNABLE true)
    Q_PROPERTY(QColor gradientColor1 READ gradientColor1 WRITE setGradientColor1 DESIGNABLE true)
    Q_PROPERTY(QColor gradientColor2 READ gradientColor2 WRITE setGradientColor2 DESIGNABLE true)
    Q_PROPERTY(double blendOpacity READ blendOpacity WRITE setBlendOpacity DESIGNABLE true)
    Q_ENUMS(DisplayMode GradientType)

public:
    /*!
    Modes for displaying the gradient.
    */
    enum DisplayMode {
        NoGradient, //!< No gradient at all. Will behave just like a QWidget
        FadedGradient, //!< Gradient will be faded with the widgets background
        SimpleGradient //!< Gradient will replace the usual widget background
    };

    /*!
    Gradient type specification.
    See GradientType for more details (part of the KDEFX library)
    */
    enum GradientType {
        VerticalGradient = KImageEffect::VerticalGradient,
        HorizontalGradient = KImageEffect::HorizontalGradient,
        DiagonalGradient = KImageEffect::DiagonalGradient,
        CrossDiagonalGradient = KImageEffect::CrossDiagonalGradient,
        PyramidGradient = KImageEffect::PyramidGradient,
        RectangleGradient = KImageEffect::RectangleGradient,
        PipeCrossGradient = KImageEffect::PipeCrossGradient,
        EllipticGradient = KImageEffect::EllipticGradient
    };

    KexiGradientWidget(QWidget *parent = 0, const char *name = 0, Qt::WFlags f = 0);

    virtual ~KexiGradientWidget();

    virtual void setPaletteBackgroundPixmap(const QPixmap& pixmap) {
        p_backgroundPixmap = pixmap;
        p_rebuildDelayTimer.start(REBUILD_DELAY);
    }

    virtual const QColor& paletteBackgroundColor() const;

    /*!
    Set the displaymode \a mode.
    The widget will be updated automatically.
    */
    void setDisplayMode(DisplayMode mode) {
        p_displayMode = mode;
        p_cacheDirty = true;
        update();
    }

    /*!
    Get the current displaymode.
    */
    DisplayMode displayMode() const {
        return p_displayMode;
    }

    /*!
    Set the gradient-type.
    */
    void setGradientType(GradientType type) {
        p_gradientType = type;
        p_cacheDirty = true;
        update();
    }

    /*!
    Get the current gradient-type.
    */
    GradientType gradientType() const {
        return p_gradientType;
    }

    /*! Set color #1 for the gradient-effect.
    \a color is the new color. */
    void setGradientColor1(const QColor& color) {
        p_color1 = color;
        p_cacheDirty = true;
    }

    /*! Set color #2 for the gradient-effect.
    \a color is the new color. */
    void setGradientColor2(const QColor& color) {
        p_color2 = color;
        p_cacheDirty = true;
    }

    /*!
    Set both colors for the gradient.
    \a color1 is the first color,
    \a color2 the second.
    */
    void setGradientColors(const QColor& color1, const QColor& color2) {
        p_color1 = color1;
        p_color2 = color2;
        p_cacheDirty = true;
    }

    /*! \return the color #1 used for the gradient. */
    QColor gradientColor1() const {
        return p_color1;
    }

    /*! \return the color #2 used for the gradient. */
    QColor gradientColor2() const {
        return p_color2;
    }

    /*!
    Sets the opacity of the gradient when fading with background.
    \a opacity has to be between 0.0 and 1.0.
    */
    void setBlendOpacity(double opacity) {
        p_opacity = opacity;
        p_cacheDirty = true;
    }

    double blendOpacity() const {
        return p_opacity;
    }

public slots:
    virtual void setPaletteBackgroundColor(const QColor& color);

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual void enabledChange(bool enabled) {
        p_cacheDirty = true;
        QWidget::enabledChange(enabled);
    }

    virtual void paletteChange(const QPalette& pal) {
        p_cacheDirty = true;
        QWidget::paletteChange(pal);
    }

    virtual void paintEvent(QPaintEvent* e);

    virtual void resizeEvent(QResizeEvent* e) {
        p_rebuildDelayTimer.start(REBUILD_DELAY);
        QWidget::resizeEvent(e);
    }

    virtual void styleChange(QStyle& style) {
        p_cacheDirty = true;
        QWidget::styleChange(style);
    }

private:
    /*!
    Builds a list of children of \a p.
    Only widgets that work correctly with KexiGradientWidget
    will be in this list.
    The results will be stored in \a list.
    The method recursively calls itself until all children of \a p
    have been found and stored in the list.
    */
    static void buildChildrenList(WidgetList& list, QWidget* p);
    /*!
    \a return if the \a child is a widget that should
    get a background set.
    */
    static bool isValidChildWidget(QObject* child);

    /*!
    Rebuilds the cache completely.
    This is done automatically if necessary.
    */
    void rebuildCache();

    /*!
    Sets the background of \a childWidget.
    This is necessary when the child has been moved.
    For performance-reasons this is used only for Move-events.
    The same code is used for PaletteChange-events, but in a
    different location.
    */
    void updateChildBackground(QWidget* childWidget);

private:
    WidgetList p_knownWidgets;
    WidgetList p_customBackgroundWidgets;
    DisplayMode p_displayMode;
    GradientType p_gradientType;
    KPixmap p_backgroundPixmap;
    QColor p_color1;
    QColor p_color2;
    QTimer p_rebuildDelayTimer;
    QWidget* p_currentChild;
    double p_opacity;
    bool p_cacheDirty;

    QColor p_backgroundColor;

public slots:
    /*!
    The cache needs to be rebuild once the widget
    is set up completely.
    */
    virtual void polish() {
        QWidget::polish();
        rebuildCache();
    }

private slots:
    void setCacheDirty() {
        rebuildCache();
    }

};

#endif
