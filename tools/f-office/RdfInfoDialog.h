/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sugnan Prabhu S <sugnan.prabhu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef RDFINFODIALOG_H
#define RDFINFODIALOG_H

#include <QDialog>
#include <QPushButton>
#include <Soprano/Soprano>
#include <Soprano/Statement>
#include <Soprano/Model>

class KoStore;
class QGridLayout;
class QVBoxLayout;

struct Details
{
    QString name;
    QString nick;
    QString phone;
    QString homePage;

    QString summary;
    QString location;
    QString taskStart;
    QString taskEnd;

    QString longitude;
    QString latitude;
};

class RdfInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RdfInfoDialog(QWidget *parent = 0);
    void setData(struct Details details);
    QList<QPushButton *> detailsButton;

    struct Details details;

private:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;

private slots:
    void doSomething();
};

#endif // RDFINFODIALOG_H
