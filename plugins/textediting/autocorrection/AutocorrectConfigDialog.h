/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef AUTOCORRECTCONFIGDIALOG_H
#define AUTOCORRECTCONFIGDIALOG_H

#include <ui_AutocorrectConfig.h>
#include "Autocorrect.h"

#include <KoDialog.h>

class KCharSelect;

class AutocorrectConfig : public QWidget
{
    Q_OBJECT
public:
    AutocorrectConfig(Autocorrect *autocorrect, QWidget *parent);
    ~AutocorrectConfig() override;

public Q_SLOTS:
    void applyConfig();

private Q_SLOTS:
    /* tab 2 */
    void enableSingleQuotes(int state);
    void enableDoubleQuotes(int state);
    void selectSingleQuoteCharOpen();
    void selectSingleQuoteCharClose();
    void setDefaultSingleQuotes();
    void selectDoubleQuoteCharOpen();
    void selectDoubleQuoteCharClose();
    void setDefaultDoubleQuotes();

    /* tab 3 */
    void enableAdvAutocorrection(int state);
    void enableAutocorrectFormat(int state);
    void addAutocorrectEntry();
    void removeAutocorrectEntry();
    void setFindReplaceText(int row, int column);
    void enableAddRemoveButton();
    void changeCharFormat();

    /* tab 4 */
    void abbreviationChanged(const QString &text);
    void twoUpperLetterChanged(const QString &text);
    void addAbbreviationEntry();
    void removeAbbreviationEntry();
    void addTwoUpperLetterEntry();
    void removeTwoUpperLetterEntry();

private:
    Ui::AutocorrectConfig widget;
    Autocorrect *m_autocorrect;
    Autocorrect::TypographicQuotes m_singleQuotes;
    Autocorrect::TypographicQuotes m_doubleQuotes;
    QSet<QString> m_upperCaseExceptions;
    QSet<QString> m_twoUpperLetterExceptions;
    QHash<QString, QString> m_autocorrectEntries;
};

class AutocorrectConfigDialog : public KoDialog
{
    Q_OBJECT

public:
    /**
     * Constructs an Autocorrect configuration dialog
     * @param autocorrect the autocorrection plugin
     * @param parent the parent widget
     */
    explicit AutocorrectConfigDialog(Autocorrect *autocorret, QWidget *parent = 0);

    /**
     * Destructor
     */
    ~AutocorrectConfigDialog() override;

private:
    AutocorrectConfig *ui;
};

class CharSelectDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit CharSelectDialog(QWidget *parent);
    QChar currentChar() const;
    void setCurrentChar(const QChar &c);

private:
    KCharSelect *m_charSelect;
};

#endif

