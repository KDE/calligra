/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef REFERENCESTOOL_H
#define REFERENCESTOOL_H

#include "TextTool.h"
#include <QWidget>
#include <signal.h>

class KoCanvasBase;
class TableOfContentsConfigure;
class SimpleTableOfContentsWidget;
class SimpleFootEndNotesWidget;
class SimpleCitationBibliographyWidget;
class KoInlineNote;
class KoTextEditor;
class KoBibliographyInfo;
class KoTableOfContentsGeneratorInfo;
class SimpleLinksWidget;
class LabeledWidget;
/// This tool is the ui for inserting Table of Contents, Citations/bibliography, footnotes, endnotes, index, table of illustrations etc

class ReferencesTool : public TextTool
{
    Q_OBJECT
public:
    explicit ReferencesTool(KoCanvasBase *canvas);

    ~ReferencesTool() override;

    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;

    void createActions() override;

    KoTextEditor *editor();
    /// inserts a ToC and open a configure dialog for customization
    void insertCustomToC(KoTableOfContentsGeneratorInfo *defaultTemplate);
    /// insert a bibliography and open a configure dialog for customization
    void insertCustomBibliography(KoBibliographyInfo *defaultTemplate);

protected:
    /// reimplemented from superclass
    QList<QPointer<QWidget>> createOptionWidgets() override;

private Q_SLOTS:
    /// insert a citation
    void insertCitation();
    /// configure a bibliography
    void configureBibliography();
    /// format the table of contents template
    void formatTableOfContents();
    /// shows the configuration dialog for a ToC
    void showConfigureDialog(QAction *action);
    /// hides the configuration dialog for ToC
    void hideCofigureDialog();
    /// insert an autonumbered footnote
    void insertAutoFootNote();
    /// insert a labeled footnote
    void insertLabeledFootNote(const QString &label);
    /// insert an autonumbered endnote
    void insertAutoEndNote();
    /// insert a labeled endnote
    void insertLabeledEndNote(const QString &label);
    /// show the configuration dialog for footnotes
    void showFootnotesConfigureDialog();
    /// show the configuration dialog for endnotes
    void showEndnotesConfigureDialog();
    /// enable/disable buttons if cursor in notes' body or not
    void updateButtons();

    void customToCGenerated();
    /// insert a Link
    void insertLink();
    /// insert a bookmark
    void insertBookmark(QString bookmarkName);
    /// validate a bookmark
    bool validateBookmark(QString bookmarkName);

private:
    TableOfContentsConfigure *m_configure;
    SimpleTableOfContentsWidget *m_stocw;
    SimpleFootEndNotesWidget *m_sfenw;
    KoInlineNote *m_note;
    SimpleCitationBibliographyWidget *m_scbw;
    SimpleLinksWidget *m_slw;
    LabeledWidget *m_bmark;
    KoCanvasBase *m_canvas;
};

class QAction;
class QLineEdit;
class QLabel;
class LabeledWidget : public QWidget
{
    Q_OBJECT
public:
    enum LabelPosition { INLINE, ABOVE };
    LabeledWidget(QAction *action, const QString &label, LabelPosition pos, bool warningLabelRequired);
    virtual ~LabeledWidget();
    void setWarningText(int pos, const QString &warning);
    void clearLineEdit();
Q_SIGNALS:
    void textTriggered(const QString &label);
    void lineEditChanged(const QString &);

private Q_SLOTS:
    void returnPressed();

protected:
    void enterEvent(QEnterEvent *event) override;

private:
    QLineEdit *m_lineEdit;
    QLabel *m_warningLabel[2];
    QAction *m_action;
};

#endif // REFERENCESTOOL_H
