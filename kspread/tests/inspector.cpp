/* This file is part of the KDE project
   Copyright 2005 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "inspector.h"

// Qt
#include <QFrame>
#include <QLayout>
#include <QTextStream>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QPen>

#include <kpagedialog.h>

// KSpread
#include "Cell.h"
#include "DependencyManager.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Style.h"
#include "Value.h"

namespace KSpread
{

class Inspector::Private
{
public:
    Cell cell;
    Style style;
    Sheet* sheet;

    QTreeWidget *cellView;
    QTreeWidget *sheetView;
    QTreeWidget *styleView;
    QTreeWidget* depView;

    void handleCell();
    void handleSheet();
    void handleStyle();
    void handleDep();
};

}

using namespace KSpread;

static QString boolAsString(bool b)
{
    if (b) return QString("True");
    else return QString("False");
}

#if 0
static QString longAsHexstring(long l)
{
    return QString("%1").arg(l, 8, 16);
}
#endif

static QString dirAsString(Qt::LayoutDirection dir)
{
    QString str;
    switch (dir) {
    case Qt::LeftToRight: str = "Left to Right"; break;
    case Qt::RightToLeft: str = "Right to Left"; break;
    default: str = "Unknown"; break;
    }
    return str;
}

void Inspector::Private::handleCell()
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
//   new QTreeWidgetItem( cellView, QStringList() << "Format Properties" << longAsHexstring( static_cast<long>( cell.style()->propertiesMask() ) ) );
//   new QTreeWidgetItem( cellView, QStringList() << "Style Properties" << longAsHexstring( static_cast<long>( cell.style()->style()->features() ) ) );
    new QTreeWidgetItem(cellView, QStringList() << "Text" << cell.userInput());
    new QTreeWidgetItem(cellView, QStringList() << "Text (Displayed)" <<
                        cell.displayText().replace(QChar('\n'), "\\n"));

    QTextStream ts(&str, QIODevice::WriteOnly);
    ts << cell.value();
    new QTreeWidgetItem(cellView, QStringList() << "Value" << str);

    new QTreeWidgetItem(cellView, QStringList() << "Link" << cell.link());

    new QTreeWidgetItem(cellView, QStringList() << "Width" << QString::number(cell.width()));
    new QTreeWidgetItem(cellView, QStringList() << "Height" << QString::number(cell.height()));
}

void Inspector::Private::handleStyle() // direct style access
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

    QTreeWidgetItem* flags = new QTreeWidgetItem(styleView, QStringList("Flags"));
    new QTreeWidgetItem(flags, QStringList() << "Border (left)" <<
                        boolAsString(style.hasAttribute(Style::LeftPen)));
    new QTreeWidgetItem(flags, QStringList() << "Border (right)" <<
                        boolAsString(style.hasAttribute(Style::RightPen)));
    new QTreeWidgetItem(flags, QStringList() << "Border (top)" <<
                        boolAsString(style.hasAttribute(Style::TopPen)));
    new QTreeWidgetItem(flags, QStringList() << "Border (bottom)" <<
                        boolAsString(style.hasAttribute(Style::BottomPen)));

    new QTreeWidgetItem(styleView, QStringList() << "Border pen width (bottom)" <<
                        QString::number(style.bottomBorderPen().width()));
}

void Inspector::Private::handleSheet()
{
    sheetView->clear();

    new QTreeWidgetItem(sheetView, QStringList() << "Name" << sheet->sheetName()) ;
    new QTreeWidgetItem(sheetView, QStringList() << "Layout Direction" << dirAsString(sheet->layoutDirection()));
}

void Inspector::Private::handleDep()
{
    DependencyManager* manager = sheet->map()->dependencyManager();
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

Inspector::Inspector(const Cell& cell)
        : KPageDialog()
        , d(new Private)
{
    setFaceType(Tabbed);
    setCaption("Inspector");
    setButtons(Close);
    setDefaultButton(Close);

    d->cell = cell;
    d->style = cell.style();
    d->sheet = cell.sheet();

    QFrame* cellPage = new QFrame();
    addPage(cellPage, QString("Cell"));
    QVBoxLayout* cellLayout = new QVBoxLayout(cellPage);
    cellLayout->setMargin(KDialog::marginHint());
    cellLayout->setSpacing(KDialog::spacingHint());
    d->cellView = new QTreeWidget(cellPage);
    cellLayout->addWidget(d->cellView);
    d->cellView->setHeaderLabels(QStringList() << "Key" << "Value");

    QFrame* stylePage = new QFrame();
    addPage(stylePage, QString("Style"));
    QVBoxLayout* styleLayout = new QVBoxLayout(stylePage);
    d->styleView = new QTreeWidget(stylePage);
    styleLayout->addWidget(d->styleView);
    d->styleView->setHeaderLabels(QStringList() << "Key" << "Value");

    QFrame* sheetPage = new QFrame();
    addPage(sheetPage,  QString("Sheet"));
    QVBoxLayout* sheetLayout = new QVBoxLayout(sheetPage);
    sheetLayout->setMargin(KDialog::marginHint());
    sheetLayout->setSpacing(KDialog::spacingHint());
    d->sheetView = new QTreeWidget(sheetPage);
    sheetLayout->addWidget(d->sheetView);
    d->sheetView->setHeaderLabels(QStringList() << "Key" << "Value");

    QFrame* depPage = new QFrame();
    addPage(depPage,  QString("Dependencies"));
    QVBoxLayout* depLayout = new QVBoxLayout(depPage);
    depLayout->setMargin(KDialog::marginHint());
    depLayout->setSpacing(KDialog::spacingHint());
    d->depView = new QTreeWidget(depPage);
    depLayout->addWidget(d->depView);
    d->depView->setHeaderLabels(QStringList() << "Cell" << "Content");

    d->handleCell();
    d->handleSheet();
    d->handleStyle();
    d->handleDep();

    resize(350, 400);
}

Inspector::~Inspector()
{
    delete d;
}

#include "inspector.moc"


