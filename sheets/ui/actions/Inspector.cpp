/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
// SPDX-FileCopyrightText: 2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Inspector.h"
#include "Actions.h"

#include <QAction>
#include <QFrame>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KMessageBox>
#include <KPageWidget>

// Sheets
#include "core/Cell.h"
#include "core/Sheet.h"
#include "engine/DependencyManager.h"
#include "engine/MapBase.h"
#include "engine/Region.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

Inspector::Inspector(Actions *actions)
    : DialogCellAction(actions,
                       "inspector",
                       i18n("Run Inspector..."),
                       koIconWanted("not used in UI, but devs might do, so nice to have", "inspector"),
                       QString())
{
}

Inspector::~Inspector() = default;

QAction *Inspector::createAction()
{
    QAction *res = CellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_I));
    return res;
}

ActionDialog *Inspector::createDialog(QWidget *canvasWidget)
{
    return new InspectorDialog(canvasWidget);
}

void Inspector::onSelectionChanged()
{
    InspectorDialog *dlg = dynamic_cast<InspectorDialog *>(m_dlg);
    Cell cell(m_selection->activeSheet(), m_selection->cursor());
    dlg->setCell(cell);
}

// * * * DIALOG * * *

namespace Calligra
{
namespace Sheets
{

class InspectorDialog::Private
{
public:
    Cell cell;
    Style style;
    Sheet *sheet;

    QTreeWidget *cellView;
    QTreeWidget *sheetView;
    QTreeWidget *styleView;
    QTreeWidget *depView;

    void handleCell();
    void handleSheet();
    void handleStyle();
    void handleDep();
};

} // namespace Sheets
} // namespace Calligra

static QString boolAsString(bool b)
{
    if (b)
        return QString("True");
    else
        return QString("False");
}

static QString dirAsString(Qt::LayoutDirection dir)
{
    QString str;
    switch (dir) {
    case Qt::LeftToRight:
        str = "Left to Right";
        break;
    case Qt::RightToLeft:
        str = "Right to Left";
        break;
    default:
        str = "Unknown";
        break;
    }
    return str;
}

void InspectorDialog::Private::handleCell()
{
    QString str;

    cellView->clear();

    new QTreeWidgetItem(cellView, QStringList() << "Column" << QString::number(cell.column()));
    new QTreeWidgetItem(cellView, QStringList() << "Row" << QString::number(cell.row()));
    new QTreeWidgetItem(cellView, QStringList() << "Name" << cell.name());
    new QTreeWidgetItem(cellView, QStringList() << "Full Name" << cell.fullName());

    new QTreeWidgetItem(cellView, QStringList() << "Default" << boolAsString(cell.isDefault()));
    new QTreeWidgetItem(cellView, QStringList() << "Empty" << boolAsString(cell.isEmpty()));
    new QTreeWidgetItem(cellView, QStringList() << "Formula" << boolAsString(cell.isFormula()));
    new QTreeWidgetItem(cellView, QStringList() << "Text" << cell.userInput());
    new QTreeWidgetItem(cellView, QStringList() << "Text (Displayed)" << cell.displayText().replace(QChar('\n'), "\\n"));

    QTextStream ts(&str, QIODeviceBase::WriteOnly);
    ts << cell.value();
    new QTreeWidgetItem(cellView, QStringList() << "Value" << str);

    new QTreeWidgetItem(cellView, QStringList() << "Link" << cell.link());

    new QTreeWidgetItem(cellView, QStringList() << "Width" << QString::number(cell.width()));
    new QTreeWidgetItem(cellView, QStringList() << "Height" << QString::number(cell.height()));
}

void InspectorDialog::Private::handleStyle() // direct style access
{
    styleView->clear();
    const Style style = cell.style();

    new QTreeWidgetItem(styleView, QStringList() << "Style Name" << style.parentName());

    new QTreeWidgetItem(styleView, QStringList() << "Angle" << QString::number(style.angle()));
    new QTreeWidgetItem(styleView, QStringList() << "Multirow" << boolAsString(style.wrapText()));
    new QTreeWidgetItem(styleView, QStringList() << "Protected" << boolAsString(!style.notProtected()));
    new QTreeWidgetItem(styleView, QStringList() << "Vertical Text" << boolAsString(style.verticalText()));

    new QTreeWidgetItem(styleView, QStringList() << "Currency symbol" << style.currency().symbol());
    new QTreeWidgetItem(styleView, QStringList() << "Currency code" << style.currency().code());

    QTreeWidgetItem *flags = new QTreeWidgetItem(styleView, QStringList("Flags"));
    new QTreeWidgetItem(flags, QStringList() << "Border (left)" << boolAsString(style.hasAttribute(Style::LeftPen)));
    new QTreeWidgetItem(flags, QStringList() << "Border (right)" << boolAsString(style.hasAttribute(Style::RightPen)));
    new QTreeWidgetItem(flags, QStringList() << "Border (top)" << boolAsString(style.hasAttribute(Style::TopPen)));
    new QTreeWidgetItem(flags, QStringList() << "Border (bottom)" << boolAsString(style.hasAttribute(Style::BottomPen)));

    new QTreeWidgetItem(styleView, QStringList() << "Border pen width (bottom)" << QString::number(style.bottomBorderPen().width()));
}

void InspectorDialog::Private::handleSheet()
{
    sheetView->clear();

    new QTreeWidgetItem(sheetView, QStringList() << "Name" << sheet->sheetName());
    new QTreeWidgetItem(sheetView, QStringList() << "Layout Direction" << dirAsString(sheet->layoutDirection()));
}

void InspectorDialog::Private::handleDep()
{
    DependencyManager *manager = sheet->map()->dependencyManager();
    Region deps = manager->consumingRegion(cell);

    depView->clear();
    Region::ConstIterator end(deps.constEnd());
    for (Region::ConstIterator it(deps.constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        for (int col = range.left(); col <= range.right(); ++col)
            for (int row = range.top(); row <= range.bottom(); ++row) {
                QString k1, k2;

                k1 = Cell::fullName((*it)->sheet(), col, row);

                new QTreeWidgetItem(depView, QStringList() << k1 << k2);
            }
    }
}

InspectorDialog::InspectorDialog(QWidget *parent)
    : ActionDialog(parent)
    , d(new Private)
{
    setWindowTitle("Inspector");
    setButtons(Close);

    KPageWidget *main = new KPageWidget();
    setMainWidget(main);
    main->setFaceType(KPageWidget::List);

    QFrame *cellPage = new QFrame();
    main->addPage(cellPage, QString("Cell"));
    QVBoxLayout *cellLayout = new QVBoxLayout(cellPage);
    d->cellView = new QTreeWidget(cellPage);
    cellLayout->addWidget(d->cellView);
    d->cellView->setHeaderLabels(QStringList() << "Key"
                                               << "Value");

    QFrame *stylePage = new QFrame();
    main->addPage(stylePage, QString("Style"));
    QVBoxLayout *styleLayout = new QVBoxLayout(stylePage);
    d->styleView = new QTreeWidget(stylePage);
    styleLayout->addWidget(d->styleView);
    d->styleView->setHeaderLabels(QStringList() << "Key"
                                                << "Value");

    QFrame *sheetPage = new QFrame();
    main->addPage(sheetPage, QString("Sheet"));
    QVBoxLayout *sheetLayout = new QVBoxLayout(sheetPage);
    d->sheetView = new QTreeWidget(sheetPage);
    sheetLayout->addWidget(d->sheetView);
    d->sheetView->setHeaderLabels(QStringList() << "Key"
                                                << "Value");

    QFrame *depPage = new QFrame();
    main->addPage(depPage, QString("Dependencies"));
    QVBoxLayout *depLayout = new QVBoxLayout(depPage);
    d->depView = new QTreeWidget(depPage);
    depLayout->addWidget(d->depView);
    d->depView->setHeaderLabels(QStringList() << "Cell"
                                              << "Content");

    resize(800, 500);
}

InspectorDialog::~InspectorDialog()
{
    delete d;
}

void InspectorDialog::setCell(const Cell &cell)
{
    d->cell = cell;
    d->style = cell.style();
    d->sheet = cell.fullSheet();

    d->handleCell();
    d->handleSheet();
    d->handleStyle();
    d->handleDep();
}
