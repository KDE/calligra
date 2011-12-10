/* This file is part of the KDE project
   Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
   
   Based on qmenu_p.h from Qt 4.7

   based on oxygenhelper.h

   Copyright 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
   Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
   Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
   Copyright 2007 Casper Boemann <cbr@boemann.dk>
   Copyright 2007 Fredrik Höglund <fredrik@kde.org>

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

#ifndef KEXIMENUWIDGET_P_H
#define KEXIMENUWIDGET_P_H

#include <KComponentData>
#include <KSharedConfig>
#include <QStyleOption>
#include <QDateTime>
#include <QCache>
#include <QMap>
#include <QHash>
#include <QBasicTimer>
#include <QPointer>
#include <QAbstractButton>

class QEventLoop;

//! Used to define transparent clickable logo area
class ClickableLogoArea : public QAbstractButton
{
    Q_OBJECT
public:
    ClickableLogoArea(QWidget *parent = 0);
protected slots:
    void slotClicked();
protected:
    virtual void paintEvent(QPaintEvent*);
};

//used to walk up the popup list
struct KexiMenuWidgetCaused {
    QPointer<QWidget> widget;
    QPointer<QAction> action;
};

class OxygenHelper
{
public:
    OxygenHelper();
    virtual ~OxygenHelper();
    bool lowThreshold(const QColor &color);
    bool highThreshold(const QColor &color);
    virtual const QColor& backgroundTopColor(const QColor &color);
    virtual const QColor& backgroundBottomColor(const QColor &color);
    virtual const QColor& backgroundRadialColor(const QColor &color);
    virtual QPixmap verticalGradient(const QColor &color, int height, int offset = 0);
    virtual QPixmap radialGradient(const QColor &color, int width, int height = 64);
    void renderOxygenWindowBackground(QPainter *p, const QRect &clipRect,
                                         const QWidget *widget, const QWidget* window,
                                         const QColor& color, int y_shift=-23,
                                         int gradientHeight = 64);

    //!@name window background gradients
    //@{
    /*!
    \par y_shift: shift the background gradient upwards, to fit with the windec
    \par gradientHeight: the height of the generated gradient.
    for different heights, the gradient is translated so that it is always at the same position from the bottom
    */
    void renderWindowBackground(QPainter *p, const QRect &clipRect,
                                const QWidget *widget, const QPalette & pal,
                                int y_shift=-23, int gradientHeight = 64);

    void renderMenuBackground(QPainter* p, const QRect& clipRect, const QWidget* widget,
                              const QPalette& pal);
    void renderMenuBackground(QPainter*, const QRect&, const QWidget*, const QColor&);

    //! draw frame that mimics some sort of shadows around a panel
    /*! it is used for menus, detached dock panels and toolbar,
        as well as window decoration when compositing is disabled */
//     virtual void drawFloatFrame(
//         QPainter *p, const QRect r, const QColor &color,
//         bool drawUglyShadow=true, bool isActive=false,
//         const QColor &frameColor=QColor(),
//         TileSet::Tiles tiles = TileSet::Ring
//     );

    typedef QCache<quint64, QPixmap> PixmapCache;
    typedef QCache<quint64, QColor> ColorCache;
    typedef QMap<quint32, bool> ColorMap;

    PixmapCache m_backgroundCache;
    ColorCache m_backgroundTopColorCache;
    ColorCache m_backgroundBottomColorCache;
    ColorCache m_backgroundRadialColorCache;
    ColorMap m_lowThreshold;
    ColorMap m_highThreshold;
                                
    KComponentData _componentData;
    KSharedConfigPtr _config;
    qreal _contrast;
    qreal _bgcontrast;
};

class KexiMenuWidgetPrivate // : public QWidgetPrivate
{
public:
    KexiMenuWidgetPrivate(KexiMenuWidget *menu) : q(menu), itemsDirty(0), maxIconWidth(0), tabWidth(0), ncols(0),
                      collapsibleSeparators(true), activationRecursionGuard(false), hasHadMouse(0), aboutToHide(0), motions(0),
                      currentAction(0),
                      scroll(0), eventLoop(0), /*tearoff(0),*/ /*tornoff(0),*/ /*tearoffHighlighted(0),*/
                      hasCheckableItems(0), sloppyAction(0), /* doChildEffects(false)*/
                      hasFrame(true)
    {
    }
    virtual ~KexiMenuWidgetPrivate()
    {
        delete scroll;
        delete oxygenHelper;
    }
    void init();

    //static KexiMenuWidgetPrivate *get(KexiMenuWidgetPrivate *m) { return m->d_func(); }
    int scrollerHeight() const;

    KexiMenuWidget *q;

    //item calculations
    mutable uint itemsDirty : 1;
    mutable uint maxIconWidth, tabWidth;
    QRect actionRect(QAction *) const;

    mutable QVector<QRect> actionRects;
    mutable QHash<QAction *, QWidget *> widgetItems;
    void updateActionRects() const;
    QRect popupGeometry(const QWidget *widget) const;
    QRect popupGeometry(int screen = -1) const;
    mutable uint ncols : 4; //4 bits is probably plenty
    uint collapsibleSeparators : 1;

    bool activationRecursionGuard;

    //selection
    static KexiMenuWidget *mouseDown;
    QPoint mousePopupPos;
    uint hasHadMouse : 1;
    uint aboutToHide : 1;
    int motions;
    QAction *currentAction;
    QBasicTimer menuDelayTimer;
    QWidget *topCausedWidget() const;
    QAction *actionAt(QPoint p) const;
    void setFirstActionActive();
    void setCurrentAction(QAction *, int popup = -1, KexiMenuWidget::SelectionReason reason = KexiMenuWidget::SelectedFromElsewhere, bool activateFirst = false);
    //void popupAction(QAction *, int, bool);
    void setSyncAction();

    //scrolling support
    struct QMenuScroller {
        enum ScrollLocation { ScrollStay, ScrollBottom, ScrollTop, ScrollCenter };
        enum ScrollDirection { ScrollNone=0, ScrollUp=0x01, ScrollDown=0x02 };
        uint scrollFlags : 2, scrollDirection : 2;
        int scrollOffset;
        QBasicTimer scrollTimer;

        QMenuScroller() : scrollFlags(ScrollNone), scrollDirection(ScrollNone), scrollOffset(0) { }
        ~QMenuScroller() { }
    } *scroll;
    void scrollMenu(QMenuScroller::ScrollLocation location, bool active=false);
    void scrollMenu(QMenuScroller::ScrollDirection direction, bool page=false, bool active=false);
    void scrollMenu(QAction *action, QMenuScroller::ScrollLocation location, bool active=false);

    //synchronous operation (ie exec())
    QEventLoop *eventLoop;
    QPointer<QAction> syncAction;

    //search buffer
    QString searchBuffer;
    QBasicTimer searchBufferTimer;

    //passing of mouse events up the parent hierarchy
    QPointer<KexiMenuWidget> activeMenu;
    bool mouseEventTaken(QMouseEvent *);

    virtual QList<QPointer<QWidget> > calcCausedStack() const;
    KexiMenuWidgetCaused causedPopup;
    void hideUpToMenuBar();
    void hideMenu(KexiMenuWidget *menu, bool justRegister = false);

    //index mappings
    inline QAction *actionAt(int i) const { return q->actions().at(i); }
    inline int indexOf(QAction *act) const { return q->actions().indexOf(act); }

    //tear off support
//     uint tearoff : 1, tornoff : 1, tearoffHighlighted : 1;
//    QPointer<QTornOffMenu> tornPopup;

    mutable bool hasCheckableItems;

    //sloppy selection
    static int sloppyDelayTimer;
    mutable QAction *sloppyAction;
    QRegion sloppyRegion;

    //default action
    QPointer<QAction> defaultAction;

    QAction *menuAction;
    QAction *defaultMenuAction;

    void setOverrideMenuAction(QAction *);
    void overrideMenuActionDestroyed();

    //firing of events
    void activateAction(QAction *, QAction::ActionEvent, bool self=true);
    void activateCausedStack(const QList<QPointer<QWidget> > &, QAction *, QAction::ActionEvent, bool);

    void actionTriggered();
    void actionHovered();

    bool hasMouseMoved(const QPoint &globalPos);

    void setLayoutDirection_helper(Qt::LayoutDirection direction);

    void updateLayoutDirection();

    void setLayoutDirection_helper(QWidget* w, Qt::LayoutDirection direction);

    int frameWidth(const QStyleOption* opt = 0) const;

    bool actionPersistentlySelected(const QAction* action) const;

    void setActionPersistentlySelected(QAction* action, bool set);

    void toggleActionPersistentlySelected(QAction* action);

    //menu fading/scrolling effects
    //bool doChildEffects;

    QPointer<QAction> actionAboutToTrigger;
    QPointer<QWidget> noReplayFor;
    //! Frame visibility
    bool hasFrame;

    QPointer<KexiMenuWidgetAction> previousPersistentlySelectedAction;
    
    //! True if persistent selections are enabled. False by default.
    //bool persistentSelectionsEnabled;

    OxygenHelper *oxygenHelper;

    QPixmap calligraLogoPixmap;
    ClickableLogoArea *clickableLogoArea;
};

#endif // KEXIMENUWIDGET_P_H
