/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIRECORDNAVIGATOR_H
#define KEXIRECORDNAVIGATOR_H

#include "kexiguiutils_export.h"

#include <QWidget>

class QToolButton;
class QAbstractScrollArea;
class QScrollBar;
class QEvent;
class QPaintEvent;
class KGuiItem;

#include <core/KexiRecordNavigatorIface.h>

//! @short KexiRecordNavigator class provides a record navigator.
/*! Record navigator is usually used for data tables (e.g. KexiTableView)
 or data-aware forms.

 You can plug KexiRecordNavigator object to your data-aware object in two ways:
 1) By connectiong appropriate signals to slots (prevButtonClicked(), etc.)
 2) A bit cleaner way: by inheriting from KexiRecordNavigatorHandler interface
    in your data-aware class and implementing all it's prototype methods like
    moveToRecordRequested(), and then calling setRecordHandler()
    on KexiRecordNavigator object.

    Note that using this method 2), you can create more than one navigator widget
    connected with your data-aware object (does not matter if this is useful).
 */
class KEXIGUIUTILS_EXPORT KexiRecordNavigator : public QWidget, public KexiRecordNavigatorIface
{
    Q_OBJECT

public:
    enum Button {
        ButtonFirst,
        ButtonPrevious,
        ButtonNext,
        ButtonLast,
        ButtonNew
    };

    explicit KexiRecordNavigator(QAbstractScrollArea &parentView, QWidget *parent = 0);
    virtual ~KexiRecordNavigator();

    /*! Sets record navigator handler. This allows to react
     on actions performed within navigator and vice versa. */
    void setRecordHandler(KexiRecordNavigatorHandler *handler);

    /*! \return true if data inserting is enabled (the default). */
    bool isInsertingEnabled() const;

    /*! \return current record number displayed for this navigator.
     can return 0, if the 'text box's content is cleared. */
    uint currentRecordNumber() const;

    /*! \return record count displayed for this navigator. */
    uint recordCount() const;

    /*! Sets horizontal bar's \a hbar (at the bottom) geometry so this record navigator
     is properly positioned together with horizontal scroll bar. This method is used
     in QScrollView::setHBarGeometry() implementations:
     see KexiTableView::setHBarGeometry() and KexiFormScrollView::setHBarGeometry()
     for usage examples. */
    virtual void setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h);

    /*! \return true if "editing" indicator is visible for this navigator.
     @see showEditingIndicator() */
    bool editingIndicatorVisible() const;

    /*! \return true if "editing" indicator is enabled for this navigator.
     Only meaningful if setEditingIndicatorEnabled(true) is called. */
    bool editingIndicatorEnabled() const;

    //! @short A set of GUI items usable for displaying related actions.
    /*! For instance, the items are used by Kexi main window to create shared actions. */
    class KEXIGUIUTILS_EXPORT Actions
    {
    public:
        static const KGuiItem& moveToFirstRecord();
        static const KGuiItem& moveToPreviousRecord();
        static const KGuiItem& moveToNextRecord();
        static const KGuiItem& moveToLastRecord();
        static const KGuiItem& moveToNewRecord();
    };

    /*! @return pixmap with a "pen" icon appropriate to indicate "editing" state for a row.
     Can be reused elsewhere for consistency.
     Foreground color from @a palette palette is used to colorize the icon. */
    static QPixmap penPixmap(const QPalette &palette);

    /*! @return pixmap with a "plus" icon appropriate to indicate "adding" state for a row
     Can be reused elsewhere for consistency.
     Foreground color from @a palette palette is used to colorize the icon. */
    static QPixmap plusPixmap(const QPalette &palette);

    /*! @return pixmap with a "pointer" icon appropriate to indicate "current" state for a row
     Can be reused elsewhere for consistency.
     Foreground color from @a palette palette is used to colorize the icon. */
    static QPixmap pointerPixmap(const QPalette &palette);

public Q_SLOTS:
    /*! Sets insertingEnabled flag. If true, "+" button will be enabled. */
    void setInsertingEnabled(bool set);

    /*! Sets visibility of "inserting" button. */
    void setInsertingButtonVisible(bool set);

    /*! Sets visibility of the place where "editing" indicator will be displayed.
     "editing" indicator will display KexiRecordMarker::penImage() image when
      setEditingIndicatorVisible() is called.
     This method is currently used e.g. within standard kexi forms
     (see KexiFormScrollView class). */
    void setEditingIndicatorEnabled(bool set);

    /*! Shows or hides "editing" indicator. */
    virtual void showEditingIndicator(bool show);

    virtual void setEnabled(bool set);

    /*! Sets current record number for this navigator,
     i.e. a value that will be displayed in the 'record number' text box.
     This can also affect button's enabling and disabling.
     @p r is counted from 1; if it is 0 'record number' text box's content is cleared. */
    virtual void setCurrentRecordNumber(uint r);

    /*! Sets record count for this navigator.
     This can also affect button's enabling and disabling.
     By default count is 0. */
    virtual void setRecordCount(uint count);

    /*! Sets label text at the left of the for record navigator's button.
     By default this label contains translated "Record:" text. */
    virtual void setLabelText(const QString& text);

    void setButtonToolTipText(KexiRecordNavigator::Button btn, const QString& tooltip);
    void setButtonWhatsThisText(KexiRecordNavigator::Button btn, const QString& whatsThis);
    void setNumberFieldToolTips(const QString& numberTooltip, const QString& countTooltip);

Q_SIGNALS:
    void prevButtonClicked();
    void nextButtonClicked();
    void lastButtonClicked();
    void firstButtonClicked();
    void newButtonClicked();
    void recordNumberEntered(uint r);

protected Q_SLOTS:
    void slotPrevButtonClicked();
    void slotNextButtonClicked();
    void slotLastButtonClicked();
    void slotFirstButtonClicked();
    void slotNewButtonClicked();

protected:
    //! @internal used for keyboard handling.
    virtual bool eventFilter(QObject *o, QEvent *e);

    virtual void wheelEvent(QWheelEvent *e);

    virtual void resizeEvent(QResizeEvent *e);

    QToolButton* createAction(const KGuiItem& item);
    virtual void paintEvent(QPaintEvent* pe);
    void updateButtons(uint recCnt);

    class Private;
    Private * const d;
};

#endif
