/*
 * SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_FONT_COMBO_BOX_H_
#define _KO_FONT_COMBO_BOX_H_

#include "kowidgets_export.h"
#include <QFontComboBox>

/**
 * This class is necessary to work around a suspected bug in QFontComboBox.
 *
 * When setCurrentFont() is called, QFontComboBox seems to regenerate
 * the list of fonts and resets its model in the process, while the reset signal is blocked.
 * Anyhow, the result is that the QAccessible framework tries to use
 * an invalid index (which it does not check for validity!),
 * and this leads to a crash.
 */
class KOWIDGETS_EXPORT KoFontComboBox : public QFontComboBox
{
    Q_OBJECT
public:
    KoFontComboBox(QWidget *parent = nullptr);

public Q_SLOTS:
    void setCurrentFont(const QFont &font);
};

#endif // _KO_FONT_COMBO_BOX_H_
