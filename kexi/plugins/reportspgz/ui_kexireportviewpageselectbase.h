#include <kdialog.h>
#include <klocale.h>

/********************************************************************************
** Form generated from reading ui file 'kexireportviewpageselectbase.ui'
**
** Created: Wed Jan 23 19:21:17 2008
**      by: Qt User Interface Compiler version 4.3.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KEXIREPORTVIEWPAGESELECTBASE_H
#define UI_KEXIREPORTVIEWPAGESELECTBASE_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include "klineedit.h"
#include "kpushbutton.h"

class Ui_KexiReportViewPageSelectBase
{
public:
    QHBoxLayout *hboxLayout;
    KPushButton *btnFirst;
    KPushButton *btnPrev;
    KLineEdit *txtPage;
    KPushButton *btnNext;
    KPushButton *btnLast;
    QLabel *lblStatus;
    KPushButton *btnPrint;

    void setupUi(QWidget *KexiReportViewPageSelectBase)
    {
    if (KexiReportViewPageSelectBase->objectName().isEmpty())
        KexiReportViewPageSelectBase->setObjectName(QString::fromUtf8("KexiReportViewPageSelectBase"));
    KexiReportViewPageSelectBase->resize(596, 23);
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(KexiReportViewPageSelectBase->sizePolicy().hasHeightForWidth());
    KexiReportViewPageSelectBase->setSizePolicy(sizePolicy);
    hboxLayout = new QHBoxLayout(KexiReportViewPageSelectBase);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    btnFirst = new KPushButton(KexiReportViewPageSelectBase);
    btnFirst->setObjectName(QString::fromUtf8("btnFirst"));

    hboxLayout->addWidget(btnFirst);

    btnPrev = new KPushButton(KexiReportViewPageSelectBase);
    btnPrev->setObjectName(QString::fromUtf8("btnPrev"));

    hboxLayout->addWidget(btnPrev);

    txtPage = new KLineEdit(KexiReportViewPageSelectBase);
    txtPage->setObjectName(QString::fromUtf8("txtPage"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(txtPage->sizePolicy().hasHeightForWidth());
    txtPage->setSizePolicy(sizePolicy1);

    hboxLayout->addWidget(txtPage);

    btnNext = new KPushButton(KexiReportViewPageSelectBase);
    btnNext->setObjectName(QString::fromUtf8("btnNext"));

    hboxLayout->addWidget(btnNext);

    btnLast = new KPushButton(KexiReportViewPageSelectBase);
    btnLast->setObjectName(QString::fromUtf8("btnLast"));

    hboxLayout->addWidget(btnLast);

    lblStatus = new QLabel(KexiReportViewPageSelectBase);
    lblStatus->setObjectName(QString::fromUtf8("lblStatus"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(5));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(lblStatus->sizePolicy().hasHeightForWidth());
    lblStatus->setSizePolicy(sizePolicy2);
    lblStatus->setWordWrap(false);

    hboxLayout->addWidget(lblStatus);

    btnPrint = new KPushButton(KexiReportViewPageSelectBase);
    btnPrint->setObjectName(QString::fromUtf8("btnPrint"));

    hboxLayout->addWidget(btnPrint);


    retranslateUi(KexiReportViewPageSelectBase);
    QObject::connect(btnNext, SIGNAL(clicked()), KexiReportViewPageSelectBase, SLOT(btnNext_clicked()));
    QObject::connect(btnLast, SIGNAL(clicked()), KexiReportViewPageSelectBase, SLOT(btnLast_clicked()));
    QObject::connect(btnPrev, SIGNAL(clicked()), KexiReportViewPageSelectBase, SLOT(btnPrev_clicked()));
    QObject::connect(btnFirst, SIGNAL(clicked()), KexiReportViewPageSelectBase, SLOT(btnFirst_clicked()));
    QObject::connect(btnPrint, SIGNAL(clicked()), KexiReportViewPageSelectBase, SLOT(btnPrint_clicked()));

    QMetaObject::connectSlotsByName(KexiReportViewPageSelectBase);
    } // setupUi

    void retranslateUi(QWidget *KexiReportViewPageSelectBase)
    {
    KexiReportViewPageSelectBase->setWindowTitle(tr2i18n("KexiReportViewPageSelect", 0));
    btnFirst->setText(QString());
    btnPrev->setText(QString());
    btnNext->setText(QString());
    btnLast->setText(QString());
    lblStatus->setText(QString());
    btnPrint->setText(QString());
    Q_UNUSED(KexiReportViewPageSelectBase);
    } // retranslateUi

};

namespace Ui {
    class KexiReportViewPageSelectBase: public Ui_KexiReportViewPageSelectBase {};
} // namespace Ui

#endif // KEXIREPORTVIEWPAGESELECTBASE_H

