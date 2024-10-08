/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWPageSettingsDialog.h"
#include "KWDocumentColumns.h"
#include "KWPageStyle.h"
#include "KWPageStyle_p.h"

#include <KWDocument.h>
#include <commands/KWChangePageStyleCommand.h>
#include <commands/KWNewPageStyleCommand.h>
#include <commands/KWPageStylePropertiesCommand.h>

#include <KoUnit.h>

#include <QInputDialog>
#include <QListWidget>
#include <QPushButton>

using namespace Qt::StringLiterals;

KWPageSettingsDialog::KWPageSettingsDialog(QWidget *parent, KWDocument *document, const KWPage &page)
    : KoPageLayoutDialog(parent, page.pageStyle().pageLayout())
    , m_document(document)
    , m_page(page)
    , m_pageStyle(page.pageStyle())
{
    Q_ASSERT(document);
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    connect(buttonBox(), &QDialogButtonBox::clicked, this, &KWPageSettingsDialog::slotButtonClicked);
    showUnitchooser(true);
    Q_ASSERT(page.isValid());

    m_columns = new KWDocumentColumns(this, m_page.pageStyle().columns());
    KPageWidgetItem *columnsPage = addPage(m_columns, i18n("Columns"));

    auto pageStyleWidget = new QWidget(this);
    auto pageStyleLayout = new QHBoxLayout(pageStyleWidget);
    pageStyleLayout->setContentsMargins({});
    pageStyleLayout->setSpacing({});
    KPageWidgetItem *stylePage = addPage(pageStyleWidget, i18n("Style"));
    m_pageStylesView = new QListWidget(this);
    pageStyleLayout->addWidget(m_pageStylesView, 1);
    auto separator = new QFrame(this);
    separator->setFrameStyle(QFrame::VLine);
    separator->setFixedWidth(1);
    pageStyleLayout->addWidget(separator);
    connect(m_pageStylesView, &QListWidget::currentRowChanged, this, &KWPageSettingsDialog::pageStyleCurrentRowChanged);
    auto pageStyleLayout2 = new QVBoxLayout();
    pageStyleLayout2->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                         style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                         style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                         style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
    pageStyleLayout2->setSpacing(style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));
    pageStyleLayout->addLayout(pageStyleLayout2);

    m_clonePageStyleButton = new QPushButton(i18n("Clone"), pageStyleWidget);
    m_clonePageStyleButton->setIcon(QIcon::fromTheme(u"edit-clone-symbolic"_s));
    connect(m_clonePageStyleButton, &QAbstractButton::clicked, this, &KWPageSettingsDialog::pageStyleCloneClicked);
    pageStyleLayout2->addWidget(m_clonePageStyleButton);

    m_deletePageStyleButton = new QPushButton(i18n("Delete"), pageStyleWidget);
    m_deletePageStyleButton->setIcon(QIcon::fromTheme(u"delete-symbolic"_s));
    connect(m_deletePageStyleButton, &QAbstractButton::clicked, this, &KWPageSettingsDialog::pageStyleDeleteClicked);
    pageStyleLayout2->addWidget(m_deletePageStyleButton);
    pageStyleLayout2->addStretch();

    for (const auto item : {columnsPage, stylePage}) {
        m_pages[item->name()] = item;
    }

    reloadPageStyles();

    showPageSpread(false); // TODO better would be allow n pages to face rather then only 2
    showTextDirection(true); // TODO can we hide this in selected usecases? Use the resource manager bidi-check maybe?
    // showApplyToDocument(true); // TODO uncommand when we can handle it.

#if 0
    bool simpleSetup = m_document->pageCount() == 1
            || (m_document->pageCount() == 2 && page.pageSide() == KWPage::PageSpread);
    if (!simpleSetup) { // if there is one style, its still a simple doc
        bool onlyOneStyle = true;
        foreach (const KWPage &p, m_document->pageManager()->pages()) {
            if (p.pageStyle() != m_page.pageStyle()) {
                onlyOneStyle = false;
                break;
            }
        }
        if (onlyOneStyle)
            simpleSetup = true;
    }

    if (simpleSetup) {
        /*
          Simple setup means we have currently exactly one page style; so the common usecase
          is that that one will be changed, and it will apply to all pages in the document.
          Lets make that usecase as simple and as straight forward as possible

          Notice that if the user unchecks the "apply to document' checkbox we will automatically
          switch to the not simple setup and use page styles.
         */
        KWPageStyle pageStyle = m_page.pageStyle();
        setPageSpread(pageStyle.isPageSpread());
        setTextDirection(pageStyle.direction());
    }
    else {
        /*
          The document is already not simple anymore; there is more than one page style
          in use. We should show that fact and the user is allowed to use the power of
          page styles.
         */
        setPageSpread(m_page.pageSide() == KWPage::PageSpread);
        setTextDirection(m_page.directionHint());

        /* TODO
        fill the styles combobox and please be smart about it with an auto-generated
        style being named after the pages its assigned to.
        Make sure the current page style is the first entry.

        */
    }
#else
    KWPageStyle pageStyle = m_page.pageStyle();
    setPageSpread(pageStyle.isPageSpread());
    setTextDirection(pageStyle.direction());
#endif

    onDocumentUnitChange(m_document->unit());
    connect(m_document, &KoDocument::unitChanged, this, &KWPageSettingsDialog::onDocumentUnitChange);
    connect(this, &KoPageLayoutDialog::unitChanged, this, &KWPageSettingsDialog::setDocumentUnit);
}

KPageWidgetItem *KWPageSettingsDialog::pageItem(const QString &name) const
{
    return m_pages.value(name);
}

void KWPageSettingsDialog::accept()
{
    slotApplyClicked();
    KoPageLayoutDialog::accept();
}

void KWPageSettingsDialog::reject()
{
    KoPageLayoutDialog::reject();
}

void KWPageSettingsDialog::slotApplyClicked()
{
    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Change Page Style"));
    KWPageStyle styleToUpdate = m_pageStyle;

    if (styleToUpdate.name() != m_page.pageStyle().name()) {
        new KWChangePageStyleCommand(m_document, m_page, styleToUpdate, cmd);
    }

    styleToUpdate.detach(styleToUpdate.name());

    styleToUpdate.setDirection(textDirection());
    KoPageLayout lay = pageLayout();
    if (lay.pageEdge >= 0 || lay.bindingSide >= 0) {
        // asserts check if our super didn't somehow mess up
        Q_ASSERT(lay.pageEdge >= 0);
        Q_ASSERT(lay.bindingSide >= 0);
        Q_ASSERT(lay.leftMargin == -1);
        Q_ASSERT(lay.rightMargin == -1);

        // its a page spread, which words can handle, so we can safely set the
        // normal page size and assume that the page object will do the right thing
        lay.width /= (qreal)2;
    }
    styleToUpdate.setPageLayout(lay);
    styleToUpdate.setColumns(m_columns->columns());

    new KWPageStylePropertiesCommand(m_document, m_pageStyle, styleToUpdate, cmd);
    m_document->addCommand(cmd);
    m_document->firePageSetupChanged();
}

void KWPageSettingsDialog::slotButtonClicked(QAbstractButton *button)
{
    if (button == buttonBox()->button(QDialogButtonBox::Apply)) {
        slotApplyClicked();
    }
}

void KWPageSettingsDialog::setDocumentUnit(const KoUnit &unit)
{
    m_document->setUnit(unit);
}

void KWPageSettingsDialog::onDocumentUnitChange(const KoUnit &unit)
{
    setUnit(unit);
    m_columns->setUnit(unit);
}

void KWPageSettingsDialog::reloadPageStyles()
{
    QStringList pagestyles = m_document->pageManager()->pageStyles().keys();
    std::sort(pagestyles.begin(), pagestyles.end());
    m_pageStylesView->clear();
    m_pageStylesView->addItems(pagestyles);
    m_pageStylesView->setCurrentRow(pagestyles.indexOf(m_pageStyle.name()));
}

void KWPageSettingsDialog::pageStyleCloneClicked()
{
    QListWidgetItem *item = m_pageStylesView->item(m_pageStylesView->currentRow());
    Q_ASSERT(item);
    KWPageStyle pagestyle = m_document->pageManager()->pageStyle(item->text());
    Q_ASSERT(pagestyle.isValid());
    class Validator : public QValidator
    {
    public:
        Validator(KWDocument *document)
            : QValidator()
            , m_document(document)
        {
        }
        State validate(QString &input, int &) const override
        {
            return input.trimmed().isEmpty() || m_document->pageManager()->pageStyle(input).isValid() ? Intermediate : Acceptable;
        }

    private:
        KWDocument *m_document;
    };
    Validator validator(m_document);
    QString name = QInputDialog::getText(this,
                                         i18n("Clone Page Style"),
                                         i18n("Add a new page style with the name:"),
                                         QLineEdit::Normal,
                                         pagestyle.name()); // QT5TODO: &validator);
    if (name.isEmpty())
        return;
    pagestyle.detach(name);
    m_document->pageManager()->addPageStyle(pagestyle);
    reloadPageStyles();
}

void KWPageSettingsDialog::pageStyleDeleteClicked()
{
    QListWidgetItem *item = m_pageStylesView->item(m_pageStylesView->currentRow());
    Q_ASSERT(item);
    QString defaultPageStyleName = m_document->pageManager()->defaultPageStyle().name();
    Q_ASSERT(item->text() != defaultPageStyleName);
    KWPageStyle pagestyle = m_document->pageManager()->pageStyle(item->text());
    Q_ASSERT(pagestyle.isValid());
    m_document->pageManager()->removePageStyle(pagestyle);
    int row = m_pageStylesView->currentRow() + (m_pageStylesView->currentRow() >= 1 ? -1 : +1);
    Q_ASSERT(row >= 0 && row < m_pageStylesView->count());
    pageStyleCurrentRowChanged(row);
    reloadPageStyles();
}

void KWPageSettingsDialog::pageStyleCurrentRowChanged(int row)
{
    QListWidgetItem *item = m_pageStylesView->item(row);
    KWPageStyle pagestyle = item ? m_document->pageManager()->pageStyle(item->text()) : KWPageStyle();
    if (pagestyle.isValid())
        m_pageStyle = pagestyle;
    setPageLayout(m_pageStyle.pageLayout());
    setPageSpread(m_pageStyle.isPageSpread());
    setTextDirection(m_pageStyle.direction());
    m_columns->setColumns(m_pageStyle.columns());
    m_clonePageStyleButton->setEnabled(pagestyle.isValid());
    m_deletePageStyleButton->setEnabled(pagestyle.isValid() && item->text() != m_document->pageManager()->defaultPageStyle().name());
    buttonBox()->button(QDialogButtonBox::Ok)->setEnabled(pagestyle.isValid());
    buttonBox()->button(QDialogButtonBox::Apply)->setEnabled(pagestyle.isValid());
}
