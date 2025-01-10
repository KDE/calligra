/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * SPDX-FileCopyrightText: 2008, 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 * SPDX-FileCopyrightText: 2009 KO GmbH <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TextTool.h"

#include "FontFamilyAction.h"
#include "FontSizeAction.h"
#include "TextEditingPluginContainer.h"
#include "TextShapeDebug.h"
#include "commands/AutoResizeCommand.h"
#include "commands/ChangeListLevelCommand.h"
#include "dialogs/FontDia.h"
#include "dialogs/InsertCharacter.h"
#include "dialogs/ParagraphSettingsDialog.h"
#include "dialogs/SectionFormatDialog.h"
#include "dialogs/SectionsSplitDialog.h"
#include "dialogs/SimpleCharacterWidget.h"
#include "dialogs/SimpleInsertWidget.h"
#include "dialogs/SimpleParagraphWidget.h"
#include "dialogs/SimpleTableWidget.h"
#include "dialogs/StyleManagerDialog.h"
#include "dialogs/TableDialog.h"

#include <KoBookmark.h>
#include <KoBookmarkManager.h>
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceManager.h>
#include <KoChangeTracker.h>
#include <KoChangeTrackerElement.h>
#include <KoColor.h>
#include <KoColorBackground.h>
#include <KoColorPopupAction.h>
#include <KoInlineNote.h>
#include <KoInlineTextObjectManager.h>
#include <KoListLevelProperties.h>
#include <KoOdf.h>
#include <KoParagraphStyle.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextDrag.h>
#include <KoTextEditingPlugin.h>
#include <KoTextEditingRegistry.h>
#include <KoTextEditor.h>
#include <KoTextLayoutRootArea.h>
#include <KoTextOdfSaveHelper.h>
#include <KoTextRangeManager.h>
#include <KoToolSelection.h>
// #include <ResizeTableCommand.h>
#include <KoIcon.h>

#include <KActionMenu>
#include <KMessageBox>
#include <KSharedConfig>
#include <KStandardAction>
#include <kstandardshortcut.h>

#include <QAction>
#include <QActionGroup>
#include <QBitmap>
#include <QDesktopServices>
#include <QDrag>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QGraphicsObject>
#include <QLinearGradient>
#include <QMenu>
#include <QMenuBar>
#include <QMimeData>
#include <QTabWidget>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QTextTable>
#include <QToolTip>

#include "AnnotationTextShape.h"
#define AnnotationShape_SHAPEID "AnnotationTextShapeID"
#include "KoShapeBasedDocumentBase.h"
#include <KUser>
#include <KoAnnotation.h>
#include <KoShapeRegistry.h>

#include <KoDocumentRdfBase.h>

#include <algorithm>

class TextToolSelection : public KoToolSelection
{
public:
    TextToolSelection(QPointer<KoTextEditor> editor)
        : KoToolSelection(nullptr)
        , m_editor(editor)
    {
    }

    bool hasSelection() override
    {
        if (!m_editor.isNull()) {
            return m_editor.data()->hasSelection();
        }
        return false;
    }

    QPointer<KoTextEditor> m_editor;
};

static bool hit(const QKeySequence &input, KStandardShortcut::StandardShortcut shortcut)
{
    foreach (const QKeySequence &ks, KStandardShortcut::shortcut(shortcut)) {
        if (input == ks)
            return true;
    }
    return false;
}

TextTool::TextTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_textShape(nullptr)
    , m_textShapeData(nullptr)
    , m_changeTracker(nullptr)
    , m_allowActions(true)
    , m_allowAddUndoCommand(true)
    , m_allowResourceManagerUpdates(true)
    , m_prevCursorPosition(-1)
    , m_caretTimer(this)
    , m_caretTimerState(true)
    , m_currentCommand(nullptr)
    , m_currentCommandHasChildren(false)
    , m_specialCharacterDocker(nullptr)
    , m_textTyping(false)
    , m_textDeleting(false)
    , m_editTipTimer(this)
    , m_delayedEnsureVisible(false)
    , m_toolSelection(nullptr)
    , m_tableDraggedOnce(false)
    , m_tablePenMode(false)
    , m_lastImMicroFocus(QRectF(0, 0, 0, 0))
    , m_drag(nullptr)
{
    setTextMode(true);

    createActions();

    m_unit = canvas->resourceManager()->unitResource(KoCanvasResourceManager::Unit);

    foreach (KoTextEditingPlugin *plugin, textEditingPluginContainer()->values()) {
        connect(plugin, &KoTextEditingPlugin::startMacro, this, &TextTool::startMacro);
        connect(plugin, &KoTextEditingPlugin::stopMacro, this, &TextTool::stopMacro);
        const QHash<QString, QAction *> actions = plugin->actions();
        QHash<QString, QAction *>::ConstIterator i = actions.begin();
        while (i != actions.end()) {
            addAction(i.key(), i.value());
            ++i;
        }
    }

    // setup the context list.
    QList<QAction *> list;
    list.append(this->action("format_font"));
    foreach (const QString &key, KoTextEditingRegistry::instance()->keys()) {
        KoTextEditingFactory *factory = KoTextEditingRegistry::instance()->value(key);
        if (factory && factory->showInMenu()) {
            QAction *a = new QAction(factory->title(), this);
            connect(a, &QAction::triggered, [this, factory] {
                startTextEditingPlugin(factory->id());
            });
            list.append(a);
            addAction(QString("apply_%1").arg(factory->id()), a);
        }
    }
    setPopupActionList(list);

    connect(canvas->shapeManager()->selection(), &KoSelection::selectionChanged, this, &TextTool::shapeAddedToCanvas);

    m_caretTimer.setInterval(500);
    connect(&m_caretTimer, &QTimer::timeout, this, &TextTool::blinkCaret);

    m_editTipTimer.setInterval(500);
    m_editTipTimer.setSingleShot(true);
    connect(&m_editTipTimer, &QTimer::timeout, this, &TextTool::showEditTip);
}

void TextTool::createActions()
{
    bool useAdvancedText =
        !(canvas()->resourceManager()->intResource(KoCanvasResourceManager::ApplicationSpeciality) & KoCanvasResourceManager::NoAdvancedText);

    m_actionConfigureSection = new QAction(koIconNeededWithSubs("", "configure-text-section", "configure"), i18n("Configure current section"), this);
    addAction("configure_section", m_actionConfigureSection);
    connect(m_actionConfigureSection, &QAction::triggered, this, &TextTool::configureSection);

    m_actionInsertSection = new QAction(koIconNeededWithSubs("", "insert-text-section", "insert-text"), i18n("Insert new section"), this);
    addAction("insert_section", m_actionInsertSection);
    connect(m_actionInsertSection, &QAction::triggered, this, &TextTool::insertNewSection);

    m_actionSplitSections = new QAction(koIconNeededWithSubs("", "text-section-split", "split"), i18n("Insert paragraph between sections"), this);
    addAction("split_sections", m_actionSplitSections);
    connect(m_actionSplitSections, &QAction::triggered, this, &TextTool::splitSections);

    m_actionPasteAsText = new QAction(koIcon("edit-paste"), i18n("Paste As Text"), this);
    addAction("edit_paste_text", m_actionPasteAsText);
    m_actionPasteAsText->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_V);
    connect(m_actionPasteAsText, &QAction::triggered, this, &TextTool::pasteAsText);

    m_actionFormatBold = new QAction(koIcon("format-text-bold"), i18n("Bold"), this);
    addAction("format_bold", m_actionFormatBold);
    m_actionFormatBold->setShortcut(Qt::CTRL | Qt::Key_B);
    m_actionFormatBold->setCheckable(true);
    connect(m_actionFormatBold, &QAction::triggered, this, &TextTool::bold);

    m_actionFormatItalic = new QAction(koIcon("format-text-italic"), i18n("Italic"), this);
    addAction("format_italic", m_actionFormatItalic);
    m_actionFormatItalic->setShortcut(Qt::CTRL | Qt::Key_I);
    m_actionFormatItalic->setCheckable(true);
    connect(m_actionFormatItalic, &QAction::triggered, this, &TextTool::italic);

    m_actionFormatUnderline = new QAction(koIcon("format-text-underline"), i18nc("Text formatting", "Underline"), this);
    addAction("format_underline", m_actionFormatUnderline);
    m_actionFormatUnderline->setShortcut(Qt::CTRL | Qt::Key_U);
    m_actionFormatUnderline->setCheckable(true);
    connect(m_actionFormatUnderline, &QAction::triggered, this, &TextTool::underline);

    m_actionFormatStrikeOut = new QAction(koIcon("format-text-strikethrough"), i18n("Strikethrough"), this);
    addAction("format_strike", m_actionFormatStrikeOut);
    m_actionFormatStrikeOut->setCheckable(true);
    connect(m_actionFormatStrikeOut, &QAction::triggered, this, &TextTool::strikeOut);

    QActionGroup *alignmentGroup = new QActionGroup(this);
    m_actionAlignLeft = new QAction(koIcon("format-justify-left"), i18n("Align Left"), this);
    addAction("format_alignleft", m_actionAlignLeft);
    m_actionAlignLeft->setShortcut(Qt::CTRL | Qt::Key_L);
    m_actionAlignLeft->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignLeft);
    connect(m_actionAlignLeft, &QAction::triggered, this, &TextTool::alignLeft);

    m_actionAlignRight = new QAction(koIcon("format-justify-right"), i18n("Align Right"), this);
    addAction("format_alignright", m_actionAlignRight);
    m_actionAlignRight->setShortcut(Qt::CTRL | Qt::Key_R);
    m_actionAlignRight->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignRight);
    connect(m_actionAlignRight, &QAction::triggered, this, &TextTool::alignRight);

    m_actionAlignCenter = new QAction(koIcon("format-justify-center"), i18n("Align Center"), this);
    addAction("format_aligncenter", m_actionAlignCenter);
    m_actionAlignCenter->setShortcut(Qt::CTRL | Qt::Key_E);
    m_actionAlignCenter->setCheckable(true);

    alignmentGroup->addAction(m_actionAlignCenter);
    connect(m_actionAlignCenter, &QAction::triggered, this, &TextTool::alignCenter);

    m_actionAlignBlock = new QAction(koIcon("format-justify-fill"), i18n("Align Block"), this);
    addAction("format_alignblock", m_actionAlignBlock);
    m_actionAlignBlock->setShortcut(Qt::CTRL | Qt::Key_J);
    m_actionAlignBlock->setCheckable(true);
    alignmentGroup->addAction(m_actionAlignBlock);
    connect(m_actionAlignBlock, &QAction::triggered, this, &TextTool::alignBlock);

    m_actionChangeDirection = new QAction(koIcon("format-text-direction-rtl"), i18n("Change text direction"), this);
    addAction("change_text_direction", m_actionChangeDirection);
    m_actionChangeDirection->setToolTip(i18n("Change writing direction"));
    m_actionChangeDirection->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_D);
    m_actionChangeDirection->setCheckable(true);
    connect(m_actionChangeDirection, &QAction::triggered, this, &TextTool::textDirectionChanged);

    m_actionFormatSuper = new QAction(koIcon("format-text-superscript"), i18n("Superscript"), this);
    m_actionFormatSuper->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_P);
    addAction("format_super", m_actionFormatSuper);
    m_actionFormatSuper->setCheckable(true);
    connect(m_actionFormatSuper, &QAction::triggered, this, &TextTool::superScript);

    m_actionFormatSub = new QAction(koIcon("format-text-subscript"), i18n("Subscript"), this);
    m_actionFormatSub->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_B);
    addAction("format_sub", m_actionFormatSub);
    m_actionFormatSub->setCheckable(true);
    connect(m_actionFormatSub, &QAction::triggered, this, &TextTool::subScript);

    const char *const increaseIndentActionIconName =
        QApplication::isRightToLeft() ? koIconNameCStr("format-indent-less") : koIconNameCStr("format-indent-more");
    m_actionFormatIncreaseIndent = new QAction(QIcon::fromTheme(QLatin1String(increaseIndentActionIconName)), i18n("Increase Indent"), this);
    addAction("format_increaseindent", m_actionFormatIncreaseIndent);
    connect(m_actionFormatIncreaseIndent, &QAction::triggered, this, &TextTool::increaseIndent);

    const char *const decreaseIndentActionIconName =
        QApplication::isRightToLeft() ? koIconNameCStr("format-indent-more") : koIconNameCStr("format-indent-less");
    m_actionFormatDecreaseIndent = new QAction(QIcon::fromTheme(QLatin1String(decreaseIndentActionIconName)), i18n("Decrease Indent"), this);
    addAction("format_decreaseindent", m_actionFormatDecreaseIndent);
    connect(m_actionFormatDecreaseIndent, &QAction::triggered, this, &TextTool::decreaseIndent);

    QAction *action = new QAction(koIcon("format-list-unordered"), i18n("Toggle List or List Level Formatting"), this);
    action->setToolTip(i18n("Toggle list on/off, or change format of current level"));
    addAction("format_list", action);

    action = new QAction(i18n("Increase Font Size"), this);
    action->setShortcut(Qt::CTRL | Qt::Key_Greater);
    addAction("fontsizeup", action);
    connect(action, &QAction::triggered, this, &TextTool::increaseFontSize);

    action = new QAction(i18n("Decrease Font Size"), this);
    action->setShortcut(Qt::CTRL | Qt::Key_Less);
    addAction("fontsizedown", action);
    connect(action, &QAction::triggered, this, &TextTool::decreaseFontSize);

    m_actionFormatFontFamily = new KoFontFamilyAction(this);
    m_actionFormatFontFamily->setText(i18n("Font Family"));
    addAction("format_fontfamily", m_actionFormatFontFamily);
    connect(m_actionFormatFontFamily, &KoFontFamilyAction::textTriggered, this, &TextTool::setFontFamily);

    m_variableMenu = new KActionMenu(i18n("Variable"), this);
    addAction("insert_variable", m_variableMenu);

    // ------------------- Actions with a key binding and no GUI item
    action = new QAction(i18n("Insert Non-Breaking Space"), this);
    addAction("nonbreaking_space", action);
    action->setShortcut(Qt::CTRL | Qt::Key_Space);
    connect(action, &QAction::triggered, this, &TextTool::nonbreakingSpace);

    action = new QAction(i18n("Insert Non-Breaking Hyphen"), this);
    addAction("nonbreaking_hyphen", action);
    action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_Minus);
    connect(action, &QAction::triggered, this, &TextTool::nonbreakingHyphen);

    action = new QAction(i18n("Insert Index"), this);
    action->setShortcut(Qt::CTRL | Qt::Key_T);
    addAction("insert_index", action);
    connect(action, &QAction::triggered, this, &TextTool::insertIndexMarker);

    action = new QAction(i18n("Insert Soft Hyphen"), this);
    addAction("soft_hyphen", action);
    // action->setShortcut(Qt::CTRL + Qt::Key_Minus); // TODO this one is also used for the kde-global zoom-out :(
    connect(action, &QAction::triggered, this, &TextTool::softHyphen);

    if (useAdvancedText) {
        action = new QAction(i18n("Line Break"), this);
        addAction("line_break", action);
        action->setShortcut(Qt::SHIFT | Qt::Key_Return);
        connect(action, &QAction::triggered, this, &TextTool::lineBreak);

        action = new QAction(koIcon("insert-page-break"), i18n("Page Break"), this);
        addAction("insert_framebreak", action);
        action->setShortcut(Qt::CTRL | Qt::Key_Return);
        connect(action, &QAction::triggered, this, &TextTool::insertFrameBreak);
        action->setToolTip(i18n("Insert a page break"));
        action->setWhatsThis(i18n("All text after this point will be moved into the next page."));
    }

    action = new QAction(i18n("Font..."), this);
    addAction("format_font", action);
    action->setShortcut(Qt::ALT | Qt::CTRL | Qt::Key_F);
    action->setToolTip(i18n("Change character size, font, boldface, italics etc."));
    action->setWhatsThis(i18n("Change the attributes of the currently selected characters."));
    connect(action, &QAction::triggered, this, &TextTool::selectFont);

    m_actionFormatFontSize = new FontSizeAction(i18n("Font Size"), this);
    addAction("format_fontsize", m_actionFormatFontSize);
    connect(m_actionFormatFontSize, &FontSizeAction::fontSizeChanged, this, &TextTool::setFontSize);

    m_actionFormatTextColor = new KoColorPopupAction(this);
    m_actionFormatTextColor->setIcon(koIcon("format-text-color"));
    m_actionFormatTextColor->setToolTip(i18n("Text Color..."));
    m_actionFormatTextColor->setText(i18n("Text Color"));
    addAction("format_textcolor", m_actionFormatTextColor);
    connect(m_actionFormatTextColor, &KoColorPopupAction::colorChanged, this, &TextTool::setTextColor);

    m_actionFormatBackgroundColor = new KoColorPopupAction(this);
    m_actionFormatBackgroundColor->setIcon(koIcon("format-fill-color"));
    m_actionFormatBackgroundColor->setToolTip(i18n("Background Color..."));
    m_actionFormatBackgroundColor->setText(i18n("Background"));
    addAction("format_backgroundcolor", m_actionFormatBackgroundColor);
    connect(m_actionFormatBackgroundColor, &KoColorPopupAction::colorChanged, this, &TextTool::setBackgroundColor);

    m_autoResizeAction = new QAction(koIcon("zoom-fit-best"), i18n("Auto Resize To Content"), this);
    addAction("auto_resize", m_autoResizeAction);
    m_autoResizeAction->setCheckable(true);
    connect(m_autoResizeAction, &QAction::triggered, this, &TextTool::setAutoResize);

    m_growWidthAction = new QAction(koIcon("zoom-fit-best"), i18n("Grow To Fit Width"), this);
    addAction("grow_to_fit_width", m_growWidthAction);
    m_growWidthAction->setCheckable(true);
    connect(m_growWidthAction, &QAction::triggered, this, &TextTool::setGrowWidthToFit);

    m_growHeightAction = new QAction(koIcon("zoom-fit-best"), i18n("Grow To Fit Height"), this);
    addAction("grow_to_fit_height", m_growHeightAction);
    m_growHeightAction->setCheckable(true);
    connect(m_growHeightAction, &QAction::triggered, this, &TextTool::setGrowHeightToFit);

    m_shrinkToFitAction = new QAction(koIcon("zoom-fit-best"), i18n("Shrink To Fit"), this);
    addAction("shrink_to_fit", m_shrinkToFitAction);
    m_shrinkToFitAction->setCheckable(true);
    connect(m_shrinkToFitAction, &QAction::triggered, this, &TextTool::setShrinkToFit);

    if (useAdvancedText) {
        action = new QAction(koIcon("insert-table"), i18n("Insert Custom..."), this);
        addAction("insert_table", action);
        action->setToolTip(i18n("Insert a table into the document."));
        connect(action, &QAction::triggered, this, &TextTool::insertTable);

        action = new QAction(koIcon("edit-table-insert-row-above"), i18n("Row Above"), this);
        action->setToolTip(i18n("Insert Row Above"));
        addAction("insert_tablerow_above", action);
        connect(action, &QAction::triggered, this, &TextTool::insertTableRowAbove);

        action = new QAction(koIcon("edit-table-insert-row-below"), i18n("Row Below"), this);
        action->setToolTip(i18n("Insert Row Below"));
        addAction("insert_tablerow_below", action);
        connect(action, &QAction::triggered, this, &TextTool::insertTableRowBelow);

        action = new QAction(koIcon("edit-table-insert-column-left"), i18n("Column Left"), this);
        action->setToolTip(i18n("Insert Column Left"));
        addAction("insert_tablecolumn_left", action);
        connect(action, &QAction::triggered, this, &TextTool::insertTableColumnLeft);

        action = new QAction(koIcon("edit-table-insert-column-right"), i18n("Column Right"), this);
        action->setToolTip(i18n("Insert Column Right"));
        addAction("insert_tablecolumn_right", action);
        connect(action, &QAction::triggered, this, &TextTool::insertTableColumnRight);

        action = new QAction(koIcon("edit-table-delete-column"), i18n("Column"), this);
        action->setToolTip(i18n("Delete Column"));
        addAction("delete_tablecolumn", action);
        connect(action, &QAction::triggered, this, &TextTool::deleteTableColumn);

        action = new QAction(koIcon("edit-table-delete-row"), i18n("Row"), this);
        action->setToolTip(i18n("Delete Row"));
        addAction("delete_tablerow", action);
        connect(action, &QAction::triggered, this, &TextTool::deleteTableRow);

        action = new QAction(koIcon("edit-table-cell-merge"), i18n("Merge Cells"), this);
        addAction("merge_tablecells", action);
        connect(action, &QAction::triggered, this, &TextTool::mergeTableCells);

        action = new QAction(koIcon("edit-table-cell-split"), i18n("Split Cells"), this);
        addAction("split_tablecells", action);
        connect(action, &QAction::triggered, this, &TextTool::splitTableCells);

        action = new QAction(koIcon("borderpainter"), "", this);
        action->setToolTip(i18n("Select a border style and paint that style onto a table"));
        addAction("activate_borderpainter", action);
    }

    action = new QAction(i18n("Paragraph..."), this);
    addAction("format_paragraph", action);
    action->setShortcut(Qt::ALT | Qt::CTRL | Qt::Key_P);
    action->setToolTip(i18n("Change paragraph margins, text flow, borders, bullets, numbering etc."));
    action->setWhatsThis(
        i18n("<p>Change paragraph margins, text flow, borders, bullets, numbering etc.</p><p>Select text in multiple paragraphs to change the formatting of "
             "all selected paragraphs.</p><p>If no text is selected, the paragraph where the cursor is located will be changed.</p>"));
    connect(action, &QAction::triggered, this, &TextTool::formatParagraph);

    action = new QAction(i18n("Style Manager..."), this);
    action->setShortcut(Qt::ALT | Qt::CTRL | Qt::Key_S);
    action->setToolTip(i18n("Change attributes of styles"));
    action->setWhatsThis(i18n("<p>Change font and paragraph attributes of styles.</p><p>Multiple styles can be changed using the dialog box.</p>"));
    addAction("format_stylist", action);
    connect(action, &QAction::triggered, this, &TextTool::showStyleManager);

    action = KStandardAction::selectAll(this, SLOT(selectAll()), this);
    addAction("edit_select_all", action);

    action = new QAction(i18n("Special Character..."), this);
    action->setIcon(koIcon("character-set"));
    action->setShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_C);
    addAction("insert_specialchar", action);
    action->setToolTip(i18n("Insert one or more symbols or characters not found on the keyboard"));
    action->setWhatsThis(i18n("Insert one or more symbols or characters not found on the keyboard."));
    connect(action, &QAction::triggered, this, &TextTool::insertSpecialCharacter);

    action = new QAction(i18n("Repaint"), this);
    action->setIcon(koIcon("view-refresh"));
    addAction("repaint", action);
    connect(action, &QAction::triggered, this, &TextTool::relayoutContent);

    action = new QAction(i18n("Insert Comment"), this);
    addAction("insert_annotation", action);
    action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::Key_C);
    connect(action, &QAction::triggered, this, &TextTool::insertAnnotation);

#ifndef NDEBUG
    action = new QAction("Paragraph Debug", this); // do NOT add i18n!
    action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::ALT | Qt::Key_P);
    addAction("detailed_debug_paragraphs", action);
    connect(action, &QAction::triggered, this, &TextTool::debugTextDocument);
    action = new QAction("Styles Debug", this); // do NOT add i18n!
    action->setShortcut(Qt::CTRL | Qt::SHIFT | Qt::ALT | Qt::Key_S);
    addAction("detailed_debug_styles", action);
    connect(action, &QAction::triggered, this, &TextTool::debugTextStyles);
#endif
}

#ifndef NDEBUG
#include "tests/MockShapes.h"
#include <QMimeDatabase>
#include <QMimeType>
#include <kundo2stack.h>

TextTool::TextTool(MockCanvas *canvas) // constructor for our unit tests;
    : KoToolBase(canvas)
    , m_textShape(nullptr)
    , m_textShapeData(nullptr)
    , m_changeTracker(nullptr)
    , m_allowActions(true)
    , m_allowAddUndoCommand(true)
    , m_allowResourceManagerUpdates(true)
    , m_prevCursorPosition(-1)
    , m_caretTimer(this)
    , m_caretTimerState(true)
    , m_currentCommand(nullptr)
    , m_currentCommandHasChildren(false)
    , m_specialCharacterDocker(nullptr)
    , m_textEditingPlugins(nullptr)
    , m_editTipTimer(this)
    , m_delayedEnsureVisible(false)
    , m_tableDraggedOnce(false)
    , m_tablePenMode(false)
{
    // we could init some vars here, but we probably don't have to
    QLocale::setDefault(QLocale("en"));
    QTextDocument *document = new QTextDocument(); // this document is leaked

    KoInlineTextObjectManager *inlineManager = new KoInlineTextObjectManager();
    KoTextDocument(document).setInlineTextObjectManager(inlineManager);

    KoTextRangeManager *locationManager = new KoTextRangeManager();
    KoTextDocument(document).setTextRangeManager(locationManager);

    m_textEditor = new KoTextEditor(document);
    KoTextDocument(document).setTextEditor(m_textEditor.data());
    m_toolSelection = new TextToolSelection(m_textEditor);

    m_changeTracker = new KoChangeTracker();
    KoTextDocument(document).setChangeTracker(m_changeTracker);

    KoTextDocument(document).setUndoStack(new KUndo2Stack());
}
#endif

TextTool::~TextTool()
{
    delete m_toolSelection;
}

void TextTool::showEditTip()
{
    if (!m_textShapeData || m_editTipPointedAt.position == -1)
        return;

    QTextCursor c(m_textShapeData->document());
    c.setPosition(m_editTipPointedAt.position);
    QString text = "<p align=center style=\'white-space:pre\' >";
    int toolTipWidth = 0;

    if (m_changeTracker && m_changeTracker->containsInlineChanges(c.charFormat()) && m_changeTracker->displayChanges()) {
        KoChangeTrackerElement *element = m_changeTracker->elementById(c.charFormat().property(KoCharacterStyle::ChangeTrackerId).toInt());
        if (element->isEnabled()) {
            QString changeType;
            if (element->getChangeType() == KoGenChange::InsertChange)
                changeType = i18n("Insertion");
            else if (element->getChangeType() == KoGenChange::DeleteChange)
                changeType = i18n("Deletion");
            else
                changeType = i18n("Formatting");

            text += "<b>" + changeType + "</b><br/>";

            QString date = element->getDate();
            // Remove the T which separates the Data and Time.
            date[10] = QLatin1Char(' ');
            date = element->getCreator() + QLatin1Char(' ') + date;
            text += date + "</p>";

            toolTipWidth = QFontMetrics(QToolTip::font()).boundingRect(date).width();
        }
    }

    if (m_editTipPointedAt.bookmark || !m_editTipPointedAt.externalHRef.isEmpty()) {
        QString help = i18n("Ctrl+click to go to link ");
        help += m_editTipPointedAt.externalHRef;
        text += help + "</p>";
        toolTipWidth = QFontMetrics(QToolTip::font()).boundingRect(help).width();
    }

    if (m_editTipPointedAt.note) {
        QString help = i18n("Ctrl+click to go to the note ");
        text += help + "</p>";
        toolTipWidth = QFontMetrics(QToolTip::font()).boundingRect(help).width();
    }

    if (m_editTipPointedAt.noteReference > 0) {
        QString help = i18n("Ctrl+click to go to the note reference");
        text += help + "</p>";
        toolTipWidth = QFontMetrics(QToolTip::font()).boundingRect(help).width();
    }

    QToolTip::hideText();

    if (toolTipWidth) {
        QRect keepRect(m_editTipPos - QPoint(3, 3), QSize(6, 6));
        QToolTip::showText(m_editTipPos - QPoint(toolTipWidth / 2, 0), text, canvas()->canvasWidget(), keepRect);
    }
}

void TextTool::blinkCaret()
{
    if (!(canvas()->canvasWidget() ? canvas()->canvasWidget()->hasFocus() : canvas()->canvasItem()->hasFocus())) {
        m_caretTimer.stop();
        m_caretTimerState = false; // not visible.
    } else {
        m_caretTimerState = !m_caretTimerState;
    }
    repaintCaret();
}

void TextTool::relayoutContent()
{
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(m_textShapeData->document()->documentLayout());
    Q_ASSERT(lay);
    foreach (KoTextLayoutRootArea *rootArea, lay->rootAreas()) {
        rootArea->setDirty();
    }
    lay->emitLayoutIsDirty();
}

void TextTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_textEditor.isNull())
        return;
    if (canvas() && ((canvas()->canvasWidget() && canvas()->canvasWidget()->hasFocus()) || (canvas()->canvasItem() && canvas()->canvasItem()->hasFocus()))
        && !m_caretTimer.isActive()) { // make sure we blink
        m_caretTimer.start();
        m_caretTimerState = true;
    }
    if (!m_caretTimerState)
        m_caretTimer.setInterval(500); // we set it lower during typing, so set it back to normal

    if (!m_textShapeData)
        return;
    if (m_textShapeData->isDirty())
        return;

    qreal zoomX, zoomY;
    converter.zoom(&zoomX, &zoomY);

    painter.save();
    QTransform shapeMatrix = m_textShape->absoluteTransformation(&converter);
    shapeMatrix.scale(zoomX, zoomY);
    shapeMatrix.translate(0, -m_textShapeData->documentOffset());

    // Possibly draw table dragging visual cues
    const qreal boxHeight = 20;
    if (m_tableDragInfo.tableHit == KoPointedAt::ColumnDivider) {
        QPointF anchorPos = m_tableDragInfo.tableDividerPos - QPointF(m_dx, 0.0);
        if (m_tableDragInfo.tableColumnDivider > 0) {
            // let's draw left
            qreal w = m_tableDragInfo.tableLeadSize - m_dx;
            QRectF rect(anchorPos - QPointF(w, 0.0), QSizeF(w, 0.0));
            QRectF drawRect(shapeMatrix.map(rect.topLeft()), shapeMatrix.map(rect.bottomRight()));
            drawRect.setHeight(boxHeight);
            drawRect.moveTop(drawRect.top() - 1.5 * boxHeight);
            QString label = m_unit.toUserStringValue(w);
            int labelWidth = QFontMetrics(QToolTip::font()).boundingRect(label).width();
            painter.fillRect(drawRect, QColor(64, 255, 64, 196));
            painter.setPen(QPen(QColor(0, 0, 0, 196), 0));
            if (labelWidth + 10 < drawRect.width()) {
                QPointF centerLeft(drawRect.left(), drawRect.center().y());
                QPointF centerRight(drawRect.right(), drawRect.center().y());
                painter.drawLine(centerLeft, drawRect.center() - QPointF(labelWidth / 2 + 5, 0.0));
                painter.drawLine(centerLeft, centerLeft + QPointF(7, -5));
                painter.drawLine(centerLeft, centerLeft + QPointF(7, 5));
                painter.drawLine(drawRect.center() + QPointF(labelWidth / 2 + 5, 0.0), centerRight);
                painter.drawLine(centerRight, centerRight + QPointF(-7, -5));
                painter.drawLine(centerRight, centerRight + QPointF(-7, 5));
                painter.drawText(drawRect, Qt::AlignCenter, label);
            }
        }
        if (m_tableDragInfo.tableColumnDivider < m_tableDragInfo.table->columns()) {
            // let's draw right
            qreal w = m_tableDragInfo.tableTrailSize + m_dx;
            QRectF rect(anchorPos, QSizeF(w, 0.0));
            QRectF drawRect(shapeMatrix.map(rect.topLeft()), shapeMatrix.map(rect.bottomRight()));
            drawRect.setHeight(boxHeight);
            drawRect.moveTop(drawRect.top() - 1.5 * boxHeight);
            QString label;
            int labelWidth;
            if (m_tableDragWithShift) {
                label = i18n("follows along");
                labelWidth = QFontMetrics(QToolTip::font()).boundingRect(label).width();
                drawRect.setWidth(2 * labelWidth);
                QLinearGradient g(drawRect.topLeft(), drawRect.topRight());
                g.setColorAt(0.6, QColor(255, 64, 64, 196));
                g.setColorAt(1.0, QColor(255, 64, 64, 0));
                QBrush brush(g);
                painter.fillRect(drawRect, brush);
            } else {
                label = m_unit.toUserStringValue(w);
                labelWidth = QFontMetrics(QToolTip::font()).boundingRect(label).width();
                drawRect.setHeight(boxHeight);
                painter.fillRect(drawRect, QColor(64, 255, 64, 196));
            }
            painter.setPen(QPen(QColor(0, 0, 0, 196), 0));
            if (labelWidth + 10 < drawRect.width()) {
                QPointF centerLeft(drawRect.left(), drawRect.center().y());
                QPointF centerRight(drawRect.right(), drawRect.center().y());
                painter.drawLine(centerLeft, drawRect.center() - QPointF(labelWidth / 2 + 5, 0.0));
                painter.drawLine(centerLeft, centerLeft + QPointF(7, -5));
                painter.drawLine(centerLeft, centerLeft + QPointF(7, 5));
                if (!m_tableDragWithShift) {
                    painter.drawLine(drawRect.center() + QPointF(labelWidth / 2 + 5, 0.0), centerRight);
                    painter.drawLine(centerRight, centerRight + QPointF(-7, -5));
                    painter.drawLine(centerRight, centerRight + QPointF(-7, 5));
                }
                painter.drawText(drawRect, Qt::AlignCenter, label);
            }
            if (!m_tableDragWithShift) {
                // let's draw a helper text too
                label = i18n("Press shift to not resize this");
                labelWidth = QFontMetrics(QToolTip::font()).boundingRect(label).width();
                labelWidth += 10;
                // if (labelWidth < drawRect.width())
                {
                    drawRect.moveTop(drawRect.top() + boxHeight);
                    drawRect.moveLeft(drawRect.left() + (drawRect.width() - labelWidth) / 2);
                    drawRect.setWidth(labelWidth);
                    painter.fillRect(drawRect, QColor(64, 255, 64, 196));
                    painter.drawText(drawRect, Qt::AlignCenter, label);
                }
            }
        }
    }
    // Possibly draw table dragging visual cues
    if (m_tableDragInfo.tableHit == KoPointedAt::RowDivider) {
        QPointF anchorPos = m_tableDragInfo.tableDividerPos - QPointF(0.0, m_dy);
        if (m_tableDragInfo.tableRowDivider > 0) {
            qreal h = m_tableDragInfo.tableLeadSize - m_dy;
            QRectF rect(anchorPos - QPointF(0.0, h), QSizeF(0.0, h));
            QRectF drawRect(shapeMatrix.map(rect.topLeft()), shapeMatrix.map(rect.bottomRight()));
            drawRect.setWidth(boxHeight);
            drawRect.moveLeft(drawRect.left() - 1.5 * boxHeight);
            QString label = m_unit.toUserStringValue(h);
            QRectF labelRect = QFontMetrics(QToolTip::font()).boundingRect(label);
            labelRect.setHeight(boxHeight);
            labelRect.setWidth(labelRect.width() + 10);
            labelRect.moveTopLeft(drawRect.center() - QPointF(labelRect.width(), labelRect.height()) / 2);
            painter.fillRect(drawRect, QColor(64, 255, 64, 196));
            painter.fillRect(labelRect, QColor(64, 255, 64, 196));
            painter.setPen(QPen(QColor(0, 0, 0, 196), 0));
            if (labelRect.height() + 10 < drawRect.height()) {
                QPointF centerTop(drawRect.center().x(), drawRect.top());
                QPointF centerBottom(drawRect.center().x(), drawRect.bottom());
                painter.drawLine(centerTop, drawRect.center() - QPointF(0.0, labelRect.height() / 2 + 5));
                painter.drawLine(centerTop, centerTop + QPointF(-5, 7));
                painter.drawLine(centerTop, centerTop + QPointF(5, 7));
                painter.drawLine(drawRect.center() + QPointF(0.0, labelRect.height() / 2 + 5), centerBottom);
                painter.drawLine(centerBottom, centerBottom + QPointF(-5, -7));
                painter.drawLine(centerBottom, centerBottom + QPointF(5, -7));
            }
            painter.drawText(labelRect, Qt::AlignCenter, label);
        }
    }
    if (m_caretTimerState) {
        // Lets draw the caret ourselves, as the Qt method doesn't take cursor
        // charFormat into consideration.
        QTextBlock block = m_textEditor.data()->block();
        if (block.isValid()) {
            int posInParag = m_textEditor.data()->position() - block.position();
            if (posInParag <= block.layout()->preeditAreaPosition())
                posInParag += block.layout()->preeditAreaText().length();

            QTextLine tl = block.layout()->lineForTextPosition(m_textEditor.data()->position() - block.position());
            if (tl.isValid()) {
                painter.setRenderHint(QPainter::Antialiasing, false);
                QRectF rect = caretRect(m_textEditor.data()->cursor());
                QPointF baselinePoint;
                if (tl.ascent() > 0) {
                    QFontMetricsF fm(m_textEditor.data()->charFormat().font(), painter.device());
                    rect.setY(rect.y() + tl.ascent() - qMin(tl.ascent(), fm.ascent()));
                    rect.setHeight(qMin(tl.ascent(), fm.ascent()) + qMin(tl.descent(), fm.descent()));
                    baselinePoint = QPoint(rect.x(), rect.y() + tl.ascent());
                } else {
                    // line only filled with characters-without-size (eg anchors)
                    //  layout will make sure line has height of block font
                    QFontMetricsF fm(block.charFormat().font(), painter.device());
                    rect.setHeight(fm.ascent() + fm.descent());
                    baselinePoint = QPoint(rect.x(), rect.y() + fm.ascent());
                }
                QRectF drawRect(shapeMatrix.map(rect.topLeft()), shapeMatrix.map(rect.bottomLeft()));
                drawRect.setWidth(2);
                painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
                if (m_textEditor.data()->isEditProtected(true)) {
                    QRectF circleRect(shapeMatrix.map(baselinePoint), QSizeF(14, 14));
                    circleRect.translate(-6.5, -6.5);
                    QPen pen(QColor(16, 255, 255));
                    pen.setWidthF(2.0);
                    painter.setPen(pen);
                    painter.setRenderHint(QPainter::Antialiasing, true);
                    painter.drawEllipse(circleRect);
                    painter.drawLine(circleRect.topLeft() + QPointF(4.5, 4.5), circleRect.bottomRight() - QPointF(4.5, 4.5));
                } else {
                    painter.fillRect(drawRect, QColor(128, 255, 128));
                }
            }
        }
    }

    painter.restore();
}

void TextTool::updateSelectedShape(const QPointF &point, bool noDocumentChange)
{
    QRectF area(point, QSizeF(1, 1));
    if (m_textEditor.data()->hasSelection())
        repaintSelection();
    else
        repaintCaret();
    QList<KoShape *> sortedShapes = canvas()->shapeManager()->shapesAt(area, true);
    std::sort(sortedShapes.begin(), sortedShapes.end(), KoShape::compareShapeZIndex);
    for (int count = sortedShapes.count() - 1; count >= 0; count--) {
        KoShape *shape = sortedShapes.at(count);

        if (shape->isContentProtected())
            continue;
        TextShape *textShape = dynamic_cast<TextShape *>(shape);
        if (textShape) {
            if (textShape != m_textShape) {
                if (static_cast<KoTextShapeData *>(textShape->userData())->document() != m_textShapeData->document()) {
                    // we should only change to another document if allowed
                    if (noDocumentChange) {
                        return;
                    }

                    // if we change to another textdocument we need to remove selection in old document
                    // or it would continue to be painted etc

                    m_textEditor.data()->setPosition(m_textEditor.data()->position());
                }
                m_textShape = textShape;

                setShapeData(static_cast<KoTextShapeData *>(m_textShape->userData()));

                // This is how we inform the rulers of the active range
                // For now we will not consider table cells, but just give the shape dimensions
                QVariant v;
                QRectF rect(QPoint(), m_textShape->size());
                rect = m_textShape->absoluteTransformation(nullptr).mapRect(rect);
                v.setValue(rect);
                canvas()->resourceManager()->setResource(KoCanvasResourceManager::ActiveRange, v);
            }
            return;
        }
    }
}

void TextTool::mousePressEvent(KoPointerEvent *event)
{
    if (m_textEditor.isNull())
        return;

    // request the software keyboard, if any
    if (event->button() == Qt::LeftButton && qApp->autoSipEnabled()) {
        QStyle::RequestSoftwareInputPanel behavior = QStyle::RequestSoftwareInputPanel(qApp->style()->styleHint(QStyle::SH_RequestSoftwareInputPanel));
        // the two following bools just make it all a lot easier to read in the following if()
        // basically, we require a widget for this to work (passing nullptr to QApplication::sendEvent
        // crashes) and there are three tests any one of which can be true to trigger the event
        const bool hasWidget = canvas()->canvasWidget();
        const bool hasItem = canvas()->canvasItem();
        if ((behavior == QStyle::RSIP_OnMouseClick && (hasWidget || hasItem)) || (hasWidget && canvas()->canvasWidget()->hasFocus())
            || (hasItem && canvas()->canvasItem()->hasFocus())) {
            QEvent event(QEvent::RequestSoftwareInputPanel);
            if (hasWidget) {
                QApplication::sendEvent(canvas()->canvasWidget(), &event);
            } else {
                QApplication::sendEvent(canvas()->canvasItem(), &event);
            }
        }
    }

    bool shiftPressed = event->modifiers() & Qt::ShiftModifier;

    updateSelectedShape(event->point, shiftPressed);

    KoSelection *selection = canvas()->shapeManager()->selection();
    if (m_textShape && !selection->isSelected(m_textShape) && m_textShape->isSelectable()) {
        selection->deselectAll();
        selection->select(m_textShape);
    }

    KoPointedAt pointedAt = hitTest(event->point);
    m_tableDraggedOnce = false;
    m_clickWithinSelection = false;
    if (pointedAt.position != -1) {
        m_tablePenMode = false;

        if ((event->button() == Qt::LeftButton) && !shiftPressed && m_textEditor.data()->hasSelection()
            && m_textEditor.data()->isWithinSelection(pointedAt.position)) {
            m_clickWithinSelection = true;
            m_draggingOrigin = event->position(); // we store the pixel pos
        } else if (!(event->button() == Qt::RightButton && m_textEditor.data()->hasSelection() && m_textEditor.data()->isWithinSelection(pointedAt.position))) {
            m_textEditor.data()->setPosition(pointedAt.position, shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
            useCursor(Qt::IBeamCursor);
        }
        m_tableDragInfo.tableHit = KoPointedAt::None;
        if (m_caretTimer.isActive()) { // make the caret not blink, (blinks again after first draw)
            m_caretTimer.stop();
            m_caretTimer.setInterval(50);
            m_caretTimer.start();
            m_caretTimerState = true; // turn caret instantly on on click
        }
    } else {
        if (event->button() == Qt::RightButton) {
            m_tablePenMode = false;
            KoTextEditingPlugin *plugin = textEditingPluginContainer()->spellcheck();
            if (plugin)
                plugin->setCurrentCursorPosition(m_textShapeData->document(), -1);

            event->ignore();
        } else if (m_tablePenMode) {
            m_textEditor.data()->beginEditBlock(kundo2_i18n("Change Border Formatting"));
            if (pointedAt.tableHit == KoPointedAt::ColumnDivider) {
                if (pointedAt.tableColumnDivider < pointedAt.table->columns()) {
                    m_textEditor.data()->setTableBorderData(pointedAt.table,
                                                            pointedAt.tableRowDivider,
                                                            pointedAt.tableColumnDivider,
                                                            KoBorder::LeftBorder,
                                                            m_tablePenBorderData);
                }
                if (pointedAt.tableColumnDivider > 0) {
                    m_textEditor.data()->setTableBorderData(pointedAt.table,
                                                            pointedAt.tableRowDivider,
                                                            pointedAt.tableColumnDivider - 1,
                                                            KoBorder::RightBorder,
                                                            m_tablePenBorderData);
                }
            } else if (pointedAt.tableHit == KoPointedAt::RowDivider) {
                if (pointedAt.tableRowDivider < pointedAt.table->rows()) {
                    m_textEditor.data()->setTableBorderData(pointedAt.table,
                                                            pointedAt.tableRowDivider,
                                                            pointedAt.tableColumnDivider,
                                                            KoBorder::TopBorder,
                                                            m_tablePenBorderData);
                }
                if (pointedAt.tableRowDivider > 0) {
                    m_textEditor.data()->setTableBorderData(pointedAt.table,
                                                            pointedAt.tableRowDivider - 1,
                                                            pointedAt.tableColumnDivider,
                                                            KoBorder::BottomBorder,
                                                            m_tablePenBorderData);
                }
            }
            m_textEditor.data()->endEditBlock();
        } else {
            m_tableDragInfo = pointedAt;
            m_tablePenMode = false;
        }
        return;
    }
    if (shiftPressed) // altered selection.
        repaintSelection();
    else
        repaintCaret();

    updateSelectionHandler();
    updateStyleManager();

    updateActions();

    // activate context-menu for spelling-suggestions
    if (event->button() == Qt::RightButton) {
        KoTextEditingPlugin *plugin = textEditingPluginContainer()->spellcheck();
        if (plugin)
            plugin->setCurrentCursorPosition(m_textShapeData->document(), m_textEditor.data()->position());

        event->ignore();
    }

    if (event->button() == Qt::MiddleButton) { // Paste
        const QMimeData *data = QApplication::clipboard()->mimeData(QClipboard::Selection);

        // on windows we do not have data if we try to paste this selection
        if (data) {
            m_prevCursorPosition = m_textEditor.data()->position();
            m_textEditor.data()->paste(canvas(), data, canvas()->resourceManager());
            editingPluginEvents();
        }
    }
}

void TextTool::setShapeData(KoTextShapeData *data)
{
    bool docChanged = !data || !m_textShapeData || m_textShapeData->document() != data->document();
    if (m_textShapeData) {
        disconnect(m_textShapeData, &QObject::destroyed, this, &TextTool::shapeDataRemoved);
    }
    m_textShapeData = data;
    if (!m_textShapeData)
        return;
    connect(m_textShapeData, &QObject::destroyed, this, &TextTool::shapeDataRemoved);
    if (docChanged) {
        if (!m_textEditor.isNull()) {
            disconnect(m_textEditor.data(), &KoTextEditor::textFormatChanged, this, &TextTool::updateActions);
        }
        m_textEditor = KoTextDocument(m_textShapeData->document()).textEditor();
        Q_ASSERT(m_textEditor.data());
        if (!m_toolSelection) {
            m_toolSelection = new TextToolSelection(m_textEditor.data());
        } else {
            m_toolSelection->m_editor = m_textEditor.data();
        }

        m_variableMenu->menu()->clear();
        KoTextDocument document(m_textShapeData->document());
        foreach (QAction *action, document.inlineTextObjectManager()->createInsertVariableActions(canvas())) {
            m_variableMenu->addAction(action);
            connect(action, &QAction::triggered, this, &TextTool::returnFocusToCanvas);
        }

        connect(m_textEditor.data(), &KoTextEditor::textFormatChanged, this, &TextTool::updateActions);
        updateActions();
    }
}

void TextTool::updateSelectionHandler()
{
    if (m_textEditor) {
        Q_EMIT selectionChanged(m_textEditor.data()->hasSelection());
        if (m_textEditor.data()->hasSelection()) {
            QClipboard *clipboard = QApplication::clipboard();
            if (clipboard->supportsSelection())
                clipboard->setText(m_textEditor.data()->selectedText(), QClipboard::Selection);
        }
    }

    KoCanvasResourceManager *p = canvas()->resourceManager();
    m_allowResourceManagerUpdates = false;
    if (m_textEditor && m_textShapeData) {
        p->setResource(KoText::CurrentTextPosition, m_textEditor.data()->position());
        p->setResource(KoText::CurrentTextAnchor, m_textEditor.data()->anchor());
        QVariant variant;
        variant.setValue<void *>(m_textShapeData->document());
        p->setResource(KoText::CurrentTextDocument, variant);
    } else {
        p->clearResource(KoText::CurrentTextPosition);
        p->clearResource(KoText::CurrentTextAnchor);
        p->clearResource(KoText::CurrentTextDocument);
    }
    m_allowResourceManagerUpdates = true;
}

QMimeData *TextTool::generateMimeData() const
{
    if (!m_textShapeData || m_textEditor.isNull() || !m_textEditor.data()->hasSelection())
        return nullptr;
    int from = m_textEditor.data()->position();
    int to = m_textEditor.data()->anchor();
    KoTextOdfSaveHelper saveHelper(m_textShapeData->document(), from, to);
    KoTextDrag drag;

#ifdef SHOULD_BUILD_RDF
    KoDocumentResourceManager *rm = 0;
    if (canvas()->shapeController()) {
        rm = canvas()->shapeController()->resourceManager();
    }

    if (rm && rm->hasResource(KoText::DocumentRdf)) {
        KoDocumentRdfBase *rdf = qobject_cast<KoDocumentRdfBase *>(rm->resource(KoText::DocumentRdf).value<QObject *>());
        if (rdf) {
            saveHelper.setRdfModel(rdf->model());
        }
    }
#endif
    drag.setOdf(KoOdf::mimeType(KoOdf::Text), saveHelper);
    QTextDocumentFragment fragment = m_textEditor.data()->selection();
    drag.setData("text/plain", fragment.toPlainText().toUtf8());

    return drag.takeMimeData();
}

TextEditingPluginContainer *TextTool::textEditingPluginContainer()
{
    m_textEditingPlugins = canvas()->resourceManager()->resource(TextEditingPluginContainer::ResourceId).value<TextEditingPluginContainer *>();

    if (m_textEditingPlugins == nullptr) {
        m_textEditingPlugins = new TextEditingPluginContainer(canvas()->resourceManager());
        QVariant variant;
        variant.setValue(m_textEditingPlugins.data());
        canvas()->resourceManager()->setResource(TextEditingPluginContainer::ResourceId, variant);

        foreach (KoTextEditingPlugin *plugin, m_textEditingPlugins->values()) {
            connect(plugin, &KoTextEditingPlugin::startMacro, this, &TextTool::startMacro);
            connect(plugin, &KoTextEditingPlugin::stopMacro, this, &TextTool::stopMacro);
            const QHash<QString, QAction *> actions = plugin->actions();
            QHash<QString, QAction *>::ConstIterator i = actions.begin();
            while (i != actions.end()) {
                addAction(i.key(), i.value());
                ++i;
            }
        }
    }
    return m_textEditingPlugins;
}

void TextTool::copy() const
{
    QMimeData *mimeData = generateMimeData();
    if (mimeData) {
        QApplication::clipboard()->setMimeData(mimeData);
    }
}

void TextTool::deleteSelection()
{
    m_textEditor.data()->deleteChar();
    editingPluginEvents();
}

bool TextTool::paste()
{
    const QMimeData *data = QApplication::clipboard()->mimeData(QClipboard::Clipboard);

    // on windows we do not have data if we try to paste the selection
    if (!data)
        return false;

    // since this is not paste-as-text we will not paste in urls, but instead let KoToolProxy solve it
    if (data->hasUrls())
        return false;

    if (data->hasFormat(KoOdf::mimeType(KoOdf::Text)) || data->hasText()) {
        m_prevCursorPosition = m_textEditor.data()->position();
        m_textEditor.data()->paste(canvas(), data);
        editingPluginEvents();
        return true;
    }

    return false;
}

void TextTool::cut()
{
    if (m_textEditor.data()->hasSelection()) {
        copy();
        KUndo2Command *topCmd = m_textEditor.data()->beginEditBlock(kundo2_i18n("Cut"));
        m_textEditor.data()->deleteChar(false, topCmd);
        m_textEditor.data()->endEditBlock();
    }
}

QStringList TextTool::supportedPasteMimeTypes() const
{
    QStringList list;
    list << "text/plain"
         << "application/vnd.oasis.opendocument.text";
    return list;
}

void TextTool::dragMoveEvent(QDragMoveEvent *event, const QPointF &point)
{
    if (event->mimeData()->hasFormat(KoOdf::mimeType(KoOdf::Text)) || event->mimeData()->hasFormat(KoOdf::mimeType(KoOdf::OpenOfficeClipboard))
        || event->mimeData()->hasText()) {
        if (m_drag) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else if (event->proposedAction() == Qt::CopyAction) {
            event->acceptProposedAction();
        } else {
            event->ignore();
            return;
        }
        KoPointedAt pointedAt = hitTest(point);

        if (pointedAt.position == -1) {
            event->ignore();
        }
        if (m_caretTimer.isActive()) { // make the caret not blink, (blinks again after first draw)
            m_caretTimer.stop();
            m_caretTimer.setInterval(50);
            m_caretTimer.start();
            m_caretTimerState = true; // turn caret instantly on on click
        }

        if (m_preDragSelection.cursor.isNull()) {
            repaintSelection();

            m_preDragSelection.cursor = QTextCursor(*m_textEditor.data()->cursor());

            if (m_drag) {
                // Make a selection that looks like the current cursor selection
                // so we can move the real caret around freely
                QVector<QAbstractTextDocumentLayout::Selection> sels = KoTextDocument(m_textShapeData->document()).selections();

                m_preDragSelection.format = QTextCharFormat();
                m_preDragSelection.format.setBackground(qApp->palette().brush(QPalette::Highlight));
                m_preDragSelection.format.setForeground(qApp->palette().brush(QPalette::HighlightedText));
                sels.append(m_preDragSelection);
                KoTextDocument(m_textShapeData->document()).setSelections(sels);
            } // else we want the selection to disappear
        }
        repaintCaret(); // will erase caret
        m_textEditor.data()->setPosition(pointedAt.position);
        repaintCaret(); // will paint caret in new spot

        // Selection has visually not appeared at a new spot so no need to repaint it
    }
}

void TextTool::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (m_drag) {
        // restore the old selections
        QVector<QAbstractTextDocumentLayout::Selection> sels = KoTextDocument(m_textShapeData->document()).selections();
        sels.pop_back();
        KoTextDocument(m_textShapeData->document()).setSelections(sels);
    }

    repaintCaret(); // will erase caret in old spot
    m_textEditor.data()->setPosition(m_preDragSelection.cursor.anchor());
    m_textEditor.data()->setPosition(m_preDragSelection.cursor.position(), QTextCursor::KeepAnchor);
    repaintCaret(); // will paint caret in new spot

    if (!m_drag) {
        repaintSelection(); // will paint selection again
    }

    // mark that we now are back to normal selection
    m_preDragSelection.cursor = QTextCursor();
    event->accept();
}

void TextTool::dropEvent(QDropEvent *event, const QPointF &)
{
    if (m_drag) {
        // restore the old selections
        QVector<QAbstractTextDocumentLayout::Selection> sels = KoTextDocument(m_textShapeData->document()).selections();
        sels.pop_back();
        KoTextDocument(m_textShapeData->document()).setSelections(sels);
    }

    QTextCursor insertCursor(*m_textEditor.data()->cursor());

    m_textEditor.data()->setPosition(m_preDragSelection.cursor.anchor());
    m_textEditor.data()->setPosition(m_preDragSelection.cursor.position(), QTextCursor::KeepAnchor);
    repaintSelection(); // will erase the selection in new spot
    if (m_drag) {
        m_textEditor.data()->deleteChar();
    }
    m_prevCursorPosition = insertCursor.position();
    m_textEditor.data()->setPosition(m_prevCursorPosition);
    m_textEditor.data()->paste(canvas(), event->mimeData());
    m_textEditor.data()->setPosition(m_prevCursorPosition);
    // since the paste made insertCursor we can now use that for the end position
    m_textEditor.data()->setPosition(insertCursor.position(), QTextCursor::KeepAnchor);

    // mark that we no are back to normal selection
    m_preDragSelection.cursor = QTextCursor();
    event->accept();
}

KoPointedAt TextTool::hitTest(const QPointF &point) const
{
    if (!m_textShape || !m_textShapeData) {
        return KoPointedAt();
    }
    QPointF p = m_textShape->convertScreenPos(point);
    KoTextLayoutRootArea *rootArea = m_textShapeData->rootArea();
    return rootArea ? rootArea->hitTest(p, Qt::FuzzyHit) : KoPointedAt();
}

void TextTool::mouseDoubleClickEvent(KoPointerEvent *event)
{
    if (canvas()->shapeManager()->shapeAt(event->point) != m_textShape) {
        event->ignore(); // allow the event to be used by another
        return;
    }

    if (event->modifiers() & Qt::ShiftModifier) {
        // When whift is pressed we behave as a single press
        return mousePressEvent(event);
    }

    m_textEditor.data()->select(QTextCursor::WordUnderCursor);

    m_clickWithinSelection = false;

    repaintSelection();
    updateSelectionHandler();
}

void TextTool::mouseTripleClickEvent(KoPointerEvent *event)
{
    if (canvas()->shapeManager()->shapeAt(event->point) != m_textShape) {
        event->ignore(); // allow the event to be used by another
        return;
    }

    if (event->modifiers() & Qt::ShiftModifier) {
        // When whift is pressed we behave as a single press
        return mousePressEvent(event);
    }

    m_textEditor.data()->clearSelection();
    m_textEditor.data()->movePosition(QTextCursor::StartOfBlock);
    m_textEditor.data()->movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    m_clickWithinSelection = false;

    repaintSelection();
    updateSelectionHandler();
}

void TextTool::mouseMoveEvent(KoPointerEvent *event)
{
    m_editTipPos = event->globalPosition().toPoint();

    if (event->buttons()) {
        updateSelectedShape(event->point, true);
    }

    m_editTipTimer.stop();

    if (QToolTip::isVisible())
        QToolTip::hideText();

    KoPointedAt pointedAt = hitTest(event->point);

    if (event->buttons() == Qt::NoButton) {
        if (m_tablePenMode) {
            if (pointedAt.tableHit == KoPointedAt::ColumnDivider || pointedAt.tableHit == KoPointedAt::RowDivider) {
                useTableBorderCursor();
            } else {
                useCursor(Qt::IBeamCursor);
            }
            // do nothing else
            return;
        }

        if (!m_textShapeData || pointedAt.position < 0) {
            if (pointedAt.tableHit == KoPointedAt::ColumnDivider) {
                useCursor(Qt::SplitHCursor);
                m_draggingOrigin = event->point;
            } else if (pointedAt.tableHit == KoPointedAt::RowDivider) {
                if (pointedAt.tableRowDivider > 0) {
                    useCursor(Qt::SplitVCursor);
                    m_draggingOrigin = event->point;
                } else
                    useCursor(Qt::IBeamCursor);
            } else {
                useCursor(Qt::IBeamCursor);
            }
            return;
        }

        QTextCursor mouseOver(m_textShapeData->document());
        mouseOver.setPosition(pointedAt.position);

        if (m_changeTracker && m_changeTracker->containsInlineChanges(mouseOver.charFormat())) {
            m_editTipPointedAt = pointedAt;
            if (QToolTip::isVisible()) {
                QTimer::singleShot(0, this, &TextTool::showEditTip);
            } else {
                m_editTipTimer.start();
            }
        }

        if ((pointedAt.bookmark || !pointedAt.externalHRef.isEmpty()) || pointedAt.note || (pointedAt.noteReference > 0)) {
            if (event->modifiers() & Qt::ControlModifier) {
                useCursor(Qt::PointingHandCursor);
            }
            m_editTipPointedAt = pointedAt;
            if (QToolTip::isVisible()) {
                QTimer::singleShot(0, this, &TextTool::showEditTip);
            } else {
                m_editTipTimer.start();
            }
            return;
        }

        // check if mouse pointer is over shape with hyperlink
        KoShape *selectedShape = canvas()->shapeManager()->shapeAt(event->point);
        if (selectedShape != nullptr && selectedShape != m_textShape && selectedShape->hyperLink().size() != 0) {
            useCursor(Qt::PointingHandCursor);
            return;
        }

        useCursor(Qt::IBeamCursor);

        // Set Arrow Cursor when mouse is on top of annotation shape.
        if (selectedShape) {
            if (selectedShape->shapeId() == "AnnotationTextShapeID") {
                QPointF point(event->point);
                if (point.y() <= (selectedShape->position().y() + 25))
                    useCursor(Qt::ArrowCursor);
            }
        }

        return;
    } else {
        if (m_tableDragInfo.tableHit == KoPointedAt::ColumnDivider) {
            m_tableDragWithShift = event->modifiers() & Qt::ShiftModifier;
            if (m_tableDraggedOnce) {
                canvas()->shapeController()->resourceManager()->undoStack()->undo();
            }
            KUndo2Command *topCmd = m_textEditor.data()->beginEditBlock(kundo2_i18n("Adjust Column Width"));
            m_dx = m_draggingOrigin.x() - event->point.x();
            if (m_tableDragInfo.tableColumnDivider < m_tableDragInfo.table->columns() && m_tableDragInfo.tableTrailSize + m_dx < 0) {
                m_dx = -m_tableDragInfo.tableTrailSize;
            }
            if (m_tableDragInfo.tableColumnDivider > 0) {
                if (m_tableDragInfo.tableLeadSize - m_dx < 0) {
                    m_dx = m_tableDragInfo.tableLeadSize;
                }
                m_textEditor.data()->adjustTableColumnWidth(m_tableDragInfo.table,
                                                            m_tableDragInfo.tableColumnDivider - 1,
                                                            m_tableDragInfo.tableLeadSize - m_dx,
                                                            topCmd);
            } else {
                m_textEditor.data()->adjustTableWidth(m_tableDragInfo.table, -m_dx, 0.0);
            }
            if (m_tableDragInfo.tableColumnDivider < m_tableDragInfo.table->columns()) {
                if (!m_tableDragWithShift) {
                    m_textEditor.data()->adjustTableColumnWidth(m_tableDragInfo.table,
                                                                m_tableDragInfo.tableColumnDivider,
                                                                m_tableDragInfo.tableTrailSize + m_dx,
                                                                topCmd);
                }
            } else {
                m_tableDragWithShift = true; // act like shift pressed
            }
            if (m_tableDragWithShift) {
                m_textEditor.data()->adjustTableWidth(m_tableDragInfo.table, 0.0, m_dx);
            }
            m_textEditor.data()->endEditBlock();
            m_tableDragInfo.tableDividerPos.setY(m_textShape->convertScreenPos(event->point).y());
            if (m_tableDraggedOnce) {
                // we need to redraw like this so we update outside the textshape too
                if (canvas()->canvasWidget())
                    canvas()->canvasWidget()->update();
                if (canvas()->canvasItem())
                    canvas()->canvasItem()->update();
            }
            m_tableDraggedOnce = true;
        } else if (m_tableDragInfo.tableHit == KoPointedAt::RowDivider) {
            if (m_tableDraggedOnce) {
                canvas()->shapeController()->resourceManager()->undoStack()->undo();
            }
            if (m_tableDragInfo.tableRowDivider > 0) {
                KUndo2Command *topCmd = m_textEditor.data()->beginEditBlock(kundo2_i18n("Adjust Row Height"));
                m_dy = m_draggingOrigin.y() - event->point.y();

                if (m_tableDragInfo.tableLeadSize - m_dy < 0) {
                    m_dy = m_tableDragInfo.tableLeadSize;
                }

                m_textEditor.data()->adjustTableRowHeight(m_tableDragInfo.table,
                                                          m_tableDragInfo.tableRowDivider - 1,
                                                          m_tableDragInfo.tableLeadSize - m_dy,
                                                          topCmd);

                m_textEditor.data()->endEditBlock();

                m_tableDragInfo.tableDividerPos.setX(m_textShape->convertScreenPos(event->point).x());
                if (m_tableDraggedOnce) {
                    // we need to redraw like this so we update outside the textshape too
                    if (canvas()->canvasWidget())
                        canvas()->canvasWidget()->update();
                    if (canvas()->canvasItem())
                        canvas()->canvasItem()->update();
                }
                m_tableDraggedOnce = true;
            }

        } else if (m_tablePenMode) {
            // do nothing
        } else if (m_clickWithinSelection) {
            if (!m_drag && (event->position() - m_draggingOrigin).manhattanLength() >= QApplication::startDragDistance()) {
                QMimeData *mimeData = generateMimeData();
                if (mimeData) {
                    m_drag = new QDrag(canvas()->canvasWidget());
                    m_drag->setMimeData(mimeData);

                    m_drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction);

                    m_drag = nullptr;
                }
            }
        } else {
            useCursor(Qt::IBeamCursor);
            if (pointedAt.position == m_textEditor.data()->position())
                return;
            if (pointedAt.position >= 0) {
                if (m_textEditor.data()->hasSelection())
                    repaintSelection(); // will erase selection
                else
                    repaintCaret();

                m_textEditor.data()->setPosition(pointedAt.position, QTextCursor::KeepAnchor);

                if (m_textEditor.data()->hasSelection())
                    repaintSelection();
                else
                    repaintCaret();
            }
        }

        updateSelectionHandler();
    }
}

void TextTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->ignore();
    editingPluginEvents();

    m_tableDragInfo.tableHit = KoPointedAt::None;
    if (m_tableDraggedOnce) {
        m_tableDraggedOnce = false;
        // we need to redraw like this so we update outside the textshape too
        if (canvas()->canvasWidget())
            canvas()->canvasWidget()->update();
        if (canvas()->canvasItem())
            canvas()->canvasItem()->update();
    }

    if (!m_textShapeData)
        return;

    // check if mouse pointer is not over some shape with hyperlink
    KoShape *selectedShape = canvas()->shapeManager()->shapeAt(event->point);
    if (selectedShape != nullptr && selectedShape != m_textShape && selectedShape->hyperLink().size() != 0) {
        QString url = selectedShape->hyperLink();
        runUrl(event, url);
        return;
    }

    KoPointedAt pointedAt = hitTest(event->point);

    if (m_clickWithinSelection && !m_drag) {
        if (m_caretTimer.isActive()) { // make the caret not blink, (blinks again after first draw)
            m_caretTimer.stop();
            m_caretTimer.setInterval(50);
            m_caretTimer.start();
            m_caretTimerState = true; // turn caret instantly on on click
        }
        repaintCaret(); // will erase caret
        repaintSelection(); // will erase selection
        m_textEditor.data()->setPosition(pointedAt.position);
        repaintCaret(); // will paint caret in new spot
    }

    // Is there an anchor here ?
    if ((event->modifiers() & Qt::ControlModifier) && !m_textEditor.data()->hasSelection()) {
        if (pointedAt.bookmark) {
            m_textEditor.data()->setPosition(pointedAt.bookmark->rangeStart());
            ensureCursorVisible();
            event->accept();
            return;
        }
        if (pointedAt.note) {
            m_textEditor.data()->setPosition(pointedAt.note->textFrame()->firstPosition());
            ensureCursorVisible();
            event->accept();
            return;
        }
        if (pointedAt.noteReference > 0) {
            m_textEditor.data()->setPosition(pointedAt.noteReference);
            ensureCursorVisible();
            event->accept();
            return;
        }
        if (!pointedAt.externalHRef.isEmpty()) {
            runUrl(event, pointedAt.externalHRef);
        }
    }
}

void TextTool::shortcutOverrideEvent(QKeyEvent *event)
{
    QKeySequence item(event->key() | ((Qt::ControlModifier | Qt::AltModifier) & event->modifiers()));
    if (hit(item, KStandardShortcut::Begin) || hit(item, KStandardShortcut::End)) {
        event->accept();
    }
}

void TextTool::keyPressEvent(QKeyEvent *event)
{
    int destinationPosition = -1; // for those cases where the moveOperation is not relevant;
    QTextCursor::MoveOperation moveOperation = QTextCursor::NoMove;
    KoTextEditor *textEditor = m_textEditor.data();
    m_tablePenMode = false; // keypress always stops the table (border) pen mode
    Q_ASSERT(textEditor);
    if (event->key() == Qt::Key_Backspace) {
        if (!textEditor->hasSelection() && textEditor->block().textList() && (textEditor->position() == textEditor->block().position())
            && !(m_changeTracker && m_changeTracker->recordChanges())) {
            if (!textEditor->blockFormat().boolProperty(KoParagraphStyle::UnnumberedListItem)) {
                // backspace at beginning of numbered list item, makes it unnumbered
                textEditor->toggleListNumbering(false);
            } else {
                KoListLevelProperties llp;
                llp.setLabelType(KoListStyle::None);
                llp.setLevel(0);
                // backspace on numbered, empty parag, removes numbering.
                textEditor->setListProperties(llp);
            }
        } else if (textEditor->position() > 0 || textEditor->hasSelection()) {
            if (!textEditor->hasSelection() && event->modifiers() & Qt::ControlModifier) { // delete prev word.
                textEditor->movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
            }
            textEditor->deletePreviousChar();

            editingPluginEvents();
        }
    } else if ((event->key() == Qt::Key_Tab)
               && ((!textEditor->hasSelection() && (textEditor->position() == textEditor->block().position()))
                   || (textEditor->block().document()->findBlock(textEditor->anchor()) != textEditor->block().document()->findBlock(textEditor->position())))
               && textEditor->block().textList()) {
        ChangeListLevelCommand::CommandType type = ChangeListLevelCommand::IncreaseLevel;
        ChangeListLevelCommand *cll = new ChangeListLevelCommand(*textEditor->cursor(), type, 1);
        textEditor->addCommand(cll);
        editingPluginEvents();
    } else if ((event->key() == Qt::Key_Backtab)
               && ((!textEditor->hasSelection() && (textEditor->position() == textEditor->block().position()))
                   || (textEditor->block().document()->findBlock(textEditor->anchor()) != textEditor->block().document()->findBlock(textEditor->position())))
               && textEditor->block().textList() && !(m_changeTracker && m_changeTracker->recordChanges())) {
        ChangeListLevelCommand::CommandType type = ChangeListLevelCommand::DecreaseLevel;
        ChangeListLevelCommand *cll = new ChangeListLevelCommand(*textEditor->cursor(), type, 1);
        textEditor->addCommand(cll);
        editingPluginEvents();
    } else if (event->key() == Qt::Key_Delete) {
        if (!textEditor->hasSelection() && event->modifiers() & Qt::ControlModifier) { // delete next word.
            textEditor->movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
        }
        // the event only gets through when the Del is not used in the app
        // if the app forwards Del then deleteSelection is used
        textEditor->deleteChar();
        editingPluginEvents();
    } else if ((event->key() == Qt::Key_Left) && (event->modifiers() & Qt::ControlModifier) == 0) {
        moveOperation = QTextCursor::Left;
    } else if ((event->key() == Qt::Key_Right) && (event->modifiers() & Qt::ControlModifier) == 0) {
        moveOperation = QTextCursor::Right;
    } else if ((event->key() == Qt::Key_Up) && (event->modifiers() & Qt::ControlModifier) == 0) {
        moveOperation = QTextCursor::Up;
    } else if ((event->key() == Qt::Key_Down) && (event->modifiers() & Qt::ControlModifier) == 0) {
        moveOperation = QTextCursor::Down;
    } else {
        // check for shortcuts.
        QKeySequence item(event->key() | ((Qt::ControlModifier | Qt::AltModifier) & event->modifiers()));
        if (hit(item, KStandardShortcut::Begin)) {
            // Goto beginning of the document. Default: Ctrl-Home
            destinationPosition = 0;
        } else if (hit(item, KStandardShortcut::End)) {
            // Goto end of the document. Default: Ctrl-End
            if (m_textShapeData) {
                QTextBlock last = m_textShapeData->document()->lastBlock();
                destinationPosition = last.position() + last.length() - 1;
            }
        } else if (hit(item, KStandardShortcut::Prior)) { // page up
            // Scroll up one page. Default: Prior
            event->ignore(); // let app level actions handle it
            return;
        } else if (hit(item, KStandardShortcut::Next)) {
            // Scroll down one page. Default: Next
            event->ignore(); // let app level actions handle it
            return;
        } else if (hit(item, KStandardShortcut::BeginningOfLine))
            // Goto beginning of current line. Default: Home
            moveOperation = QTextCursor::StartOfLine;
        else if (hit(item, KStandardShortcut::EndOfLine))
            // Goto end of current line. Default: End
            moveOperation = QTextCursor::EndOfLine;
        else if (hit(item, KStandardShortcut::BackwardWord))
            moveOperation = QTextCursor::WordLeft;
        else if (hit(item, KStandardShortcut::ForwardWord))
            moveOperation = QTextCursor::WordRight;
#ifdef Q_WS_MAC
        // Don't reject "alt" key, it may be used for typing text on Mac OS
        else if ((event->modifiers() & Qt::ControlModifier)
#else
        else if ((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
#endif
                 || event->text().length() == 0 || event->key() == Qt::Key_Escape) {
            event->ignore();
            return;
        } else if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
            // enter on numbered, empty parag, removes numbering.
            auto currentBlock = textEditor->constCursor().block();
            if (currentBlock.text().isEmpty() && currentBlock.textList()) {
                KoListLevelProperties llp;
                llp.setLabelType(KoListStyle::None);
                llp.setLevel(0);
                textEditor->setListProperties(llp);
            } else {
                m_prevCursorPosition = textEditor->position();
                textEditor->newLine();
                updateActions();
            }
            editingPluginEvents();
        } else if ((event->key() == Qt::Key_Tab || !(event->text().length() == 1 && !event->text().at(0).isPrint()))) { // insert the text
            m_prevCursorPosition = textEditor->position();
            startingSimpleEdit(); // signal editing plugins that this is a simple edit
            textEditor->insertText(event->text());
            editingPluginEvents();
        }
    }
    if (moveOperation != QTextCursor::NoMove || destinationPosition != -1) {
        useCursor(Qt::BlankCursor);
        bool shiftPressed = event->modifiers() & Qt::ShiftModifier;
        if (textEditor->hasSelection())
            repaintSelection(); // will erase selection
        else
            repaintCaret();
        QTextBlockFormat format = textEditor->blockFormat();

        KoText::Direction dir = static_cast<KoText::Direction>(format.intProperty(KoParagraphStyle::TextProgressionDirection));
        bool isRtl;
        if (dir == KoText::AutoDirection)
            isRtl = textEditor->block().text().isRightToLeft();
        else
            isRtl = dir == KoText::RightLeftTopBottom;

        if (isRtl) { // if RTL toggle direction of cursor movement.
            switch (moveOperation) {
            case QTextCursor::Left:
                moveOperation = QTextCursor::Right;
                break;
            case QTextCursor::Right:
                moveOperation = QTextCursor::Left;
                break;
            case QTextCursor::WordRight:
                moveOperation = QTextCursor::WordLeft;
                break;
            case QTextCursor::WordLeft:
                moveOperation = QTextCursor::WordRight;
                break;
            default:
                break;
            }
        }
        int prevPosition = textEditor->position();
        if (moveOperation != QTextCursor::NoMove)
            textEditor->movePosition(moveOperation, shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        else
            textEditor->setPosition(destinationPosition, shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        if (moveOperation == QTextCursor::Down && prevPosition == textEditor->position()) {
            // change behavior a little big from Qt; at the bottom of the doc we go to the end of the doc
            textEditor->movePosition(QTextCursor::End, shiftPressed ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        }
        if (shiftPressed) // altered selection.
            repaintSelection();
        else
            repaintCaret();
        updateActions();
        editingPluginEvents();
    }
    if (m_caretTimer.isActive()) { // make the caret not blink but decide on the action if its visible or not.
        m_caretTimer.stop();
        m_caretTimer.setInterval(50);
        m_caretTimer.start();
        m_caretTimerState = true; // turn caret on while typing
    }
    if (moveOperation != QTextCursor::NoMove)
        // this difference in handling is need to prevent leaving a trail of old cursors onscreen
        ensureCursorVisible();
    else
        m_delayedEnsureVisible = true;
    updateActions();
    updateSelectionHandler();
}

QVariant TextTool::inputMethodQuery(Qt::InputMethodQuery query, const KoViewConverter &converter) const
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor || !m_textShapeData)
        return QVariant();

    switch (query) {
    case Qt::ImCursorRectangle: {
        // The rectangle covering the area of the input cursor in widget coordinates.
        QRectF rect = caretRect(textEditor->cursor());
        rect.moveTop(rect.top() - m_textShapeData->documentOffset());
        QTransform shapeMatrix = m_textShape->absoluteTransformation(&converter);
        qreal zoomX, zoomY;
        converter.zoom(&zoomX, &zoomY);
        shapeMatrix.scale(zoomX, zoomY);
        rect = shapeMatrix.mapRect(rect);
        return rect.toRect();
    }
    case Qt::ImFont:
        // The currently used font for text input.
        return textEditor->charFormat().font();
    case Qt::ImCursorPosition:
        // The logical position of the cursor within the text surrounding the input area (see ImSurroundingText).
        return textEditor->position() - textEditor->block().position();
    case Qt::ImSurroundingText:
        // The plain text around the input area, for example the current paragraph.
        return textEditor->block().text();
    case Qt::ImCurrentSelection:
        // The currently selected text.
        return textEditor->selectedText();
    default:; // Qt 4.6 adds ImMaximumTextLength and ImAnchorPosition
    }
    return QVariant();
}

void TextTool::inputMethodEvent(QInputMethodEvent *event)
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (textEditor == nullptr)
        return;
    if (event->replacementLength() > 0) {
        textEditor->setPosition(textEditor->position() + event->replacementStart());
        for (int i = event->replacementLength(); i > 0; --i) {
            textEditor->deleteChar();
        }
    }
    if (!event->commitString().isEmpty()) {
        QKeyEvent ke(QEvent::KeyPress, -1, Qt::KeyboardModifiers(), event->commitString());
        keyPressEvent(&ke);
        // The cursor may reside in a different block before vs. after keyPressEvent.
        QTextBlock block = textEditor->block();
        QTextLayout *layout = block.layout();
        Q_ASSERT(layout);
        layout->setPreeditArea(-1, QString());
    } else {
        QTextBlock block = textEditor->block();
        QTextLayout *layout = block.layout();
        Q_ASSERT(layout);
        layout->setPreeditArea(textEditor->position() - block.position(), event->preeditString());
        const_cast<QTextDocument *>(textEditor->document())->markContentsDirty(textEditor->position(), event->preeditString().length());
    }
    event->accept();
}

void TextTool::ensureCursorVisible(bool moveView)
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor || !m_textShapeData)
        return;

    bool upToDate;
    QRectF cRect = caretRect(textEditor->cursor(), &upToDate);

    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(m_textShapeData->document()->documentLayout());
    Q_ASSERT(lay);
    KoTextLayoutRootArea *rootArea = lay->rootAreaForPoint(cRect.center());
    if (rootArea && rootArea->associatedShape() && m_textShapeData->rootArea() != rootArea) {
        // If we have changed root area we need to update m_textShape and m_textShapeData
        m_textShape = static_cast<TextShape *>(rootArea->associatedShape());
        Q_ASSERT(m_textShape);
        disconnect(m_textShapeData, &QObject::destroyed, this, &TextTool::shapeDataRemoved);
        m_textShapeData = static_cast<KoTextShapeData *>(m_textShape->userData());
        Q_ASSERT(m_textShapeData);
        connect(m_textShapeData, &QObject::destroyed, this, &TextTool::shapeDataRemoved);
    }

    if (!moveView) {
        return;
    }

    if (!upToDate) { // paragraph is not yet layouted.
        // The number one usecase for this is when the user pressed enter.
        // try to do it on next caret blink
        m_delayedEnsureVisible = true;
        return; // we shouldn't move to an obsolete position
    }
    cRect.moveTop(cRect.top() - m_textShapeData->documentOffset());
    canvas()->ensureVisible(m_textShape->absoluteTransformation(nullptr).mapRect(cRect));
}

void TextTool::keyReleaseEvent(QKeyEvent *event)
{
    event->accept();
}

void TextTool::updateActions()
{
    bool notInAnnotation = !dynamic_cast<AnnotationTextShape *>(m_textShape);
    KoTextEditor *textEditor = m_textEditor.data();
    if (textEditor == nullptr) {
        return;
    }
    m_allowActions = false;

    // Update the characterStyle related GUI elements
    QTextCharFormat cf = textEditor->charFormat();
    m_actionFormatBold->setChecked(cf.fontWeight() > QFont::Normal);
    m_actionFormatItalic->setChecked(cf.fontItalic());
    m_actionFormatUnderline->setChecked(cf.intProperty(KoCharacterStyle::UnderlineType) != KoCharacterStyle::NoLineType);
    m_actionFormatStrikeOut->setChecked(cf.intProperty(KoCharacterStyle::StrikeOutType) != KoCharacterStyle::NoLineType);
    bool super = false, sub = false;
    switch (cf.verticalAlignment()) {
    case QTextCharFormat::AlignSuperScript:
        super = true;
        break;
    case QTextCharFormat::AlignSubScript:
        sub = true;
        break;
    default:;
    }
    m_actionFormatSuper->setChecked(super);
    m_actionFormatSub->setChecked(sub);
    m_actionFormatFontSize->setFontSize(cf.font().pointSizeF());
    m_actionFormatFontFamily->setFont(cf.font().family());

    KoTextShapeData::ResizeMethod resizemethod = KoTextShapeData::AutoResize;
    if (m_textShapeData) {
        resizemethod = m_textShapeData->resizeMethod();
    }
    m_shrinkToFitAction->setEnabled(resizemethod != KoTextShapeData::AutoResize && notInAnnotation);
    m_shrinkToFitAction->setChecked(resizemethod == KoTextShapeData::ShrinkToFitResize);

    m_growWidthAction->setEnabled(resizemethod != KoTextShapeData::AutoResize && notInAnnotation);
    m_growWidthAction->setChecked(resizemethod == KoTextShapeData::AutoGrowWidth || resizemethod == KoTextShapeData::AutoGrowWidthAndHeight);

    m_growHeightAction->setEnabled(resizemethod != KoTextShapeData::AutoResize && notInAnnotation);
    m_growHeightAction->setChecked(resizemethod == KoTextShapeData::AutoGrowHeight || resizemethod == KoTextShapeData::AutoGrowWidthAndHeight);

    // update paragraphStyle GUI element
    QTextBlockFormat bf = textEditor->blockFormat();

    if (bf.hasProperty(KoParagraphStyle::TextProgressionDirection)) {
        switch (bf.intProperty(KoParagraphStyle::TextProgressionDirection)) {
        case KoText::RightLeftTopBottom:
            m_actionChangeDirection->setChecked(true);
            break;
        case KoText::LeftRightTopBottom:
        default:
            m_actionChangeDirection->setChecked(false);
            break;
        }
    } else {
        m_actionChangeDirection->setChecked(textEditor->block().text().isRightToLeft());
    }
    if (bf.alignment() == Qt::AlignLeading || bf.alignment() == Qt::AlignTrailing) {
        bool revert = (textEditor->block().layout()->textOption().textDirection() == Qt::RightToLeft);
        if ((bf.alignment() == Qt::AlignLeading) ^ revert)
            m_actionAlignLeft->setChecked(true);
        else
            m_actionAlignRight->setChecked(true);
    } else if (bf.alignment() == Qt::AlignHCenter)
        m_actionAlignCenter->setChecked(true);
    if (bf.alignment() == Qt::AlignJustify)
        m_actionAlignBlock->setChecked(true);
    else if (bf.alignment() == (Qt::AlignLeft | Qt::AlignAbsolute))
        m_actionAlignLeft->setChecked(true);
    else if (bf.alignment() == (Qt::AlignRight | Qt::AlignAbsolute))
        m_actionAlignRight->setChecked(true);

    if (textEditor->block().textList()) {
        QTextListFormat listFormat = textEditor->block().textList()->format();
        if (listFormat.intProperty(KoListStyle::Level) > 1) {
            m_actionFormatDecreaseIndent->setEnabled(true);
        } else {
            m_actionFormatDecreaseIndent->setEnabled(false);
        }

        if (listFormat.intProperty(KoListStyle::Level) < 10) {
            m_actionFormatIncreaseIndent->setEnabled(true);
        } else {
            m_actionFormatIncreaseIndent->setEnabled(false);
        }
    } else {
        m_actionFormatDecreaseIndent->setEnabled(textEditor->blockFormat().leftMargin() > 0.);
    }

    m_allowActions = true;

    bool useAdvancedText =
        !(canvas()->resourceManager()->intResource(KoCanvasResourceManager::ApplicationSpeciality) & KoCanvasResourceManager::NoAdvancedText);
    if (useAdvancedText) {
        action("insert_table")->setEnabled(notInAnnotation);

        bool hasTable = textEditor->currentTable();
        action("insert_tablerow_above")->setEnabled(hasTable && notInAnnotation);
        action("insert_tablerow_below")->setEnabled(hasTable && notInAnnotation);
        action("insert_tablecolumn_left")->setEnabled(hasTable && notInAnnotation);
        action("insert_tablecolumn_right")->setEnabled(hasTable && notInAnnotation);
        action("delete_tablerow")->setEnabled(hasTable && notInAnnotation);
        action("delete_tablecolumn")->setEnabled(hasTable && notInAnnotation);
        action("merge_tablecells")->setEnabled(hasTable && notInAnnotation);
        action("split_tablecells")->setEnabled(hasTable && notInAnnotation);
        action("activate_borderpainter")->setEnabled(hasTable && notInAnnotation);
    }
    action("insert_annotation")->setEnabled(notInAnnotation);

    /// TODO if selection contains several different format
    Q_EMIT blockChanged(textEditor->block());
    Q_EMIT charFormatChanged(cf, textEditor->blockCharFormat());
    Q_EMIT blockFormatChanged(bf);
}

void TextTool::updateStyleManager()
{
    if (!m_textShapeData)
        return;
    KoStyleManager *styleManager = KoTextDocument(m_textShapeData->document()).styleManager();
    Q_EMIT styleManagerChanged(styleManager);

    // TODO move this to its own method
    m_changeTracker = KoTextDocument(m_textShapeData->document()).changeTracker();
}

void TextTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);
    m_caretTimer.start();
    m_caretTimerState = true;
    foreach (KoShape *shape, shapes) {
        TextShape *textShape = dynamic_cast<TextShape *>(shape);
        if (textShape) {
            if (!m_textEditor) {
                m_textShape = textShape;
                break;
            }
            // Since there is a text editor, we must select the shape that is edited,
            // or else we get out of sync
            KoTextShapeData *data = static_cast<KoTextShapeData *>(textShape->userData());
            if (data && data->document() == m_textEditor->constCursor().document()) {
                m_textShape = textShape;
                break;
            }
            if (!m_textShape) {
                m_textShape = textShape;
            }
        }
    }
    if (!m_textShape) { // none found
        Q_EMIT done();
        // This is how we inform the rulers of the active range
        // No shape means no active range
        canvas()->resourceManager()->setResource(KoCanvasResourceManager::ActiveRange, QVariant(QRectF()));
        return;
    }

    // This is how we inform the rulers of the active range
    // For now we will not consider table cells, but just give the shape dimensions
    QVariant v;
    QRectF rect(QPoint(), m_textShape->size());
    rect = m_textShape->absoluteTransformation(nullptr).mapRect(rect);
    v.setValue(rect);
    canvas()->resourceManager()->setResource(KoCanvasResourceManager::ActiveRange, v);
    if ((!m_oldTextEditor.isNull()) && m_oldTextEditor.data()->document() != static_cast<KoTextShapeData *>(m_textShape->userData())->document()) {
        m_oldTextEditor.data()->setPosition(m_oldTextEditor.data()->position());
        // we need to redraw like this so we update the old textshape wherever it may be
        if (canvas()->canvasWidget())
            canvas()->canvasWidget()->update();
    }
    setShapeData(static_cast<KoTextShapeData *>(m_textShape->userData()));
    useCursor(Qt::IBeamCursor);

    updateStyleManager();
    repaintSelection();
    updateSelectionHandler();
    updateActions();
    if (m_specialCharacterDocker)
        m_specialCharacterDocker->setEnabled(true);
}

void TextTool::deactivate()
{
    m_caretTimer.stop();
    m_caretTimerState = false;
    repaintCaret();
    m_textShape = nullptr;

    // This is how we inform the rulers of the active range
    // No shape means no active range
    canvas()->resourceManager()->setResource(KoCanvasResourceManager::ActiveRange, QVariant(QRectF()));

    m_oldTextEditor = m_textEditor;
    setShapeData(nullptr);

    updateSelectionHandler();
    if (m_specialCharacterDocker) {
        m_specialCharacterDocker->setEnabled(false);
        m_specialCharacterDocker->setVisible(false);
    }
}

void TextTool::repaintDecorations()
{
    if (m_textShapeData)
        repaintSelection();
}

void TextTool::repaintCaret()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor || !m_textShapeData)
        return;

    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(m_textShapeData->document()->documentLayout());
    Q_ASSERT(lay);
    Q_UNUSED(lay);

    // If we have changed root area we need to update m_textShape and m_textShapeData
    if (m_delayedEnsureVisible) {
        m_delayedEnsureVisible = false;
        ensureCursorVisible();
        return;
    }

    ensureCursorVisible(false); // ensures the various vars are updated

    bool upToDate;
    QRectF repaintRect = caretRect(textEditor->cursor(), &upToDate);
    repaintRect.moveTop(repaintRect.top() - m_textShapeData->documentOffset());
    if (repaintRect.isValid()) {
        repaintRect = m_textShape->absoluteTransformation(nullptr).mapRect(repaintRect);

        // Make sure there is enough space to show an icon
        QRectF iconSize = canvas()->viewConverter()->viewToDocument(QRect(0, 0, 18, 18));
        repaintRect.setX(repaintRect.x() - iconSize.width() / 2);
        repaintRect.setRight(repaintRect.right() + iconSize.width() / 2);
        repaintRect.setTop(repaintRect.y() - iconSize.height() / 2);
        repaintRect.setBottom(repaintRect.bottom() + iconSize.height() / 2);
        canvas()->updateCanvas(repaintRect);
    }
}

void TextTool::repaintSelection()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (textEditor == nullptr)
        return;
    QTextCursor cursor = *textEditor->cursor();

    QList<TextShape *> shapes;
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(textEditor->document()->documentLayout());
    Q_ASSERT(lay);
    foreach (KoShape *shape, lay->shapes()) {
        TextShape *textShape = dynamic_cast<TextShape *>(shape);
        if (textShape == nullptr) // when the shape is being deleted its no longer a TextShape but a KoShape
            continue;

        // Q_ASSERT(!shapes.contains(textShape));
        if (!shapes.contains(textShape)) {
            shapes.append(textShape);
        }
    }

    // loop over all shapes that contain the text and update per shape.
    QRectF repaintRect = textRect(cursor);
    foreach (TextShape *ts, shapes) {
        QRectF rect = repaintRect;
        rect.moveTop(rect.y() - ts->textShapeData()->documentOffset());
        rect = ts->absoluteTransformation(nullptr).mapRect(rect);
        QRectF r = ts->boundingRect().intersected(rect);
        canvas()->updateCanvas(r);
    }
}

QRectF TextTool::caretRect(QTextCursor *cursor, bool *upToDate) const
{
    QTextCursor tmpCursor(*cursor);
    tmpCursor.setPosition(cursor->position()); // looses the anchor

    QRectF rect = textRect(tmpCursor);
    if (rect.size() == QSizeF(0, 0)) {
        if (upToDate) {
            *upToDate = false;
        }
        rect = m_lastImMicroFocus; // prevent block changed but layout not done
    } else {
        if (upToDate) {
            *upToDate = true;
        }
        m_lastImMicroFocus = rect;
    }
    return rect;
}

QRectF TextTool::textRect(QTextCursor &cursor) const
{
    if (!m_textShapeData)
        return QRectF();
    KoTextEditor *textEditor = m_textEditor.data();
    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(textEditor->document()->documentLayout());
    return lay->selectionBoundingBox(cursor);
}

KoToolSelection *TextTool::selection()
{
    return m_toolSelection;
}

QList<QPointer<QWidget>> TextTool::createOptionWidgets()
{
    QList<QPointer<QWidget>> widgets;
    SimpleCharacterWidget *scw = new SimpleCharacterWidget(this, nullptr);
    SimpleParagraphWidget *spw = new SimpleParagraphWidget(this, nullptr);
    if (m_textEditor.data()) {
        //        connect(m_textEditor.data(), SIGNAL(paragraphStyleApplied(KoParagraphStyle*)), spw, SLOT(slotParagraphStyleApplied(KoParagraphStyle*)));
        //        connect(m_textEditor.data(), SIGNAL(characterStyleApplied(KoCharacterStyle*)), scw, SLOT(slotCharacterStyleApplied(KoCharacterStyle*)));
        // initialise the char- and par- widgets with the current block and formats.
        scw->setCurrentBlockFormat(m_textEditor.data()->blockFormat());
        scw->setCurrentFormat(m_textEditor.data()->charFormat(), m_textEditor.data()->blockCharFormat());
        spw->setCurrentBlock(m_textEditor.data()->block());
        spw->setCurrentFormat(m_textEditor.data()->blockFormat());
    }
    SimpleTableWidget *stw = new SimpleTableWidget(this, nullptr);
    SimpleInsertWidget *siw = new SimpleInsertWidget(this, nullptr);

    /* We do not use these for now. Let's see if they become useful at a certain point in time. If not, we can remove the whole chain (SimpleCharWidget,
       SimpleParWidget, DockerStyleComboModel) if (m_textShapeData && KoTextDocument(m_textShapeData->document()).styleManager()) {
            scw->setInitialUsedStyles(KoTextDocument(m_textShapeData->document()).styleManager()->usedCharacterStyles());
            spw->setInitialUsedStyles(KoTextDocument(m_textShapeData->document()).styleManager()->usedParagraphStyles());
        }
    */
    // Connect to/with simple character widget (docker)
    connect(this, &TextTool::styleManagerChanged, scw, &SimpleCharacterWidget::setStyleManager);
    connect(this, &TextTool::charFormatChanged, scw, &SimpleCharacterWidget::setCurrentFormat);
    connect(this, &TextTool::blockFormatChanged, scw, &SimpleCharacterWidget::setCurrentBlockFormat);
    connect(scw, &SimpleCharacterWidget::doneWithFocus, this, &TextTool::returnFocusToCanvas);
    connect(scw, &SimpleCharacterWidget::characterStyleSelected, this, QOverload<KoCharacterStyle *>::of(&TextTool::setStyle));
    connect(scw, &SimpleCharacterWidget::newStyleRequested, this, &TextTool::createStyleFromCurrentCharFormat);
    connect(scw, &SimpleCharacterWidget::showStyleManager, this, &TextTool::showStyleManager);

    // Connect to/with simple paragraph widget (docker)
    connect(this, &TextTool::styleManagerChanged, spw, &SimpleParagraphWidget::setStyleManager);
    connect(this, &TextTool::blockChanged, spw, &SimpleParagraphWidget::setCurrentBlock);
    connect(this, &TextTool::blockFormatChanged, spw, &SimpleParagraphWidget::setCurrentFormat);
    connect(spw, &SimpleParagraphWidget::doneWithFocus, this, &TextTool::returnFocusToCanvas);
    connect(spw, &SimpleParagraphWidget::paragraphStyleSelected, this, QOverload<KoParagraphStyle *>::of(&TextTool::setStyle));
    connect(spw, &SimpleParagraphWidget::newStyleRequested, this, &TextTool::createStyleFromCurrentBlockFormat);
    connect(spw, &SimpleParagraphWidget::showStyleManager, this, &TextTool::showStyleManager);

    // Connect to/with simple table widget (docker)
    connect(this, &TextTool::styleManagerChanged, stw, &SimpleTableWidget::setStyleManager);
    connect(stw, &SimpleTableWidget::doneWithFocus, this, &TextTool::returnFocusToCanvas);
    connect(stw, &SimpleTableWidget::tableBorderDataUpdated, this, &TextTool::setTableBorderData);

    // Connect to/with simple insert widget (docker)
    connect(siw, &SimpleInsertWidget::doneWithFocus, this, &TextTool::returnFocusToCanvas);
    connect(siw, &SimpleInsertWidget::insertTableQuick, this, &TextTool::insertTableQuick);

    updateStyleManager();
    if (m_textShape) {
        updateActions();
    }
    scw->setWindowTitle(i18n("Character"));
    widgets.append(scw);
    spw->setWindowTitle(i18n("Paragraph"));
    widgets.append(spw);

    bool useAdvancedText =
        !(canvas()->resourceManager()->intResource(KoCanvasResourceManager::ApplicationSpeciality) & KoCanvasResourceManager::NoAdvancedText);
    if (useAdvancedText) {
        stw->setWindowTitle(i18n("Table"));
        widgets.append(stw);
        siw->setWindowTitle(i18n("Insert"));
        widgets.append(siw);
    }
    return widgets;
}

void TextTool::returnFocusToCanvas()
{
    canvas()->canvasWidget()->setFocus();
}

void TextTool::startEditing(KUndo2Command *command)
{
    m_currentCommand = command;
    m_currentCommandHasChildren = true;
}

void TextTool::stopEditing()
{
    m_currentCommand = nullptr;
    m_currentCommandHasChildren = false;
}

void TextTool::insertNewSection()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor)
        return;

    textEditor->newSection();
}

void TextTool::configureSection()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor)
        return;

    SectionFormatDialog *dia = new SectionFormatDialog(nullptr, m_textEditor.data());
    dia->exec();
    delete dia;

    returnFocusToCanvas();
    updateActions();
}

void TextTool::splitSections()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor)
        return;

    SectionsSplitDialog *dia = new SectionsSplitDialog(nullptr, m_textEditor.data());
    dia->exec();
    delete dia;

    returnFocusToCanvas();
    updateActions();
}

void TextTool::pasteAsText()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor)
        return;

    const QMimeData *data = QApplication::clipboard()->mimeData(QClipboard::Clipboard);
    // on windows we do not have data if we try to paste this selection
    if (!data)
        return;

    if (data->hasFormat(KoOdf::mimeType(KoOdf::Text)) || data->hasText()) {
        m_prevCursorPosition = m_textEditor.data()->position();
        m_textEditor.data()->paste(canvas(), data, true);
        editingPluginEvents();
    }
}

void TextTool::bold(bool bold)
{
    m_textEditor.data()->bold(bold);
}

void TextTool::italic(bool italic)
{
    m_textEditor.data()->italic(italic);
}

void TextTool::underline(bool underline)
{
    m_textEditor.data()->underline(underline);
}

void TextTool::strikeOut(bool strikeOut)
{
    m_textEditor.data()->strikeOut(strikeOut);
}

void TextTool::nonbreakingSpace()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->insertText(QString(QChar(QChar::Nbsp)));
}

void TextTool::nonbreakingHyphen()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->insertText(QString(QChar(0x2013)));
}

void TextTool::softHyphen()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->insertText(QString(QChar(QChar::SoftHyphen)));
}

void TextTool::lineBreak()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->insertText(QString(QChar(0x2028)));
}

void TextTool::alignLeft()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->setHorizontalTextAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
}

void TextTool::alignRight()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->setHorizontalTextAlignment(Qt::AlignRight | Qt::AlignAbsolute);
}

void TextTool::alignCenter()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->setHorizontalTextAlignment(Qt::AlignHCenter);
}

void TextTool::alignBlock()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->setHorizontalTextAlignment(Qt::AlignJustify);
}

void TextTool::superScript(bool on)
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    if (on)
        m_actionFormatSub->setChecked(false);
    m_textEditor.data()->setVerticalTextAlignment(on ? Qt::AlignTop : Qt::AlignVCenter);
}

void TextTool::subScript(bool on)
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    if (on)
        m_actionFormatSuper->setChecked(false);
    m_textEditor.data()->setVerticalTextAlignment(on ? Qt::AlignBottom : Qt::AlignVCenter);
}

void TextTool::increaseIndent()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    if (m_textEditor.data()->block().textList()) {
        ChangeListLevelCommand::CommandType type = ChangeListLevelCommand::IncreaseLevel;
        ChangeListLevelCommand *cll = new ChangeListLevelCommand(*(m_textEditor.data()->cursor()), type, 1);
        m_textEditor.data()->addCommand(cll);
        editingPluginEvents();
    } else {
        m_textEditor.data()->increaseIndent();
    }
    updateActions();
}

void TextTool::decreaseIndent()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    if (m_textEditor.data()->block().textList()) {
        ChangeListLevelCommand::CommandType type = ChangeListLevelCommand::DecreaseLevel;
        ChangeListLevelCommand *cll = new ChangeListLevelCommand(*(m_textEditor.data()->cursor()), type, 1);
        m_textEditor.data()->addCommand(cll);
        editingPluginEvents();
    } else {
        m_textEditor.data()->decreaseIndent();
    }
    updateActions();
}

void TextTool::decreaseFontSize()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->decreaseFontSize();
}

void TextTool::increaseFontSize()
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->increaseFontSize();
}

void TextTool::setFontFamily(const QString &font)
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->setFontFamily(font);
}

void TextTool::setFontSize(qreal size)
{
    if (!m_allowActions || !m_textEditor.data())
        return;
    m_textEditor.data()->setFontSize(size);
}

void TextTool::insertIndexMarker()
{
    // TODO handle result when we figure out how to report errors from a tool.
    m_textEditor.data()->insertIndexMarker();
}

void TextTool::insertFrameBreak()
{
    m_textEditor.data()->insertFrameBreak();

    ensureCursorVisible();
    m_delayedEnsureVisible = true;
}

void TextTool::setStyle(KoCharacterStyle *style)
{
    KoCharacterStyle *charStyle = style;
    // if the given KoCharacterStyle is null, set the KoParagraphStyle character properties
    if (!charStyle) {
        charStyle = static_cast<KoCharacterStyle *>(KoTextDocument(m_textShapeData->document())
                                                        .styleManager()
                                                        ->paragraphStyle(m_textEditor.data()->blockFormat().intProperty(KoParagraphStyle::StyleId)));
    }
    if (charStyle) {
        m_textEditor.data()->setStyle(charStyle);
        updateActions();
    }
}

void TextTool::setStyle(KoParagraphStyle *style)
{
    m_textEditor.data()->setStyle(style);
    updateActions();
}

void TextTool::insertTable()
{
    TableDialog *dia = new TableDialog(nullptr);
    if (dia->exec() == TableDialog::Accepted)
        m_textEditor.data()->insertTable(dia->rows(), dia->columns());
    delete dia;

    updateActions();
}

void TextTool::insertTableQuick(int rows, int columns)
{
    m_textEditor.data()->insertTable(rows, columns);
    updateActions();
}

void TextTool::insertTableRowAbove()
{
    m_textEditor.data()->insertTableRowAbove();
}

void TextTool::insertTableRowBelow()
{
    m_textEditor.data()->insertTableRowBelow();
}

void TextTool::insertTableColumnLeft()
{
    m_textEditor.data()->insertTableColumnLeft();
}

void TextTool::insertTableColumnRight()
{
    m_textEditor.data()->insertTableColumnRight();
}

void TextTool::deleteTableColumn()
{
    m_textEditor.data()->deleteTableColumn();
}

void TextTool::deleteTableRow()
{
    m_textEditor.data()->deleteTableRow();
}

void TextTool::mergeTableCells()
{
    m_textEditor.data()->mergeTableCells();
}

void TextTool::splitTableCells()
{
    m_textEditor.data()->splitTableCells();
}

void TextTool::useTableBorderCursor()
{
    static const unsigned char data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0xf4, 0x00, 0x00, 0x00, 0xfa,
                                         0x00, 0x00, 0x00, 0xfd, 0x00, 0x00, 0x80, 0x7e, 0x00, 0x00, 0x40, 0x3f, 0x00, 0x00, 0xa0, 0x1f, 0x00, 0x00, 0xd0,
                                         0x0f, 0x00, 0x00, 0xe8, 0x07, 0x00, 0x00, 0xf4, 0x03, 0x00, 0x00, 0xe4, 0x01, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x80,
                                         0x41, 0x00, 0x00, 0x40, 0x32, 0x00, 0x00, 0xa0, 0x0f, 0x00, 0x00, 0xd0, 0x0f, 0x00, 0x00, 0xd0, 0x0f, 0x00, 0x00,
                                         0xe8, 0x07, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    QBitmap result(32, 32);
    result.fill(Qt::color0);
    QPainter painter(&result);
    painter.drawPixmap(0, 0, QBitmap::fromData(QSize(25, 23), data));
    QBitmap brushMask = result.createHeuristicMask(false);

    useCursor(QCursor(result, brushMask, 1, 21));
}

void TextTool::setTableBorderData(const KoBorder::BorderData &data)
{
    m_tablePenMode = true;
    m_tablePenBorderData = data;
}

void TextTool::formatParagraph()
{
    ParagraphSettingsDialog *dia = new ParagraphSettingsDialog(this, m_textEditor.data());
    dia->setUnit(canvas()->unit());
    dia->setImageCollection(m_textShape->imageCollection());
    dia->exec();
    delete dia;
    returnFocusToCanvas();
}

void TextTool::testSlot(bool on)
{
    debugTextShape << "signal received. bool:" << on;
}

void TextTool::selectAll()
{
    KoTextEditor *textEditor = m_textEditor.data();
    if (!textEditor || !m_textShapeData)
        return;
    const int selectionLength = qAbs(textEditor->position() - textEditor->anchor());
    textEditor->movePosition(QTextCursor::End);
    textEditor->setPosition(0, QTextCursor::KeepAnchor);
    repaintSelection();
    if (selectionLength != qAbs(textEditor->position() - textEditor->anchor())) // it actually changed
        Q_EMIT selectionChanged(true);
}

void TextTool::startMacro(const QString &title)
{
    if (title != i18n("Key Press") && title != i18n("Autocorrection")) // dirty hack while waiting for refactor of text editing
        m_textTyping = false;
    else
        m_textTyping = true;

    if (title != i18n("Delete") && title != i18n("Autocorrection")) // same dirty hack as above
        m_textDeleting = false;
    else
        m_textDeleting = true;

    if (m_currentCommand)
        return;

    class MacroCommand : public KUndo2Command
    {
    public:
        MacroCommand(const KUndo2MagicString &title)
            : KUndo2Command(title)
            , m_first(true)
        {
        }
        void redo() override
        {
            if (!m_first)
                KUndo2Command::redo();
            m_first = false;
        }
        bool mergeWith(const KUndo2Command *) override
        {
            return false;
        }
        bool m_first;
    };

    /**
     * FIXME: The messages generated by the Text Tool might not be
     *        properly translated, since we don't control it in
     *        type-safe way.
     *
     *        The title is already translated string, we just don't
     *        have any type control over it.
     */
    KUndo2MagicString title_workaround = kundo2_noi18n(title);
    m_currentCommand = new MacroCommand(title_workaround);
    m_currentCommandHasChildren = false;
}

void TextTool::stopMacro()
{
    if (!m_currentCommand)
        return;
    if (!m_currentCommandHasChildren)
        delete m_currentCommand;
    m_currentCommand = nullptr;
}

void TextTool::showStyleManager(int styleId)
{
    if (!m_textShapeData)
        return;
    KoStyleManager *styleManager = KoTextDocument(m_textShapeData->document()).styleManager();
    Q_ASSERT(styleManager);
    if (!styleManager)
        return; // don't crash
    StyleManagerDialog *dia = new StyleManagerDialog(canvas()->canvasWidget());
    dia->setStyleManager(styleManager);
    dia->setUnit(canvas()->unit());

    KoParagraphStyle *paragraphStyle = styleManager->paragraphStyle(styleId);
    if (paragraphStyle) {
        dia->setParagraphStyle(paragraphStyle);
    }
    KoCharacterStyle *characterStyle = styleManager->characterStyle(styleId);
    if (characterStyle) {
        dia->setCharacterStyle(characterStyle);
    }
    dia->show();
}

void TextTool::startTextEditingPlugin(const QString &pluginId)
{
    KoTextEditingPlugin *plugin = textEditingPluginContainer()->plugin(pluginId);
    if (plugin) {
        if (m_textEditor.data()->hasSelection()) {
            plugin->checkSection(m_textShapeData->document(), m_textEditor.data()->selectionStart(), m_textEditor.data()->selectionEnd());
        } else
            plugin->finishedWord(m_textShapeData->document(), m_textEditor.data()->position());
    }
}

void TextTool::canvasResourceChanged(int key, const QVariant &var)
{
    if (m_textEditor.isNull())
        return;
    if (!m_textShapeData)
        return;
    if (m_allowResourceManagerUpdates == false)
        return;
    if (key == KoText::CurrentTextPosition) {
        repaintSelection();
        m_textEditor.data()->setPosition(var.toInt());
        ensureCursorVisible();
    } else if (key == KoText::CurrentTextAnchor) {
        repaintSelection();
        int pos = m_textEditor.data()->position();
        m_textEditor.data()->setPosition(var.toInt());
        m_textEditor.data()->setPosition(pos, QTextCursor::KeepAnchor);
    } else if (key == KoCanvasResourceManager::Unit) {
        m_unit = var.value<KoUnit>();
    } else
        return;

    repaintSelection();
}

void TextTool::insertSpecialCharacter()
{
    if (m_specialCharacterDocker == nullptr) {
        m_specialCharacterDocker = new InsertCharacter(canvas()->canvasWidget());
        connect(m_specialCharacterDocker, QOverload<const QString &>::of(&InsertCharacter::insertCharacter), this, &TextTool::insertString);
    }

    m_specialCharacterDocker->show();
}

void TextTool::insertString(const QString &string)
{
    m_textEditor.data()->insertText(string);
    returnFocusToCanvas();
}

void TextTool::selectFont()
{
    FontDia *fontDlg = new FontDia(m_textEditor.data());
    fontDlg->exec();
    delete fontDlg;
    returnFocusToCanvas();
}

void TextTool::shapeAddedToCanvas()
{
    debugTextShape;
    if (m_textShape) {
        KoSelection *selection = canvas()->shapeManager()->selection();
        KoShape *shape = selection->firstSelectedShape();
        if (shape != m_textShape && canvas()->shapeManager()->shapes().contains(m_textShape)) {
            // this situation applies when someone, not us, changed the selection by selecting another
            // text shape. Possibly by adding one.
            // Deselect the new shape again, so we can keep editing what we were already editing
            selection->select(m_textShape);
            selection->deselect(shape);
        }
    }
}

void TextTool::shapeDataRemoved()
{
    m_textShapeData = nullptr;
    m_textShape = nullptr;
    if (!m_textEditor.isNull() && !m_textEditor.data()->cursor()->isNull()) {
        const QTextDocument *doc = m_textEditor.data()->document();
        Q_ASSERT(doc);
        KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(doc->documentLayout());
        if (!lay || lay->shapes().isEmpty()) {
            Q_EMIT done();
            return;
        }
        m_textShape = static_cast<TextShape *>(lay->shapes().first());
        m_textShapeData = static_cast<KoTextShapeData *>(m_textShape->userData());
        connect(m_textShapeData, &QObject::destroyed, this, &TextTool::shapeDataRemoved);
    }
}

void TextTool::createStyleFromCurrentBlockFormat(const QString &name)
{
    KoTextDocument document(m_textShapeData->document());
    KoStyleManager *styleManager = document.styleManager();
    KoParagraphStyle *paragraphStyle = new KoParagraphStyle(m_textEditor.data()->blockFormat(), m_textEditor.data()->charFormat());
    paragraphStyle->setName(name);
    styleManager->add(paragraphStyle);
    m_textEditor.data()->setStyle(paragraphStyle);
    Q_EMIT charFormatChanged(m_textEditor.data()->charFormat(), m_textEditor.data()->blockCharFormat());
    Q_EMIT blockFormatChanged(m_textEditor.data()->blockFormat());
}

void TextTool::createStyleFromCurrentCharFormat(const QString &name)
{
    KoTextDocument document(m_textShapeData->document());
    KoStyleManager *styleManager = document.styleManager();
    KoCharacterStyle *originalCharStyle = styleManager->characterStyle(m_textEditor.data()->charFormat().intProperty(KoCharacterStyle::StyleId));
    KoCharacterStyle *autoStyle;
    if (!originalCharStyle) {
        KoCharacterStyle blankStyle;
        originalCharStyle = &blankStyle;
        autoStyle = originalCharStyle->autoStyle(m_textEditor.data()->charFormat(), m_textEditor.data()->blockCharFormat());
        autoStyle->setParentStyle(nullptr);
    } else {
        autoStyle = originalCharStyle->autoStyle(m_textEditor.data()->charFormat(), m_textEditor.data()->blockCharFormat());
    }
    autoStyle->setName(name);
    styleManager->add(autoStyle);
    m_textEditor.data()->setStyle(autoStyle);
    Q_EMIT charFormatChanged(m_textEditor.data()->charFormat(), m_textEditor.data()->blockCharFormat());
}

// ---------- editing plugins methods.
void TextTool::editingPluginEvents()
{
    if (m_prevCursorPosition == -1 || m_prevCursorPosition == m_textEditor.data()->position()) {
        debugTextShape << "m_prevCursorPosition=" << m_prevCursorPosition << "m_textEditor.data()->position()=" << m_textEditor.data()->position();
        return;
    }

    QTextBlock block = m_textEditor.data()->block();
    if (!block.contains(m_prevCursorPosition)) {
        debugTextShape << "m_prevCursorPosition=" << m_prevCursorPosition;
        finishedWord();
        finishedParagraph();
        m_prevCursorPosition = -1;
    } else {
        characterInserted();
        int from = m_prevCursorPosition;
        int to = m_textEditor.data()->position();
        if (from > to)
            qSwap(from, to);
        QString section = block.text().mid(from - block.position(), to - from);
        debugTextShape << "from=" << from << "to=" << to;
        if (section.contains(' ')) {
            finishedWord();
            m_prevCursorPosition = -1;
        }
    }
}

void TextTool::finishedWord()
{
    if (m_textShapeData && textEditingPluginContainer()) {
        foreach (KoTextEditingPlugin *plugin, textEditingPluginContainer()->values()) {
            plugin->finishedWord(m_textShapeData->document(), m_prevCursorPosition);
        }
    }
}

void TextTool::finishedParagraph()
{
    if (m_textShapeData && textEditingPluginContainer()) {
        foreach (KoTextEditingPlugin *plugin, textEditingPluginContainer()->values()) {
            plugin->finishedParagraph(m_textShapeData->document(), m_prevCursorPosition);
        }
    }
}

void TextTool::startingSimpleEdit()
{
    if (m_textShapeData && textEditingPluginContainer()) {
        foreach (KoTextEditingPlugin *plugin, textEditingPluginContainer()->values()) {
            plugin->startingSimpleEdit(m_textShapeData->document(), m_prevCursorPosition);
        }
    }
}

void TextTool::characterInserted()
{
    qInfo() << Q_FUNC_INFO << m_textShapeData;
    if (m_textShapeData && textEditingPluginContainer()) {
        const QList<KoTextEditingPlugin *> plugins = textEditingPluginContainer()->values();
        qInfo() << Q_FUNC_INFO << "plugins" << plugins;
        if (plugins.isEmpty()) {
            qInfo() << Q_FUNC_INFO << "No plugins";
        }
        for (KoTextEditingPlugin *plugin : plugins) {
            m_prevCursorPosition = plugin->characterInserted(m_textShapeData->document(), m_prevCursorPosition);
        }
    } else {
        qInfo() << Q_FUNC_INFO << "No shape data or plugin container";
    }
}

void TextTool::setTextColor(const KoColor &color)
{
    m_textEditor.data()->setTextColor(color.toQColor());
}

void TextTool::setBackgroundColor(const KoColor &color)
{
    m_textEditor.data()->setTextBackgroundColor(color.toQColor());
}

void TextTool::setAutoResize(bool enabled)
{
    m_textEditor.data()->addCommand(new AutoResizeCommand(m_textShapeData, KoTextShapeData::AutoResize, enabled));
    updateActions();
}

void TextTool::setGrowWidthToFit(bool enabled)
{
    m_textEditor.data()->addCommand(new AutoResizeCommand(m_textShapeData, KoTextShapeData::AutoGrowWidth, enabled));
    updateActions();
}

void TextTool::setGrowHeightToFit(bool enabled)
{
    m_textEditor.data()->addCommand(new AutoResizeCommand(m_textShapeData, KoTextShapeData::AutoGrowHeight, enabled));
    updateActions();
}

void TextTool::setShrinkToFit(bool enabled)
{
    m_textEditor.data()->addCommand(new AutoResizeCommand(m_textShapeData, KoTextShapeData::ShrinkToFitResize, enabled));
    updateActions();
}

void TextTool::runUrl(KoPointerEvent *event, QString &url)
{
    QUrl _url = QUrl::fromLocalFile(url);
    if (_url.isLocalFile()) {
        return;

        // QMimeDatabase db;
        // QString type = db.mimeTypeForUrl(_url).name();

        // if (KRun::isExecutableFile(_url, type)) {
        //     const QString question = i18n("This link points to the program or script '%1'.\n"
        //                             "Malicious programs can harm your computer. "
        //                             "Are you sure that you want to run this program?", url);
        //     // this will also start local programs, so adding a "don't warn again"
        //     // checkbox will probably be too dangerous
        //     int choice = KMessageBox::warningTwoActions(nullptr, question, i18n("Open Link?"), KStandardGuiItem::open(), KStandardGuiItem::cancel());
        //     if (choice != KMessageBox::PrimaryAction)
        //         return;
        // }
    }

    event->accept();
    QDesktopServices::openUrl(_url);
}

void TextTool::debugTextDocument()
{
#ifndef NDEBUG
    if (!m_textShapeData)
        return;
    const int CHARSPERLINE = 80; // TODO Make configurable using ENV var?
    const int CHARPOSITION = 278301935;
    KoTextDocument document(m_textShapeData->document());
    KoStyleManager *styleManager = document.styleManager();
    KoInlineTextObjectManager *inlineManager = document.inlineTextObjectManager();

    QTextBlock block = m_textShapeData->document()->begin();
    for (; block.isValid(); block = block.next()) {
        QVariant var = block.blockFormat().property(KoParagraphStyle::StyleId);
        if (!var.isNull()) {
            KoParagraphStyle *ps = styleManager->paragraphStyle(var.toInt());
            debugTextShape << "--- Paragraph Style:" << (ps ? ps->name() : QString()) << var.toInt();
        }
        var = block.charFormat().property(KoCharacterStyle::StyleId);
        if (!var.isNull()) {
            KoCharacterStyle *cs = styleManager->characterStyle(var.toInt());
            debugTextShape << "--- Character Style:" << (cs ? cs->name() : QString()) << var.toInt();
        }
        int lastPrintedChar = -1;
        QTextBlock::iterator it;
        QString fragmentText;
        QList<QTextCharFormat> inlineCharacters;
        for (it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;
            QTextCharFormat fmt = fragment.charFormat();
            debugTextShape << "changeId: " << fmt.property(KoCharacterStyle::ChangeTrackerId);
            const int fragmentStart = fragment.position() - block.position();
            for (int i = fragmentStart; i < fragmentStart + fragment.length(); i += CHARSPERLINE) {
                if (lastPrintedChar == fragmentStart - 1)
                    fragmentText += '|';
                if (lastPrintedChar < fragmentStart || i > fragmentStart) {
                    QString debug = block.text().mid(lastPrintedChar, CHARSPERLINE);
                    lastPrintedChar += CHARSPERLINE;
                    if (lastPrintedChar > block.length())
                        debug += "\\n";
                    debugTextShape << debug;
                }
                var = fmt.property(KoCharacterStyle::StyleId);
                QString charStyleLong, charStyleShort;
                if (!var.isNull()) { // named style
                    charStyleShort = QString::number(var.toInt());
                    KoCharacterStyle *cs = styleManager->characterStyle(var.toInt());
                    if (cs)
                        charStyleLong = cs->name();
                }
                if (inlineManager && fmt.hasProperty(KoCharacterStyle::InlineInstanceId)) {
                    QTextCharFormat inlineFmt = fmt;
                    inlineFmt.setProperty(CHARPOSITION, fragmentStart);
                    inlineCharacters << inlineFmt;
                }

                if (fragment.length() > charStyleLong.length())
                    fragmentText += charStyleLong;
                else if (fragment.length() > charStyleShort.length())
                    fragmentText += charStyleShort;
                else if (fragment.length() >= 2)
                    fragmentText += QChar(8230); // ellipsis

                int rest = fragmentStart - (lastPrintedChar - CHARSPERLINE) + fragment.length() - fragmentText.length();
                rest = qMin(rest, CHARSPERLINE - fragmentText.length());
                if (rest >= 2)
                    fragmentText = QString("%1%2").arg(fragmentText).arg(' ', rest);
                if (rest >= 0)
                    fragmentText += '|';
                if (fragmentText.length() >= CHARSPERLINE) {
                    debugTextShape << fragmentText;
                    fragmentText.clear();
                }
            }
        }
        if (!fragmentText.isEmpty()) {
            debugTextShape << fragmentText;
        } else if (block.length() == 1) { // no actual tet
            debugTextShape << "\\n";
        }
        foreach (const QTextCharFormat &cf, inlineCharacters) {
            KoInlineObject *object = inlineManager->inlineTextObject(cf);
            debugTextShape << "At pos:" << cf.intProperty(CHARPOSITION) << object;
            // debugTextShape << "-> id:" << cf.intProperty(577297549);
        }
        QTextList *list = block.textList();
        if (list) {
            if (list->format().hasProperty(KoListStyle::StyleId)) {
                KoListStyle *ls = styleManager->listStyle(list->format().intProperty(KoListStyle::StyleId));
                debugTextShape << "   List style applied:" << ls->styleId() << ls->name();
            } else
                debugTextShape << " +- is a list..." << list;
        }
    }
#endif
}

void TextTool::debugTextStyles()
{
#ifndef NDEBUG
    if (!m_textShapeData)
        return;
    KoTextDocument document(m_textShapeData->document());
    KoStyleManager *styleManager = document.styleManager();

    QSet<int> seenStyles;

    foreach (KoParagraphStyle *style, styleManager->paragraphStyles()) {
        debugTextShape << style->styleId() << style->name() << (styleManager->defaultParagraphStyle() == style ? "[Default]" : "");
        KoListStyle *ls = style->listStyle();
        if (ls) { // optional ;)
            debugTextShape << "  +- ListStyle: " << ls->styleId() << ls->name() << (ls == styleManager->defaultListStyle() ? "[Default]" : "");
            foreach (int level, ls->listLevels()) {
                KoListLevelProperties llp = ls->levelProperties(level);
                debugTextShape << "  |  level" << llp.level() << " style (enum):" << llp.labelType();
                if (llp.bulletCharacter().unicode() != 0) {
                    debugTextShape << "  |  bullet" << llp.bulletCharacter();
                }
            }
            seenStyles << ls->styleId();
        }
    }

    bool first = true;
    foreach (KoCharacterStyle *style, styleManager->characterStyles()) {
        if (seenStyles.contains(style->styleId()))
            continue;
        if (first) {
            debugTextShape << "--- Character styles ---";
            first = false;
        }
        debugTextShape << style->styleId() << style->name();
        debugTextShape << style->font();
    }

    first = true;
    foreach (KoListStyle *style, styleManager->listStyles()) {
        if (seenStyles.contains(style->styleId()))
            continue;
        if (first) {
            debugTextShape << "--- List styles ---";
            first = false;
        }
        debugTextShape << style->styleId() << style->name() << (style == styleManager->defaultListStyle() ? "[Default]" : "");
    }
#endif
}

void TextTool::textDirectionChanged()
{
    if (!m_allowActions || !m_textEditor.data())
        return;

    QTextBlockFormat blockFormat;
    if (m_actionChangeDirection->isChecked()) {
        blockFormat.setProperty(KoParagraphStyle::TextProgressionDirection, KoText::RightLeftTopBottom);
    } else {
        blockFormat.setProperty(KoParagraphStyle::TextProgressionDirection, KoText::LeftRightTopBottom);
    }
    m_textEditor.data()->mergeBlockFormat(blockFormat);
}

void TextTool::setListLevel(int level)
{
    if (level < 1 || level > 10) {
        return;
    }

    KoTextEditor *textEditor = m_textEditor.data();
    if (textEditor->block().textList()) {
        ChangeListLevelCommand::CommandType type = ChangeListLevelCommand::SetLevel;
        ChangeListLevelCommand *cll = new ChangeListLevelCommand(*textEditor->cursor(), type, level);
        textEditor->addCommand(cll);
        editingPluginEvents();
    }
}

void TextTool::insertAnnotation()
{
    // HACK to avoid crash when we try to add a comment to an annotation shape.
    // We should not get here when the shape is an annotation shape,
    // but just disabling the insert_annotation action somehow does not work.
    if (m_textShape->shapeId() == AnnotationShape_SHAPEID) {
        return;
    }
    AnnotationTextShape *shape =
        (AnnotationTextShape *)KoShapeRegistry::instance()->value(AnnotationShape_SHAPEID)->createDefaultShape(canvas()->shapeController()->resourceManager());
    textEditor()->addAnnotation(shape);

    // Set annotation creator.
    KConfig cfg("calligrarc");
    cfg.reparseConfiguration();
    KConfigGroup authorGroup(&cfg, "Author");
    QStringList profiles = authorGroup.readEntry("profile-names", QStringList());
    KSharedConfig::openConfig()->reparseConfiguration();
    KConfigGroup appAuthorGroup(KSharedConfig::openConfig(), "Author");
    QString profile = appAuthorGroup.readEntry("active-profile", "");
    KConfigGroup cgs(&authorGroup, "Author-" + profile);

    if (profiles.contains(profile)) {
        KConfigGroup cgs(&authorGroup, "Author-" + profile);
        shape->setCreator(cgs.readEntry("creator"));
    } else {
        if (profile == "anonymous") {
            shape->setCreator("Anonymous");
        } else {
            KUser user(KUser::UseRealUserID);
            shape->setCreator(user.property(KUser::FullName).toString());
        }
    }
    // Set Annotation creation date.
    shape->setDate(QDate::currentDate().toString(Qt::ISODate));
}
