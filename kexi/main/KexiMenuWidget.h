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

#include <kexi_export.h>
#include <QtGui/qwidget.h>
#include <QtCore/qstring.h>
#include <QtGui/qicon.h>
#include <QtGui/qaction.h>

class KexiMenuWidgetPrivate;
class QStyleOptionMenuItem;
class QEventLoop;
struct KexiMenuWidgetCaused;

class KEXIMAIN_EXPORT KexiMenuWidget : public QWidget
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
    //KexiMenuWidget(KexiMenuWidgetPrivate &dd, QWidget* parent = 0);

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
//    friend class QMenuBar;
//    friend class QMenuBarPrivate;
//    friend class QTornOffMenu;
//    friend class QComboBox;
//    friend class QAction;
//    friend class QToolButtonPrivate;
    friend class KexiMenuWidgetPrivate;
};

#endif
