/* This file is part of the KDE project

   SPDX-FileCopyrightText: 1999-2006 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_FUNCTION_COMPLETION
#define CALLIGRA_SHEETS_FUNCTION_COMPLETION

#include <QObject>

class QListWidgetItem;

namespace Calligra
{
namespace Sheets
{
class CellEditor;

/**
 * Provides autocompletion facilities in formula editors.
 * When the user types in the first few characters of a
 * function name in a CellEditor which has a FunctionCompletion
 * object installed on it, the FunctionCompletion object
 * creates and displays a list of possible names which the user
 * can select from. If the user selects a function name from the list,
 * the @ref FunctionCompletion::selectedCompletion() signal is emitted
 */
class FunctionCompletion : public QObject
{
    Q_OBJECT

public:
    explicit FunctionCompletion(CellEditor *editor);
    ~FunctionCompletion() override;

    /**
     * Handles various keyboard and mouse actions which may occur on the autocompletion popup list
     */
    bool eventFilter(QObject *o, QEvent *e) override;

    /**
     * Populates the autocompletion list box with the specified choices and shows it so that the user can view and select a function name.
     * @param choices A list of possible function names which match the characters that the user has already entered.
     */
    void showCompletion(const QStringList &choices);

public Q_SLOTS:
    /**
     * Hides the autocompletion list box if it is visible and emits the @ref selectedCompletion signal.
     */
    void doneCompletion();

private Q_SLOTS:
    void itemSelected(QListWidgetItem *item = nullptr);

Q_SIGNALS:
    /**
     * Emitted, if the user selects a function name from the list.
     */
    void selectedCompletion(const QString &item);

private:
    class Private;
    Private *const d;
    FunctionCompletion(const FunctionCompletion &);
    FunctionCompletion &operator=(const FunctionCompletion &);
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_FUNCTION_COMPLETION
