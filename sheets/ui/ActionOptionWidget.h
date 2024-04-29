/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTIONOPTIONWIDGET_H
#define CALLIGRA_SHEETS_ACTIONOPTIONWIDGET_H

#include <QWidget>

class QDomElement;

namespace Calligra
{
namespace Sheets
{
class CellToolBase;

class ActionOptionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActionOptionWidget(CellToolBase *cellTool, const QDomElement &e, QWidget *parent = nullptr);

private:
};

}
}

#endif // CALLIGRA_SHEETS_ACTIONOPTIONWIDGET_H
