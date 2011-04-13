/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIMAINWINDOW_P_H
#define KEXIMAINWINDOW_P_H

#define KEXI_NO_PROCESS_EVENTS

#ifdef KEXI_NO_PROCESS_EVENTS
# define KEXI_NO_PENDING_DIALOGS
#endif

#define PROJECT_NAVIGATOR_TABBAR_ID 0
#define PROPERTY_EDITOR_TABBAR_ID 1

#include <KToolBar>
#include <KColorUtils>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPainter>
#include <QDesktopWidget>
#include <QKeyEvent>
 
#include <kexiutils/SmallToolButton.h>
class KexiProjectNavigator;

static const int KEXITABBEDTOOLBAR_FIRSTTAB_SPACING = 20;

static const int KEXITABBEDTOOLBAR_SPACER_TAB_INDEX = 1;

//! @short Main application's tabbed toolbar
class KexiTabbedToolBar : public KTabWidget
{
    Q_OBJECT
public:
    explicit KexiTabbedToolBar(QWidget *parent);
    virtual ~KexiTabbedToolBar();

    KToolBar *createWidgetToolBar() const;

    KToolBar *toolBar(const QString& name) const;

    void appendWidgetToToolbar(const QString& name, QWidget* widget);

    void setWidgetVisibleInToolbar(QWidget* widget, bool visible);
//    void removeWidgetFromToolbar(const QString& name);

//! @todo replace with the final Actions API
    void addAction(const QString& toolBarName, QAction *action);

    bool mainMenuVisible() const;
    
    QRect tabRect(int index) const { return tabBar()->tabRect(index); }

public slots:
    void setMainMenuContent(QWidget *w);
    void selectMainMenuItem(const char *actionName);
    void showMainMenu(const char* actionName = 0);
    void hideMainMenu();
    void toggleMainMenu();

protected:
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual bool eventFilter(QObject* watched, QEvent* event);

protected slots:
    void slotCurrentChanged(int index);
    void slotDelayedTabRaise();
    void slotSettingsChanged(int category);
    //! Used for delayed loading of the "create" toolbar. Called only once.
    void setupCreateWidgetToolbar();
    void slotTabDoubleClicked(int index);
    void tabBarAnimationFinished();

private:
    void addAction(KToolBar *tbar, const char* actionName);
    void addSeparatorAndAction(KToolBar *tbar, const char* actionName);

    class Private;
    Private * const d;
};

//! @internal window container created to speedup opening new tabs
class KexiWindowContainer : public QWidget
{
public:
    explicit KexiWindowContainer(QWidget* parent)
            : QWidget(parent)
            , window(0)
            , lyr(new QVBoxLayout(this)) {
        lyr->setContentsMargins(0, 0, 0, 0);
    }
    void setWindow(KexiWindow* w) {
        window = w;
        if (w)
            lyr->addWidget(w);
    }
    KexiWindow *window;
private:
    QVBoxLayout *lyr;
};

#ifdef KEXI_MODERN_STARTUP
#include <qimageblitz/qimageblitz.h>
#include <KFadeWidgetEffect>
#include <QStyleOptionMenuItem>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStackedLayout>
#include "KexiMenuWidget.h"

class EmptyMenuContentWidget : public QWidget
{
public:
    EmptyMenuContentWidget(QWidget* parent = 0)
     : QWidget(parent)
     , m_gradientVisible(true)
    {
        m_resizeEvent = true;
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_StaticContents, true);
    }
    void setGradientVisible(bool set) {
        if (m_gradientVisible != set) {
            m_gradientVisible = set;
            update();
        }
    }
protected:
    void paintEvent(QPaintEvent*) {
        if (m_gradientVisible && !m_buffer.isNull()) {
            QPainter p(this);
            p.drawPixmap(0, 0, m_buffer);
        }
    }
    void resizeEvent(QResizeEvent* event) {
        if (!m_resizeEvent)
            return;
        m_resizeEvent = false;
        QWidget::resizeEvent(event);
        //return;
        QWidget *mainWindow = KexiMainWindowIface::global()->thisWidget();
//         kDebug() << pos();
//         kDebug() << mainWindow->mapFromGlobal(pos());
//         kDebug() << mapTo(mainWindow, QPoint(0,0));
        QRect r(
            mapTo(mainWindow, QPoint(0,0)),
            size()
            //mainWindow->mapFromGlobal(mapToGlobal(pos())),
            //mainWindow->mapFromGlobal(mapToGlobal(rect().bottomRight()))
        );
        //QImage img(event->size(), QImage::Format_ARGB32);
        //img.fill(QColor(Qt::transparent).rgb());
        m_buffer = QPixmap(event->size());
        m_buffer.fill(Qt::transparent);
        //mainWindow->render(&img, QPoint(0, 0), QRegion(r));
        //QColor fillColor(palette().color(QPalette::Mid /*Window*/));
        //Blitz::fade(img, 0.8, fillColor);
        //img = Blitz::blur(img, 3);
        
        int gwidth = m_buffer.height();
        int xoffset = gwidth / 2;
        QRadialGradient rgrad(-xoffset, m_buffer.height() / 2, gwidth);
        rgrad.setColorAt(0.0, palette().color(QPalette::Dark));
        rgrad.setColorAt(1.0, Qt::transparent);
        QPainter p(&m_buffer);
        p.fillRect(QRect(0, 0, gwidth - xoffset, m_buffer.height()), QBrush(rgrad));
        
        //m_buffer = QPixmap::fromImage(img);
        m_resizeEvent = true;
    }
private:
    QPixmap m_buffer;
    bool m_resizeEvent;
    bool m_gradientVisible;
};

class TopLineKexiMainMenuWidget : public QWidget
{
public:
    TopLineKexiMainMenuWidget(QWidget* parent)
     : QWidget(parent) 
    {
        //setAutoFillBackground(true);
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setContentsMargins(0, 0, 0, 0);
    }
protected:
    void paintEvent(QPaintEvent*) {
        if (m_buffer.size() != size()) {
            //pal.setBrush(m_topLine->backgroundRole(), QBrush(gr));
            //m_topLineImg = QImage(m_topLine->size());
            //m_topLineImg.fill(Qt::transparent.rgb());
            //QPainter topLinePainter(&m_topLineImg);
            QImage img(width(), height(), QImage::Format_ARGB32_Premultiplied);
            img.fill(QColor(Qt::transparent).rgba());
            QPainter p(&img);
            
            QColor c(palette().color(QPalette::Highlight));
            QLinearGradient gr(0, 0, 0, height() - 1);
            gr.setColorAt(0.0, c);
            gr.setColorAt(2.0 / qreal(height()), c);
            gr.setColorAt(1.0, Qt::transparent);
            p.fillRect(rect(), QBrush(gr));
            
            if (style()->objectName() == "oxygen") {
                p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                QLinearGradient gr2(0, 0, 100, 0);
                QColor wcol(Qt::white);
                wcol.setAlpha(0);
                gr2.setColorAt(0.0, wcol);
                gr2.setColorAt(1.0, Qt::white);
                p.fillRect(QRect(0, 0, 100, height() - 1), QBrush(gr2));
            }
            m_buffer = QPixmap::fromImage(img);
        }
        QPainter p(this);
        p.drawPixmap(0, 0, m_buffer);
        //m_topLine->setPalette(pal);
    }
private:
    QPixmap m_buffer;
};

class KexiFadeWidgetEffect : public KFadeWidgetEffect
{
    Q_OBJECT
public:
    KexiFadeWidgetEffect(QWidget *destWidget, int duration = 250)
    : KFadeWidgetEffect(destWidget)
    , m_duration(duration)
    {
    }
public slots:
    void start() { KFadeWidgetEffect::start(m_duration); }
private:
    int m_duration;
};

//! Main menu
class KexiMainMenu : public QWidget
{
    Q_OBJECT
public:
    KexiMainMenu(KexiTabbedToolBar *toolBar, QWidget* parent = 0) : QWidget(parent),
        m_topLineHeight(5), m_toolBar(toolBar), m_initialized(false)
    {
        //setAutoFillBackground(true); // to cover the lower layer
        m_content = 0;
        m_topLineSpacer = 0;
        m_topLine = 0;
        m_selectFirstItem = false;
    }
    ~KexiMainMenu() {
        delete (QWidget*)m_contentWidget;
    }
    virtual bool eventFilter(QObject * watched, QEvent* event) {
        // kDebug() << m_contentWidget;
        if (event->type() == QEvent::MouseButtonPress && watched == m_content && !m_contentWidget) {
            emit contentAreaPressed();
        }
        else if (event->type() == QEvent::KeyPress) {
            QKeyEvent* ke = static_cast<QKeyEvent*>(event);
            if ((ke->key() == Qt::Key_Escape) && ke->modifiers() == Qt::NoModifier) {
                emit hideContentsRequested();
                return true;
            }
        }
        return QWidget::eventFilter(watched, event);
    }

    void setContent(QWidget *contentWidget) {
        if (m_menuWidget && m_persistentlySelectedAction) {
            m_menuWidget->setPersistentlySelectedAction(m_persistentlySelectedAction, 
                                                        m_persistentlySelectedAction->persistentlySelected());
        }
        /*if (m_menuWidget->persistentlySelectedAction())
            kDebug() << "****" << m_menuWidget->persistentlySelectedAction()->objectName();*/
        KexiFadeWidgetEffect *fadeEffect = 0;
        
        if (m_contentWidget && contentWidget) {
            fadeEffect = new KexiFadeWidgetEffect(m_content);
        }
        delete (QWidget*)m_contentWidget;
        m_contentWidget = contentWidget;
        if (m_content) {
            m_content->setGradientVisible(!m_contentWidget);
            if (m_contentOpacityAnimation) {
                m_contentOpacityAnimation->stop();
                m_effect->setOpacity(1.0);
            }
        }
        if (m_contentWidget) {
            m_topLineSpacer->show();

            QPalette contentWidgetPalette(m_contentWidget->palette());
            contentWidgetPalette.setBrush(QPalette::Window, contentWidgetPalette.brush(QPalette::Base));
            contentWidgetPalette.setBrush(QPalette::WindowText, contentWidgetPalette.brush(QPalette::Text));
            m_contentWidget->setPalette(contentWidgetPalette);

            m_contentWidget->setAutoFillBackground(true);
            m_contentWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
            m_contentWidget->setContentsMargins(0, 0, 0, 0);
            m_contentLayout->addWidget(m_contentWidget);
            m_contentLayout->setCurrentWidget(m_contentWidget);
            m_contentWidget->setFocus();
            m_contentWidget->installEventFilter(this);
        }
        else {
            if (m_topLineSpacer) {
                m_topLineSpacer->hide();
            }
//            if (m_menuWidget->persistentlySelectedAction())
//                 m_menuWidget->persistentlySelectedAction()->setPersistentlySelected(false);
        }
        if (m_topLine)
            m_topLine->raise();
        if (fadeEffect) {
            if (m_contentWidget)
                m_contentLayout->update();
            
            QTimer::singleShot(10, fadeEffect, SLOT(start()));
        }
    }

    const QWidget *contentWidget() const {
        return m_contentWidget;
    }

    void updateTopLineGeometry()
    {
        int tab0width = style()->objectName() == "oxygen"
            ? m_toolBar->tabRect(0).width() : 0;
        m_topLine->setGeometry(
            tab0width, 0, width() - 1 - tab0width, m_topLineHeight);
    }

    void setPersistentlySelectedAction(KexiMenuWidgetAction* action, bool set)
    {
        m_persistentlySelectedAction = action;
        m_persistentlySelectedAction->setPersistentlySelected(set);
    }

/*    void setActiveAction(QAction* action = 0) {
        if (!action && !m_menuWidget->actions().isEmpty()) {
            action = actions().first();
        }
        if (action) {
            m_menuWidget->setActiveAction(action);
        }
    }*/

    void selectFirstItem() {
        m_selectFirstItem = true;
    }

signals:
    void contentAreaPressed();
    void hideContentsRequested();

protected:
    virtual void showEvent(QShowEvent * event) {
        if (!m_initialized) {
            m_initialized = true;
            KActionCollection *ac = KexiMainWindowIface::global()->actionCollection();
            QHBoxLayout *hlyr = new QHBoxLayout(this);
            hlyr->setSpacing(0);
            hlyr->setMargin(0);
            m_menuWidget = new KexiMenuWidget;
            m_menuWidget->installEventFilter(this);
            m_menuWidget->setFocusPolicy(Qt::StrongFocus);
            setFocusProxy(m_menuWidget);
            m_menuWidget->setFrame(false);
            m_menuWidget->setAutoFillBackground(true);
            int leftmargin, topmargin, rightmargin, bottommargin;
            m_menuWidget->getContentsMargins(&leftmargin, &topmargin, &rightmargin, &bottommargin);
            topmargin += m_topLineHeight;
            m_menuWidget->setContentsMargins(leftmargin, topmargin, rightmargin, bottommargin);

            m_menuWidget->addAction(ac->action("project_new"));
            m_menuWidget->addAction(ac->action("project_open"));
            //menu->addAction(new KexiMenuWidgetAction(KStandardAction::New, this));
            //menu->addAction(new KexiMenuWidgetAction(KStandardAction::Open, this));
            //menu->setActiveAction(ac->action("project_open"));
            m_menuWidget->addAction(ac->action("project_open_recent"));
            m_menuWidget->addSeparator();
            //menu->addAction(ac->action("project_save"));
            //menu->addAction(ac->action("project_saveas"));
            m_menuWidget->addAction(ac->action("project_close"));
            m_menuWidget->addSeparator();
            //menu->addAction(ac->action("tools_import_project"));
            m_menuWidget->addAction(ac->action("project_import_export_send"));
            m_menuWidget->addAction(ac->action("project_properties"));
            // todo: project information
            m_menuWidget->addAction(ac->action("settings"));
            m_menuWidget->addAction(ac->action("quit"));
            //menu->setFixedWidth(300);
            hlyr->addWidget(m_menuWidget);
            m_content = new EmptyMenuContentWidget;
            m_content->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
            //m_content->setAutoFillBackground(true);
            m_content->installEventFilter(this);
            m_mainContentLayout = new QVBoxLayout;
            hlyr->addLayout(m_mainContentLayout);
            m_topLineSpacer = new QWidget;
            m_topLineSpacer->setFixedHeight(m_topLineHeight);
            m_mainContentLayout->addWidget(m_topLineSpacer); // offset to avoid blinking top line
            m_topLineSpacer->hide();
            m_contentLayout = new QStackedLayout(m_content);
            m_contentLayout->setStackingMode(QStackedLayout::StackAll);
            m_contentLayout->setContentsMargins(0, 0, 0, 0);
            //QLabel *l;
            //test setContent(l = new QLabel("aaaaaaaaaaaa..........a.aa.a.a....."));
            //l->setAutoFillBackground(true);

            m_effect = new QGraphicsOpacityEffect(m_content);
            m_effect->setOpacity(0.4);
            m_content->setGraphicsEffect(m_effect);
            m_mainContentLayout->addWidget(m_content);
            hlyr->setStretchFactor(m_mainContentLayout, 1);

            m_contentOpacityAnimation = new QPropertyAnimation(m_effect, "opacity", m_effect);
            m_contentOpacityAnimation->setDuration(200);
            m_contentOpacityAnimation->setStartValue(m_effect->opacity());
            m_contentOpacityAnimation->setEndValue(1.0);

            m_topLine = new TopLineKexiMainMenuWidget(this);
            updateTopLineGeometry();
            m_topLine->show();
            m_topLine->raise();
        }
        m_contentOpacityAnimation->start();
        QWidget::showEvent(event);
        if (m_selectFirstItem && !m_menuWidget->actions().isEmpty()) {
            QAction* action = m_menuWidget->actions().first();
            m_menuWidget->setActiveAction(action);
            m_selectFirstItem = false;
        }
    }

//     virtual void hideEvent(QHideEvent * event) {
//         if (m_menuWidget->persistentlySelectedAction())
//             m_menuWidget->persistentlySelectedAction()->setPersistentlySelected(false);
//         QWidget::hideEvent(event);
//     }

private:
    QPointer<KexiMenuWidget> m_menuWidget;
    const int m_topLineHeight;
    KexiTabbedToolBar* m_toolBar;
    bool m_initialized;
    EmptyMenuContentWidget *m_content;
    QStackedLayout *m_contentLayout;
    QPointer<QWidget> m_contentWidget;
    TopLineKexiMainMenuWidget *m_topLine;
    QWidget* m_topLineSpacer;
    QPointer<QGraphicsOpacityEffect> m_effect;
    QPointer<QPropertyAnimation> m_contentOpacityAnimation;
    QVBoxLayout* m_mainContentLayout;
    QPointer<KexiMenuWidgetAction> m_persistentlySelectedAction;
    bool m_selectFirstItem;
};

class KexiTabbedToolBarTabBar;
#endif // KEXI_MODERN_STARTUP

//! @internal
class KexiTabbedToolBar::Private : public QObject
{
    Q_OBJECT
public:
    explicit Private(KexiTabbedToolBar *t)
            : q(t), createWidgetToolBar(0)
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
            , tabToRaise(-1)
#endif
            , rolledUp(false)
    {
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
        tabRaiseTimer.setSingleShot(true);
        tabRaiseTimer.setInterval(300);
#endif
        tabBarAnimation.setPropertyName("opacity");
        tabBarAnimation.setDuration(500);
        connect(&tabBarAnimation, SIGNAL(finished()), q, SLOT(tabBarAnimationFinished()));
    }

    KToolBar *createToolBar(const char *name, const QString& caption);

#ifdef KEXI_MODERN_STARTUP
public slots:
    void showMainMenu(const char* actionName = 0);
    void hideMainMenu();
    void hideContentsOrMainMenu();
    void toggleMainMenu();
    void updateMainMenuGeometry();

public:
    KexiTabbedToolBarTabBar *customTabBar;
    QPointer<KexiMainMenu> mainMenu;
#endif

    KexiTabbedToolBar *q;
    KActionCollection *ac;
    int createId;
    KToolBar *createWidgetToolBar;
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
    //! Used for delayed tab raising
    int tabToRaise;
    //! Used for delayed tab raising
    QTimer tabRaiseTimer;
#endif
    //! Toolbars for name
    QHash<QString, KToolBar*> toolbarsForName;
    QHash<QWidget*, QAction*> extraActions;
    bool rolledUp;
    QPropertyAnimation tabBarAnimation;
    QGraphicsOpacityEffect tabBarOpacityEffect;
    int rolledUpIndex;
};

#ifdef KEXI_MODERN_STARTUP
#include <kexiutils/styleproxy.h>
#include <KTabBar>
#include <QTabBar>
#include <QPainter>

class KexiTabbedToolBarStyle;
//! Tab bar reimplementation for KexiTabbedToolBar.
/*! The main its purpose is to alter the width of "Kexi" tab. 
*/
class KexiTabbedToolBarTabBar : public KTabBar
{
    Q_OBJECT
public:
    explicit KexiTabbedToolBarTabBar(QWidget *parent = 0);
    virtual QSize originalTabSizeHint(int index) const;
    virtual QSize tabSizeHint(int index) const;

    KexiTabbedToolBarStyle* customStyle;
};

//! Style proxy for KexiTabbedToolBar, to get the "Kexi" tab style right.
class KexiTabbedToolBarStyle : public KexiUtils::StyleProxy
{
public:
    explicit KexiTabbedToolBarStyle(QStyle *style) : KexiUtils::StyleProxy(style) {
        const QString kexiBlackFname
            = KStandardDirs::locate("data", "kexi/pics/kexi-logo.png");
        kexiBlackPixmap = QPixmap(kexiBlackFname);
    }
    virtual ~KexiTabbedToolBarStyle() {
    }
    virtual void drawControl(ControlElement element, const QStyleOption *option,
                             QPainter *painter, const QWidget *widget = 0) const
    {
        qreal origOpacity = -1.0;
        if (element == CE_TabBarTab) {
            const QStyleOptionTabV2* opt
                = qstyleoption_cast<const QStyleOptionTabV2*>(option);
            const QTabBar* tabBar = qobject_cast<const QTabBar*>(widget);
            KexiTabbedToolBar* tbar = tabBar
                ? qobject_cast<KexiTabbedToolBar*>(tabBar->parentWidget()) : 0;
            if (opt && tbar) {
                const int index = tabBar->tabAt(opt->rect.center());
                if (index == KEXITABBEDTOOLBAR_SPACER_TAB_INDEX)
                    return;
                bool mouseOver = opt->state & QStyle::State_MouseOver;
                if (!mouseOver
                    && (!(opt->state & State_Selected) || tbar->mainMenuVisible())
                    && index > 0)
                {
                    QStyleOptionTabV2 newOpt(*opt);
                    if (tbar->mainMenuVisible())
                        newOpt.state &= ~QStyle::State_HasFocus;
                    KexiUtils::StyleProxy::drawControl(CE_TabBarTabLabel, &newOpt, painter, widget);
                    return;
                }
                else if (index == 0) {
                    //kDebug() << "RECT:" << opt->rect << tabBar->tabAt(opt->rect.center());
                    QStyleOptionTabV2 newOpt(*opt);
                    QBrush bg;
                    newOpt.state |= State_Selected;
                    if (tbar->mainMenuVisible()) {
                        bg = newOpt.palette.brush(QPalette::Highlight);
                    }
                    else {
                        /*bg = newOpt.palette.brush(QPalette::Dark);
                        if (mouseOver) {
                            bg = KColorUtils::mix(
                                newOpt.palette.color(QPalette::Highlight), bg.color());
                        }
                        */
/*                        if (mouseOver) {
                        }
                        else {*/
                            bg = Qt::transparent;
//                         }
                    }
                    newOpt.text.clear();
                    //newOpt.rect.setHeight(newOpt.rect.height() + 15);
                    newOpt.palette.setBrush(QPalette::Window, bg);
                    newOpt.palette.setBrush(QPalette::Button, // needed e.g. for Plastique style
                                            bg);
/*                    newOpt.palette.setBrush(QPalette::WindowText,
                                            newOpt.palette.brush(QPalette::HighlightedText));
                    newOpt.palette.setBrush(QPalette::ButtonText, // needed e.g. for Plastique style
                                            newOpt.palette.brush(QPalette::HighlightedText));*/
                    //newOpt.rect.setWidth(newOpt.rect.width() - KEXITABBEDTOOLBAR_FIRSTTAB_SPACING);
                    KexiUtils::StyleProxy::drawControl(element, &newOpt, painter, widget);
                    painter->drawPixmap(
                        newOpt.rect.center()
                        - QPoint(kexiBlackPixmap.width(), kexiBlackPixmap.height()) / 2,
                        kexiBlackPixmap);
                    if (!mouseOver)
                        return;
                }
                if (index > 0 || mouseOver) {
                    QStyleOptionTabV2 newOpt(*opt);
                    QColor hc(newOpt.palette.color(QPalette::Highlight));
                    //hc.setAlpha(150);
                    newOpt.palette.setBrush(QPalette::Window, hc);
                    newOpt.palette.setBrush(QPalette::Button, hc); // needed e.g. for Plastique style
                    if (mouseOver && (index != tbar->currentIndex() || tbar->mainMenuVisible())) {
                        // use lower opacity for diplaying hovered tabs
                        origOpacity = painter->opacity();
                        painter->setOpacity(0.3);
                        newOpt.state |= State_Selected;
                    }
                    else {
                        if (parentStyle()->objectName() != "oxygen") {
                            newOpt.palette.setBrush(QPalette::WindowText,
                                                    newOpt.palette.brush(QPalette::HighlightedText));
                            newOpt.palette.setBrush(QPalette::ButtonText,
                                                    newOpt.palette.brush(QPalette::HighlightedText));
                        }
                    }
                    if (index == 0)
                        newOpt.text.clear();
                    KexiUtils::StyleProxy::drawControl(element, &newOpt, painter, widget);
                    if (origOpacity != -1.0) {
                        // restore opacity and draw labels using full this opacity
                        painter->setOpacity(origOpacity);
                        KexiUtils::StyleProxy::drawControl(CE_TabBarTabLabel, &newOpt, painter, widget);
                    }
                    return;
                }
            }
        }
        else if (element == CE_ToolBar) {
            return;
        }
        //if (element == CE_TabBarTabShape)
        //    return;
        //if (element == CE_TabBarTabLabel)
        //    return;
        KexiUtils::StyleProxy::drawControl(element, option, painter, widget);
    }

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                          QPainter *painter, const QWidget *widget = 0) const
    {
        if (element == PE_FrameTabWidget) {
            return;
        }
        if (element == QStyle::PE_PanelToolBar || element == QStyle::PE_FrameMenu) {
            return;
        }
        KexiUtils::StyleProxy::drawPrimitive(element, option, painter, widget);
    }

    virtual int pixelMetric(PixelMetric metric, const QStyleOption* option = 0,
                            const QWidget* widget = 0) const
    {
        if (metric == QStyle::PM_SmallIconSize)
            return KIconLoader::SizeMedium;
        return KexiUtils::StyleProxy::pixelMetric(metric, option, widget);
    }

    QPixmap kexiBlackPixmap;
    QPixmap kexiWhitePixmap;
};

//! Tab bar reimplementation for KexiTabbedToolBar.
/*! The main its purpose is to alter the width of "Kexi" tab. 
*/
KexiTabbedToolBarTabBar::KexiTabbedToolBarTabBar(QWidget *parent)
    : KTabBar(parent)
{
    setObjectName("tabbar");
    customStyle = new KexiTabbedToolBarStyle(style());
    customStyle->setParent(this);
    setStyle(customStyle);
    installEventFilter(parent);
    QWidget *mainWindow = KexiMainWindowIface::global()->thisWidget();
    mainWindow->installEventFilter(parent);
    setAttribute(Qt::WA_Hover, true);
}

QSize KexiTabbedToolBarTabBar::originalTabSizeHint(int index) const
{
    return KTabBar::tabSizeHint(index);
}

QSize KexiTabbedToolBarTabBar::tabSizeHint(int index) const
{
    if (index == 0) {
        QSize s = KTabBar::tabSizeHint(index);
        s.setWidth(customStyle->kexiBlackPixmap.width()
                   + 10/*left*/ + 8/*right*/);
        s.setHeight(qMax(s.height(), customStyle->kexiBlackPixmap.height() + 3));
        return s;
    }
    return KTabBar::tabSizeHint(index);
}

void KexiTabbedToolBar::Private::toggleMainMenu()
{
    if (q->mainMenuVisible())
        hideMainMenu();
    else
        showMainMenu();
}

void KexiTabbedToolBar::Private::showMainMenu(const char* actionName)
{
    q->tabBar()->update(q->tabRect(q->tabBar()->currentIndex()));
    q->tabBar()->update(q->tabRect(0));

    QWidget *mainWindow = KexiMainWindowIface::global()->thisWidget();
    if (!mainMenu) {
        mainMenu = new KexiMainMenu(q, mainWindow);
        connect(mainMenu, SIGNAL(contentAreaPressed()), this, SLOT(hideMainMenu()));
        connect(mainMenu, SIGNAL(hideContentsRequested()), this, SLOT(hideContentsOrMainMenu()));
    }
    updateMainMenuGeometry();
    if (actionName) {
        q->selectMainMenuItem(actionName);
    }
    else {
        mainMenu->selectFirstItem();
    }
    //mainMenu->setContent(0);
    mainMenu->show();
    mainMenu->setFocus();
}

void KexiTabbedToolBar::Private::updateMainMenuGeometry()
{
    if (!mainMenu)
        return;
    QWidget *mainWindow = KexiMainWindowIface::global()->thisWidget();
    KexiTabbedToolBarTabBar *tabBar = static_cast<KexiTabbedToolBarTabBar*>(q->tabBar());
    QPoint pos = q->mapToGlobal(QPoint(0, tabBar->originalTabSizeHint(0).height() - 1));
//     kDebug() << "1." << pos;
    pos = mainWindow->mapFromGlobal(pos);
//     kDebug() << "2." << pos;
//     kDebug() << "3." << q->pos();

    QStyleOptionTab ot;
    ot.initFrom(tabBar);
    int overlap = tabBar->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &ot, tabBar) - 2;
//     kDebug() << "4. overlap=" << overlap;

    mainMenu->setGeometry(0, pos.y() - overlap /*- q->y()*/,
                          mainWindow->width(),
                          mainWindow->height() - pos.y() + overlap /*+ q->y()*/);
    
/*    QRect scr(QApplication::desktop()->screenGeometry(q));
    if (mainMenu->width() < mainWindow->width()) {
        if (scr.width() < mainWindow->width()) {
            scr.setWidth(mainWindow->width() * 3 / 2);
        }
        mainMenu->setGeometry(0, pos.y() - overlap, qMax(scr.width(), mainWindow->width()), 
                            mainWindow->height() - pos.y() + overlap);
    }*/
}

void KexiTabbedToolBar::Private::hideMainMenu()
{
    if (!mainMenu || !mainMenu->isVisible())
        return;
    q->tabBar()->update(q->tabRect(q->tabBar()->currentIndex()));
    q->tabBar()->update(q->tabRect(0));
    mainMenu->hide();
    //mainMenu->deleteLater();
    mainMenu->setContent(0);
}

void KexiTabbedToolBar::Private::hideContentsOrMainMenu()
{
    if (!mainMenu || !mainMenu->isVisible())
        return;
    if (mainMenu->contentWidget()) {
        mainMenu->setContent(0);
    }
    else {
        hideMainMenu();
    }
}

#endif // KEXI_MODERN_STARTUP

KToolBar *KexiTabbedToolBar::Private::createToolBar(const char *name, const QString& caption)
{
    KToolBar *tbar = new KToolBar(q);
#ifdef KEXI_MODERN_STARTUP
    // needed e.g. for Windows style to remove the toolbar's frame
    tbar->setStyle(customTabBar->customStyle);
#endif
    toolbarsForName.insert(name, tbar);
    tbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tbar->setObjectName(name);
    q->addTab(tbar, caption);
    return tbar;
}

KexiTabbedToolBar::KexiTabbedToolBar(QWidget *parent)
        : KTabWidget(parent)
        , d(new Private(this))
{
#ifdef KEXI_MODERN_STARTUP
    d->customTabBar = new KexiTabbedToolBarTabBar(this);
    setTabBar(d->customTabBar);
    setStyle(d->customTabBar->customStyle);
#endif
    // from ktabwidget.cpp
    //! @todo KTabWidget::setTabBar() should be added with this code
    connect(tabBar(), SIGNAL(contextMenu( int, const QPoint & )), SLOT(contextMenu( int, const QPoint & )));
    connect(tabBar(), SIGNAL(tabDoubleClicked( int )), SLOT(mouseDoubleClick( int )));
    connect(tabBar(), SIGNAL(newTabRequest()), this, SIGNAL(mouseDoubleClick())); // #185487
    connect(tabBar(), SIGNAL(mouseMiddleClick( int )), SLOT(mouseMiddleClick( int )));
    connect(tabBar(), SIGNAL(initiateDrag( int )), SLOT(initiateDrag( int )));
    connect(tabBar(), SIGNAL(testCanDecode(const QDragMoveEvent *, bool & )), SIGNAL(testCanDecode(const QDragMoveEvent *, bool & )));
    connect(tabBar(), SIGNAL(receivedDropEvent( int, QDropEvent * )), SLOT(receivedDropEvent( int, QDropEvent * )));
    connect(tabBar(), SIGNAL(moveTab( int, int )), SLOT(moveTab( int, int )));
    connect(tabBar(), SIGNAL(tabCloseRequested( int )), SLOT(closeRequest( int )));

    setMouseTracking(true); // for mouseMoveEvent()
    setWhatsThis(i18n("Task-based tabbed toolbar groups commands for application using tabs."));
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
    connect(&d->tabRaiseTimer, SIGNAL(timeout()), this, SLOT(slotDelayedTabRaise()));
#endif
    connect(tabBar(), SIGNAL(tabDoubleClicked(int)), this, SLOT(slotTabDoubleClicked(int)));

    d->ac = KexiMainWindowIface::global()->actionCollection();
    const bool userMode = KexiMainWindowIface::global()->userMode();
    KToolBar *tbar;
    QAction* a;

    KexiUtils::smallFont(this/*init*/);
    slotSettingsChanged(KGlobalSettings::FontChanged);
    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), this, SLOT(slotSettingsChanged(int)));

// KToolBar* helpToolBar = new KToolBar(this);
// helpToolBar->setFont(Kexi::smallFont());
    /*
      helpToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      helpToolBar->setIconSize( QSize(IconSize(KIconLoader::Small), IconSize(KIconLoader::Small)) );
      a = d->ac->action("help_whats_this");
      helpToolBar->addAction(a);
      a = d->ac->action("help_contents");
      helpToolBar->addAction(a);
      setCornerWidget(helpToolBar, Qt::TopRightCorner);*/

    QWidget *helpWidget = new QWidget(this);
    helpWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QHBoxLayout *helpLyr = new QHBoxLayout(helpWidget);
    helpLyr->setContentsMargins(0, 0, 0, 2);
    helpLyr->setSpacing(2);
    a = d->ac->action("help_whats_this");
    helpLyr->addWidget(new KexiSmallToolButton(a, helpWidget));
    a = d->ac->action("help_contents");
    helpLyr->addWidget(new KexiSmallToolButton(a, helpWidget));
    setCornerWidget(helpWidget, Qt::TopRightCorner);

#ifdef KEXI_MODERN_STARTUP
    // needed e.g. for Windows style to remove the toolbar's frame
    QWidget *dummyWidgetForMainMenu = new QWidget(this);
    dummyWidgetForMainMenu->setObjectName("kexi");
    addTab(dummyWidgetForMainMenu, i18nc("File menu", "&File"));
    addTab(new QWidget(this), QString());
#else
    tbar = d->createToolBar("kexi", i18nc("Application name as menu entry", "Kexi"));
    addAction(tbar, "options_configure");
    addAction(tbar, "options_configure_keybinding");
    addSeparatorAndAction(tbar, "help_about_app");
    addAction(tbar, "help_about_kde");
# ifdef KEXI_NO_REPORTBUG_COMMAND
    //remove "bug report" action to avoid confusion for with commercial technical support
    addSeparatorAndAction(tbar, "help_report_bug");
# endif
    addSeparatorAndAction(tbar, "quit");
#endif // KEXI_MODERN_STARTUP

#ifndef KEXI_MODERN_STARTUP
    tbar = d->createToolBar("project", i18n("Project"));
    addAction(tbar, "project_new");
    addAction(tbar, "project_open");
//! @todo re-add    addAction(tbar, "project_print");
//! @todo re-add    addAction(tbar, "project_print_preview");
//! @todo re-add    addAction(tbar, "project_print_setup");
    //no "project_save" here...
//! @todo re-add    addSeparatorAndAction(tbar, "project_saveas");
//! @todo re-add    addSeparatorAndAction(tbar, "project_properties");
    addSeparatorAndAction(tbar, "project_close");
#endif

    if (!userMode) {
        d->createWidgetToolBar = d->createToolBar("create", i18n("Create"));
    }

    tbar = d->createToolBar("data", i18n("Data"));
    addAction(tbar, "edit_cut");
    addAction(tbar, "edit_copy");
//moved to project navigator    addAction(tbar, "edit_copy_special_data_table");
    addAction(tbar, "edit_paste");
    if (!userMode)
        addAction(tbar, "edit_paste_special_data_table");
    tbar->addSeparator();
// todo move undo/redo to quickbar:
/* moved to local toolbars
    addAction(tbar, "edit_undo");
    addAction(tbar, "edit_redo");
    tbar->addSeparator();*/
    addAction(tbar, "edit_find");

    tbar = d->createToolBar("external", i18n("External Data"));

    /*   QGroupBox *gbox = new QGroupBox( i18n("Import"), tbar );
      gbox->setFlat(true);
      gbox->setFont(Kexi::smallFont(this));
      tbar->addWidget( gbox );
      QVBoxLayout *gbox_lyr = new QVBoxLayout(gbox);
      gbox_lyr->setContentsMargins(0,0,0,0);
      gbox_lyr->setSpacing(2); //ok?
      QToolBar *sub_tbar = new KToolBar(gbox);
      gbox_lyr->addWidget(sub_tbar);
    */
    if (!userMode) {
        addAction(tbar, "project_import_data_table");
        addAction(tbar, "tools_import_tables");
    }
    /*   gbox = new QGroupBox( i18n("Export"), tbar );
      gbox->setFlat(true);
      gbox->setFont(Kexi::smallFont(this));
      tbar->addWidget( gbox );
      gbox_lyr = new QVBoxLayout(gbox);
      gbox_lyr->setContentsMargins(0,0,0,0);
      gbox_lyr->setSpacing(2); //ok?
      sub_tbar = new KToolBar(gbox);
      gbox_lyr->addWidget(sub_tbar);
    */
    addSeparatorAndAction(tbar, "project_export_data_table");

    tbar = d->createToolBar("tools", i18n("Tools"));
    addAction(tbar, "tools_import_project");
    addAction(tbar, "tools_compact_database");

//! @todo move to form plugin
    tbar = d->createToolBar("form", i18n("Form Design"));

//! @todo move to report plugin
    tbar = d->createToolBar("report", i18n("Report Design"));
    
// tbar = new KToolBar(this);
// addTab( tbar, i18n("Settings") );
//moved up addAction(tbar, "options_configure_keybinding");

    /*moved up
      tbar = new KToolBar(this);
      addTab( tbar, i18n("Help") );
      addAction(tbar, "help_contents");
      addAction(tbar, "help_whats_this");
      addAction(tbar, "help_report_bug");
      a = d->ac->action("help_report_bug");
      a->setIcon(KIcon("tools-report-bug"));
      addAction(tbar, "help_about_app");
      addAction(tbar, "help_about_kde");
    */

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentChanged(int)));
    /*tmp
      QPalette pal(palette());
      QBrush bg( pal.brush( backgroundRole() ) );
      bg.setColor( Qt::red ); //pal.color( QPalette::Button ) );
      pal.setColor( QPalette::Window, Qt::red );
      setPalette( pal );*/

    setCurrentWidget(widget(KEXITABBEDTOOLBAR_SPACER_TAB_INDEX + 1)); // the default
    setFocusPolicy(Qt::NoFocus);
}

KexiTabbedToolBar::~KexiTabbedToolBar()
{
    delete d;
}

bool KexiTabbedToolBar::mainMenuVisible() const
{
    return d->mainMenu && d->mainMenu->isVisible();
}

void KexiTabbedToolBar::slotSettingsChanged(int category)
{
    if (category == KGlobalSettings::FontChanged) {
        setFont(KGlobalSettings::menuFont());   // toolbar acts like a menu
    }
}

KToolBar* KexiTabbedToolBar::createWidgetToolBar() const
{
    return d->createWidgetToolBar;
}

void KexiTabbedToolBar::mouseMoveEvent(QMouseEvent* event)
{
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
    QPoint p = event->pos();
    int tab = tabBar()->tabAt(p);
    if (d->tabToRaise != -1 && (tab == -1 || tab == currentIndex())) {
        d->tabRaiseTimer.stop();
        d->tabToRaise = -1;
    } else if (d->tabToRaise != tab) {
        d->tabRaiseTimer.start();

        d->tabToRaise = tab;
    }
#endif
    KTabWidget::mouseMoveEvent(event);
}

void KexiTabbedToolBar::leaveEvent(QEvent* event)
{
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
    d->tabRaiseTimer.stop();
    d->tabToRaise = -1;
#endif
    KTabWidget::leaveEvent(event);
}

bool KexiTabbedToolBar::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QWidget *mainWin = KexiMainWindowIface::global()->thisWidget();
        if (watched == tabBar()) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            QPoint p = me->pos();
            KexiTabbedToolBarTabBar *tb = static_cast<KexiTabbedToolBarTabBar*>(tabBar());
            int index = tb->tabAt(p);
            if (index == 0) {
                d->toggleMainMenu();
                return true;
            }
            d->hideMainMenu();
            if (index == KEXITABBEDTOOLBAR_SPACER_TAB_INDEX) {
                return true;
            }
        }
        else if (watched == mainWin && d->mainMenu) {
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if (!QRect(d->mainMenu->mapToGlobal(QPoint(0,0)), d->mainMenu->size())
                    .contains(mainWin->mapToGlobal(me->pos())))
            {
                // hide if clicked outside of the menu
                d->hideMainMenu();
            }
        }
        }
        break;
    case QEvent::KeyPress: {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
//         kDebug() << "**********" << QString::number(ke->key(), 16)
//                  << QKeySequence::mnemonic(tabText(0))[0];
        if (QKeySequence::mnemonic(tabText(0)) == QKeySequence(ke->key())) {
//             kDebug() << "eat the &File accel";
            if (!d->mainMenu || !d->mainMenu->isVisible()) {
                d->showMainMenu();
            }
            /*this could be unexpected:
            else if (d->mainMenu && d->mainMenu->isVisible()) {
                d->hideMainMenu();
            }*/
            return true;
        }
        if (d->mainMenu && d->mainMenu->isVisible() && (ke->key() == Qt::Key_Escape) && ke->modifiers() == Qt::NoModifier) {
            d->hideContentsOrMainMenu();
            return true;
        }
        break;
    }
    case QEvent::Resize:
        if (watched == KexiMainWindowIface::global()->thisWidget()) {
            d->updateMainMenuGeometry();
            if (d->mainMenu)
                d->mainMenu->updateTopLineGeometry();
        }
        break;
    case QEvent::Shortcut: {
        QShortcutEvent *se = static_cast<QShortcutEvent*>(event);
        if (watched == tabBar() && QKeySequence::mnemonic(tabText(0)) == se->key()) {
//             kDebug() << "eat the &File accel";
            if (!d->mainMenu || !d->mainMenu->isVisible()) {
                d->showMainMenu();
                return true;
            }
            /*this could be unexpected:
            else if (d->mainMenu && d->mainMenu->isVisible()) {
                d->hideMainMenu();
                return true;
            }*/
        }
        break;
    }
    default:;
    }
    return KTabWidget::eventFilter(watched, event);
}

void KexiTabbedToolBar::slotCurrentChanged(int index)
{
    if (index == indexOf(d->createWidgetToolBar) && index != -1) {
        if (d->createWidgetToolBar->actions().isEmpty()) {
            QTimer::singleShot(10, this, SLOT(setupCreateWidgetToolbar()));
        }
    }
    if (d->rolledUp) { // switching the tab rolls down
        slotTabDoubleClicked(index);
    }
#ifdef KEXI_MODERN_STARTUP
    if (index == 0) { // main menu
        d->showMainMenu();
    }
    else {
        d->hideMainMenu();
    }
#endif
}

void KexiTabbedToolBar::slotTabDoubleClicked(int index)
{
    if (index == 0)
        return; // main item does not count here
    d->rolledUp = !d->rolledUp;
    d->tabBarAnimation.stop();
    //KexiTabbedToolBarTabBar *tb = static_cast<KexiTabbedToolBarTabBar*>(tabBar());
    QWidget *w = widget(currentIndex());
    w->setGraphicsEffect(&d->tabBarOpacityEffect);
    if (d->rolledUp) {
        //setMaximumHeight(tb->tabSizeHint(index).height());
        d->tabBarOpacityEffect.setOpacity(1.0);
        d->tabBarAnimation.setTargetObject(&d->tabBarOpacityEffect);
        d->tabBarAnimation.setStartValue(1.0);
        d->tabBarAnimation.setEndValue(0.0);//tb->tabSizeHint(index).height());
        d->tabBarAnimation.start();
    }
    else { // roll down
        d->tabBarOpacityEffect.setOpacity(0.0);
        setMaximumHeight(QWIDGETSIZE_MAX);
        widget(d->rolledUpIndex)->show();
        widget(d->rolledUpIndex)->setMaximumHeight(QWIDGETSIZE_MAX);
        w->setMaximumHeight(QWIDGETSIZE_MAX);
        w->show();
        d->tabBarAnimation.setTargetObject(&d->tabBarOpacityEffect);
        d->tabBarAnimation.setStartValue(0.0);
        d->tabBarAnimation.setEndValue(1.0);//tb->tabSizeHint(index).height());
        d->tabBarAnimation.start();
    }
}

void KexiTabbedToolBar::tabBarAnimationFinished()
{
    if (d->rolledUp) {
        // hide and collapse the area
        widget(currentIndex())->hide();
        KexiTabbedToolBarTabBar *tb = static_cast<KexiTabbedToolBarTabBar*>(tabBar());
        setFixedHeight(tb->tabSizeHint(currentIndex()).height());
        widget(currentIndex())->setFixedHeight(0);
        d->rolledUpIndex = currentIndex();
    }
}

void KexiTabbedToolBar::setupCreateWidgetToolbar()
{
//! @todo separate core object types from custom....
    KexiPart::PartInfoList *plist = Kexi::partManager().partInfoList(); //this list is properly sorted
    foreach(KexiPart::Info *info, *plist) {
        QAction* a = d->ac->action(
                         KexiPart::nameForCreateAction(*info));
        if (a) {
            d->createWidgetToolBar->addAction(a);//->icon(), a->text());
        } else {
            //! @todo err
        }
    }
}

void KexiTabbedToolBar::slotDelayedTabRaise()
{
#ifdef KEXI_AUTORISE_TABBED_TOOLBAR
    QPoint p = mapFromGlobal(QCursor::pos()); // make sure cursor is still over the tab
    int tab = tabBar()->tabAt(p);
    if (tab != d->tabToRaise) {
        d->tabToRaise = -1;
    } else if (d->tabToRaise != -1) {
        setCurrentIndex(d->tabToRaise);
        d->tabToRaise = -1;
    }
#endif
}

KToolBar *KexiTabbedToolBar::toolBar(const QString& name) const
{
    return d->toolbarsForName[name];
}

void KexiTabbedToolBar::addAction(KToolBar *tbar, const char* actionName)
{
    QAction *a = d->ac->action(actionName);
    if (a)
        tbar->addAction(a);
}

void KexiTabbedToolBar::addAction(const QString& toolBarName, QAction *action)
{
    if (!action)
        return;
    KToolBar *tbar = d->toolbarsForName[toolBarName];
    if (!tbar)
        return;
    tbar->addAction(action);
}

void KexiTabbedToolBar::addSeparatorAndAction(KToolBar *tbar, const char* actionName)
{
    QAction *a = d->ac->action(actionName);
    if (a) {
        tbar->addSeparator();
        tbar->addAction(a);
    }
}

void KexiTabbedToolBar::appendWidgetToToolbar(const QString& name, QWidget* widget)
{
    KToolBar *tbar = d->toolbarsForName[name];
    if (!tbar) {
        return;
    }
    QAction *action = tbar->addWidget(widget);
    d->extraActions.insert(widget, action);
}

void KexiTabbedToolBar::setWidgetVisibleInToolbar(QWidget* widget, bool visible)
{
    QAction *action = d->extraActions[widget];
    if (!action) {
        return;
    }
    action->setVisible(visible);
}

void KexiTabbedToolBar::showMainMenu(const char* actionName)
{
    d->showMainMenu(actionName);
}

void KexiTabbedToolBar::hideMainMenu()
{
    d->hideMainMenu();
}

void KexiTabbedToolBar::toggleMainMenu()
{
    d->toggleMainMenu();
}

void KexiTabbedToolBar::setMainMenuContent(QWidget *w)
{
    d->mainMenu->setContent(w);
}

void KexiTabbedToolBar::selectMainMenuItem(const char *actionName)
{
    if (actionName) {
        KActionCollection *ac = KexiMainWindowIface::global()->actionCollection();
        KexiMenuWidgetAction *a = qobject_cast<KexiMenuWidgetAction*>(ac->action(actionName));
        d->mainMenu->setPersistentlySelectedAction(a, true);
//        a->setPersistentlySelected(true);
    }
}

/*
class KexiTopDockWidget : public QDockWidget
{
  public:
    KexiTopDockWidget(QWidget *parent)
     : QDockWidget(parent)
    {
      setFeatures(QDockWidget::NoDockWidgetFeatures);
      QWidget *topSpacer = new QWidget(this);
      topSpacer->setFixedHeight( 10 );//KDialog::marginHint() );
      setTitleBarWidget(topSpacer);
      m_widget = new KTabWidget(this);
      setWidget( m_widget );
//   QMenuBar *menu = new QMenuBar(m_widget);
//   menu->addAction( i18n("Open..") );
//   m_widget->addTab( menu, i18n("Project") );

      KToolBar *tbar = new KToolBar(m_widget);
      m_widget->addTab( tbar, i18n("Project") );

      tbar->addAction( KIcon("document-new"), i18n("&New...") );
      KAction* a = KStandardAction::open(0, 0, this);
      tbar->addAction( a->icon(), a->text() );
      a = KStandardAction::close(0, 0, this);
      tbar->addAction( a->icon(), a->text() );

      m_widget->addTab( new QWidget(m_widget), i18n("Create") );
      m_widget->addTab( new QWidget(m_widget), i18n("External Data") );
      m_widget->addTab( new QWidget(m_widget), i18n("Tools") );
      m_widget->addTab( new QWidget(m_widget), i18n("Help") );
    }
    ~KexiTopDockWidget()
    {
    }
  private:
    KTabWidget *m_widget;
};
*/

//! @short A widget being main part of KexiMainWindow
class KexiMainWidget : public KMainWindow
{
    Q_OBJECT
public:
    KexiMainWidget();
    virtual ~KexiMainWidget();

    void setParent(KexiMainWindow* mainWindow) {
        KMainWindow::setParent(mainWindow); m_mainWindow = mainWindow;
    }

    KexiMainWindowTabWidget* tabWidget() const {
        return m_tabWidget;
    }
protected:
    virtual bool queryClose();
    virtual bool queryExit();
protected slots:
    void slotCurrentTabIndexChanged(int index);

private:
    void setupCentralWidget();

    KexiMainWindowTabWidget* m_tabWidget;
    KexiMainWindow *m_mainWindow;
    QPointer<KexiWindow> m_previouslyActiveWindow;

    friend class KexiMainWindow;
    friend class KexiMainWindowTabWidget;
};

KexiMainWidget::KexiMainWidget()
        : KMainWindow(0, Qt::Widget)
        , m_mainWindow(0)
{
    setupCentralWidget();
}

KexiMainWidget::~KexiMainWidget()
{
}

void KexiMainWidget::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralWidgetLyr = new QVBoxLayout(centralWidget);
    m_tabWidget = new KexiMainWindowTabWidget(centralWidget, this);
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabIndexChanged(int)));
    centralWidgetLyr->setContentsMargins(0, 0, 0, 0);
    //centralWidgetLyr->setContentsMargins( 0, KDialog::marginHint()/2, 0, 0 );
    centralWidgetLyr->addWidget(m_tabWidget);
    setCentralWidget(centralWidget);
// connect( m_tabWidget, SIGNAL( closeTab() ), m_mainWindow, SLOT(closeCurrentWindow()) );
}

bool KexiMainWidget::queryClose()
{
    return m_mainWindow ? m_mainWindow->queryClose() : true;
}

bool KexiMainWidget::queryExit()
{
    return m_mainWindow ? m_mainWindow->queryExit() : true;
}

void KexiMainWidget::slotCurrentTabIndexChanged(int index)
{
    KexiWindowContainer* cont = dynamic_cast<KexiWindowContainer*>(m_tabWidget->widget(index));
    if (! cont || (KexiWindow*)m_previouslyActiveWindow == cont->window)
        return;
    if (m_mainWindow)
        m_mainWindow->activeWindowChanged(cont->window, (KexiWindow*)m_previouslyActiveWindow);
    m_previouslyActiveWindow = cont->window;
}

//------------------------------------------

//! @internal safer dictionary
typedef QMap< int, KexiWindow* > KexiWindowDict;

//! @internal
class KexiMainWindow::Private
{
public:
    Private(KexiMainWindow* w)
//  : dialogs(401)
            : wnd(w) {
        dummy_KXMLGUIClient = new KXMLGUIClient();
        dummy_KXMLGUIFactory = new KXMLGUIFactory(0);

        actionCollection = new KActionCollection(w);
        propEditor = 0;
        propEditorDockWidget = 0;
        navDockWidget = 0;
//2.0: unused    propEditorToolWindow=0;
        propEditorTabWidget = 0;
        KexiProjectData *pdata = Kexi::startupHandler().projectData();
        userMode = Kexi::startupHandler().forcedUserMode() /* <-- simply forced the user mode */
                   /* project has 'user mode' set as default and not 'design mode' override is found: */
                   || (pdata && pdata->userMode() && !Kexi::startupHandler().forcedDesignMode());
        isProjectNavigatorVisible = Kexi::startupHandler().isProjectNavigatorVisible();
        isMainMenuVisible = Kexi::startupHandler().isMainMenuVisible();
        navigator = 0;
//2.0: unused    navToolWindow=0;
        prj = 0;
        config = KGlobal::config();
        curWindowGUIClient = 0;
        curWindowViewGUIClient = 0;
        closedWindowGUIClient = 0;
        closedWindowViewGUIClient = 0;
        nameDialog = 0;
//  curWindow=0;
        m_findDialog = 0;
//2.0: unused  block_KMdiMainFrm_eventFilter=false;
        focus_before_popup = 0;
//  relationPart=0;
//moved  privateIDCounter=0;
        action_view_nav = 0;
        action_view_propeditor = 0;
        action_view_mainarea = 0;
        action_open_recent_projects_title_id = -1;
        action_open_recent_connections_title_id = -1;
        forceWindowClosing = false;
        insideCloseWindow = false;
#ifndef KEXI_NO_PENDING_DIALOGS
        actionToExecuteWhenPendingJobsAreFinished = NoAction;
#endif
//  callSlotLastChildViewClosedAfterCloseDialog=false;
//        createMenu = 0;
        showImportantInfoOnStartup = true;
//  disableErrorMessages=false;
//  last_checked_mode=0;
        propEditorDockSeparatorPos = -1;
        navDockSeparatorPos = -1;
//  navDockSeparatorPosWithAutoOpen=-1;
        wasAutoOpen = false;
        windowExistedBeforeCloseProject = false;
#ifndef KEXI_SHOW_UNIMPLEMENTED
        dummy_action = new KActionMenu(QString(), wnd);
#endif
        maximizeFirstOpenedChildFrm = false;
#ifdef HAVE_KNEWSTUFF
        newStuff = 0;
#endif
//2.0: unused  mdiModeToSwitchAfterRestart = (KMdi::MdiMode)0;
        forceShowProjectNavigatorOnCreation = false;
        forceHideProjectNavigatorOnCreation = false;
        navWasVisibleBeforeProjectClosing = false;
        saveSettingsForShowProjectNavigator = true;
        propertyEditorCollapsed = false;
        enable_slotPropertyEditorVisibilityChanged = true;
    }
    ~Private() {
        qDeleteAll(m_openedCustomObjectsForItem);
        delete dummy_KXMLGUIClient;
        delete dummy_KXMLGUIFactory;
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    //! Job type. Currently used for marking items as being opened or closed.
    enum PendingJobType {
        NoJob = 0,
        WindowOpeningJob,
        WindowClosingJob
    };

    KexiWindow *openedWindowFor(const KexiPart::Item* item, PendingJobType &pendingType) {
        return openedWindowFor(item->identifier(), pendingType);
    }

    KexiWindow *openedWindowFor(int identifier, PendingJobType &pendingType) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        QHash<int, PendingJobType>::ConstIterator it = pendingWindows.find(identifier);
        if (it == pendingWindows.end())
            pendingType = NoJob;
        else
            pendingType = it.value();

        if (pendingType == WindowOpeningJob) {
            return 0;
        }
        return windows.contains(identifier) ? (KexiWindow*)windows.value(identifier) : 0;
    }
#else
    KexiWindow *openedWindowFor(const KexiPart::Item* item) {
        return openedWindowFor(item->identifier());
    }

    KexiWindow *openedWindowFor(int identifier) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        return windows.contains(identifier) ? (KexiWindow*)windows.value(identifier) : 0;
    }
#endif

    void insertWindow(KexiWindow *window) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.insert(window->id(), window);
#ifndef KEXI_NO_PENDING_DIALOGS
        pendingWindows.remove(window->id());
#endif
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    void addItemToPendingWindows(const KexiPart::Item* item, PendingJobType jobType) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        pendingWindows.insert(item->identifier(), jobType);
    }

    bool pendingWindowsExist() {
        if (pendingWindows.begin() != pendingWindows.end())
            kDebug() <<  pendingWindows.constBegin().key() << " " << (int)pendingWindows.constBegin().value();
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        return !pendingWindows.isEmpty();
    }
#endif

    void updateWindowId(KexiWindow *window, int oldItemID) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.remove(oldItemID);
#ifndef KEXI_NO_PENDING_DIALOGS
        pendingWindows.remove(oldItemID);
#endif
        windows.insert(window->id(), window);
    }

    void removeWindow(int identifier) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.remove(identifier);
    }

#ifndef KEXI_NO_PENDING_DIALOGS
    void removePendingWindow(int identifier) {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        pendingWindows.remove(identifier);
    }
#endif

    uint openedWindowsCount() {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        return windows.count();
    }

    //! Used in KexiMainWindowe::closeProject()
    void clearWindows() {
//todo(threads)  QMutexLocker dialogsLocker( &dialogsMutex );
        windows.clear();
#ifndef KEXI_NO_PENDING_DIALOGS
        pendingWindows.clear();
#endif
    }

    /* UNUSED, see KexiToggleViewModeAction
      // Toggles last checked view mode radio action, if available.
      void toggleLastCheckedMode()
      {
        if (!wnd->currentWindow())
          return;
        KToggleAction *ta = actions_for_view_modes.value( (int)wnd->currentWindow()->currentViewMode() );
        if (ta)
          ta->setChecked(true);
    //  if (!last_checked_mode)
    //   return;
    //  last_checked_mode->setChecked(true);
      }*/

    /*
    void updatePropEditorDockWidthInfo() {
        if (propEditor) {
          KDockWidget *dw = (KDockWidget *)propEditor->parentWidget();
    #if defined(KDOCKWIDGET_P)
          KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
          if (ds) {
            propEditorDockSeparatorPos = ds->separatorPosInPercent();*/
    /*    if (propEditorDockSeparatorPos<=0) {
                config->setGroup("MainWindow");
                propEditorDockSeparatorPos = config->readEntry("RightDockPosition", 80);
                ds->setSeparatorPos(propEditorDockSeparatorPos, true);
            }*/
    /*}
    #endif
    }
    }*/

    void showStartProcessMsg(const QStringList& args) {
        wnd->showErrorMessage(i18n("Could not start %1 application.", QString(KEXI_APP_NAME)),
                              i18n("Command \"%1\" failed.", args.join(" ")));
    }

    /* void hideMenuItem(const QString& menuName, const QString& itemText, bool alsoSeparator)
      {
        Q3PopupMenu *pm = popups[menuName.toLatin1()];
        if (!pm)
          return;
        const QList<QAction*> actions( pm->actions() );
        bool nowHideSeparator = false;
        foreach( QAction *action, actions ) {
          kDebug() << action->text();
          if (nowHideSeparator) {
            if (action->isSeparator())
              action->setVisible(false);
            break;
          }
          else if (action->text().toLower().trimmed() == itemText.toLower().trimmed()) {
    #ifdef __GNUC__
    #warning OK? this should only hide menu item, not global action... action->setVisible(false);
    #endif
            action->setVisible(false);
            if (alsoSeparator)
              nowHideSeparator = true; //continue look to find separtator, if present
            else
              break;
          }
        }
      }

      void disableMenuItem(const QString& menuName, const QString& itemText)
      {
    #ifdef __GNUC__
    #warning reimplement disableMenuItem()
    #else
    #pragma WARNING( reimplement disableMenuItem() )
    #endif
        Q3PopupMenu *pm = popups[menuName.toLatin1()];
        if (!pm)
          return;
        const QList<QAction*> actions( pm->actions() );
        foreach( QAction *action, actions ) {
          if (action->text().toLower().trimmed()==itemText.toLower().trimmed()) {
            action->setEnabled(false);
            break;
          }
        }
      }*/

    //! Updates Property Editor Pane's visibility for the current window and the @a viewMode view mode.
    /*! @a info can be provided to hadle cases when current window is not yet defined (in openObject()). */
    void updatePropEditorVisibility(Kexi::ViewMode viewMode, KexiPart::Info *info = 0) {
        if (!propEditorDockWidget)
            return;
        KexiWindow *currentWindow = wnd->currentWindow();
        if (!info && currentWindow) {
            info = currentWindow->part()->info();
        }
        const bool visible = (viewMode == Kexi::DesignViewMode)
            && ((currentWindow && currentWindow->propertySet()) || info->isPropertyEditorAlwaysVisibleInDesignMode());
        kDebug() << "visible == " << visible;
        enable_slotPropertyEditorVisibilityChanged = false;
        if (visible && propertyEditorCollapsed) { // used when we're switching back to a window with propeditor available but collapsed
            propEditorDockWidget->setVisible(!visible);
            setPropertyEditorTabBarVisible(true);
        }
        else {
            propEditorDockWidget->setVisible(visible);
            setPropertyEditorTabBarVisible(false);
        }
        enable_slotPropertyEditorVisibilityChanged = true;
    }

    void setPropertyEditorTabBarVisible(bool visible) {
        KMultiTabBar *mtbar = multiTabBars[KMultiTabBar::Right];
        int id = PROPERTY_EDITOR_TABBAR_ID;
        if (!visible) {
            mtbar->removeTab(id);
        }
        else if (!mtbar->tab(id)) {
            QString t(propEditorDockWidget->windowTitle());
            t.remove('&');
            mtbar->appendTab(QPixmap(), id, t);
            KMultiTabBarTab *tab = mtbar->tab(id);
            QObject::connect(tab, SIGNAL(clicked(int)), wnd, SLOT(slotMultiTabBarTabClicked(int)));
        }
    }

//2.0: unused
#if 0
    void restoreNavigatorWidth() {
#if defined(KDOCKWIDGET_P)
        if (wnd->mdiMode() == KMdi::ChildframeMode || wnd->mdiMode() == KMdi::TabPageMode) {
            KDockWidget *dw = (KDockWidget *)nav->parentWidget();
            KDockSplitter *ds = (KDockSplitter *)dw->parentWidget();
//    ds->setKeepSize(true);

            config->setGroup("MainWindow");
            if (wasAutoOpen) //(dw2->isVisible())
//    ds->setSeparatorPosInPercent( 100 * nav->width() / wnd->width() );
                ds->setSeparatorPosInPercent(
                    qMax(qMax(config->readEntry("LeftDockPositionWithAutoOpen", 20),
                              config->readEntry("LeftDockPosition", 20)), 20)
                );
            else
                ds->setSeparatorPosInPercent(
                    qMax(20, config->readEntry("LeftDockPosition", 20/* % */)));

            //   dw->resize( d->config->readEntry("LeftDockPosition", 115/* % */), dw->height() );
            //if (!wasAutoOpen) //(dw2->isVisible())
//     ds->setSeparatorPos( ds->separatorPos(), true );
        }
#endif

    }
#endif

    template<class type>
    type *openedCustomObjectsForItem(KexiPart::Item* item, const char* name) {
        if (!item || !name) {
            kWarning() <<
            "KexiMainWindow::Private::openedCustomObjectsForItem(): !item || !name";
            return 0;
        }
        QByteArray key(QByteArray::number(item->identifier()) + name);
        return qobject_cast<type*>(m_openedCustomObjectsForItem.value(key));
    }

    void addOpenedCustomObjectForItem(KexiPart::Item* item, QObject* object, const char* name) {
        QByteArray key(QByteArray::number(item->identifier()) + name);
        m_openedCustomObjectsForItem.insert(key, object);
    }

    KexiFindDialog *findDialog() {
        if (!m_findDialog) {
            m_findDialog = new KexiFindDialog(wnd);
            m_findDialog->setActions(action_edit_findnext, action_edit_findprev,
                                     action_edit_replace, action_edit_replace_all);
            /*   connect(m_findDialog, SIGNAL(findNext()), action_edit_findnext, SLOT(trigger()));
                  connect(m_findDialog, SIGNAL(find()), wnd, SLOT(slotEditFindNext()));
                  connect(m_findDialog, SIGNAL(replace()), wnd, SLOT(slotEditReplaceNext()));
                  connect(m_findDialog, SIGNAL(replaceAll()), wnd, SLOT(slotEditReplaceAll()));*/
        }
        return m_findDialog;
    }

    /*! Updates the find/replace dialog depending on the active view.
     Nothing is performed if the dialog is not instantiated yet or is invisible. */
    void updateFindDialogContents(bool createIfDoesNotExist = false) {
        if (!wnd->currentWindow())
            return;
        if (!createIfDoesNotExist && (!m_findDialog || !m_findDialog->isVisible()))
            return;
        KexiSearchAndReplaceViewInterface* iface = currentViewSupportingSearchAndReplaceInterface();
        if (!iface) {
            if (m_findDialog) {
                m_findDialog->setButtonsEnabled(false);
                m_findDialog->setLookInColumnList(QStringList(), QStringList());
            }
            return;
        }
//! @todo use ->caption() here, depending on global settings related to displaying captions
        findDialog()->setObjectNameForCaption(wnd->currentWindow()->partItem()->name());

        QStringList columnNames;
        QStringList columnCaptions;
        QString currentColumnName; // for 'look in'
        if (!iface->setupFindAndReplace(columnNames, columnCaptions, currentColumnName)) {
            m_findDialog->setButtonsEnabled(false);
            m_findDialog->setLookInColumnList(QStringList(), QStringList());
            return;
        }
        m_findDialog->setButtonsEnabled(true);

        /* //update "look in" list
          KexiTableViewColumn::List columns( dataAwareObject()->data()->columns );
          QStringList columnNames;
          QStringList columnCaptions;
          for (KexiTableViewColumn::ListIterator it(columns); it.current(); ++it) {
            if (!it.current()->visible())
              continue;
            columnNames.append( it.current()->field()->name() );
            columnCaptions.append( it.current()->captionAliasOrName() );
          }*/
        const QString prevColumnName(m_findDialog->currentLookInColumnName());
        m_findDialog->setLookInColumnList(columnNames, columnCaptions);
        m_findDialog->setCurrentLookInColumnName(prevColumnName);
    }

    //! \return the current view if it supports \a actionName, otherwise returns 0.
    KexiView *currentViewSupportingAction(const char* actionName) const {
        if (!wnd->currentWindow())
            return 0;
        KexiView *view = wnd->currentWindow()->selectedView();
        if (!view)
            return 0;
        QAction *action = view->sharedAction(actionName);
        if (!action || !action->isEnabled())
            return 0;
        return view;
    }

    //! \return the current view if it supports KexiSearchAndReplaceViewInterface.
    KexiSearchAndReplaceViewInterface* currentViewSupportingSearchAndReplaceInterface() const {
        if (!wnd->currentWindow())
            return 0;
        KexiView *view = wnd->currentWindow()->selectedView();
        if (!view)
            return 0;
        return dynamic_cast<KexiSearchAndReplaceViewInterface*>(view);
    }

    KXMLGUIClient* dummy_KXMLGUIClient;
    KXMLGUIFactory* dummy_KXMLGUIFactory;

    KexiMainWindow *wnd;
    KexiMainWidget *mainWidget;
//  KexiMainWindowTabWidget *tabWidget;
    KActionCollection *actionCollection;
    KexiStatusBar *statusBar;
    KHelpMenu *helpMenu;
    KexiProject *prj;
    KSharedConfig::Ptr config;
#ifndef KEXI_NO_CTXT_HELP
    KexiContextHelp *ctxHelp;
#endif
    KexiProjectNavigator *navigator;
    KexiTabbedToolBar *tabbedToolBar;
    KexiDockWidget *navDockWidget;
    KTabWidget *propEditorTabWidget;
    KexiDockWidget *propEditorDockWidget;
    QPointer<KexiDockableWidget> propEditorDockableWidget;
    //! poits to kexi part which has been previously used to setup proppanel's tabs using
    //! KexiPart::setupCustomPropertyPanelTabs(), in updateCustomPropertyPanelTabs().
    QPointer<KexiPart::Part> partForPreviouslySetupPropertyPanelTabs;
    QMap<KexiPart::Part*, int> recentlySelectedPropertyPanelPages;
    QPointer<KexiPropertyEditorView> propEditor;
    QPointer<KoProperty::Set> propertySet;

    KXMLGUIClient *curWindowGUIClient, *curWindowViewGUIClient,
    *closedWindowGUIClient, *closedWindowViewGUIClient;
//unused  QPointer<KexiWindow> curWindow;

    KexiNameDialog *nameDialog;

    QTimer timer; //helper timer
//  QSignalMapper *actionMapper;

//! @todo move menu handling outside
//  Q3AsciiDict<QPopupMenu> popups; //list of menu popups
//    QMenu *createMenu;

//  QString origAppCaption; //<! original application's caption (without project name)
    QString appCaptionPrefix; //<! application's caption prefix - prj name (if opened), else: null

#ifndef KEXI_SHOW_UNIMPLEMENTED
    KActionMenu *dummy_action;
#endif

    //! Kexi menu
    KAction *action_save, *action_save_as, 
    *action_project_import_export_send, *action_close,
    *action_project_properties, *action_open_recent_more,
    *action_project_relations, *action_project_import_data_table,
    *action_project_export_data_table;
#ifndef KEXI_NO_QUICK_PRINTING
    KAction *action_project_print, *action_project_print_preview,
        *action_project_print_setup;
#endif
    KAction *action_open_recent;
    KAction *action_show_other;
//  int action_open_recent_more_id;
    int action_open_recent_projects_title_id,
    action_open_recent_connections_title_id;
    KAction *action_settings;

    //! edit menu
    KAction *action_edit_delete, *action_edit_delete_row,
    *action_edit_cut, *action_edit_copy, *action_edit_paste,
    *action_edit_find, *action_edit_findnext, *action_edit_findprev,
    *action_edit_replace, *action_edit_replace_all,
    *action_edit_select_all,
    *action_edit_undo, *action_edit_redo,
    *action_edit_insert_empty_row,
    *action_edit_edititem, *action_edit_clear_table,
    *action_edit_paste_special_data_table,
    *action_edit_copy_special_data_table;

    //! view menu
    KAction *action_view_nav, *action_view_propeditor, *action_view_mainarea;
//! todo see FormManager::createActions() for usage
    /* UNUSED, see KexiToggleViewModeAction
        QActionGroup* action_view_mode;
        KToggleAction *action_view_data_mode, *action_view_design_mode, *action_view_text_mode;
        QHash<int, KToggleAction*> actions_for_view_modes;*/
#ifndef KEXI_NO_CTXT_HELP
    KToggleAction *action_show_helper;
#endif
    //! data menu
    KAction *action_data_save_row;
    KAction *action_data_cancel_row_changes;
    KAction *action_data_execute;

    //! format menu
    KAction *action_format_font;

    //! tools menu
    KAction *action_tools_data_migration, *action_tools_compact_database, *action_tools_data_import;
    KActionMenu *action_tools_scripts;

    //! window menu
    KAction *action_window_next, *action_window_previous;

    //! for dock windows
//2.0: unused  KMdiToolViewAccessor* navToolWindow;
//2.0: unused  KMdiToolViewAccessor* propEditorToolWindow;

    QPointer<QWidget> focus_before_popup;
//  KexiRelationPart *relationPart;

//moved  int privateIDCounter; //!< counter: ID for private "document" like Relations window

    //! Set to true only in destructor, used by closeWindow() to know if
    //! user can cancel window closing. If true user even doesn't see any messages
    //! before closing a window. This is for extremely sanity... and shouldn't be even needed.
    bool forceWindowClosing;

    //! Indicates that we're inside closeWindow() method - to avoid inf. recursion
    //! on window removing
    bool insideCloseWindow;

#ifndef KEXI_NO_PENDING_DIALOGS
    //! Used in executeActionWhenPendingJobsAreFinished().
    enum ActionToExecuteWhenPendingJobsAreFinished {
        NoAction,
        QuitAction,
        CloseProjectAction
    };
    ActionToExecuteWhenPendingJobsAreFinished actionToExecuteWhenPendingJobsAreFinished;

    void executeActionWhenPendingJobsAreFinished() {
        ActionToExecuteWhenPendingJobsAreFinished a = actionToExecuteWhenPendingJobsAreFinished;
        actionToExecuteWhenPendingJobsAreFinished = NoAction;
        switch (a) {
        case QuitAction:
            qApp->quit();
            break;
        case CloseProjectAction:
            wnd->closeProject();
            break;
        default:;
        }
    }
#endif

    //! Used for delayed windows closing for 'close all'
    QList<KexiWindow*> windowsToClose;

#ifndef KEXI_NO_QUICK_PRINTING
    //! Opened page setup dialogs, used by printOrPrintPreviewForItem().
    QHash<int, KexiWindow*> pageSetupWindows;

    /*! A map from Kexi dialog to "print setup" part item's ID of the data item
     used by closeWindow() to find an ID of the data item, so the entry
     can be removed from pageSetupWindows dictionary. */
    QMap<int, int> pageSetupWindowItemID2dataItemID_map;
#endif

    //! Used in several places to show info dialog at startup (only once per session)
    //! before displaying other stuff
    bool showImportantInfoOnStartup;

//  //! Used sometimes to block showErrorMessage()
//  bool disableErrorMessages;

    //! Indicates if project is started in User Mode
    bool userMode;

    //! Indicates if project navigator should be visible
    bool isProjectNavigatorVisible;
    
    //! Indicates if the main menu should be visible
    bool isMainMenuVisible;

    //! Used on opening 1st child window
    bool maximizeFirstOpenedChildFrm;

    //! Set in restoreSettings() and used in initNavigator()
    //! to customize navigator visibility on startup
    bool forceShowProjectNavigatorOnCreation;
    bool forceHideProjectNavigatorOnCreation;

    bool navWasVisibleBeforeProjectClosing;
    bool saveSettingsForShowProjectNavigator;
#ifdef HAVE_KNEWSTUFF
    KexiNewStuff  *newStuff;
#endif

    //! Used by openedCustomObjectsForItem() and addOpenedCustomObjectForItem()
    QHash<QByteArray, QObject*> m_openedCustomObjectsForItem;

    int propEditorDockSeparatorPos, navDockSeparatorPos;
// int navDockSeparatorPosWithAutoOpen;
    bool wasAutoOpen;
    bool windowExistedBeforeCloseProject;

//2.0: unused  KMdi::MdiMode mdiModeToSwitchAfterRestart;

    QMap<KMultiTabBar::KMultiTabBarPosition, KMultiTabBar*> multiTabBars;
    bool propertyEditorCollapsed;

    bool enable_slotPropertyEditorVisibilityChanged;

private:
    //! @todo move to KexiProject
    KexiWindowDict windows;
#ifndef KEXI_NO_PROCESS_EVENTS
    QHash<int, PendingJobType> pendingWindows; //!< part item identifiers for windows whoose opening has been started
    //todo(threads) QMutex dialogsMutex; //!< used for locking windows and pendingWindows dicts
#endif
    KexiFindDialog *m_findDialog;
};

#endif
