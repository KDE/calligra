/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef DATEVARIABLE_H
#define DATEVARIABLE_H

#include <KoVariable.h>

#include <QDateTime>
#include <QString>

/**
 * Base class for in-text variables.
 * A variable is a field inserted into the text and the content is set to a specific value that
 * is used as text.  This class is pretty boring in that it has just a setValue() to alter the
 * text shown; we depend on plugin writers to create more exciting ways to update variables.
 */
class DateVariable : public KoVariable
{
public:
    enum DateType { Fixed, AutoUpdate };

    enum DisplayType { Date, Time };

    enum ValueType { DateOrTime, DateTime };

    /**
     * Constructor.
     */
    explicit DateVariable(DateType type);
    ~DateVariable() override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) override;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    void readProperties(const KoProperties *props);

    QWidget *createOptionsWidget() override;

    const QString &definition() const
    {
        return m_definition;
    }
    void setDefinition(const QString &definition);

    int daysOffset() const
    {
        return m_daysOffset;
    }
    void setDaysOffset(int daysOffset);

    int monthsOffset() const
    {
        return m_monthsOffset;
    }
    void setMonthsOffset(int monthsOffset);

    int yearsOffset() const
    {
        return m_yearsOffset;
    }
    void setYearsOffset(int yearsOffset);

    int secsOffset() const
    {
        return m_secsOffset;
    }
    void setSecsOffset(int secsOffset);

private:
    void update();
    void adjustTime(const QString &adjustTime);

    DateType m_type;
    DisplayType m_displayType;
    ValueType m_valueType;
    QString m_definition;
    QDateTime m_datetime;
    int m_daysOffset;
    int m_monthsOffset;
    int m_yearsOffset;
    int m_secsOffset;
};

#endif
