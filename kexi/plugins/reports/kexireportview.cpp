/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexireportview.h"

#include <kdebug.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>
#include <widgetpropertyset.h>
#include <container.h>

#include <KexiWindow.h>
//#include <kexidatasourcewizard.h>
#include <kexidb/fieldlist.h>
#include <kexidb/connection.h>

#include "kexireportform.h"
#include <utils/kexirecordnavigator.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QResizeEvent>

#define NO_DSWIZARD

KexiReportScrollView::KexiReportScrollView(QWidget *parent, bool preview)
        : KexiScrollView(parent, preview)
{
    if (preview) {
        setRecordNavigatorVisible(true);
        recordNavigator()->setLabelText(i18n("Page:"));
        recordNavigator()->setInsertingButtonVisible(false);
    }
    connect(this, SIGNAL(resizingStarted()), this, SLOT(slotResizingStarted()));
}

KexiReportScrollView::~KexiReportScrollView()
{
}

void
KexiReportScrollView::show()
{
    KexiScrollView::show();

    //now get resize mode settings for entire form
    if (m_preview) {
        KexiReportView* fv = dynamic_cast<KexiReportView*>(parent());
        int resizeMode = fv ? fv->resizeMode() : KexiReportView::ResizeAuto;
        if (resizeMode == KexiReportView::ResizeAuto)
            setResizePolicy(AutoOneFit);
    }
}

void
KexiReportScrollView::slotResizingStarted()
{
    if (m_form && KFormDesigner::FormManager::self())
        setSnapToGrid(KFormDesigner::FormManager::self()->snapWidgetsToGrid(), m_form->gridSize());
    else
        setSnapToGrid(false);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

KexiReportView::KexiReportView(QWidget *parent, KexiDB::Connection *conn)
        : KexiView(parent), m_propertySet(0), m_conn(conn)
        , m_resizeMode(KexiReportView::ResizeDefault)
{
    Q3HBoxLayout *l = new Q3HBoxLayout(this);
    l->setAutoAdd(true);

    m_scrollView = new KexiReportScrollView(this, viewMode() == Kexi::DataViewMode);
    setViewWidget(m_scrollView);
// m_scrollView->show();

    m_reportform = new KexiReportForm(m_scrollView->viewport(), name/*, conn*/);
// m_reportform->resize(QSize(400, 300));
    m_scrollView->setWidget(m_reportform);
    m_scrollView->setResizingEnabled(viewMode() != Kexi::DataViewMode);

// initForm();

    if (viewMode() == Kexi::DataViewMode) {
        m_scrollView->viewport()->setPaletteBackgroundColor(m_reportform->palette().active().background());
#if 0
        connect(reportPart()->manager(), SIGNAL(noFormSelected()), SLOT(slotNoFormSelected()));
#endif
    } else {
        connect(KFormDesigner::FormManager::self(), SIGNAL(propertySetSwitched(KoProperty::Set *, bool)),
                this, SLOT(slotPropertySetSwitched(KoProperty::Set *, bool)));
        connect(KFormDesigner::FormManager::self(), SIGNAL(dirty(KFormDesigner::Form *, bool)),
                this, SLOT(slotDirty(KFormDesigner::Form *, bool)));

        // action stuff
        /*connect(reportPart()->manager(), SIGNAL(widgetSelected(KFormDesigner::Form*, bool)), SLOT(slotWidgetSelected(KFormDesigner::Form*, bool)));
        connect(reportPart()->manager(), SIGNAL(formWidgetSelected(KFormDesigner::Form*)), SLOT(slotFormWidgetSelected(KFormDesigner::Form*)));
        connect(reportPart()->manager(), SIGNAL(undoEnabled(bool, const QString&)), this, SLOT(setUndoEnabled(bool)));
        connect(reportPart()->manager(), SIGNAL(redoEnabled(bool, const QString&)), this, SLOT(setRedoEnabled(bool)));*/

        plugSharedAction("edit_copy", KFormDesigner::FormManager::self(), SLOT(copyWidget()));
        plugSharedAction("edit_cut", KFormDesigner::FormManager::self(), SLOT(cutWidget()));
        plugSharedAction("edit_paste", KFormDesigner::FormManager::self(), SLOT(pasteWidget()));
        plugSharedAction("edit_delete", KFormDesigner::FormManager::self(), SLOT(deleteWidget()));
        plugSharedAction("edit_select_all", KFormDesigner::FormManager::self(), SLOT(selectAll()));
        plugSharedAction("reportpart_clear_contents", KFormDesigner::FormManager::self(), SLOT(clearWidgetContent()));
        plugSharedAction("edit_undo", KFormDesigner::FormManager::self(), SLOT(undo()));
        plugSharedAction("edit_redo", KFormDesigner::FormManager::self(), SLOT(redo()));

        plugSharedAction("reportpart_format_raise", KFormDesigner::FormManager::self(), SLOT(bringWidgetToFront()));
        plugSharedAction("reportpart_format_lower", KFormDesigner::FormManager::self(), SLOT(sendWidgetToBack()));

        plugSharedAction("reportpart_align_menu", KFormDesigner::FormManager::self(), 0);
        plugSharedAction("reportpart_align_to_left", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToLeft()));
        plugSharedAction("reportpart_align_to_right", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToRight()));
        plugSharedAction("reportpart_align_to_top", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToTop()));
        plugSharedAction("reportpart_align_to_bottom", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToBottom()));
        plugSharedAction("reportpart_align_to_grid", KFormDesigner::FormManager::self(), SLOT(alignWidgetsToGrid()));

        plugSharedAction("reportpart_adjust_size_menu", KFormDesigner::FormManager::self(), 0);
        plugSharedAction("reportpart_adjust_to_fit", KFormDesigner::FormManager::self(), SLOT(adjustWidgetSize()));
        plugSharedAction("reportpart_adjust_size_grid", KFormDesigner::FormManager::self(), SLOT(adjustSizeToGrid()));
        plugSharedAction("reportpart_adjust_height_small", KFormDesigner::FormManager::self(),  SLOT(adjustHeightToSmall()));
        plugSharedAction("reportpart_adjust_height_big", KFormDesigner::FormManager::self(), SLOT(adjustHeightToBig()));
        plugSharedAction("reportpart_adjust_width_small", KFormDesigner::FormManager::self(), SLOT(adjustWidthToSmall()));
        plugSharedAction("reportpart_adjust_width_big", KFormDesigner::FormManager::self(), SLOT(adjustWidthToBig()));
    }

    initForm();

    connect(this, SIGNAL(focus(bool)), this, SLOT(slotFocus(bool)));
    /// @todo skip this if ther're no borders
// m_reportform->resize( m_reportform->size()+QSize(m_scrollView->verticalScrollBar()->width(), m_scrollView->horizontalScrollBar()->height()) );
}

KexiReportView::~KexiReportView()
{
    // Important: form window is closed.
    // Set property set to 0 because there is *only one* instance of a property set class
    // in Kexi, so the main window wouldn't know the set in fact has been changed.
    m_propertySet = 0;
    propertySetSwitched();
}

KFormDesigner::Form*
KexiReportView::form() const
{
    if (viewMode() == Kexi::DataViewMode)
        return tempData()->previewForm;
    else
        return tempData()->form;
}

void
KexiReportView::setForm(KFormDesigner::Form *f)
{
    if (viewMode() == Kexi::DataViewMode)
        tempData()->previewForm = f;
    else
        tempData()->form = f;
}

void
KexiReportView::initForm()
{
    setForm(new KFormDesigner::Form(KexiReportPart::library()));
    form()->createToplevel(m_reportform, m_reportform);

    // Show the form wizard if this is a new Form
// KexiDB::FieldList *fields = 0;
    if (parentWindow()->id() < 0) {
#ifndef NO_DSWIZARD
        KexiDataSourceWizard *w = new KexiDataSourceWizard(KexiMainWindow::self(),
                KexiMainWindowIface::global()->thisWidget());
        if (!w->exec())
            fields = 0;
        else
            fields = w->fields();
        delete w;
#endif
    }

    /* if(fields)
      {
       @todo generate a report from a table or a query
        QDomDocument dom;
        reportPart()->generateForm(fields, dom);
        KFormDesigner::FormIO::loadFormFromDom(form(), m_reportform, dom);
      }
      else*/
    loadForm();

    KFormDesigner::FormManager::self()->importForm(form(), viewMode() == Kexi::DataViewMode);
    m_scrollView->setForm(form());
    m_scrollView->refreshContentsSize();
}

void
KexiReportView::loadForm()
{

//@todo also load m_resizeMode !

    kexipluginsdbg << "KexiReportForm::loadForm() Loading the form with id : " << parentWindow()->id() << endl;
    // If we are previewing the Form, use the tempData instead of the form stored in the db
    if (viewMode() == Kexi::DataViewMode && !tempData()->tempForm.isNull()) {
        KFormDesigner::FormIO::loadFormFromString(form(), m_reportform, tempData()->tempForm);
        return;
    }

    // normal load
    QString data;
    loadDataBlock(data);
    KFormDesigner::FormIO::loadFormFromString(form(), m_reportform, data);
}

void
KexiReportView::slotPropertySetSwitched(KoProperty::Set *set, bool forceReload)
{
    m_propertySet = set;
    if (forceReload)
        propertySetReloaded(true/*preservePrevSelection*/);
    else
        propertySetSwitched();
}

tristate
KexiReportView::beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore)
{
    if (mode != viewMode() && viewMode() != Kexi::DataViewMode) {
        //remember our pos
        tempData()->scrollViewContentsPos
        = QPoint(m_scrollView->contentsX(), m_scrollView->contentsY());
    }

    // we don't store on db, but in our TempData
    dontStore = true;
    if (dirty() && (mode == Kexi::DataViewMode) && form()->objectTree())
        KFormDesigner::FormIO::saveFormToString(form(), tempData()->tempForm);

    return true;
}

tristate
KexiReportView::afterSwitchFrom(Kexi::ViewMode mode)
{
    if (mode != 0 && mode != Kexi::DesignViewMode) {
        //preserve contents pos after switching to other view
        m_scrollView->setContentsPos(tempData()->scrollViewContentsPos.x(),
                                     tempData()->scrollViewContentsPos.y());
    }
// if (mode == Kexi::DesignViewMode) {
    //m_scrollView->move(0,0);
    //m_scrollView->setContentsPos(0,0);
    //m_scrollView->moveChild(m_reportform, 0, 0);
// }

    if ((mode == Kexi::DesignViewMode) && viewMode() == Kexi::DataViewMode) {
        // The form may have been modified, so we must recreate the preview
        delete m_reportform; // also deletes form()
        m_reportform = new KexiReportForm(m_scrollView->viewport());
        m_scrollView->setWidget(m_reportform);

        initForm();
#if 0
        slotNoFormSelected();
#endif

        //reset position
        m_scrollView->setContentsPos(0, 0);
        m_reportform->move(0, 0);
    }
    return true;
}

void
KexiReportView::slotDirty(KFormDesigner::Form *dirtyForm, bool isDirty)
{
    if (dirtyForm == form())
        KexiView::setDirty(isDirty);
}

KexiDB::SchemaData*
KexiReportView::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
    KexiDB::SchemaData *s = KexiView::storeNewData(sdata, cancel);
    kexipluginsdbg << "KexiReportForm::storeNewData(): new id:" << s->id() << endl;

    if (!s || cancel) {
        delete s;
        return 0;
    }
    if (!storeData()) {
        //failure: remove object's schema data to avoid garbage
        m_conn->removeObject(s->id());
        delete s;
        return 0;
    }
    return s;
}

tristate
KexiReportView::storeData(bool dontAsk)
{
    Q_UNUSED(dontAsk)
    kexipluginsdbg << "KexiReportForm::storeData(): " << parentWindow()->partItem()->name()
    << " [" << parentWindow()->id() << "]" << endl;
    QString data;
    KFormDesigner::FormIO::saveFormToString(tempData()->form, data);
    if (!storeDataBlock(data))
        return false;
    tempData()->tempForm = QString();

    return true;
}

#if 0
/// Action stuff /////////////////
void
KexiReportView::slotWidgetSelected(KFormDesigner::Form *f, bool multiple)
{
    if (f != form())
        return;

    enableFormActions();
    // Enable edit actions
    setAvailable("edit_copy", true);
    setAvailable("edit_cut", true);
    setAvailable("edit_clear", true);

    // 'Align Widgets' menu
    setAvailable("reportpart_align_menu", multiple);
    setAvailable("reportpart_align_to_left", multiple);
    setAvailable("reportpart_align_to_right", multiple);
    setAvailable("reportpart_align_to_top", multiple);
    setAvailable("reportpart_align_to_bottom", multiple);

    setAvailable("reportpart_adjust_size_menu", true);
    setAvailable("reportpart_adjust_width_small", multiple);
    setAvailable("reportpart_adjust_width_big", multiple);
    setAvailable("reportpart_adjust_height_small", multiple);
    setAvailable("reportpart_adjust_height_big", multiple);

    setAvailable("reportpart_format_raise", true);
    setAvailable("reportpart_format_lower", true);
}

void
KexiReportView::slotFormWidgetSelected(KFormDesigner::Form *f)
{
    if (f != form())
        return;

    disableWidgetActions();
    enableFormActions();
}

void
KexiReportView::slotNoFormSelected() // == form in preview mode
{
    disableWidgetActions();

    // Disable paste action
    setAvailable("edit_paste", false);
    setAvailable("edit_undo", false);
    setAvailable("edit_redo", false);
}

void
KexiReportView::enableFormActions()
{
    setAvailable("edit_paste", KFormDesigner::FormManager::self()->isPasteEnabled());
}

void
KexiReportView::disableWidgetActions()
{
    // Disable edit actions
    setAvailable("edit_copy", false);
    setAvailable("edit_cut", false);
    setAvailable("edit_clear", false);

    // Disable format functions
    setAvailable("reportpart_align_menu", false);
    setAvailable("reportpart_align_to_left", false);
    setAvailable("reportpart_align_to_right", false);
    setAvailable("reportpart_align_to_top", false);
    setAvailable("reportpart_align_to_bottom", false);

    setAvailable("reportpart_adjust_size_menu", false);
    setAvailable("reportpart_adjust_width_small", false);
    setAvailable("reportpart_adjust_width_big", false);
    setAvailable("reportpart_adjust_height_small", false);
    setAvailable("reportpart_adjust_height_big", false);

    setAvailable("reportpart_format_raise", false);
    setAvailable("reportpart_format_lower", false);
}

void
KexiReportView::setUndoEnabled(bool enabled)
{
    setAvailable("edit_undo", enabled);
}

void
KexiReportView::setRedoEnabled(bool enabled)
{
    setAvailable("edit_redo", enabled);
}
#endif

QSize
KexiReportView::preferredSizeHint(const QSize& otherSize)
{
    return (m_reportform->size()
            + QSize(m_scrollView->verticalScrollBar()->isVisible() ? m_scrollView->verticalScrollBar()->width()*3 / 2 : 10,
                    m_scrollView->horizontalScrollBar()->isVisible() ? m_scrollView->horizontalScrollBar()->height()*3 / 2 : 10))
           .expandedTo(KexiView::preferredSizeHint(otherSize));
}

void
KexiReportView::resizeEvent(QResizeEvent *e)
{
    if (viewMode() == Kexi::DataViewMode) {
        m_scrollView->refreshContentsSizeLater(
            e->size().width() != e->oldSize().width(),
            e->size().height() != e->oldSize().height()
        );
    }
    KexiView::resizeEvent(e);
    m_scrollView->updateNavPanelGeometry();
}

void
KexiReportView::show()
{
    KexiView::show();

//moved from KexiFormScrollView::show():

    //now get resize mode settings for entire form
    // if (resizeMode() == KexiFormView::ResizeAuto)
    if (viewMode() == Kexi::DataViewMode) {
        if (resizeMode() == ResizeAuto)
            m_scrollView->setResizePolicy(Q3ScrollView::AutoOneFit);
    }
}

void
KexiReportView::slotFocus(bool in)
{
    if (in && form() && KFormDesigner::FormManager::self() && KFormDesigner::FormManager::self()->activeForm() != form())
        KFormDesigner::FormManager::self()->windowChanged(form()->widget());//m_dbform);
}


#include "kexireportview.moc"

