/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIUSERFEEDBACKAGENT_H
#define KEXIUSERFEEDBACKAGENT_H

#include <QObject>

class KJob;

//! User Feedback Agent
class KexiUserFeedbackAgent : public QObject
{
    Q_OBJECT

public:
    explicit KexiUserFeedbackAgent(QObject* parent = 0);

    ~KexiUserFeedbackAgent();

    enum Area {
        NoAreas = 0,
        BasicArea = 1,
        AnonymousIdentificationArea = 2,
        SystemInfoArea = 4,
        ScreenInfoArea = 8,
        RegionalSettingsArea = 16,
        AllAreas = 31,
    };
    Q_DECLARE_FLAGS(Areas, Area)

    void setEnabledAreas(Areas areas);

    Areas enabledAreas() const;

    QVariant value(const QString& key) const;

private slots:
    void sendDataFinished(KJob* job);
    void sendRedirectQuestionFinished(KJob* job);

private:
    void sendData();
    void sendRedirectQuestion();

    class Private;
    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiUserFeedbackAgent::Areas)

#endif
