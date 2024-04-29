/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTEXTTABLETEMPLATE_H
#define KOTEXTTABLETEMPLATE_H

#include "kotext_export.h"

#include <QObject>

#include <KoXmlReaderForward.h>

class KoXmlWriter;
class KoShapeLoadingContext;
class KoTextSharedSavingData;

class KOTEXT_EXPORT KoTextTableTemplate : public QObject
{
    Q_OBJECT
public:
    enum Property { StyleId = 0, BackGround, Body, EvenColumns, EvenRows, FirstColumn, FirstRow, LastColumn, LastRow, OddColumns, OddRows };

    int background() const;
    void setBackground(int styleId);

    int body() const;
    void setBody(int styleId);

    int evenColumns() const;
    void setEvenColumns(int styleId);

    int evenRows() const;
    void setEvenRows(int styleId);

    int firstColumn() const;
    void setFirstColumn(int styleId);

    int firstRow() const;
    void setFirstRow(int styleId);

    int lastColumn() const;
    void setLastColumn(int styleId);

    int lastRow() const;
    void setLastRow(int styleId);

    int oddColumns() const;
    void setOddColumns(int styleId);

    int oddRows() const;
    void setOddRows(int styleId);

    /// Constructor
    explicit KoTextTableTemplate(QObject *parent = nullptr);

    /// Destructor
    ~KoTextTableTemplate() override;

    /// return the name of the style.
    QString name() const;

    /// set a user-visible name on the style.
    void setName(const QString &name);

    /// each style has a unique ID (non persistent) given out by the styleManager
    int styleId() const;

    /// each style has a unique ID (non persistent) given out by the styleManager
    void setStyleId(int id);

    /**
     * Load the template style from the element
     *
     * @param context the odf loading context
     * @param element the element containing the
     */
    void loadOdf(const KoXmlElement *element, KoShapeLoadingContext &context);

    /**
     * save the table-template element
     */
    void saveOdf(KoXmlWriter *writer, KoTextSharedSavingData *savingData) const;

private:
    class Private;
    Private *const d;
};

#endif // KOTEXTTABLETEMPLATE_H
