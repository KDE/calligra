/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEOFCONTENTSCONFIGURE_H
#define TABLEOFCONTENTSCONFIGURE_H

#include "ui_TableOfContentsConfigure.h"

#include <KoZoomHandler.h>

#include <QDialog>
#include <QTextBlock>

namespace Ui
{
class TableOfContentsConfigure;
}

class QTextBlock;
class TableOfContentsStyleConfigure;
class TableOfContentsEntryModel;
class TableOfContentsEntryDelegate;
class KoTableOfContentsGeneratorInfo;
class KoTextEditor;

class TableOfContentsConfigure : public QDialog
{
    Q_OBJECT

public:
    explicit TableOfContentsConfigure(KoTextEditor *editor, QTextBlock block, QWidget *parent = nullptr);
    TableOfContentsConfigure(KoTextEditor *editor, KoTableOfContentsGeneratorInfo *info, QWidget *parent = nullptr);
    ~TableOfContentsConfigure() override;
    KoTableOfContentsGeneratorInfo *currentToCData();

public Q_SLOTS:
    void setDisplay();
    void save();
    void cleanUp();
    void updatePreview();

private Q_SLOTS:
    void showStyleConfiguration();
    void titleTextChanged(const QString &text);
    void useOutline(int state);
    void useIndexSourceStyles(int state);

private:
    Ui::TableOfContentsConfigure ui;
    KoTextEditor *m_textEditor;
    TableOfContentsStyleConfigure *m_tocStyleConfigure;
    KoTableOfContentsGeneratorInfo *m_tocInfo;
    QTextBlock m_block;
    QTextDocument *m_document;
    TableOfContentsEntryModel *m_tocEntryStyleModel;
    TableOfContentsEntryDelegate *m_tocEntryConfigureDelegate;

    void init();
};

#endif // TABLEOFCONTENTSCONFIGURE_H
