/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2005 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FIND_DIALOG
#define CALLIGRA_SHEETS_FIND_DIALOG


#include <kfinddialog.h>
#include <QStringList>
#include <kreplacedialog.h>

class QCheckBox;
class QPushButton;
class KComboBox;

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup UI
 * Options for finding and replacing cell values.
 */
class FindOption : public QObject
{
    Q_OBJECT
public:
    explicit FindOption(QWidget *parent);
    bool searchInAllSheet() const;

    enum searchTypeValue { Value, Note};
    enum searchDirectionValue { Row, Column};

    searchTypeValue searchType() const;
    searchDirectionValue searchDirection() const;

private Q_SLOTS:
    void slotMoreOptions();

Q_SIGNALS:
    void adjustSize();

private:
    QPushButton *m_moreOptions;
    QCheckBox *m_searchInAllSheet;
    QWidget *m_findExtension;
    KComboBox *m_searchIn;
    KComboBox *m_searchDirection;
};


/**
 * \ingroup UI
 * Dialog for finding cell values.
 */
class FindDlg : public KFindDialog
{
    Q_OBJECT

public:
    explicit FindDlg(QWidget *parent = 0,
                     const QString &name = QString(),
                     long options = 0,
                     const QStringList &findStrings = QStringList(),
                     bool hasSelection = false);
    ~FindDlg() override;
    bool searchInAllSheet() const;
    FindOption::searchTypeValue searchType() const {
        return m_findOptions->searchType();
    }
    FindOption::searchDirectionValue searchDirection() const {
        return m_findOptions->searchDirection();
    }

private Q_SLOTS:
    void slotAjustSize();
private:
    FindOption *m_findOptions;
};


/**
 * \ingroup UI
 * Dialog for replacing cell values.
 */
class SearchDlg : public KReplaceDialog
{
    Q_OBJECT

public:
    explicit SearchDlg(QWidget *parent = 0,
                       const QString &name = QString(),
                       long options = 0,
                       const QStringList &findStrings = QStringList(),
                       const QStringList &replaceStrings = QStringList(),
                       bool hasSelection = false);
    ~SearchDlg() override;
    bool searchInAllSheet() const;
    FindOption::searchTypeValue searchType() const {
        return m_findOptions->searchType();
    }

private Q_SLOTS:
    void slotAjustSize();
private:
    FindOption *m_findOptions;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FIND_DIALOG
