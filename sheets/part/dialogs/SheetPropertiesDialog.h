/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHEET_PROPERTIES_DIALOG
#define SHEET_PROPERTIES_DIALOG

#include <KoDialog.h>

#include <ui_SheetPropertiesWidget.h>

namespace Calligra
{
namespace Sheets
{

class SheetPropertiesWidget : public QWidget, public ::Ui::SheetPropertiesWidget
{
public:
    explicit SheetPropertiesWidget(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

/**
 * \ingroup UI
 * Dialog to set sheet properties.
 */
class SheetPropertiesDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit SheetPropertiesDialog(QWidget *parent = nullptr);

    ~SheetPropertiesDialog() override;

    Qt::LayoutDirection layoutDirection() const;

    void setLayoutDirection(Qt::LayoutDirection dir);

    bool autoCalc() const;

    void setAutoCalculationEnabled(bool b);

    bool showGrid() const;

    void setShowGrid(bool b);

    bool showPageOutline() const;

    void setShowPageOutline(bool b);

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
public Q_SLOTS:
    virtual void slotDefault();

private:
    SheetPropertiesWidget *m_widget;
};

} // namespace Sheets
} // namespace Calligra

#endif /* SHEET_PROPERTIES_DIALOG */
