/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LAYOUT_DIALOG
#define CALLIGRA_SHEETS_LAYOUT_DIALOG

#include "ui/actions/dialogs/ActionDialog.h"

#include "core/Style.h"

class KPageWidgetItem;

namespace Calligra
{
namespace Sheets
{
class Localization;
class Sheet;
class StyleManager;

class LayoutPageGeneral;
class LayoutPageFloat;
class LayoutPageBorder;
class LayoutPageFont;
class LayoutPagePosition;
class LayoutPagePattern;
class LayoutPageProtection;

/**
 * \ingroup UI
 * Dialog to set the cell style.
 */
class LayoutDialog : public ActionDialog
{
    Q_OBJECT
public:
    LayoutDialog(QWidget *parent, Sheet *sheet, StyleManager *manager, bool isStyle);

    ~LayoutDialog() override;

    /** Generates a style from the dialog controls. */
    Style style(bool multicell);
    /** Generates a style from the dialog controls. */
    CustomStyle customStyle();
    /** Copies the style into the dialog controls. */
    void setStyle(const Style &style, bool multicell);
    /** Copies the style into the dialog controls. */
    void setCustomStyle(const CustomStyle &style);

    Sheet *getSheet() const
    {
        return m_sheet;
    }
    StyleManager *getStyleManager() const
    {
        return m_styleManager;
    }

    Localization *locale() const;

Q_SIGNALS:
    void applyStyle();

public Q_SLOTS:
    void setOkButtonEnabled(bool enabled);

protected:
    void init(bool isStyle);
    virtual void onApply() override;

    LayoutPageGeneral *generalPage;
    LayoutPageFloat *floatPage;
    LayoutPageBorder *borderPage;
    LayoutPageFont *fontPage;
    LayoutPagePosition *positionPage;
    LayoutPagePattern *patternPage;
    LayoutPageProtection *protectPage;

    KPageWidgetItem *generalHeader, *floatHeader;

    Sheet *m_sheet;
    StyleManager *m_styleManager;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_DIALOG
