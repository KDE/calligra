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

#include "DigitalSignatureDialog.h"

#include <KoStore.h>
#include<QFile>
#include<QXmlStreamAttributes>
#include<QDomDocument>
#include<QCryptographicHash>
#include<QIcon>
#include<QPushButton>
#include<QGridLayout>
#include<QMessageBox>
#include<QFrame>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/c14n.h>
#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif
#include <QProcess>

static QString runc14(QIODevice *ioDevice, int with_comments, int exclusive,
                      const char* xpath_filename, xmlChar **inclusive_namespaces) {
    xmlDocPtr doc;
    xmlXPathObjectPtr xpath = 0;
    xmlChar *result = 0;
    int ret;
    QString sha1Hash;
    /*
     * build an XML tree from a the file; we need to add default
     * attributes and resolve all character and entities references
     */
    xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);
    /*
     * Do not fetch DTD over network
     */
    xmlSetExternalEntityLoader(xmlNoNetExternalEntityLoader);
    xmlLoadExtDtdDefaultValue = 0;

    QByteArray temp=ioDevice->readAll();
    doc = xmlParseMemory(temp.data(),temp.length());

    if (doc == 0) {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, QString("Error: unable to parse file \n"));
#endif
        return "";
    }

    /*
     * Check the document is of the right kind
     */
    if(xmlDocGetRootElement(doc) == 0) {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, QString("Error: empty document for file \n"));
#endif
        xmlFreeDoc(doc);
        return "";
    }
    /*
     * load xpath file if specified
     */
    if(xpath_filename) {
        if(xpath == 0) {
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, QString("Error: unable to evaluate xpath expression\n"));
#endif
            xmlFreeDoc(doc);
            return "";
        }
    }

    /*
     * Canonical form
     */
    ret = xmlC14NDocDumpMemory(doc,
                               (xpath) ? xpath->nodesetval : 0,
                               exclusive, inclusive_namespaces,
                               with_comments, &result);
    if(ret >= 0) {
        if(result != 0) {
            sha1Hash=QString((QCryptographicHash::hash(QByteArray((char *)result),QCryptographicHash::Sha1)).toBase64());
            fflush(0);
            xmlFree(result);
        }
    } else {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, QString("Error: failed to canonicalize XML file"));
#endif
        if(result != 0) xmlFree(result);
        xmlFreeDoc(doc);
        return "";
    }

    /*
     * Cleanup
     */
    if(xpath != 0)
        xmlXPathFreeObject(xpath);

    xmlFreeDoc(doc);

    return sha1Hash;
}

DigitalSignatureDialog::DigitalSignatureDialog(QString filePath,QWidget *parent) :
        QDialog(parent)
{
    VLmain = new QVBoxLayout(this);
    VLmain->setSpacing(6);
    VLmain->setContentsMargins(11, 11, 11, 11);

    signerInfoFrame = new QFrame(this);
    VLsignerInfoFrame = new QVBoxLayout(signerInfoFrame);
    VLsignerInfoFrame->setSpacing(6);
    VLsignerInfoFrame->setContentsMargins(11, 11, 11, 11);


    VLmain->addWidget(signerInfoFrame);

    signerFrame = new QFrame(this);
    VLsignerFrame = new QVBoxLayout(signerFrame);
    VLsignerFrame->setSpacing(6);
    VLsignerFrame->setContentsMargins(11, 11, 11, 11);
    signerName = new QPushButton(signerFrame);

    VLsignerFrame->addWidget(signerName);

    countryButton = new QPushButton(signerFrame);

    VLsignerFrame->addWidget(countryButton);

    organizationButton = new QPushButton(signerFrame);

    VLsignerFrame->addWidget(organizationButton);

    emailButton = new QPushButton(signerFrame);

    VLsignerFrame->addWidget(emailButton);


    VLmain->addWidget(signerFrame);

    this->filePath=filePath;
    this->setWindowTitle("Signer Information");

#ifdef Q_WS_MAEMO_5
    this->emailButton->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/64x64/hildon/general_email.png")));
    this->countryButton->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/64x64/hildon/tasklaunch_map.png")));
    this->signerName->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/64x64/hildon/general_certificate.png")));
    this->organizationButton->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/96x96/hildon/general_conference_avatar.png")));
    connect(this->emailButton,SIGNAL(clicked()),this,SLOT(sendMail()));
#endif
    validSignature=true;
}

DigitalSignatureDialog::~DigitalSignatureDialog()
{

}


void DigitalSignatureDialog::printChild(QDomNode element)
{
    if(element.isElement()) {
        if(element.toElement().tagName()=="SignatureValue") {
            return;
        }

        if(element.toElement().tagName()=="Reference") {
            KoStore::Backend backend = KoStore::Auto;
            KoStore * store = KoStore::createStore(this->filePath, KoStore::Read, "", backend);

            if(store->open(element.toElement().attribute("URI"))) {

                QDomNode node=element.firstChild();
                QString sha1Hash;

                if(node.toElement().tagName()=="Transforms") {
                    sha1Hash=runc14(store->device(), 1, 0, 0, 0);
                    node=node.nextSibling();
                }
                else
                    sha1Hash=QString((QCryptographicHash::hash(store->device()->readAll(),QCryptographicHash::Sha1)).toBase64());

                node.nextSibling();

                if(element.toElement().text()!=sha1Hash){
                    validSignature=false;
                    return;
                }

                node.nextSibling();
                store->close();
            }
            delete store;
        }
        printChild(element.nextSibling());

        return;
    }
}

void DigitalSignatureDialog::signatureInfoRetriever(QDomNode element)
{
    if(element.isElement()) {
        if(element.toElement().tagName()=="X509IssuerName"){
            QStringList signerDetails=element.toElement().text().split(",");
            struct SignerInfo signer;
            foreach(QString info,signerDetails){
                if(info.startsWith("E=")){
                    signer.email=info.mid(info.indexOf("=")+1);
                }

                if(info.startsWith("C=") || info.startsWith("ST=")){
                    signer.country=info.mid(info.indexOf("=")+1)+", "+signer.country;
                }

                if(info.startsWith("CN=")){
                    signer.signerName=info.mid(info.indexOf("=")+1);
                }
                if(info.startsWith("O=")){
                    signer.organization=info.mid(info.indexOf("=")+1);
                }
            }
            signerInfo.append(signer);
        }

        if(element.hasChildNodes())
            signatureInfoRetriever(element.firstChild());
        signatureInfoRetriever(element.nextSibling());
    }
}


bool DigitalSignatureDialog::verifySignature()
{
    if(filePath.isNull())
        return false;


    KoStore::Backend backend = KoStore::Auto;
    KoStore * store = KoStore::createStore(filePath, KoStore::Read, "", backend);

    if(store->open("META-INF/documentsignatures.xml")) {
        QDomDocument doc("mydocument");

        if (!doc.setContent(store->device())) {
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, QString("Unable to Process the file"));
#endif
            return false;
        }
        printChild(doc.documentElement().firstChild().firstChild().firstChild());
        if(validSignature==true) {
            signatureInfoRetriever(doc.documentElement().firstChild());
            QString signerNameString=signerInfo.at(0).signerName;
            for(int i=1;i<signerInfo.length();i++){
                signerNameString+=(", "+signerInfo.at(i).signerName);
            }
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, QString("The Document is signed by "+signerNameString));
#else
                    QMessageBox msgBox;
                    msgBox.setText("The Document is signed by "+signerNameString);
                    msgBox.exec();
#endif
        }
        else{
            signatureInfoRetriever(doc.documentElement().firstChild());
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, QString("Signature has broken"),QMaemo5InformationBox::NoTimeout);
#else
                    QMessageBox msgBox;
                    msgBox.setText("Signature has broken");
                    msgBox.exec();
#endif
        }
        store->close();
    }
    else{
	delete store;
        return false;
    }
    xmlCleanupParser();
    xmlMemoryDump();

    signerFrame->hide();
    for(int i=0;i<signerInfo.length();i++){
        QPushButton *button=new QPushButton(signerInfoFrame);
        button->setText(signerInfo.at(i).signerName);
#ifdef Q_WS_MAEMO_5
        button->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/64x64/hildon/general_certificate.png")));
#endif
        button->setObjectName(QString::number(i));
        signerListButton.append(button);
        VLsignerInfoFrame->addWidget(button);
        connect(button,SIGNAL(clicked()),this,SLOT(showSignerInformation()));
    }
    delete store;
    return true;
}

void DigitalSignatureDialog::sendMail()
{
    this->accept();
    QProcess::startDetached("dbus-send --print-reply --type=method_call --dest=com.nokia.modest /com/nokia/modest com.nokia.modest.MailTo string:mailto:"+this->emailButton->text());
}

void DigitalSignatureDialog::showSignerInformation()
{
    //this->resize(400, 271);
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if(clickedButton){
        int index=clickedButton->objectName().toInt();
        signerInfoFrame->hide();
        signerName->setText(signerInfo.at(index).signerName);
        countryButton->setText(signerInfo.at(index).country);
        organizationButton->setText(signerInfo.at(index).organization);
        emailButton->setText(signerInfo.at(index).email);
        signerFrame->show();
    }
}
void DigitalSignatureDialog::initializeDialog()
{
    signerInfoFrame->show();
    signerFrame->hide();
}
