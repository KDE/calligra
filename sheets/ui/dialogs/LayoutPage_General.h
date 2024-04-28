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

#ifndef CALLIGRA_SHEETS_LAYOUT_PAGE_GENERAL
#define CALLIGRA_SHEETS_LAYOUT_PAGE_GENERAL

#include <QWidget>

class KComboBox;
class KLineEdit;
class QLabel;

namespace Calligra
{
namespace Sheets
{

class CustomStyle;
class StyleManager;

/**
 * \ingroup UI
 * Dialog page to select the cell style inheritance.
 * Only shown when modifying a named cell style.
 */
class LayoutPageGeneral : public QWidget
{
    Q_OBJECT

public:
    LayoutPageGeneral(QWidget *parent, StyleManager *manager);
    ~LayoutPageGeneral() override;

    bool apply(CustomStyle *style);
    void loadFrom(const CustomStyle &style);

Q_SIGNALS:
    void validDataChanged(bool ok);

protected Q_SLOTS:
    void parentChanged(const QString &);
    void styleNameChanged(const QString &);

private:
    KComboBox *m_parentBox;
    KLineEdit *m_nameEdit;
    QLabel *m_nameStatus;
    QLabel *m_parentStatus;

    QString m_name;
    QString m_parent;
    StyleManager *m_manager;

    bool checkParent(const QString &parentName);
    bool checkName();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_PAGE_GENERAL
