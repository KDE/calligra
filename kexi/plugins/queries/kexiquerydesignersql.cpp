/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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

#include <QSplitter>
#include <QLayout>
#include <QTimer>
#include <QLabel>
#include <QPalette>
#include <QToolTip>

#include <KDebug>
#include <KMessageBox>
#include <KAction>
#include <KMenu>
#include <KDialog>

#include <KoIcon.h>

#include <kexiutils/utils.h>
#include <db/driver.h>
#include <db/connection.h>
#include <db/parser/parser.h>

#include <kexiproject.h>
#include <KexiMainWindowIface.h>
#include <KexiWindow.h>

#include "kexiquerydesignersqleditor.h"
#include "kexiquerydesignersql.h"
#include "kexiquerypart.h"

#include "kexisectionheader.h"


static bool compareSQL(const QString& sql1, const QString& sql2)
{
    //TODO: use reformatting functions here
    return sql1.trimmed() == sql2.trimmed();
}

//===================

//! @internal
class KexiQueryDesignerSQLView::Private
{
public:
    Private() :
            statusPixmapOk(koDesktopIcon("dialog-ok"))
            , statusPixmapErr(koDesktopIcon("dialog-error"))
            , statusPixmapInfo(koDesktopIcon("dialog-information"))
            , parsedQuery(0)
            , heightForStatusMode(-1)
            , justSwitchedFromNoViewMode(false)
            , slotTextChangedEnabled(true) {
    }
    KexiQueryDesignerSQLEditor *editor;
    QLabel *pixmapStatus, *lblStatus;
    QHBoxLayout *statusHLyr;
    QFrame *statusMainWidget;
    KexiSectionHeader *head;
    QWidget *bottomPane;
    QPixmap statusPixmapOk, statusPixmapErr, statusPixmapInfo;
    QSplitter *splitter;
    //! For internal use, this pointer is usually copied to TempData structure,
    //! when switching out of this view (then it's cleared).
    KexiDB::QuerySchema *parsedQuery;
    //! For internal use, statement passed in switching to this view
    QString origStatement;
    //! needed to remember height for both modes, between switching
    int heightForStatusMode;
    //! helper for beforeSwitchTo()
    bool justSwitchedFromNoViewMode : 1;
    //! helper for slotTextChanged()
    bool slotTextChangedEnabled : 1;
};

//===================

KexiQueryDesignerSQLView::KexiQueryDesignerSQLView(QWidget *parent)
        : KexiView(parent)
        , d(new Private())
{
    d->splitter = new QSplitter(this);
    d->splitter->setOrientation(Qt::Vertical);
    d->head = new KexiSectionHeader(i18n("SQL Query Text"), Qt::Vertical, d->splitter);
    d->splitter->addWidget(d->head);
    d->splitter->setStretchFactor(
        d->splitter->indexOf(d->head), 3/*stretch*/);
    d->editor = new KexiQueryDesignerSQLEditor(d->head);
    d->editor->setObjectName("sqleditor");
    d->head->setWidget(d->editor);
    connect(d->editor, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

    // -- bottom pane (status)
    d->bottomPane = new QWidget(d->splitter);
    QVBoxLayout *bottomPaneLyr = new QVBoxLayout(d->bottomPane);
    d->splitter->addWidget(d->bottomPane);
    d->splitter->setStretchFactor(
        d->splitter->indexOf(d->bottomPane), 1/*KeepSize*/);

    // -- status pane
    d->statusMainWidget = new QFrame(d->bottomPane);
    bottomPaneLyr->addWidget(d->statusMainWidget);
    d->statusMainWidget->setAutoFillBackground(true);
    d->statusMainWidget->setFrameShape(QFrame::StyledPanel);
    d->statusMainWidget->setFrameShadow(QFrame::Plain);
    d->statusMainWidget->setBackgroundRole(QPalette::Base);
    QPalette pal(QToolTip::palette());
    pal.setBrush(QPalette::Base, QToolTip::palette().brush(QPalette::Button));
    d->statusMainWidget->setPalette(pal);

    d->splitter->setCollapsible(1, false);
    d->statusHLyr = new QHBoxLayout(d->statusMainWidget);
    d->statusHLyr->setContentsMargins(0, KDialog::marginHint() / 2, 0, KDialog::marginHint() / 2);
    d->statusHLyr->setSpacing(0);

    d->pixmapStatus = new QLabel(d->statusMainWidget);
    d->statusHLyr->addWidget(d->pixmapStatus);
    d->pixmapStatus->setFixedWidth(d->statusPixmapOk.width()*3 / 2);
    d->pixmapStatus->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->pixmapStatus->setAutoFillBackground(true);

    d->lblStatus = new QLabel(d->statusMainWidget);
    d->statusHLyr->addWidget(d->lblStatus);
    d->lblStatus->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->lblStatus->setWordWrap(true);
    d->lblStatus->setTextInteractionFlags(Qt::TextBrowserInteraction);
    d->lblStatus->setMinimumHeight(d->statusPixmapOk.width());

    addChildView(d->editor);
    setViewWidget(d->splitter, false/* no focus proxy*/);
    d->splitter->setFocusProxy(d->editor);
    setFocusProxy(d->editor);

    // -- setup local actions
    QList<QAction*> viewActions;
    QAction* a;
    viewActions << (a = new KAction(koIcon("test_it"), i18n("Check Query"), this));
    a->setObjectName("querypart_check_query");
    a->setShortcut(Qt::Key_F9);
    a->setToolTip(i18n("Check Query"));
    a->setWhatsThis(i18n("Checks query for validity."));
    connect(a, SIGNAL(triggered()), this, SLOT(slotCheckQuery()));

    setViewActions(viewActions);

    slotUpdateMode();
    slotCheckQuery();
    updateGeometry();
}

KexiQueryDesignerSQLView::~KexiQueryDesignerSQLView()
{
    delete d;
}

KexiQueryDesignerSQLEditor *KexiQueryDesignerSQLView::editor() const
{
    return d->editor;
}

void KexiQueryDesignerSQLView::setStatusOk()
{
    d->pixmapStatus->setPixmap(d->statusPixmapOk);
    setStatusText("<h3>" + i18n("The query is correct") + "</h3>");
}

void KexiQueryDesignerSQLView::setStatusError(const QString& msg)
{
    d->pixmapStatus->setPixmap(d->statusPixmapErr);
    setStatusText("<h3>" + i18n("The query is incorrect") + "</h3><p>" + msg + "</p>");
}

void KexiQueryDesignerSQLView::setStatusEmpty()
{
    d->pixmapStatus->setPixmap(d->statusPixmapInfo);
    setStatusText(
        i18n("Please enter your query and execute \"Check query\" function to verify it."));
}

void KexiQueryDesignerSQLView::setStatusText(const QString& text)
{
    d->lblStatus->setText(text);
}

tristate KexiQueryDesignerSQLView::beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore)
{
//TODO
    dontStore = true;
    if (mode == Kexi::DesignViewMode || mode == Kexi::DataViewMode) {
        QString sqlText = d->editor->text().trimmed();
        KexiQueryPart::TempData * temp = tempData();
        if (sqlText.isEmpty()) {
            //special case: empty SQL text
            if (temp->query()) {
                temp->queryChangedInPreviousView = true; //query changed
                temp->setQuery(0);
//    delete temp->query; //safe?
//    temp->query = 0;
            }
        } else {
            const bool designViewWasVisible = window()->viewForMode(mode) != 0;
            //should we check SQL text?
            if (designViewWasVisible
                    && !d->justSwitchedFromNoViewMode //unchanged, but we should check SQL text
                    && compareSQL(d->origStatement, d->editor->text())) {
                //statement unchanged! - nothing to do
                temp->queryChangedInPreviousView = false;
            } else {
                //yes: parse SQL text
                if (!slotCheckQuery()) {
                    if (KMessageBox::No == KMessageBox::warningYesNo(this,
                            "<p>" + i18n("The query you entered is incorrect.")
                            + "</p><p>" + i18n("Do you want to cancel any changes made to this SQL text?") + "</p>"
                            + "</p><p>" + i18n("Answering \"No\" allows you to make corrections.") + "</p>")) {
                        return cancelled;
                    }
                    //do not change original query - it's invalid
                    temp->queryChangedInPreviousView = false;
                    //this view is no longer _just_ switched from "NoViewMode"
                    d->justSwitchedFromNoViewMode = false;
                    return true;
                }
                //this view is no longer _just_ switched from "NoViewMode"
                d->justSwitchedFromNoViewMode = false;
                //replace old query schema with new one
                temp->setQuery(d->parsedQuery);   //this will also delete temp->query()
//    delete temp->query; //safe?
//    temp->query = d->parsedQuery;
                d->parsedQuery = 0;
                temp->queryChangedInPreviousView = true;
            }
        }
    }

    d->editor->setFocus();
    return true;
}

tristate
KexiQueryDesignerSQLView::afterSwitchFrom(Kexi::ViewMode mode)
{
    kDebug() << "KexiQueryDesignerSQLView::afterSwitchFrom()";
// if (mode==Kexi::DesignViewMode || mode==Kexi::DataViewMode) {
    if (mode == Kexi::NoViewMode) {
        //User opened text view _directly_.
        //This flag is set to indicate for beforeSwitchTo() that even if text has not been changed,
        //SQL text should be invalidated.
        d->justSwitchedFromNoViewMode = true;
    }
    KexiQueryPart::TempData * temp = tempData();
    KexiDB::QuerySchema *query = temp->query();
    if (!query) {//try to just get saved schema, instead of temporary one
        query = dynamic_cast<KexiDB::QuerySchema *>(window()->schemaData());
    }

    if (mode != 0/*failure only if it is switching from prev. view*/ && !query) {
        //TODO msg
        return false;
    }

    if (!query) {
        //no valid query schema delivered: just load sql text, no matter if it's valid
        if (!loadDataBlock(d->origStatement, "sql", true /*canBeEmpty*/))
            return false;
    } else {
        // Use query with Kexi keywords (but not driver-specific keywords) escaped.
        temp->setQuery(query);
//  temp->query = query;
        KexiDB::Connection::SelectStatementOptions options;
        options.identifierEscaping = KexiDB::Driver::EscapeKexi;
        options.addVisibleLookupColumns = false;
        d->origStatement = KexiDB::selectStatement(0, *query, options).trimmed();
    }

    d->slotTextChangedEnabled = false;
    d->editor->setText(d->origStatement);
    d->slotTextChangedEnabled = true;
    QTimer::singleShot(100, d->editor, SLOT(setFocus()));
    return true;
}

QString
KexiQueryDesignerSQLView::sqlText() const
{
    return d->editor->text();
}

bool KexiQueryDesignerSQLView::slotCheckQuery()
{
    QString sqlText(d->editor->text().trimmed());
    if (sqlText.isEmpty()) {
        delete d->parsedQuery;
        d->parsedQuery = 0;
        setStatusEmpty();
        return true;
    }

    kDebug();
    KexiDB::Parser *parser = KexiMainWindowIface::global()->project()->sqlParser();
    const bool ok = parser->parse(sqlText);
    delete d->parsedQuery;
    d->parsedQuery = parser->query();
    if (!d->parsedQuery || !ok || !parser->error().type().isEmpty()) {
        KexiDB::ParserError err = parser->error();
        setStatusError(err.error());
        d->editor->jump(err.at());
        delete d->parsedQuery;
        d->parsedQuery = 0;
        return false;
    }

    setStatusOk();
    return true;
}

void KexiQueryDesignerSQLView::slotUpdateMode()
{
    slotCheckQuery();
}

void KexiQueryDesignerSQLView::slotTextChanged()
{
    if (!d->slotTextChangedEnabled)
        return;
    setDirty(true);
    setStatusEmpty();
}

void KexiQueryDesignerSQLView::updateActions(bool activated)
{
    if (activated) {
        slotUpdateMode();
    }
    setAvailable("querypart_check_query", true);
    KexiView::updateActions(activated);
}

KexiQueryPart::TempData *
KexiQueryDesignerSQLView::tempData() const
{
    return dynamic_cast<KexiQueryPart::TempData*>(window()->data());
}

KexiDB::SchemaData* KexiQueryDesignerSQLView::storeNewData(const KexiDB::SchemaData& sdata,
                                                           KexiView::StoreNewDataOptions options,
                                                           bool &cancel)
{
    Q_UNUSED(options);
    Q_UNUSED(cancel);

    //here: we won't store query layout: it will be recreated 'by hand' in GUI Query Editor
    bool queryOK = slotCheckQuery();
    bool ok = true;
    KexiDB::SchemaData* query = 0;
    if (queryOK) {
        //query is ok
        if (d->parsedQuery) {
            query = d->parsedQuery; //will be returned, so: don't keep it
            d->parsedQuery = 0;
        } else {//empty query
            query = new KexiDB::SchemaData(); //just empty
        }

        (KexiDB::SchemaData&)*query = sdata; //copy main attributes
        ok = KexiMainWindowIface::global()->project()->dbConnection()->storeObjectSchemaData(*query, true /*newObject*/);
        if (ok) {
            ok = KexiMainWindowIface::global()->project()->removeUserDataBlock(sdata.id()); // for sanity
        }
        if (ok) {
            window()->setId(query->id());
            ok = storeDataBlock(d->editor->text(), "sql");
        }
    } else {
        //query is not ok
//#if 0
        //TODO: allow saving invalid queries
        //TODO: just ask this question:
        query = new KexiDB::SchemaData(); //just empty

        ok = (KMessageBox::questionYesNo(this, i18n("Do you want to save invalid query?"),
                                         0, KStandardGuiItem::yes(), KStandardGuiItem::no(), "askBeforeSavingInvalidQueries"/*config entry*/) == KMessageBox::Yes);
        if (ok) {
            (KexiDB::SchemaData&)*query = sdata; //copy main attributes
            ok = KexiMainWindowIface::global()->project()->dbConnection()->storeObjectSchemaData(
                     *query, true /*newObject*/);
        }
        if (ok) {
            window()->setId(query->id());
            ok = storeDataBlock(d->editor->text(), "sql");
        }
//#else
        //ok = false;
//#endif
    }
    if (!ok) {
        delete query;
        query = 0;
    }
    return query;
}

tristate KexiQueryDesignerSQLView::storeData(bool dontAsk)
{
    tristate res = KexiView::storeData(dontAsk);
    if (~res)
        return res;
    if (res == true) {
        res = storeDataBlock(d->editor->text(), "sql");
#if 0
        bool queryOK = slotCheckQuery();
        if (queryOK) {
            res = storeDataBlock(d->editor->text(), "sql");
        } else {
            //query is not ok
            //TODO: allow saving invalid queries
            //TODO: just ask this question:
            res = false;
        }
#endif
    }
    if (res == true) {
        QString empty_xml;
        res = storeDataBlock(empty_xml, "query_layout");   //clear
    }
    if (!res)
        setDirty(true);
    return res;
}

#include "kexiquerydesignersql.moc"
