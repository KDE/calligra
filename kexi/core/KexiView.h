/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIVIEW_H
#define KEXIVIEW_H

#include <QWidget>
#include <QEvent>
#include <QCloseEvent>

#include "kexiactionproxy.h"

class KexiWindow;

namespace KoProperty {
  class Set;
}

namespace KexiDB {
  class SchemaData;
}

//! @short action for toggling view mode
class KEXICORE_EXPORT KexiToggleViewModeAction : public KAction
{
  //Q_OBJECT
  public:
    //! Creates action for toggling to view mode @a mode. @a slot should have signature 
    //! matching switchedTo(Kexi::ViewMode mode) signal.
    KexiToggleViewModeAction(Kexi::ViewMode mode, QObject* parent);//, QObject* receiver, const char* slot);
/*	signals:
    void switchedTo(Kexi::ViewMode mode);
  private slots:
    void slotToggled(bool);

  private:
    class Private;
    Private * const d;*/
};

//! Base class for single view embeddable in KexiWindow.
/*! This class automatically works as a proxy for shared (application-wide) actions.
 KexiView has 'dirty' flag to indicate that view's data has changed.
 This flag's state is reused by KexiWindow object that contain the view.
 KexiView objects can be also nested, using addChildView(): any actions and 'dirty' flag
 are transmited to parent view in this case.

 KexiView objects are usually allocated within KexiWindow objects by implementing
 KexiPart::createView() method. See query or table part code for examples.

 KexiView object can be also allocated without attaching it KexiWindow,
 especially within dock window. see KexiMainWindow::initNavigator() to see example
 how KexiBrowser does this.
*/
class KEXICORE_EXPORT KexiView : public QWidget, public KexiActionProxy
{
  Q_OBJECT

  public:
    KexiView(QWidget *parent);
    virtual ~KexiView();

//		//! \return kexi main window that contain this view
//kde4 not needed		inline KexiMainWindow *mainWin() const { return m_mainWin; }

    //! \return parent KexiWindow that containing this view, 
    //! or 0 if no window contain this view
    KexiWindow* window() const;

    /*! Added for convenience.
     \return KexiPart object that was used to create this view (with a window)
     or 0 if this view is not created using KexiPart. \sa window() */
    KexiPart::Part* part() const;

    /*! \return preferred size hint, that can be used to resize the view.
     It is computed using maximum of (a) \a otherSize and (b) current KMDI dock area's size,
     so the view won't exceed this maximum size. The method is used e.g. in KexiWindow::sizeHint().
     If you reimplement this method, do not forget to return value of
     yoursize.boundedTo( KexiView::preferredSizeHint(otherSize) ). */
    virtual QSize preferredSizeHint(const QSize& otherSize);

    virtual bool eventFilter( QObject *o, QEvent *e );

    void addChildView( KexiView* childView );

    /*! True if contents (data) of the view is dirty and need to be saved
     This may or not be used, depending if changes in the window
     are saved immediately (e.g. like in datatableview) or saved by hand (by user)
     (e.g. like in alter-table window).
     "Dirty" flag is reused by KexiWindow::dirty().
     Default implementation just uses internal dirty flag, that is false by default.
     Reimplement this if you e.g. want reuse other "dirty"
     flag from internal structures that may be changed. */
    virtual bool isDirty() const;

    /*! \return the view mode for this view. */
    Kexi::ViewMode viewMode() const;

    /*! Reimpelmented from KexiActionProxy.
     \return shared action with name \a action_name for this view.
     If there's no such action declared in Kexi Part (part()),
     global shared action is returned (if exists). */
    virtual QAction* sharedAction( const char *action_name );

    /*! Enables or disables shared action declared in Kexi Part (part()).
     If there's no such action, global shared action is enabled or disabled (if exists). */
    virtual void setAvailable(const char* action_name, bool set);

  public slots:
    virtual void setFocus();

    /*! Call this in your view's implementation whenever current property set
     (returned by propertySet()) is switched to other,
     so property editor contents need to be completely replaced. */
    virtual void propertySetSwitched();

    /*! Sets dirty flag on or off. It the flag changes,
     dirty(bool) signal is emitted by the parent window (KexiWindow),
     to inform the world about that. If this view has a parent view, setDirty()
     is called also on parent view.
     Always use this function to update 'dirty' flag information. */
    void setDirty(bool set);

    /*! Equal to setDirty(true). */
    void setDirty() { setDirty(true); }

  signals:
    //! emitted when the view is about to close
    void closing(bool& cancel);

    void focus(bool in);

  protected:
    /*! called by KexiWindow::switchToViewMode() right before window is switched to new mode
     By default does nothing. Reimplement this if you need to do something
     before switching to this view.
     \return true if you accept or false if a error occupied and view shouldn't change
     If there is no error but switching should be just cancelled
     (probably after showing some info messages), you need to return cancelled.
     Set \a dontStore to true (it's false by default) if you want to avoid data storing
     by storeData() or storeNewData(). */
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);

    /*! called by KexiWindow::switchToViewMode() right after window is switched to new mode
     By default does nothing. Reimplement this if you need to do something
     after switching to this view.
     \return true if you accept or false if a error occupied and view shouldn't change
     If there is no error but switching should be just cancelled
     (probably after showing some info messages), you need to return cancelled. */
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);

    virtual void closeEvent( QCloseEvent * e );

    /*! \return a property set for this view. For reimplementation. By default returns NULL. */
    virtual KoProperty::Set *propertySet();

    /*! Call this in your view's implementation whenever current property set
     is changed that few properties are now visible and/or few other are invisible,
     so property editor operating on this property set should be completely reloaded.
     If \a preservePrevSelection is true and there was a property set
     assigned before call, previously selected item will be preselected
     in the editor (if found). */
    void propertySetReloaded(bool preservePrevSelection = false, 
      const QByteArray& propertyToSelect = QByteArray());

    /*! Tells this view to create and store data of the new object
     pointed by \a sdata on the backend.
     Called by KexiWindow::storeNewData().
     Default implementation:
     - makes a deep copy of \a sdata
     - stores object schema data \a sdata in 'kexi__objects' internal table
       using Connection::storeObjectSchemaData().
     Reimpelment this for your needs.
     Requirements:
     - deep copy of \a sdata should be made
     - schema data should be created at the backend
       (by calling KexiView::storeNewData(const KexiDB::SchemaData& sdata)),
       or using Connection::storeObjectSchemaData() or more specialized
       method. For example, KexiAlterTableDialog
       uses Connection::createTable(TableSchema) for this
       (tableschema is SchemaData subclass) to store more information than
       just a schem adata. You should use such subclasses if needed.
     Should return newly created schema data object on success.
     In this case, do not store schema object yourself (make deep copy if needed). */
    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

    /*! Loads large string data \a dataString block (e.g. xml form's representation),
     indexed with optional \a dataID, from the database backend. 
     If \a canBeEmpty is true and there is no data block for dataID, true is returned
     and \a dataString is set to null string. The default is false.
     \return true on success
     \sa storeDataBlock(). */
    bool loadDataBlock( QString &dataString, const QString& dataID = QString(), 
      bool canBeEmpty = false );

    /*! Tells this view to store data changes on the backend.
     Called by KexiWindow::storeData().
     Default implementation:
     - makes a deep copy of \a sdata
     - stores object schema data \a sdata in 'kexi__objects' internal table
       using Connection::storeObjectSchemaData().
     If \a dontAsk is true, no question dialog will 
     be shown to the user. The default is false.

     Reimpelment this for your needs. Should return true on success 
     or cancelled when the task should be cancelled.
     \sa storeNewData() */
    virtual tristate storeData(bool dontAsk = false);

    /*! Stores (potentially large) string data \a dataString, block (e.g. xml form's representation),
     at the database backend. Block will be stored in "kexi__objectdata" table pointed by
     this object's id and an optional \a dataID identifier.

     If window's id is not available (KexiWindow::id()),
     then ID that was just created in storeNewData() is used
     (see description of newlyAssignedID()).
     If there is already such record in the table, it's simply overwritten.
     \return true on success
    */
    bool storeDataBlock( const QString &dataString, const QString &dataID = QString() );

    /*! Removes (potentially large) string data (e.g. xml form's representation),
     pointed by optional \a dataID, from the database backend.
     \return true on success. Does not fail if the block doe not exists.
     Note that if \a dataID is not specified, all data blocks for this view will be removed.
     \sa storeDataBlock(). */
    bool removeDataBlock( const QString& dataID = QString());

    void setViewWidget(QWidget* w, bool focusProxy = false);

    /*! Updates actions (e.g. availability). Reimplement it, if needed (you must
     call superclass impelmentation at the end!).
     This implementation does nothing for this view but calls updateActions()
     for every child-view of this view.
     called by KexiWindow on window's activation (\a activated is true)
     or deactivation. */
    virtual void updateActions(bool activated);

    virtual void setFocusInternal() { QWidget::setFocus(); }

    /*! Allows to react on parent window's detaching (only for KMDI's ChildFrame mode)
     - it is called by KexiWindow::youAreDetached().
     Default implementation does nothing.
     Implement it if you want to perform some appropriate actions. */
    virtual void windowDetached() {}

    /*! Allows to react on parent window's attaching (only for KMDI's ChildFrame mode)
     - it is called by KexiWindow::youAreAttached().
     Default implementation does nothing.
     Implement it if you want to perform some appropriate actions. */
    virtual void windowAttached() {}

    /*! Assigns a list of view-level actions. Used by KexiView ctor. */
    void setViewActions( const QList<QAction*>& actions );

    /*! @return a list of view-level actions. */
    QList<QAction*> viewActions() const;

    /*! @return view-level action for name @a name or 0 if there is no such action. */
    QAction* viewAction(const char* name) const;

    void initViewActions();

    void toggleViewModeButtonBack();

    QString m_defaultIconName;

#ifdef __GNUC__
#warning todo: add some protected access methods
#else
#pragma WARNING( todo: add some protected access methods )
#endif
/*

    KexiMainWindow *m_mainWin;

    KexiWindow *m_dialog;

    QWidget *m_viewWidget;

    KexiView *m_parentView;

    QPointer<QWidget> m_lastFocusedChildBeforeFocusOut;

  private:*/
    /*! Member set to newly assigned object's ID in storeNewData()
     and used in storeDataBlock(). This is needed because usually,
     storeDataBlock() can be called from storeNewData() and in this case
     dialog has not yet assigned valid identifier (it has just negative temp. number).
     \sa KexiWindow::id()
     */
/*		int m_newlyAssignedID;

    //! Mode for this view. Initialized by KexiWindow::switchToViewMode().
    //! Can be useful when single class is used for more than one view (e.g. KexiDBForm). 
    int m_viewMode;

    Q3PtrList<KexiView> m_children;

    bool m_dirty : 1; */
  private slots:
    void slotSwitchToViewModeInternal(Kexi::ViewMode mode);
    void slotSwitchToDataViewModeInternal(bool);
    void slotSwitchToDesignViewModeInternal(bool);
    void slotSwitchToTextViewModeInternal(bool);

  private:
    void createViewModeToggleButtons();

  class Private;
  Private * const d;
  friend class KexiWindow;
};

#endif
