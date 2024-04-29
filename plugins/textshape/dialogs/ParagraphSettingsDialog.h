/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PARAGRAPHSETTINGSDIALOG_H
#define PARAGRAPHSETTINGSDIALOG_H

#include <KoTextEditor.h>

#include <KoDialog.h>

class TextTool;
class ParagraphGeneral;
class KoImageCollection;

class KoUnit;

/// A dialog to show the settings for a paragraph
class ParagraphSettingsDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit ParagraphSettingsDialog(TextTool *tool, KoTextEditor *editor, QWidget *parent = nullptr);
    ~ParagraphSettingsDialog() override;

    void setUnit(const KoUnit &unit);

    void setImageCollection(KoImageCollection *imageCollection);

protected Q_SLOTS:
    void styleChanged(bool state = true);

    void slotApply();
    void slotOk();

private:
    void initTabs();

    ParagraphGeneral *m_paragraphGeneral;
    TextTool *m_tool;
    KoTextEditor *m_editor;
    bool m_uniqueFormat;
    bool m_styleChanged;
};

#endif
