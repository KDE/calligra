/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2010 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIWINDOWBASE_H
#define KEXIWINDOWBASE_H

//#define KEXI_IMPL_WARNINGS

#include "kexipartguiclient.h"
#include "kexiactionproxy.h"
#include "kexi.h"
#include "kexipart.h"
#include "KexiView.h"
//#include "keximdi.h"

#include <QPointer>
#include <QEvent>
#include <QCloseEvent>

//#include <k3mdichildview.h>
//#include <kxmlguiwindow.h>
//#include <kxmlguiclient.h>

class KexiMainWindow;
class KexiWindowData;
class KexiView;
namespace KexiPart
{
class Part;
}

namespace KoProperty
{
class Set;
}

//! Base class for child window of Kexi's main application window.
/*! This class can contain a number of configurable views, switchable using toggle action.
 It also automatically works as a proxy for shared (application-wide) actions. */
class KEXICORE_EXPORT KexiWindow
            : public QWidget
            , public KexiActionProxy
            , public Kexi::ObjectStatus
{
    Q_OBJECT

public:
    virtual ~KexiWindow();

    //! \return true if the window is registered.
    bool isRegistered() const;

    //! \return currently selected view or 0 if there is no current view
    KexiView *selectedView() const;

    /*! \return a view for a given \a mode or 0 if there's no such mode available (or opened).
     This does not open mode if it's not opened. */
    KexiView *viewForMode(Kexi::ViewMode mode) const;

    //! Adds \a view for the dialog. It will be the _only_ view (of unspecified mode) for the dialog
    void addView(KexiView *view);

    /*! \return main (top level) widget inside this dialog.
     This widget is used for e.g. determining minimum size hint and size hint. */
//  virtual QWidget* mainWidget() = 0;

    /*! reimplemented: minimum size hint is inherited from currently visible view. */
    virtual QSize minimumSizeHint() const;

    /*! reimplemented: size hint is inherited from currently visible view. */
    virtual QSize sizeHint() const;

#if 0 //main window moved to a singleton
    KexiMainWindow *mainWin() const {
        return m_parentWindow;
    }
#endif

    //js todo: maybe remove this since it's often the same as partItem()->identifier()?:

    /*! This method sets internal identifier for the dialog, but
     if there is a part item associated with this dialog (see partItem()),
     partItem()->identifier() will be is used as identifier, so this method is noop.
     Thus, it's usable only for dialog types when no part item is assigned. */
    void setId(int id);

    /*! If there is a part item associated with this dialog (see partItem()),
     partItem()->identifier() is returned, otherwise internal dialog's identifier
     (previously set by setID()) is returned. */
    int id() const;

    //! \return Kexi part used to create this window
    KexiPart::Part* part() const;

    //! \return Kexi part item used to create this window
    KexiPart::Item* partItem() const;

    //! Kexi dialog's gui COMMON client.
    //! It's obtained by querying part object for this dialog.
    KexiPart::GUIClient* commonGUIClient() const;

    //! Kexi dialog's gui client for currently selected view.
    //! It's obtained by querying part object for this dialog.
    KexiPart::GUIClient* guiClient() const;

    /*! Tries to close the dialog. \return true if closing is accepted
     (sometimes, user may not want to close the dialog by pressing cancel).
     If \a dontSaveChanges if true, changes are not saved even if this dialog is dirty. */
//js removed  bool tryClose(bool dontSaveChanges);

    /*! \return name of icon provided by part that created this dialog.
     The name is used by KexiMainWindow to set/reset icon for this dialog. */
    virtual QString itemIcon();

    /*! \return true if this dialog supports switching to \a mode.
     \a mode is one of Kexi::ViewMode enum elements.
     The flags are used e.g. for testing by KexiMainWindow, if actions
     of switching to given view mode is available.
     This member is intialised in KexiPart that creates this window object. */
    bool supportsViewMode(Kexi::ViewMode mode) const;

    /*! \return information about supported view modes. */
    Kexi::ViewModes supportedViewModes() const;

    /*! \return current view mode for this dialog. */
    Kexi::ViewMode currentViewMode() const;

    void setContextHelp(const QString& caption, const QString& text, const QString& iconName);

    /*! Internal reimplementation. */
    virtual bool eventFilter(QObject *obj, QEvent *e);

    /*! Used by Main Window
     \todo js: PROBABLY REMOVE THESE TWO?
    */
    virtual void attachToGUIClient();
    virtual void detachFromGUIClient();

    //! \return true if the window is attached within the main window
#ifdef KEXI_IMPL_WARNINGS
#ifdef __GNUC__
#warning TODO isAttached()
#else
#pragma WARNING( TODO isAttached() )
#endif
#endif
    bool isAttached() const {
        return true;
    }

    /*! True if contents (data) of the dialog is dirty and need to be saved
     This may or not be used, depending if changes in the dialog
     are saved immediately (e.g. like in datatableview) or saved by hand (by user)
     (e.g. like in alter-table dialog).
     \return true if at least one "dirty" flag is set for one of the dialog's view. */
    bool isDirty() const;

    /*! \return a pointer to view that has recently set dirty flag.
     This value is cleared when dirty flag is set to false (i.e. upon saving changes). */
    KexiView* viewThatRecentlySetDirtyFlag() const;

    /*! \return true, if this dialog's data were never saved.
     If it's true we're usually try to ask a user if the dialog's
     data should be saved somewhere. After dialog construction,
     "neverSaved" flag is set to appropriate value.
     KexiPart::Item::neverSaved() is reused.
    */
    bool neverSaved() const;

    /*! \return property set provided by the current view,
     or NULL if there is no view set (or the view has no set assigned). */
    KoProperty::Set *propertySet();

    KexiDB::SchemaData* schemaData() const;

    void setSchemaData(KexiDB::SchemaData* schemaData);

    //! Sets 'owned' property for schema data.
    //! If true, the window will delete the schema data before destruction.
    //! By default schema data is not owned.
    //! @see setSchemaData(), KexiPart::loadSchemaData(), KexiPart::loadAndSetSchemaData()
    void setSchemaDataOwned(bool set);

    /*! Reimplemented: "*" is added if for 'dirty' dialog's data. */
//  QString caption() const;

    /*! Used by KexiView subclasses. \return temporary data shared between
     views */
    KexiWindowData *data() const;

    /*! Called primarily by KexiMainWindow to activate dialog.
     Selected view (if present) is also informed about activation. */
    void activate();

    /*! Called primarily by KexiMainWindow to deactivate dialog.
     Selected view (if present) is also informed about deactivation. */
    void deactivate();

public slots:
    virtual void setFocus();

    void updateCaption();

    /*! Internal. Called by KexiMainWindow::saveObject().
     Tells this dialog to save changes of the existing object
     to the backend. If \a dontAsk is true, no question dialog will
     be shown to the user. The default is false.
     \sa storeNewData()
     \return true on success, false on failure and cancelled when storing has been cancelled. */
    tristate storeData(bool dontAsk = false);

    /*! Internal. Called by KexiMainWindow::saveObject().
     Tells this dialog to create and store data of the new object
     to the backend.
     Object's schema data has been never stored,
     so it is created automatically, using information obtained
     form part item. On success, part item's ID is updated to new value,
     and schema data is set. \sa schemaData().
     \return true on success, false on failure and cancelled when storing has been cancelled. */
    tristate storeNewData();

    /*! Reimplemented - we're informing the current view about performed
     detaching by calling KexiView::parentDialogDetached(), so the view
     can react on this event
     (by default KexiView::parentDialogDetached() does nothing, you can
     reimplement it). */
    void sendDetachedStateToCurrentView();

    /*! W're informing the current view about performed atttaching by calling
     KexiView::parentDialogAttached(), so the view can react on this event
     (by default KexiView::parentDialogAttached() does nothing, you can
     reimplement it). */
    void sendAttachedStateToCurrentView();

signals:
    void updateContextHelp();

    //! emitted when the window is about to close
    void closing();

    /*! Emitted to inform the world that 'dirty' flag changes.
     Activated by KexiView::setDirty(). */
    void dirtyChanged(KexiWindow*);

protected slots:
    /*!  Sets 'dirty' flag on every dialog's view. */
    void setDirty(bool dirty);

    /*! Switches this dialog to \a newViewMode.
     \a viewMode is one of Kexi::ViewMode enum elements.
     \return true for successful switching
     True is returned also if user has cancelled switching
     (rarely, but for any reason) - cancelled is returned. */
    tristate switchToViewMode(Kexi::ViewMode newViewMode);

protected:
    //! Used by KexiPart::Part
    KexiWindow(QWidget *parent, Kexi::ViewModes supportedViewModes, KexiPart::Part& part,
               KexiPart::Item& item);

    //! Used by KexiInternalPart
    KexiWindow();

    /*! Used by Part::openInstance(),
     like switchToViewMode( int newViewMode ), but passed \a staticObjectArgs.
     Only used for parts of class KexiPart::StaticPart. */
    tristate switchToViewMode(Kexi::ViewMode newViewMode,
                              QMap<QString, QVariant>* staticObjectArgs,
                              bool& proposeOpeningInTextViewModeBecauseOfProblems);

    void registerWindow();

    virtual void closeEvent(QCloseEvent * e);

    //! \internal
    void addView(KexiView *view, Kexi::ViewMode mode);

    //! \internal
    void removeView(Kexi::ViewMode mode);

    //! Used by \a view to inform the dialog about changing state of the "dirty" flag.
    void dirtyChanged(KexiView* view);

    bool isDesignModePreloadedForTextModeHackUsed(Kexi::ViewMode newViewMode) const;

    /*! Created view's mode - helper for switchToViewMode(),
     KexiView ctor uses that info. >0 values are useful. */
    Kexi::ViewMode creatingViewsMode() const;

    /*! Sets temporary data shared between views. */
    void setData(KexiWindowData* data);

    /*! @return action for name @a name, shared between views.
     @since 2.0 */
//  KAction* sharedViewAction(const char* name) const;

    //! Used by KexiView
    QVariant internalPropertyValue(const QByteArray& name,
                                   const QVariant& defaultValue = QVariant()) const;

private slots:
    /*! Helper, calls KexiMainWindowIface::switchToViewMode() which in turn calls KexiWindow::switchToViewMode()
     to get error handling and reporting as well on main window level. */
    tristate switchToViewModeInternal(Kexi::ViewMode newViewMode);

private:
    void createSubwidgets();
//moved to KexiView  void createViewModeToggleButtons();
    void showSaveDesignButton(bool show);
//moved to KexiView  void initViewActions(KexiView* view, Kexi::ViewMode mode);

    class Private;
    Private *d;

    bool m_destroying; //!< true after entering to the dctor

    friend class KexiMainWindow;
    friend class KexiPart::Part;
    friend class KexiInternalPart;
    friend class KexiView;
};

#endif

