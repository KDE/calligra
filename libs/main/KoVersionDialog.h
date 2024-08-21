/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __VERSION_DIALOG__
#define __VERSION_DIALOG__

#include <KoDialog.h>

#include "KoDocument.h"

class QPushButton;
class QTreeWidget;
class QTextEdit;

class KoVersionDialog : public KoDialog
{
    Q_OBJECT
public:
    KoVersionDialog(QWidget *parent, KoDocument *doc);
    ~KoVersionDialog() override;

public Q_SLOTS:
    void slotRemove();
    void slotAdd();
    void slotOpen();
    void slotModify();

protected:
    void init();
    void updateButton();
    void updateVersionList();

    QTreeWidget *list;
    QPushButton *m_pRemove;
    QPushButton *m_pAdd;
    QPushButton *m_pOpen;
    QPushButton *m_pModify;
    KoDocument *m_doc;
};

class KoVersionModifyDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit KoVersionModifyDialog(QWidget *parent, KoVersionInfo *info = nullptr);

    QString comment() const;

private:
    QTextEdit *m_textEdit;
};

#endif
