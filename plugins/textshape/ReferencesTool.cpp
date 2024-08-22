/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ReferencesTool.h"
#include "TextShape.h"
#include "dialogs/BibliographyConfigureDialog.h"
#include "dialogs/CitationInsertionDialog.h"
#include "dialogs/InsertBibliographyDialog.h"
#include "dialogs/LinkInsertionDialog.h"
#include "dialogs/NotesConfigurationDialog.h"
#include "dialogs/SimpleCaptionsWidget.h"
#include "dialogs/SimpleCitationBibliographyWidget.h"
#include "dialogs/SimpleFootEndNotesWidget.h"
#include "dialogs/SimpleLinksWidget.h"
#include "dialogs/SimpleTableOfContentsWidget.h"
#include "dialogs/TableOfContentsConfigure.h"

#include <KoBookmark.h>
#include <KoCanvasBase.h>
#include <KoIcon.h>
#include <KoInlineNote.h>
#include <KoParagraphStyle.h>
#include <KoTableOfContentsGeneratorInfo.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>
#include <KoTextLayoutRootArea.h>
#include <QMessageBox>

#include <QAction>
#include <QBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QMenu>
#include <QTextDocument>
#include <QWidgetAction>

LabeledWidget::~LabeledWidget() = default;

LabeledWidget::LabeledWidget(QAction *action, const QString &label, LabelPosition lb, bool warningLabelRequired)
    : QWidget()
    , m_action(action)
{
    setMouseTracking(true);
    QBoxLayout *layout;
    QLabel *l = new QLabel(label);
    l->setWordWrap(true);
    m_lineEdit = new QLineEdit();
    if (lb == LabeledWidget::INLINE) { // label followed by line edit
        layout = new QHBoxLayout();
        l->setIndent(l->style()->pixelMetric(QStyle::PM_SmallIconSize) + l->style()->pixelMetric(QStyle::PM_MenuPanelWidth) + 4);
    } else { // Label goes above the text edit
        layout = new QVBoxLayout();
        m_lineEdit->setFixedWidth(300); // TODO : assuming a reasonable width, is there a better way?
    }
    layout->addWidget(l);
    layout->addWidget(m_lineEdit);
    if (warningLabelRequired) {
        m_warningLabel[0] = new QLabel("");
        m_warningLabel[1] = new QLabel("");
        m_warningLabel[0]->setWordWrap(true);
        m_warningLabel[1]->setWordWrap(true);
        layout->addWidget(m_warningLabel[0]);
        layout->addWidget(m_warningLabel[1]);
    }
    layout->setContentsMargins({});
    setLayout(layout);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, &LabeledWidget::returnPressed);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &LabeledWidget::lineEditChanged);
}

void LabeledWidget::returnPressed()
{
    Q_EMIT textTriggered(m_lineEdit->text());
}

void LabeledWidget::enterEvent(QEnterEvent *event)
{
    m_action->activate(QAction::Hover);
    QWidget::enterEvent(event);
}

void LabeledWidget::setWarningText(int pos, const QString &warning)
{
    if (m_warningLabel[pos] == nullptr) {
        return;
    }
    m_warningLabel[pos]->setText(warning);
}

void LabeledWidget::clearLineEdit()
{
    m_lineEdit->setText("");
}

ReferencesTool::ReferencesTool(KoCanvasBase *canvas)
    : TextTool(canvas)
    , m_configure(nullptr)
    , m_stocw(nullptr)
    , m_canvas(canvas)
{
    createActions();
}

ReferencesTool::~ReferencesTool() = default;

void ReferencesTool::createActions()
{
    QWidgetAction *wAction = nullptr;

    QAction *action = new QAction(i18n("Insert"), this);
    addAction("insert_tableofcontents", action);
    action->setToolTip(i18n("Insert a Table of Contents into the document."));

    action = new QAction(i18n("Insert Custom..."), this);
    addAction("insert_configure_tableofcontents", action);
    action->setToolTip(i18n("Insert a custom Table of Contents into the document."));

    action = new QAction(koIcon("configure"), i18n("Configure..."), this);
    addAction("format_tableofcontents", action);
    action->setToolTip(i18n("Configure the Table of Contents"));
    connect(action, &QAction::triggered, this, &ReferencesTool::formatTableOfContents);

    action = new QAction(i18n("Insert footnote with auto number"), this);
    addAction("insert_autofootnote", action);
    connect(action, &QAction::triggered, this, &ReferencesTool::insertAutoFootNote);

    wAction = new QWidgetAction(this);
    wAction->setText(i18n("Insert Labeled Footnote"));
    LabeledWidget *w = new LabeledWidget(wAction, i18n("Insert with label:"), LabeledWidget::INLINE, false);
    wAction->setDefaultWidget(w);
    addAction("insert_labeledfootnote", wAction);
    connect(w, &LabeledWidget::textTriggered, this, &ReferencesTool::insertLabeledFootNote);

    action = new QAction(i18n("Insert endnote with auto number"), this);
    addAction("insert_autoendnote", action);
    connect(action, &QAction::triggered, this, &ReferencesTool::insertAutoEndNote);

    wAction = new QWidgetAction(this);
    wAction->setText(i18n("Insert Labeled Endnote"));
    w = new LabeledWidget(wAction, i18n("Insert with label:"), LabeledWidget::INLINE, false);
    wAction->setDefaultWidget(w);
    addAction("insert_labeledendnote", wAction);
    connect(w, &LabeledWidget::textTriggered, this, &ReferencesTool::insertLabeledEndNote);

    action = new QAction(koIcon("configure"), i18n("Settings..."), this);
    addAction("format_footnotes", action);
    connect(action, &QAction::triggered, this, &ReferencesTool::showFootnotesConfigureDialog);

    action = new QAction(koIcon("configure"), i18n("Settings..."), this);
    addAction("format_endnotes", action);
    connect(action, &QAction::triggered, this, &ReferencesTool::showEndnotesConfigureDialog);

    action = new QAction(i18n("Insert Citation"), this);
    addAction("insert_citation", action);
    action->setToolTip(i18n("Insert a citation into the document."));
    connect(action, &QAction::triggered, this, &ReferencesTool::insertCitation);

    action = new QAction(i18n("Insert Bibliography"), this);
    addAction("insert_bibliography", action);
    action->setToolTip(i18n("Insert a bibliography into the document."));

    action = new QAction(i18n("Insert Custom Bibliography"), this);
    addAction("insert_custom_bibliography", action);
    action->setToolTip(i18n("Insert a custom Bibliography into the document."));

    action = new QAction(i18n("Configure"), this);
    addAction("configure_bibliography", action);
    action->setToolTip(i18n("Configure the bibliography"));
    connect(action, &QAction::triggered, this, &ReferencesTool::configureBibliography);

    action = new QAction(i18n("Insert Link"), this);
    addAction("insert_link", action);
    action->setToolTip(i18n("Insert a weblink or link to a bookmark."));
    connect(action, &QAction::triggered, this, &ReferencesTool::insertLink);

    wAction = new QWidgetAction(this);
    wAction->setText(i18n("Add Bookmark"));
    m_bmark = new LabeledWidget(wAction, i18n("Add Bookmark :"), LabeledWidget::ABOVE, true);
    connect(m_bmark, &LabeledWidget::lineEditChanged, this, &ReferencesTool::validateBookmark);
    wAction->setDefaultWidget(m_bmark);
    addAction("insert_bookmark", wAction);
    connect(m_bmark, &LabeledWidget::textTriggered, this, &ReferencesTool::insertBookmark);
    wAction->setToolTip(i18n("Insert a Bookmark. This is useful to create links that point to areas within the document"));

    action = new QAction(i18n("Bookmarks"), this);
    addAction("invoke_bookmark_handler", action);
    action->setToolTip(i18n("Display a pop up that hosts the options to add new Bookmark or handle existing Bookmarks"));

    action = new QAction(i18n("Manage Bookmarks"), this);
    addAction("manage_bookmarks", action);
    action->setToolTip(i18n("Manage your Bookmarks. Check where are they pointing to, Delete or Rename."));
}

void ReferencesTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    TextTool::activate(toolActivation, shapes);
}

void ReferencesTool::deactivate()
{
    TextTool::deactivate();
    canvas()->canvasWidget()->setFocus();
}

QList<QPointer<QWidget>> ReferencesTool::createOptionWidgets()
{
    QList<QPointer<QWidget>> widgets;
    m_stocw = new SimpleTableOfContentsWidget(this, nullptr);

    m_sfenw = new SimpleFootEndNotesWidget(this, nullptr);

    m_scbw = new SimpleCitationBibliographyWidget(this, nullptr);

    m_slw = new SimpleLinksWidget(this, nullptr);
    // Connect to/with simple table of contents option widget
    connect(m_stocw, &SimpleTableOfContentsWidget::doneWithFocus, this, &ReferencesTool::returnFocusToCanvas);

    // Connect to/with simple citation index option widget
    // connect(scw, SIGNAL(doneWithFocus()), this, SLOT(returnFocusToCanvas()));

    // Connect to/with simple citation index option widget
    connect(m_sfenw, &SimpleFootEndNotesWidget::doneWithFocus, this, &ReferencesTool::returnFocusToCanvas);

    connect(m_slw, &SimpleLinksWidget::doneWithFocus, this, &ReferencesTool::returnFocusToCanvas);

    m_stocw->setWindowTitle(i18nc("as in table of contents, list of pictures, index", "Tables, Lists & Indexes"));
    widgets.append(m_stocw);

    m_sfenw->setWindowTitle(i18n("Footnotes and Endnotes"));
    widgets.append(m_sfenw);

    m_scbw->setWindowTitle(i18n("Citations and Bibliography"));
    widgets.append(m_scbw);

    m_slw->setWindowTitle(i18n("Links and Bookmarks"));
    widgets.append(m_slw);
    // widgets.insert(i18n("Captions"), scapw);
    connect(textEditor(), &KoTextEditor::cursorPositionChanged, this, &ReferencesTool::updateButtons);
    return widgets;
}

void ReferencesTool::insertCitation()
{
    new CitationInsertionDialog(textEditor(), m_scbw);
}

void ReferencesTool::insertCustomBibliography(KoBibliographyInfo *defaultTemplate)
{
    Q_UNUSED(defaultTemplate);
    new InsertBibliographyDialog(textEditor(), m_scbw);
}

void ReferencesTool::configureBibliography()
{
    new BibliographyConfigureDialog(textEditor()->document(), m_scbw);
}

void ReferencesTool::formatTableOfContents()
{
    if (textEditor()->block().blockFormat().hasProperty(KoParagraphStyle::TableOfContentsData)) {
        m_configure = new TableOfContentsConfigure(textEditor(), textEditor()->block(), m_stocw);
        connect(m_configure, &QDialog::finished, this, &ReferencesTool::hideCofigureDialog);
    }
}

void ReferencesTool::showConfigureDialog(QAction *action)
{
    m_configure = new TableOfContentsConfigure(textEditor(), action->data().value<QTextBlock>(), m_stocw);
    connect(m_configure, &QDialog::finished, this, &ReferencesTool::hideCofigureDialog);
}

void ReferencesTool::hideCofigureDialog()
{
    disconnect(m_configure, &QDialog::finished, this, &ReferencesTool::hideCofigureDialog);
    m_configure->deleteLater();
}

void ReferencesTool::insertAutoFootNote()
{
    m_note = textEditor()->insertFootNote();
    m_note->setAutoNumbering(true);
}

void ReferencesTool::insertLabeledFootNote(const QString &label)
{
    m_note = textEditor()->insertFootNote();
    m_note->setAutoNumbering(false);
    m_note->setLabel(label);
}

void ReferencesTool::insertAutoEndNote()
{
    m_note = textEditor()->insertEndNote();
    m_note->setAutoNumbering(true);
}

void ReferencesTool::insertLabeledEndNote(const QString &label)
{
    m_note = textEditor()->insertEndNote();
    m_note->setAutoNumbering(false);
    m_note->setLabel(label);
}

void ReferencesTool::showFootnotesConfigureDialog()
{
    NotesConfigurationDialog *dialog = new NotesConfigurationDialog((QTextDocument *)textEditor()->document(), true);
    dialog->exec();
}

void ReferencesTool::showEndnotesConfigureDialog()
{
    NotesConfigurationDialog *dialog = new NotesConfigurationDialog((QTextDocument *)textEditor()->document(), false);
    dialog->exec();
}

void ReferencesTool::updateButtons()
{
    if (textEditor()->currentFrame()->format().intProperty(KoText::SubFrameType) == KoText::NoteFrameType) {
        m_sfenw->widget.addFootnote->setEnabled(false);
        m_sfenw->widget.addEndnote->setEnabled(false);
    } else {
        m_sfenw->widget.addFootnote->setEnabled(true);
        m_sfenw->widget.addEndnote->setEnabled(true);
    }
    if (textEditor()->block().blockFormat().hasProperty(KoParagraphStyle::TableOfContentsData)) {
        action("format_tableofcontents")->setEnabled(true);
    } else {
        action("format_tableofcontents")->setEnabled(false);
    }
}

KoTextEditor *ReferencesTool::editor()
{
    return textEditor();
}

void ReferencesTool::insertCustomToC(KoTableOfContentsGeneratorInfo *defaultTemplate)
{
    m_configure = new TableOfContentsConfigure(textEditor(), defaultTemplate, m_stocw);
    connect(m_configure, &QDialog::accepted, this, &ReferencesTool::customToCGenerated);
    connect(m_configure, &QDialog::finished, this, &ReferencesTool::hideCofigureDialog);
}

void ReferencesTool::customToCGenerated()
{
    if (m_configure) {
        textEditor()->insertTableOfContents(m_configure->currentToCData());
    }
}

void ReferencesTool::insertLink()
{
    new LinkInsertionDialog(textEditor(), m_slw);
}

bool ReferencesTool::validateBookmark(QString bookmarkName)
{
    bookmarkName = bookmarkName.trimmed();
    if (bookmarkName.isEmpty()) {
        m_bmark->setWarningText(0, i18n("Bookmark cannot be empty"));
        return false;
    }
    const KoBookmarkManager *manager = KoTextDocument(editor()->document()).textRangeManager()->bookmarkManager();
    QStringList existingBookmarks = manager->bookmarkNameList();
    int position = existingBookmarks.indexOf(bookmarkName);
    if (position != -1) {
        m_bmark->setWarningText(0, i18n("Duplicate Name. Click \"Manage Bookmarks\""));
        m_bmark->setWarningText(1, i18n("to Rename or Delete Bookmarks"));
        return false;
    } else {
        m_bmark->setWarningText(0, "");
        m_bmark->setWarningText(1, "");
        return true;
    }
}

void ReferencesTool::insertBookmark(QString bookMarkName)
{
    bookMarkName = bookMarkName.trimmed();
    m_bmark->setWarningText(0, "");
    m_bmark->setWarningText(1, "");
    if (validateBookmark(bookMarkName)) {
        editor()->addBookmark(bookMarkName);
        m_bmark->clearLineEdit();
    }
}
