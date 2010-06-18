/* This file is part of the KDE project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef SHEET_PROPERTIES_DIALOG
#define SHEET_PROPERTIES_DIALOG

#include <QWidget>

#include <kdialog.h>

#include <ui_SheetPropertiesWidget.h>

namespace KSpread
{

class SheetPropertiesWidget : public QWidget, public ::Ui::SheetPropertiesWidget
{
public:
    explicit SheetPropertiesWidget(QWidget* parent) : QWidget(parent) {
        setupUi(this);
    }
};

/**
 * \ingroup UI
 * Dialog to set sheet properties.
 */
class SheetPropertiesDialog : public KDialog
{
    Q_OBJECT

public:

    SheetPropertiesDialog(QWidget* parent = 0);

    ~SheetPropertiesDialog();


    Qt::LayoutDirection layoutDirection() const;

    void setLayoutDirection(Qt::LayoutDirection dir);

    bool autoCalc() const;

    void setAutoCalculationEnabled(bool b);

    bool showGrid() const;

    void setShowGrid(bool b);

    bool showPageBorders() const;

    void setShowPageBorders(bool b);

    bool showFormula() const;

    void setShowFormula(bool b);

    bool hideZero() const;

    void setHideZero(bool b);

    bool showFormulaIndicator() const;

    void setShowFormulaIndicator(bool b);

    bool showCommentIndicator() const;

    void setShowCommentIndicator(bool b);

    bool columnAsNumber() const;

    void setColumnAsNumber(bool b);

    bool lcMode() const;

    void setLcMode(bool b);

    bool capitalizeFirstLetter() const;

    void setCapitalizeFirstLetter(bool b);
public slots:
    virtual void slotDefault();
private:
    SheetPropertiesWidget* m_widget;
};

} // namespace

#endif /* SHEET_PROPERTIES_DIALOG */
