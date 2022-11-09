/*
  SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "autocorrection_export.h"

#include "spellchecklineedit.h"

class AutoCorrection;
class LineEditWithAutoCorrectionPrivate;
/**
 * @brief The LineEditWithAutoCorrection class
 * @author Laurent Montel <montel@kde.org>
 */
class AUTOCORRECTION_EXPORT LineEditWithAutoCorrection : public SpellCheckLineEdit
{
    Q_OBJECT
public:
    explicit LineEditWithAutoCorrection(QWidget *parent, const QString &configFile);
    ~LineEditWithAutoCorrection() override;

    Q_REQUIRED_RESULT AutoCorrection *autocorrection() const;

    void setAutocorrection(AutoCorrection *autocorrect);
    void setAutocorrectionLanguage(const QString &language);

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    std::unique_ptr<LineEditWithAutoCorrectionPrivate> const d;
};
