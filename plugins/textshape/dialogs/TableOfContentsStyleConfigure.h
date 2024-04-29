/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEOFCONTENTSSTYLECONFIGURE_H
#define TABLEOFCONTENTSSTYLECONFIGURE_H

#include "TableOfContentsStyleDelegate.h"
#include "TableOfContentsStyleModel.h"

#include <QDialog>

namespace Ui
{
class TableOfContentsStyleConfigure;
}

class QStandardItemModel;
class KoStyleManager;
class TableOfContentsStyleModel;
class KoTableOfContentsGeneratorInfo;

class TableOfContentsStyleConfigure : public QDialog
{
    Q_OBJECT

public:
    explicit TableOfContentsStyleConfigure(KoStyleManager *manager, QWidget *parent = nullptr);
    ~TableOfContentsStyleConfigure() override;
    void initializeUi(KoTableOfContentsGeneratorInfo *info);

public Q_SLOTS:
    void save();
    void discardChanges();

private:
    Ui::TableOfContentsStyleConfigure *ui;
    QStandardItemModel *m_stylesTree;
    KoStyleManager *m_styleManager;
    KoTableOfContentsGeneratorInfo *m_tocInfo;
    TableOfContentsStyleModel *m_stylesModel;
    TableOfContentsStyleDelegate m_delegate;
};

#endif // TABLEOFCONTENTSSTYLECONFIGURE_H
