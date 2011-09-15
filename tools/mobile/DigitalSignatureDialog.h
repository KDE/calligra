/*
 * This file is part of Maemo 5 Office UI for Calligra
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

#ifndef DIGITALSIGNATUREDIALOG_H
#define DIGITALSIGNATUREDIALOG_H

#include <QDialog>

class QDomNode;
class QVBoxLayout;
class QFrame;

struct SignerInfo
{
    QString email;
    QString country;
    QString state;
    QString organization;
    QString signerName;
};

class DigitalSignatureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DigitalSignatureDialog(QString filePath,QWidget *parent = 0);
    ~DigitalSignatureDialog();
    bool verifySignature();
    void initializeDialog();

private:
    QString filePath;
    QList<struct SignerInfo> signerInfo;

    QVBoxLayout *VLmain;
    QFrame *signerInfoFrame;
    QVBoxLayout *VLsignerInfoFrame;
    QList<QPushButton *> signerListButton;

    QFrame *signerFrame;
    QVBoxLayout *VLsignerFrame;
    QPushButton *signerName;
    QPushButton *countryButton;
    QPushButton *organizationButton;
    QPushButton *emailButton;

    bool validSignature;

private:
    void printChild(QDomNode element);
    void signatureInfoRetriever(QDomNode element);
private slots:
    void sendMail();
    void showSignerInformation();
};

#endif // DIGITALSIGNATUREDIALOG_H
