/* This file is part of the KDE project
   Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
   
   Based on qmenu.h from Qt 4.7

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

#ifndef KEXIMENUWIDGET_H
#define KEXIMENUWIDGET_H

#include <KStandardAction>
#include <KAction>
#include <qwidget.h>
#include <qstring.h>
#include <qicon.h>

class KexiMenuWidgetPrivate;
class KexiMenuWidgetActionPrivate;
class QStyleOptionMenuItem;
class QEventLoop;
struct KexiMenuWidgetCaused;

class KexiMenuWidgetAction : public KAction
{
    Q_OBJECT
public:
    KexiMenuWidgetAction(QObject *parent);
    KexiMenuWidgetAction(const QString &text, QObject *parent);
    KexiMenuWidgetAction(const KIcon &icon, const QString &text, QObject *parent);
    KexiMenuWidgetAction(KStandardAction::StandardAction id, QObject *parent);

    void setPersistentlySelected(bool set);
    bool persistentlySelected() const;

protected:
    KexiMenuWidgetActionPrivate * const d;
};

class KexiMenuWidget : public QWidget
{
private:
    Q_OBJECT

    //Q_PROPERTY(bool tearOffEnabled READ isTearOffEnabled WRITE setTearOffEnabled)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool separatorsCollapsible READ separatorsCollapsible WRITE setSeparatorsCollapsible)
    Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)

public:
    explicit KexiMenuWidget(QWidget *parent = 0);
    explicit KexiMenuWidget(const QString &title, QWidget *parent = 0);
    ~KexiMenuWidget();

    KexiMenuWidgetAction* persistentlySelectedAction() const;
    void setPersistentlySelectedAction(KexiMenuWidgetAction* action, bool set);

#ifdef Q_NO_USING_KEYWORD
    inline void addAction(QAction *action) { QWidget::addAction(action); }
#else
    using QWidget::addAction;
#endif
    QAction *addAction(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addAction(const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut = 0);
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut = 0);

//    QAction *addMenu(QMenu *menu);
//    QMenu *addMenu(const QString &title);
//    QMenu *addMenu(const QIcon &icon, const QString &title);

    QAction *addSeparator();

//    QAction *insertMenu(QAction *before, QMenu *menu);
    QAction *insertSeparator(QAction *before);

    //! @return true if frame is visible. By default the frame is visible.
    //! It may be still visually no change, e.g. on Oxygen style.
    bool hasFrame() const;

    //! Sets frame visibility.
    void setFrame(bool set);

    //! @return true if persistent selections are enabled. False by default.
    //! @see setPersistentSelectionsEnabled()
//    bool persistentSelectionsEnabled() const;

    //! Sets flag of for persistent selections in action items.
    //! In KexiMenuWidget, persistent selection flag is used in place of submenus.
//    void setPersistentSelectionsEnabled(bool set);

    bool isEmpty() const;
    void clear();

    void setTearOffEnabled(bool);
    bool isTearOffEnabled() const;

    bool isTearOffMenuVisible() const;
    void hideTearOffMenu();

    void setDefaultAction(QAction *);
    QAction *defaultAction() const;

    void setActiveAction(QAction *act);
    QAction *activeAction() const;

    void popup(const QPoint &pos, QAction *at=0);
    // QAction *exec();
    // QAction *exec(const QPoint &pos, QAction *at=0);

    // ### Qt 5: merge
    // static QAction *exec(QList<QAction*> actions, const QPoint &pos, QAction *at=0);
    // static QAction *exec(QList<QAction*> actions, const QPoint &pos, QAction *at, QWidget *parent);

    QSize sizeHint() const;

    QRect actionGeometry(QAction *) const;
    QAction *actionAt(const QPoint &) const;

    QAction *menuAction() const;

    QString title() const;
    void setTitle(const QString &title);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    void setNoReplayFor(QWidget *widget);

    bool separatorsCollapsible() const;
    void setSeparatorsCollapsible(bool collapse);

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();
    void triggered(QAction *action);
    void hovered(QAction *action);

protected:
    int columnCount() const;

    void changeEvent(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *);
#endif
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent *);
    void paintEvent(QPaintEvent *);
    void actionEvent(QActionEvent *);
    void timerEvent(QTimerEvent *);
    bool event(QEvent *);
    bool focusNextPrevChild(bool next);
    void initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const;

private Q_SLOTS:
    void internalSetSloppyAction();
    //void internalDelayedPopup();
    void actionTriggered();
    void actionHovered();
    void overrideMenuActionDestroyed();

protected:
    const KexiMenuWidgetCaused& causedPopup() const;
    enum SelectionReason {
        SelectedFromKeyboard,
        SelectedFromElsewhere
    };
    void setCurrentAction(QAction *, int popup = -1, SelectionReason reason = SelectedFromElsewhere, bool activateFirst = false);
    QEventLoop *eventLoop() const;
    void setSyncAction(QAction *a);

private:
    Q_DISABLE_COPY(KexiMenuWidget)

    KexiMenuWidgetPrivate * const d;
    friend class KexiMenuWidgetPrivate;
};

#endif
