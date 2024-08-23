/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoFormat.h"
#include "Actions.h"
#include "dialogs/AutoFormatDialog.h"

#include <KConfig>
#include <KLocalizedString>
#include <KMessageBox>

#include <KoResourcePaths.h>
#include <KoXmlReader.h>

#include <QFile>

#include "core/Cell.h"
#include "core/CellStorage.h"
#include "core/Database.h"
#include "core/Sheet.h"
#include "core/Style.h"
#include "core/ksp/SheetsKsp.h"

using namespace Calligra::Sheets;

AutoFormat::AutoFormat(Actions *actions)
    : DialogCellAction(actions, "autoFormat", i18n("Auto-Format..."), QIcon(), i18n("Apply pre-defined formatting to a range of cells."))
    , m_canvasWidget(nullptr)
{
}

AutoFormat::~AutoFormat() = default;

ActionDialog *AutoFormat::createDialog(QWidget *canvasWidget)
{
    m_canvasWidget = canvasWidget;
    AutoFormatDialog *dlg = new AutoFormatDialog(canvasWidget);
    connect(dlg, &AutoFormatDialog::applyFormat, this, &AutoFormat::applyFormat);
    m_xmls.clear();

    const QStringList lst = KoResourcePaths::findAllResources("sheet-styles", "*.ksts", KoResourcePaths::Recursive);
    QMap<QString, QPixmap> pixmaps;

    for (const QString &fname : lst) {
        KConfig config(fname, KConfig::SimpleConfig);
        const KConfigGroup sheetStyleGroup = config.group("Sheet-Style");
        QString name = sheetStyleGroup.readEntry("Name");
        QString xml = sheetStyleGroup.readEntry("XML");
        QString image = sheetStyleGroup.readEntry("Image");

        QString imageName = KoResourcePaths::findResource("sheet-styles", image);
        if (imageName.isEmpty())
            continue;
        QPixmap pixmap(imageName);
        if (pixmap.isNull())
            continue;

        m_xmls[name] = xml;
        pixmaps[name] = pixmap;
    }
    dlg->setList(pixmaps);

    return dlg;
}

void AutoFormat::applyFormat(const QString &name)
{
    if (!m_xmls.contains(name))
        return;

    QString xml = m_xmls[name];
    QString xmlname = KoResourcePaths::findResource("sheet-styles", xml);
    if (xmlname.isEmpty()) {
        KMessageBox::error(m_canvasWidget, i18n("Could not find sheet-style XML file '%1'.", xmlname));
        return;
    }

    QFile file(xmlname);
    file.open(QIODevice::ReadOnly);
    KoXmlDocument doc;
    doc.setContent(&file);
    file.close();

    bool ok;
    QList<Style> styles = parseXML(doc, &ok);
    if (!ok) {
        KMessageBox::error(m_canvasWidget, i18n("Parsing error in sheet-style XML file %1.", xmlname));
        return;
    }

    AutoFormatCommand *command = new AutoFormatCommand();
    command->setSheet(m_selection->activeSheet());
    command->setStyles(styles);
    command->add(*m_selection);
    command->execute(m_selection->canvas());
}

QList<Style> AutoFormat::parseXML(const KoXmlDocument &doc, bool *ok)
{
    *ok = false;
    QList<Style> styles;
    styles.clear();
    for (int i = 0; i < 16; ++i)
        styles.append(Style());

    for (KoXmlNode node = doc.documentElement().firstChild(); !node.isNull(); node = node.nextSibling()) {
        KoXmlElement e = node.toElement();
        if (e.tagName() == "cell") {
            Style style;
            KoXmlElement tmpElement(e.namedItem("format").toElement());
            if (!Ksp::loadStyle(&style, tmpElement))
                return styles;

            int row = e.attribute("row").toInt();
            int column = e.attribute("column").toInt();
            int i = (row - 1) * 4 + (column - 1);
            if (i < 0 || i >= 16)
                return styles;

            styles[i] = style;
        }
    }
    *ok = true;
    return styles;
}

AutoFormatCommand::AutoFormatCommand()
{
    setText(kundo2_i18n("Auto-Format"));
}

AutoFormatCommand::~AutoFormatCommand() = default;

void AutoFormatCommand::setStyles(const QList<Style> &styles)
{
    m_styles = styles;
}

bool AutoFormatCommand::process(Element *element)
{
    const QRect rect = element->rect();
    CellStorage *cs = m_sheet->fullCellStorage();

    // always reset the style of the processed region
    Style defaultStyle;
    defaultStyle.setDefault();
    Region::ConstIterator end(constEnd());
    for (Region::ConstIterator it = constBegin(); it != end; ++it)
        cs->setStyle(Region((*it)->rect()), defaultStyle);

    // Top left corner
    if (!m_styles[0].isDefault())
        cs->setStyle(Region(rect.topLeft()), m_styles[0]);
    // Top row
    for (int col = rect.left() + 1; col <= rect.right(); ++col) {
        int pos = 1 + ((col - rect.left() - 1) % 2);
        Cell cell(m_sheet, col, rect.top());
        if (!cell.isPartOfMerged()) {
            Style style;
            if (!m_styles[pos].isDefault())
                style = m_styles[pos];

            Style tmpStyle = (col == rect.left() + 1) ? m_styles[1] : m_styles[2];
            if (!tmpStyle.isDefault())
                style.setLeftBorderPen(tmpStyle.leftBorderPen());

            cs->setStyle(Region(col, rect.top()), style);
        }
    }

    // Left column
    for (int row = rect.top() + 1; row <= rect.bottom(); ++row) {
        int pos = 4 + ((row - rect.top() - 1) % 2) * 4;
        Cell cell(m_sheet, rect.left(), row);
        if (!cell.isPartOfMerged()) {
            Style style;
            if (!m_styles[pos].isDefault())
                style = m_styles[pos];

            Style tmpStyle = (row == rect.top() + 1) ? m_styles[4] : m_styles[8];
            if (!tmpStyle.isDefault())
                style.setTopBorderPen(tmpStyle.topBorderPen());

            cs->setStyle(Region(rect.left(), row), style);
        }
    }

    // Body
    for (int col = rect.left() + 1; col <= rect.right(); ++col) {
        for (int row = rect.top() + 1; row <= rect.bottom(); ++row) {
            int pos = 5 + ((row - rect.top() - 1) % 2) * 4 + ((col - rect.left() - 1) % 2);
            Cell cell(m_sheet, col, row);
            if (!cell.isPartOfMerged()) {
                if (!m_styles[pos].isDefault())
                    cs->setStyle(Region(col, row), m_styles[pos]);

                Style style;
                if (col == rect.left() + 1)
                    style = m_styles[5 + ((row - rect.top() - 1) % 2) * 4];
                else
                    style = m_styles[6 + ((row - rect.top() - 1) % 2) * 4];

                if (!style.isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setLeftBorderPen(style.leftBorderPen());
                    cs->setStyle(Region(col, row), tmpStyle);
                }

                if (row == rect.top() + 1)
                    style = m_styles[5 + ((col - rect.left() - 1) % 2)];
                else
                    style = m_styles[9 + ((col - rect.left() - 1) % 2)];

                if (!style.isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setTopBorderPen(style.topBorderPen());
                    cs->setStyle(Region(col, row), tmpStyle);
                }
            }
        }
    }

    // Outer right border
    for (int row = rect.top(); row <= rect.bottom(); ++row) {
        Cell cell(m_sheet, rect.right(), row);
        if (!cell.isPartOfMerged()) {
            if (row == rect.top()) {
                if (!m_styles[3].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setRightBorderPen(m_styles[3].leftBorderPen());
                    cs->setStyle(Region(rect.right(), row), tmpStyle);
                }
            } else if (row == rect.right()) {
                if (!m_styles[11].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setRightBorderPen(m_styles[11].leftBorderPen());
                    cs->setStyle(Region(rect.right(), row), tmpStyle);
                }
            } else {
                if (!m_styles[7].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setRightBorderPen(m_styles[7].leftBorderPen());
                    cs->setStyle(Region(rect.right(), row), tmpStyle);
                }
            }
        }
    }

    // Outer bottom border
    for (int col = rect.left(); col <= rect.right(); ++col) {
        Cell cell(m_sheet, col, rect.bottom());
        if (!cell.isPartOfMerged()) {
            if (col == rect.left()) {
                if (!m_styles[12].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setBottomBorderPen(m_styles[12].topBorderPen());
                    cs->setStyle(Region(col, rect.bottom()), tmpStyle);
                }
            } else if (col == rect.right()) {
                if (!m_styles[14].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setBottomBorderPen(m_styles[14].topBorderPen());
                    cs->setStyle(Region(col, rect.bottom()), tmpStyle);
                }
            } else {
                if (!m_styles[13].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setBottomBorderPen(m_styles[13].topBorderPen());
                    cs->setStyle(Region(col, rect.bottom()), tmpStyle);
                }
            }
        }
    }
    return true;
}

bool AutoFormat::enabledForSelection(Selection *selection, const Cell &)
{
    if (!selection->isContiguous())
        return false;
    // don't allow full rows/columns
    if (selection->isRowSelected())
        return false;
    if (selection->isColumnSelected())
        return false;
    QRect range = selection->lastRange();
    if ((range.width() < 2) && (range.height() < 2))
        return false;
    return true;
}
