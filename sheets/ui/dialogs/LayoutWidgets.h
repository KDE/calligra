/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LAYOUT_WIDGETS
#define CALLIGRA_SHEETS_LAYOUT_WIDGETS

#include <QCheckBox>
#include <QWidget>

#include "Style.h"

namespace Calligra
{
namespace Sheets
{

// If partial, we're setting if the value changed
// If not partial, we're setting if the value is not the default one
inline bool shouldApplyLayoutChange(bool isDefault, bool changed, bool partial)
{
    if (partial)
        return changed;
    return (!isDefault);
}

class LayoutCheckbox : public QCheckBox
{
public:
    LayoutCheckbox(QWidget *parent = nullptr);
    virtual ~LayoutCheckbox();

    void setStyleKey(const Style::Key &key, bool defaultValue = false);
    void loadFrom(const Style &style, bool partial);
    void saveTo(Style &style, bool partial);
    void setReversed(bool rev)
    {
        m_reversed = rev;
    }

protected:
    Style::Key m_styleKey;
    bool m_defaultValue;
    bool m_reversed;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_WIDGETS
