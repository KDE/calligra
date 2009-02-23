/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIFORMVIEW_H
#define KEXIFORMVIEW_H

#include <qtimer.h>
#include <QDragMoveEvent>
#include <QResizeEvent>
#include <QDropEvent>

#include <widget/kexidataawareview.h>

#include <core/KexiWindow.h>
#include <core/KexiWindowData.h>
#include <core/kexiblobbuffer.h>
#include <formeditor/form.h>

#include "kexiformpart.h"

class KexiFormPart;
class KexiDBForm;
class KexiFormScrollView;
namespace KexiDB
{
class Cursor;
}
namespace KFormDesigner
{
class Container;
}

//! The KexiFormView lass provides a data-driven (record-based) form view .
/*! The KexiFormView can display data provided "by hand"
 or from KexiDB-compatible database source.

 This class provides a single view used inside KexiWindow.
 It takes care of saving/loading form, of enabling actions when needed.
 One KexiFormView object is instantiated for data view mode
 and a second KexiFormView object is instantiated for design view mode.

 @see KexiDataTable
*/
class KEXIFORMUTILS_EXPORT KexiFormView : public KexiDataAwareView
{
    Q_OBJECT

public:
    enum ResizeMode {
        ResizeAuto = 0,
        ResizeDefault = ResizeAuto,
        ResizeFixed = 1,
        NoResize = 2 /*! @todo */
    };

    KexiFormView(QWidget *parent, bool dbAware = true);
    virtual ~KexiFormView();

//  KexiDB::Connection* connection() { return m_conn; }

    virtual QSize preferredSizeHint(const QSize& otherSize);

    int resizeMode() const {
        return m_resizeMode;
    }

    KFormDesigner::Form* form() const { return m_form; }

    /*! Assigns \a id local (static) BLOB's identifier for \a widget widget.
     Previously assigned BLOB will be usassigned.
     If \a id is 0, BLOB is unassigned and no new is assigned.

     This method is called when a widget supporting BLOB data
     (currently, images from KexiDBImageBox, within KexiDBFactory) has BLOB assigned by identifier \a id.
     BLOB identifiers are defined by KexiBLOBBuffer (KexiBLOBBuffer::self() instance).

     The data collected by this method is used on form's design saving (in design mode).
     Local BLOBs are retrieved KexiBLOBBuffer::self() and stored in "kexi__blobs" 'system' table.
     Note that db-aware BLOBs (non local) are not handled this way.
    */
    void setUnsavedLocalBLOB(QWidget *widget, KexiBLOBBuffer::Id_t id);

public slots:
    /*! Reimplemented to update resize policy. */
    virtual void show();

    /*! Inserts autofields onto the form at \a pos position.
     \a sourcePartClass can be "org.kexi-project.table" or "org.kexi-project.query",
     \a sourceName is a name of a table or query, \a fields is a list of fields to insert (one or more)
     Fields are inserted using standard KFormDesigner::InsertWidgetCommand framework,
     so undo/redo is available for this operation.

     If multiple fields are provided, they will be aligned vertically.
     If \a pos is QPoint(-1,-1) (the default), position is computed automatically
     based on a position last inserted field using this method.
     If this method has not been called yet, position of QPoint(40, 40) will be set.

     Called by:
     - slotHandleDropEvent() when field(s) are dropped from the data source pane onto the form
     - KexiFormManager is a used clicked "Insert fields" button on the data source pane. */
    void insertAutoFields(const QString& sourcePartClass, const QString& sourceName,
                          const QStringList& fields, KFormDesigner::Container* targetContainerWidget,
                          const QPoint& pos = QPoint(-1, -1));

protected slots:
//2.0 changed    void slotPropertySetSwitched(KoProperty::Set *b, bool forceReload = false,
//2.0 changed                                 const QByteArray& propertyToSelect = QByteArray());
    void slotPropertySetSwitched();

//2.0 changed    void slotDirty(KFormDesigner::Form *f, bool isDirty);
    void slotModified();

    void slotFocus(bool in);

    void slotHandleDragMoveEvent(QDragMoveEvent* e);

    //! Handles field(s) dropping from the data source pane onto the form
    //! @see insertAutoFields()
    void slotHandleDropEvent(QDropEvent* e);

//moved to formmanager  void slotWidgetSelected(KFormDesigner::Form *form, bool multiple);
//moved to formmanager  void slotFormWidgetSelected(KFormDesigner::Form *form);
//moved to formmanager  void slotNoFormSelected();

//moved to formmanager  void setUndoEnabled(bool enabled);
//moved to formmanager  void setRedoEnabled(bool enabled);

protected:
    virtual tristate beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore);
    virtual tristate afterSwitchFrom(Kexi::ViewMode mode);
    virtual KoProperty::Set* propertySet() {
        return &m_form->propertySet(); // 2.0 m_propertySet;
    }

    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
    virtual tristate storeData(bool dontAsk = false);

    KexiFormPart::TempData* tempData() const {
        return dynamic_cast<KexiFormPart::TempData*>(window()->data());
    }
    KexiFormPart* formPart() const {
        return dynamic_cast<KexiFormPart*>(part());
    }

//moved to formmanager  void disableWidgetActions();
//moved to formmanager  void enableFormActions();

    void setForm(KFormDesigner::Form *f);

    void initForm();

    void loadForm();

    //! Used in loadForm()
    void updateAutoFieldsDataSource();

    //! Used in loadForm()
    void updateValuesForSubproperties();

    virtual void resizeEvent(QResizeEvent *);

    void initDataSource();

    virtual void setFocusInternal();

    /*  // for navigator
        virtual void moveToRecordRequested(uint r);
        virtual void moveToLastRecordRequested();
        virtual void moveToPreviousRecordRequested();
        virtual void moveToNextRecordRequested();
        virtual void moveToFirstRecordRequested();
        virtual void addNewRecordRequested();*/

    /*! Called after loading the form contents (before showing it).
     Also called when the form window (KexiWindow) is detached
     (in KMDI's Child Frame mode), because otherwise tabstop ordering can get broken. */
    void updateTabStopsOrder();

    /*! @internal */
    void deleteQuery();

    /*! @internal */
    void updateDataSourcePage();

    /*! Reimplemented after KexiView.
     Updates actions (e.g. availability). */
// todo  virtual void updateActions(bool activated);

    KexiDBForm *m_dbform;
    KexiFormScrollView *m_scrollView;
//2.0     KoProperty::Set *m_propertySet;

    /*! Database cursor used for data retrieving.
     It is shared between subsequent Data view sessions (just reopened on switch),
     but deleted and recreated from scratch when form's "dataSource" property changed
     since last form viewing (m_previousDataSourceString is used for that). */
    QString m_previousDataSourceString;

    int m_resizeMode;

    KexiDB::QuerySchema* m_query;

    /*! True, if m_query is created as temporary object within this form.
     If user selected an existing, predefined (stored) query, m_queryIsOwned will be false,
     so the query object will not be destroyed. */
    bool m_queryIsOwned;

    KexiDB::Cursor *m_cursor;

    /*! For new (empty) forms only:
     Our form's area will be resized more than once.
     We will resize form widget itself later (in resizeEvent()). */
    int m_delayedFormContentsResizeOnShow;

    //! Used in setFocusInternal()
    QPointer<QWidget> m_setFocusInternalOnce;

    /*! Stores geometry of widget recently inserted using insertAutoFields() method.
     having this information, we'r eable to compute position for a newly
     inserted widget in insertAutoFields() is such position has not been specified.
     (the position is specified when a widget is inserted with mouse drag & dropping
     but not with clicking of 'Insert fields' button from Data Source pane) */
    QRect m_widgetGeometryForRecentInsertAutoFields;

    //! Cached form pointer
    QPointer<KFormDesigner::Form> m_form;
};

#endif
