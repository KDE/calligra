/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Kaushik Pendurthi <kaushiksjce@gmail.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "VirtualKeyBoard.h"

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#define POPULATELANGUAGES "/.VirtualKeyBoardLanguages/.headers"
#define FONTS   "/.fonts/"
#define HEADERS "/.VirtualKeyBoardLanguages/.headers/"
#define LISTOFAVAILABLELANGUAGESBASKET  "/.VirtualKeyBoardLanguages/ListOfAvailableLanguages.txt"
#define LISTOFAVAILABLELANGUAGES  "/.VirtualKeyBoardLanguages/"


VirtualKeyBoard::VirtualKeyBoard(QWidget *parent) :
        QMainWindow(parent)
{
    manager =new DownloadManager();
    count=0;
    showLanguage=2;
    switchForVirtualKeyBoard=false;
    switchForNextVirtualKeyBoardCharactors=false;

    QDir directory = QDir::home();
    homeDirPath = directory.path();
    QStringList files;
    QString name(".VirtualKeyBoardLanguages");
    files = directory.entryList(QStringList("*"),QDir::Hidden );
    bool filepresent=false;
    for(int i=0;i<files.count();i++) {
        if(!name.compare(files.at(i))) {
            filepresent=true;
        }
    }
    if(!filepresent) {
        if (directory.mkdir(".VirtualKeyBoardLanguages")) {
            directory.cd(".VirtualKeyBoardLanguages");
            directory.mkdir(".headers");
        }
    }
    directory=QDir::home();
    name= ".fonts";
    files = directory.entryList(QStringList("*"),QDir::Hidden );
    filepresent=false;
    for(int i=0;i<files.count();i++) {
        if(!name.compare(files.at(i))) {
            filepresent=true;
        }
    }
    if(!filepresent) {
        directory.mkdir(".fonts");
    }
}

VirtualKeyBoard::~VirtualKeyBoard()
{

}

void VirtualKeyBoard::ShowVirtualKeyBoard(MainWindow *parentOfVirtualKeyBoard,KoTextEditor *toDisplayCharactorAtPosition)
{
    switchForVirtualKeyBoard=!switchForVirtualKeyBoard;
    if(switchForVirtualKeyBoard) {

        if(showLanguage > 4 || showLanguage < 2) {
            showLanguage=2;
        }

        parentForTheNumbersFrame=parentOfVirtualKeyBoard;
        showThePosition=toDisplayCharactorAtPosition;
        for(int numberCount=0;numberCount<10;numberCount++) {
            showTheNumbers[numberCount]=new QPushButton();
        }

        returnToVirtualKeyBoard=new QPushButton();
        Q_CHECK_PTR(returnToVirtualKeyBoard);
        returnToVirtualKeyBoard->setIcon(QIcon(":/images/64x64/VirtualKeyBoard/back.png"));

        showTheNumberFrame= new QFrame(parentForTheNumbersFrame);
        Q_CHECK_PTR(showTheNumberFrame);
        showTheNumberLayout=new QHBoxLayout();
        Q_CHECK_PTR(showTheNumberLayout);
        for(int i=0;i<45;i++) {
            virtualKeyBoardButton[i]=new QPushButton();
            Q_CHECK_PTR(virtualKeyBoardButton[i]);
        }
        virtualKeyBoardLayout = new QGridLayout;
        Q_CHECK_PTR(virtualKeyBoardLayout);
        virtualKeyBoardFrame = new QFrame(parentOfVirtualKeyBoard);
        Q_CHECK_PTR(virtualKeyBoardFrame);
        virtualKeyBoardLayout = new QGridLayout;
        Q_CHECK_PTR(virtualKeyBoardLayout);
        virtualKeyBoardFrame = new QFrame(parentOfVirtualKeyBoard);
        Q_CHECK_PTR(virtualKeyBoardFrame);

        languages=new QComboBox();
        Q_CHECK_PTR(languages);
        languages->addItem(QIcon(":/images/64x64/VirtualKeyBoard/languages.png"),"+add Language");
        languages->addItem(QIcon(":/images/64x64/VirtualKeyBoard/languages.png"),"-remove language");
        languages->addItem(QIcon(":/images/64x64/VirtualKeyBoard/languages.png"),"Finnish");
        languages->addItem(QIcon(":/images/64x64/VirtualKeyBoard/languages.png"),"Arabic");
        languages->addItem(QIcon(":/images/64x64/VirtualKeyBoard/languages.png"),"Hindi");
        languages->setMaxCount(7+populateTheLanguages());

        numbers=new QPushButton();
        Q_CHECK_PTR(numbers);
        numbers->setIcon(QIcon(":/images/64x64/VirtualKeyBoard/numbers.png"));

        nextSetOfCharactors=new QPushButton();
        Q_CHECK_PTR(nextSetOfCharactors);
        nextSetOfCharactors->setIcon(QIcon(":/images/64x64/VirtualKeyBoard/change.png"));
        nextSetOfCharactors->setCheckable(true);

        // permanant languages which cannot be deleted
        // unicode for hindi charactors

        virtualKeyBoardButtonValuesHindi
                <<(QString)0x0905<<(QString)0x0906<<(QString)0x0907<<(QString)0x0908<<(QString)0x0909
                <<(QString)0x090A<<(QString)0x090B<<(QString)0x090F<<(QString)0x0910<<(QString)0x0913
                <<(QString)0x0914<<(QString)0x0915<<(QString)0x0916<<(QString)0x0917<<(QString)0x0918
                <<(QString)0x0919<<(QString)0x091A<<(QString)0x091B<<(QString)0x091C<<(QString)0x091D
                <<(QString)0x091E<<(QString)0x091F<<(QString)0x0920<<(QString)0x0921<<(QString)0x0922
                <<(QString)0x0923<<(QString)0x0924<<(QString)0x0925<<(QString)0x0926<<(QString)0x0927
                <<(QString)0x0928<<(QString)0x092A<<(QString)0x092B<<(QString)0x092C<<(QString)0x092D
                <<(QString)0x092E<<(QString)0x092F<<(QString)0x0930<<(QString)0x0932<<(QString)0x0935
                <<(QString)0x0936<<(QString)0x0937<<(QString)0x0938<<(QString)0x0939<<(QString)0x093E
                <<(QString)0x093F<<(QString)0x0940<<(QString)0x0941<<(QString)0x0942<<(QString)0x0943
                <<(QString)0x0947<<(QString)0x0948<<(QString)0x094B<<(QString)0x094C<<(QString)0x094D
                <<(QString)0x0901<<(QString)0x0902
                <<(QString)0x0903<<(QString)0x0964<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<(QString)0x0966
                <<(QString)0x0967<<(QString)0x0968<<(QString)0x0969<<(QString)0x096A<<(QString)0x096B
                <<(QString)0x096C<<(QString)0x096D<<(QString)0x096E<<(QString)0x096F;


        // unicode +english charactors for finnish

        virtualKeyBoardButtonValuesFinnish
                <<"a"<<"b"<<"c"<<"d"<<"e"<<"f"<<"g"<<"h"<<"i"<<"j"<<"k"<<"l"<<"m"<<"n"<<"o"<<"p"
                <<"q"<<"r"<<"s"<<"t"<<"u"<<"v"<<"w"<<"x"<<"y"<<"z"<<(QString)0x00E4<<(QString)0x00E5
                <<(QString)0x00F6<<(QString)0x0161<<(QString)0x017E<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"
                <<"H"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"<<"O"<<"P"<<"Q"<<"R"<<"S"<<"T"<<"U"<<"V"<<"W"<<"X"
                <<"Y"<<"Z"<<(QString)0x00C4<<(QString)0x00C5 <<(QString)0x00D6 <<(QString)0x0160
                <<(QString)0x017D <<" " <<" " <<" " <<" " <<" " <<" " <<" "<<"0"<<"1"<<"2"<<"3"
                <<"4"<<"5"<<"6"<<"7"<<"8"<<"9";

        //unicode for  arabic charactors

        virtualKeyBoardButtonValuesArabic
                <<(QString)0x0636<<(QString)0x0635<<(QString)0x062B<<(QString)0x0642<<(QString)0x0641
                <<(QString)0x063A<<(QString)0x0647<<(QString)0x062E<<(QString)0x062D<<(QString)0x062C
                <<(QString)0x062F<<(QString)0x0634<<(QString)0x0633<<(QString)0x064A<<(QString)0x0628
                <<(QString)0x0644<<(QString)0x0627<<(QString)0x062A<<(QString)0x0646<<(QString)0x0645
                <<(QString)0x0643<<(QString)0x0637<<(QString)0x0626<<(QString)0x0621<<(QString)0x0624
                <<(QString)0x0631<<(QString)0x0644<<(QString)0x0649<<(QString)0x0629<<(QString)0x0632
                <<(QString)0x0638<<(QString)0x0648<<(QString)0x0630<<(QString)0x0652<<(QString)0x0651
                <<(QString)0x0650<<(QString)0x064D<<(QString)0x064C<<(QString)0x064F<<(QString)0x064E
                <<(QString)0x064B<<(QString)0x0644<<(QString)0x0622<<(QString)0x061F<<(QString)0x0623
                <<(QString)0x060C<<(QString)0x0625<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "
                <<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<" "<<(QString)0x0660
                <<(QString)0x0661<<(QString)0x0662<<(QString)0x0663<<(QString)0x0664<<(QString)0x0665
                <<(QString)0x0666<<(QString)0x0667<<(QString)0x0668<<(QString)0x0669;


        ///////for dynamic language
        virtualKeyBoardFrame->setFrameStyle(QFrame::Raised);
        virtualKeyBoardLayout->setVerticalSpacing(0);
        virtualKeyBoardLayout->setHorizontalSpacing(0);
        virtualKeyBoardFrame->setGeometry(VIRTUALKEYBOARDFRAME_XCORDINATE_VALUE,
                                          VIRTUALKEYBOARDFRAME_YCORDINATE_VALUE,
                                          VIRTUALKEYBOARDFRAME_WIDTH,
                                          VIRTUALKEYBOARDFRAME_HEIGHT);
        count=0;
        for(int i=0;i<NUMBEROFROWSINVIRTUALKEYBOARD;i++)
            for(int j=0;j<NUMBEROFCOLOUMNSINVIRTUALKEYBOARD;j++) {
            if(i==0&&j==0) {
                virtualKeyBoardLayout->addWidget(languages,i,j);
            }
            else if(i==0&&j==1) {
                virtualKeyBoardLayout->addWidget(numbers,i,j);
            }
            else if(i==3&&j==0) {
                virtualKeyBoardLayout->addWidget(nextSetOfCharactors,i,j);
            }
            else {
                switch(showLanguage) {
                case 2:
                    virtualKeyBoardButton[count]->setText((QString)virtualKeyBoardButtonValuesFinnish.at(count));
                    break;
                case 3:
                    virtualKeyBoardButton[count]->setText((QString)virtualKeyBoardButtonValuesArabic.at(count));
                    break;
                case 4:
                    virtualKeyBoardButton[count]->setText((QString)virtualKeyBoardButtonValuesHindi.at(count));
                    break;
                default:
                    QComboBox*ForSelection =languages;
                    QString filename = ForSelection->currentText();
                    QFile file(HEADERS+filename);
                    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                        return;
                    QTextStream in(&file);
                    QString line =in.readLine();
                    bool ok;
                    ok=true;
                    int i=0;
                    virtualKeyBoardButtonValuesCurrentLanguage.clear();
                    while (!line.isNull()) {
                        int x=line.toInt(&ok,16);
                        virtualKeyBoardButtonValuesCurrentLanguage<<(QString)x;
                        i++;
                        line = in.readLine();
                    }
                    file.close();

                    virtualKeyBoardButton[count]->setText((QString)virtualKeyBoardButtonValuesCurrentLanguage.at(count));
                    break;
                }
                virtualKeyBoardLayout->addWidget(virtualKeyBoardButton[count],i,j);
                count++;
            }
        }
        switch(showLanguage) {
        case 2:
            for(int numberCount=0;numberCount<10;numberCount++) {
                showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesFinnish.at(numberCount+69));
                showTheNumberLayout->addWidget(showTheNumbers[numberCount]);
            }
            break;
         case 3:

            for(int numberCount=0;numberCount<10;numberCount++) {
                showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesArabic.at(numberCount+69));
                showTheNumberLayout->addWidget(showTheNumbers[numberCount]);
            }
            break;
         case 4:
            for(int numberCount=0;numberCount<10;numberCount++) {
                showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesHindi.at(numberCount+69));
                showTheNumberLayout->addWidget(showTheNumbers[numberCount]);
            }
            break;
         default:
            QComboBox*ForSelection =languages;
            QString filename =ForSelection->currentText();
            QFile file(HEADERS+filename);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return;
            QTextStream in(&file);
            QString line =in.readLine();
            bool ok;
            ok=true;
            int i=0;
            virtualKeyBoardButtonValuesCurrentLanguage.clear();
            while (!line.isNull()) {
                int x=line.toInt(&ok,16);
                virtualKeyBoardButtonValuesCurrentLanguage<<(QString)x;
                i++;
                line = in.readLine();
            }
            file.close();
            for(int numberCount=0;numberCount<10;numberCount++) {
                showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesCurrentLanguage.at(numberCount+69));
                showTheNumberLayout->addWidget(showTheNumbers[numberCount]);
            }

            break;
        }

        showTheNumberLayout->addWidget(returnToVirtualKeyBoard);
        showTheNumberFrame->setGeometry(VIRTUALKEYBOARDFRAMENUMBERS_XCORDINATE_VALUE,
                                        VIRTUALKEYBOARDFRAMENUMBERS_YCORDINATE_VALUE,
                                        VIRTUALKEYBOARDFRAMENUMBERS_WIDTH,
                                        VIRTUALKEYBOARDFRAMENUMBERS_HEIGHT);

        connect(numbers,SIGNAL(clicked()),this,SLOT(showNumbersOfLanguage()));
        connect(nextSetOfCharactors,SIGNAL(clicked()),this,SLOT(displayTheRestOfTheCharactors()));
        connect(virtualKeyBoardButton[0],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[1],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[2],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[3],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[4],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[5],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[6],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[7],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[8],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[9],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[10],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[11],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[12],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[13],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[14],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[15],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[16],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[17],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[18],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[19],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[20],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[21],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[22],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[23],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[24],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[25],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[26],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[27],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[28],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[29],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[30],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[31],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[32],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[33],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[34],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[35],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[36],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[37],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[38],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[39],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[40],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[41],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[42],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[43],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(virtualKeyBoardButton[44],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        connect(languages,SIGNAL(activated(int)),this,SLOT(showLanguagesList()));

        showTheNumberFrame->setLayout(showTheNumberLayout);
        virtualKeyBoardFrame->setLayout(virtualKeyBoardLayout);
        virtualKeyBoardFrame->show();
    }

    else {
        disconnect(languages,SIGNAL(activated(int)),this,SLOT(showLanguagesList()));
        disconnect(virtualKeyBoardButton[0],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[1],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[2],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[3],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[4],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[5],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[6],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[7],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[8],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[9],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[10],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[11],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[12],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[13],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[14],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[15],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[16],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[17],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[18],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[19],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[20],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[21],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[22],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[23],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[24],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[25],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[26],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[27],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[28],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[29],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[30],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[31],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[32],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[33],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[34],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[35],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[36],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[37],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[38],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[39],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[40],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[41],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[42],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[43],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
        disconnect(virtualKeyBoardButton[44],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));

        virtualKeyBoardFrame->close();

        delete numbers;
        numbers=0;

        delete virtualKeyBoardButton[0];
        virtualKeyBoardButton[0]=0;
        delete virtualKeyBoardButton[1];
        virtualKeyBoardButton[1]=0;
        delete virtualKeyBoardButton[2];
        virtualKeyBoardButton[2]=0;
        delete virtualKeyBoardButton[3];
        virtualKeyBoardButton[3]=0;
        delete virtualKeyBoardButton[4];
        virtualKeyBoardButton[4]=0;
        delete virtualKeyBoardButton[5];
        virtualKeyBoardButton[5]=0;
        delete virtualKeyBoardButton[6];
        virtualKeyBoardButton[6]=0;
        delete virtualKeyBoardButton[7];
        virtualKeyBoardButton[7]=0;
        delete virtualKeyBoardButton[8];
        virtualKeyBoardButton[8]=0;
        delete virtualKeyBoardButton[9];
        virtualKeyBoardButton[9]=0;
        delete virtualKeyBoardButton[10];
        virtualKeyBoardButton[10]=0;
        delete virtualKeyBoardButton[11];
        virtualKeyBoardButton[11]=0;
        delete virtualKeyBoardButton[12];
        virtualKeyBoardButton[12]=0;
        delete virtualKeyBoardButton[13];
        virtualKeyBoardButton[13]=0;
        delete virtualKeyBoardButton[14];
        virtualKeyBoardButton[14]=0;
        delete virtualKeyBoardButton[15];
        virtualKeyBoardButton[15]=0;
        delete virtualKeyBoardButton[16];
        virtualKeyBoardButton[16]=0;
        delete virtualKeyBoardButton[17];
        virtualKeyBoardButton[17]=0;
        delete virtualKeyBoardButton[18];
        virtualKeyBoardButton[18]=0;
        delete virtualKeyBoardButton[19];
        virtualKeyBoardButton[19]=0;
        delete virtualKeyBoardButton[20];
        virtualKeyBoardButton[20]=0;
        delete virtualKeyBoardButton[21];
        virtualKeyBoardButton[21]=0;
        delete virtualKeyBoardButton[22];
        virtualKeyBoardButton[22]=0;
        delete virtualKeyBoardButton[23];
        virtualKeyBoardButton[23]=0;
        delete virtualKeyBoardButton[24];
        virtualKeyBoardButton[24]=0;
        delete virtualKeyBoardButton[25];
        virtualKeyBoardButton[25]=0;
        delete virtualKeyBoardButton[26];
        virtualKeyBoardButton[26]=0;
        delete virtualKeyBoardButton[27];
        virtualKeyBoardButton[27]=0;
        delete virtualKeyBoardButton[28];
        virtualKeyBoardButton[28]=0;
        delete virtualKeyBoardButton[29];
        virtualKeyBoardButton[29]=0;
        delete virtualKeyBoardButton[30];
        virtualKeyBoardButton[30]=0;
        delete virtualKeyBoardButton[31];
        virtualKeyBoardButton[31]=0;
        delete virtualKeyBoardButton[32];
        virtualKeyBoardButton[32]=0;
        delete virtualKeyBoardButton[33];
        virtualKeyBoardButton[33]=0;
        delete virtualKeyBoardButton[34];
        virtualKeyBoardButton[34]=0;
        delete virtualKeyBoardButton[35];
        virtualKeyBoardButton[35]=0;
        delete virtualKeyBoardButton[36];
        virtualKeyBoardButton[36]=0;
        delete virtualKeyBoardButton[37];
        virtualKeyBoardButton[37]=0;
        delete virtualKeyBoardButton[38];
        virtualKeyBoardButton[38]=0;
        delete virtualKeyBoardButton[39];
        virtualKeyBoardButton[39]=0;
        delete virtualKeyBoardButton[40];
        virtualKeyBoardButton[40]=0;
        delete virtualKeyBoardButton[41];
        virtualKeyBoardButton[41]=0;
        delete virtualKeyBoardButton[42];
        virtualKeyBoardButton[42]=0;
        delete virtualKeyBoardButton[43];
        virtualKeyBoardButton[43]=0;
        delete virtualKeyBoardButton[44];
        virtualKeyBoardButton[44]=0;
        delete nextSetOfCharactors;
        nextSetOfCharactors=0;
        delete showTheNumbers[0];
        showTheNumbers[0]=0;
        delete showTheNumbers[1];
        showTheNumbers[1]=0;
        delete showTheNumbers[2];
        showTheNumbers[2]=0;
        delete showTheNumbers[3];
        showTheNumbers[3]=0;
        delete showTheNumbers[4];
        showTheNumbers[4]=0;
        delete showTheNumbers[5];
        showTheNumbers[5]=0;
        delete showTheNumbers[6];
        showTheNumbers[6]=0;
        delete showTheNumbers[7];
        showTheNumbers[7]=0;
        delete showTheNumbers[8];
        showTheNumbers[8]=0;
        delete showTheNumbers[9];
        showTheNumbers[9]=0;
        delete returnToVirtualKeyBoard;
        returnToVirtualKeyBoard=0;
        delete languages;
        languages=0;
    }
}

void VirtualKeyBoard::displayCharactorVirtualKeyBoard()
{
    QPushButton *TextToBeEntered = qobject_cast<QPushButton *>(sender());
    showThePosition->position();
    showThePosition->insertText(TextToBeEntered->text());
    showThePosition->setPosition(showThePosition->position());
}

void VirtualKeyBoard::showLanguagesList()
{
    QComboBox*ForSelection=qobject_cast<QComboBox*>(sender());

    if(switchForNextVirtualKeyBoardCharactors)
        displayTheRestOfTheCharactors();

    switch(ForSelection->currentIndex())
    {
    case 0:
        showLanguage=0;

        addNewLanguage();
        break;

    case 1:
        showLanguage=1;
        removeInstalledLanguage();
        break;
    case 2:
        showLanguage=2;
        for(int i=0;i<45;i++) {
            virtualKeyBoardButton[i]->setText((QString)virtualKeyBoardButtonValuesFinnish.at(i));
        }
        for(int numberCount=0;numberCount<10;numberCount++) {
            showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesFinnish.at(69+numberCount));
        }
        break;
    case 3:showLanguage=3;
        for(int i=0;i<45;i++) {
            virtualKeyBoardButton[i]->setText((QString)virtualKeyBoardButtonValuesArabic.at(i));
        }
        for(int numberCount=0;numberCount<10;numberCount++) {
            showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesArabic.at(69+numberCount));
        }
        break;
    case 4:showLanguage=4;
        for(int i=0;i<45;i++) {
            virtualKeyBoardButton[i]->setText((QString)virtualKeyBoardButtonValuesHindi.at(i));
        }
        for(int numberCount=0;numberCount<10;numberCount++) {
            showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesHindi.at(69+numberCount));
        }
        break;
    default :

            showLanguage=99;
    QString filename =ForSelection->currentText();
    QFile file(homeDirPath+HEADERS+filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString line =in.readLine();
    bool ok;
    ok=true;
    int i=0;
    virtualKeyBoardButtonValuesCurrentLanguage.clear();
    while (!line.isNull()) {
        int x=line.toInt(&ok,16);
        virtualKeyBoardButtonValuesCurrentLanguage<<(QString)x;
        i++;
        line = in.readLine();
    }
    file.close();
    for(int i=0;i<45;i++) {
        virtualKeyBoardButton[i]->setText((QString)virtualKeyBoardButtonValuesCurrentLanguage.at(i));
    }
    for(int numberCount=0;numberCount<10;numberCount++) {
        showTheNumbers[numberCount]->setText(virtualKeyBoardButtonValuesCurrentLanguage.at(69+numberCount));
    }
    break;

}
}



void VirtualKeyBoard::showNumbersOfLanguage()
{
    virtualKeyBoardFrame->setVisible(false);
    showTheNumberFrame->show();

    connect(showTheNumbers[0],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[1],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[2],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[3],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[4],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[5],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[6],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[7],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[8],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(showTheNumbers[9],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    connect(returnToVirtualKeyBoard,SIGNAL(clicked()),this,SLOT(closeNumbers()));

}
void VirtualKeyBoard::closeNumbers()
{
    disconnect(showTheNumbers[0],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[1],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[2],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[3],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[4],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[5],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[6],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[7],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[8],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(showTheNumbers[9],SIGNAL(clicked()),this,SLOT(displayCharactorVirtualKeyBoard()));
    disconnect(returnToVirtualKeyBoard,SIGNAL(clicked()),this,SLOT(closeNumbers()));

    showTheNumberFrame->hide();
    virtualKeyBoardFrame->setVisible(true);
}

void VirtualKeyBoard::displayTheRestOfTheCharactors()
{
    switchForNextVirtualKeyBoardCharactors=!switchForNextVirtualKeyBoardCharactors;
    if(switchForNextVirtualKeyBoardCharactors) {
        switch(showLanguage) {
        case 2:
            for(int i=0;i<24;i++) {
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesFinnish.at(45+i));
                if(!virtualKeyBoardButton[i]->text().compare(" ")) {
                    virtualKeyBoardButton[i]->setVisible(false);
                }
            }
            break;
        case 3:
            for(int i=0;i<24;i++) {
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesArabic.at(45+i));
                if(!virtualKeyBoardButton[i]->text().compare(" ")) {
                    virtualKeyBoardButton[i]->setVisible(false);
                }
            }
            break;
        case 4:
            for(int i=0;i<24;i++) {
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesHindi.at(45+i));
                if(!virtualKeyBoardButton[i]->text().compare(" ")) {
                    virtualKeyBoardButton[i]->setVisible(false);
                }
            } break;
        default:
            for(int i=0;i<24;i++) {
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesCurrentLanguage.at(45+i));
                if(!virtualKeyBoardButton[i]->text().compare(" ")) {
                    virtualKeyBoardButton[i]->setVisible(false);
                }
            }

            break;

        }
    }
    else {
        switch(showLanguage) {
        case 2:
            for(int i=0;i<45;i++) {
                virtualKeyBoardButton[i]->setVisible(true);
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesFinnish.at(i));
            }
            break;
        case 3:
            for(int i=0;i<45;i++) {
                virtualKeyBoardButton[i]->setVisible(true);
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesArabic.at(i));
            }
            break;
        case 4:
            for(int i=0;i<45;i++) {
                virtualKeyBoardButton[i]->setVisible(true);
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesHindi.at(i));
            }
            break;
        default:
            for(int i=0;i<45;i++) {
                virtualKeyBoardButton[i]->setVisible(true);
                virtualKeyBoardButton[i]->setText(virtualKeyBoardButtonValuesCurrentLanguage.at(i));
            }
            break;
        }
    }
}


void VirtualKeyBoard::addNewLanguage()
{

    manager->execute("ListOfAvailableLanguages.txt",homeDirPath+LISTOFAVAILABLELANGUAGES,"language basket refreshed");
    addingNewLanguage=new QDialog();
    additionLayout=new QVBoxLayout();
    stateOfAddingNewLanguage = new QLabel();
    listOfAvailableLanguages=new QComboBox();
    add =new QPushButton("Add");

    QFile file(homeDirPath+LISTOFAVAILABLELANGUAGESBASKET);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString line = in.readLine();
    int i=0;
    while (!line.isNull()) {
        containerForListOfAvailableLanguages<<line;
        listOfAvailableLanguages->addItem(containerForListOfAvailableLanguages.at(i));
        line = in.readLine();
        i++;
    }
    file.close();
    stateOfAddingNewLanguage->setText(i18n("Select the language to be added from the list"));
    additionLayout->addWidget(stateOfAddingNewLanguage);
    additionLayout->addWidget(listOfAvailableLanguages);
    additionLayout->addWidget(add);
    addingNewLanguage->setLayout(additionLayout);
    addingNewLanguage->raise();
    addingNewLanguage->show();
    connect(add,SIGNAL(clicked()),this,SLOT(downloadAndAdd()));
}

void VirtualKeyBoard::downloadAndAdd()
{
    newLanguage =listOfAvailableLanguages->currentText();
    manager->execute(newLanguage,homeDirPath+HEADERS,newLanguage+" added successfully");

    disconnect(manager,SIGNAL(downloadedSuccessfully()),this,SLOT(nowDownloadFont()));
    connect(manager,SIGNAL(downloadedSuccessfully()),this,SLOT(nowDownloadFont()));
    delete add ;
    add=0;

    delete listOfAvailableLanguages;
    listOfAvailableLanguages=0;

    delete stateOfAddingNewLanguage;
    stateOfAddingNewLanguage=0;

    delete additionLayout;
    additionLayout=0;

    delete  addingNewLanguage;
    addingNewLanguage=0;
}

void VirtualKeyBoard::removeInstalledLanguage()
{
    removeLanguage=new QDialog();
    removeLayout=new QVBoxLayout();
    stateOfRemovingNewLanguage = new QLabel();
    listOfInstalledLanguages=new QComboBox();
    remove =new QPushButton("remove");
    QDir directory = QDir(homeDirPath+POPULATELANGUAGES);
    QStringList files;
    files = directory.entryList(QStringList("*"),QDir::Files | QDir::NoSymLinks);
    for(int i=0;i<files.count();i++) {
        listOfInstalledLanguages->addItem(files.at(i));
    }
    listOfInstalledLanguages->setMaxCount(files.count());
    stateOfRemovingNewLanguage->setText(i18n("Select the language to be removed from the language basket"));
    removeLayout->addWidget(stateOfRemovingNewLanguage);
    removeLayout->addWidget(listOfInstalledLanguages);
    removeLayout->addWidget(remove);
    if(files.count() == 0) {
        stateOfRemovingNewLanguage->setText("no new languages are present \n use +add Language for adding languages");
        remove->hide();
        listOfInstalledLanguages->hide();
    }

    removeLanguage->setLayout(removeLayout);
    removeLanguage->raise();
    removeLanguage->show();
    connect(remove,SIGNAL(clicked()),this,SLOT(deleteFilesOfLanguage()));
}

void VirtualKeyBoard::deleteFilesOfLanguage()
{
    QFile toremove;
    if(!toremove.remove(homeDirPath+HEADERS+listOfInstalledLanguages->currentText())) {  ///scratchbox/users/maemo/home/maemo/VirtualKeyBoardLanguages/headers/
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, i18n("unable to remove the file "),
                                           QMaemo5InformationBox::DefaultTimeout);
#else
        stateOfRemovingNewLanguage->setText("unable to remove the file ");
#endif
    }
    else {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, i18n("language removed select other language or restart the keyboard for changes"),
                                           QMaemo5InformationBox::DefaultTimeout);
#else
        stateOfRemovingNewLanguage->setText("language removed select other language or restart the keyboard for changes");
#endif

    }
}

int VirtualKeyBoard::populateTheLanguages()
{
    QDir directory = QDir(homeDirPath+POPULATELANGUAGES);
    QStringList files;
    files = directory.entryList(QStringList("*"),QDir::Files | QDir::NoSymLinks);
    for(int i=0;i<files.count();i++) {
        languages->addItem(QIcon(":/images/64x64/VirtualKeyBoard/languages.png"),files.at(i));
    }
    return files.count();
}

void VirtualKeyBoard::nowDownloadFont()
{
    QString newLanguageFont;
    newLanguageFont = newLanguage;
    newLanguageFont.append(".ttf");
    manager->execute(newLanguageFont,homeDirPath+FONTS,newLanguage+" is ready for use,please restart the Keyboard");
}
