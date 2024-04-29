/*
   This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2000 Werner Trobin <trobin@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef koTemplateCreateDia_h
#define koTemplateCreateDia_h

#include "komain_export.h"
#include <KoDialog.h>

class QString;
class QPixmap;
class KoDocument;
class KoTemplateCreateDiaPrivate;

/****************************************************************************
 *
 * Class: koTemplateCreateDia
 *
 ****************************************************************************/

class KOMAIN_EXPORT KoTemplateCreateDia : public KoDialog
{
    Q_OBJECT

private:
    KoTemplateCreateDia(const QString &templatesResourcePath, const QString &filePath, const QPixmap &thumbnail, QWidget *parent = nullptr);
    ~KoTemplateCreateDia() override;

public:
    static void createTemplate(const QString &templatesResourcePath, const char *suffix, KoDocument *document, QWidget *parent = nullptr);

private Q_SLOTS:
    void slotOk();

    void slotDefault();
    void slotCustom();
    void slotSelect();
    void slotNameChanged(const QString &name);

    void slotAddGroup();
    void slotRemove();
    void slotSelectionChanged();

private:
    void updatePixmap();
    void fillGroupTree();

private:
    KoTemplateCreateDiaPrivate *const d;
};

#endif
