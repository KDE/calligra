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

#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLayout>
#include <QFrame>
#include <QPushButton>
#include <QStringList>
#include <QComboBox>
#include "MainWindow.h"
#include <KoTextEditor.h>
#include <QFile>
#include <QDialog>
#include <QLabel>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QThread>
#include <klocale.h>

#include "VirtualKeyBoardLanguageDownloader.h"

#define VIRTUALKEYBOARDFRAME_YCORDINATE_VALUE 180
#define VIRTUALKEYBOARDFRAME_XCORDINATE_VALUE 5

#define VIRTUALKEYBOARDFRAME_WIDTH 795
#define VIRTUALKEYBOARDFRAME_HEIGHT 190

#define NUMBEROFROWSINVIRTUALKEYBOARD 4
#define NUMBEROFCOLOUMNSINVIRTUALKEYBOARD 12

#define VIRTUALKEYBOARDFRAMENUMBERS_XCORDINATE_VALUE 5
#define VIRTUALKEYBOARDFRAMENUMBERS_YCORDINATE_VALUE 180
#define VIRTUALKEYBOARDFRAMENUMBERS_WIDTH  800
#define VIRTUALKEYBOARDFRAMENUMBERS_HEIGHT 200

/*!

 * \brief VirtualKeyBoard class displays the virtual keyboard
 * on the screen to add multiple languages
 */
class MainWindow ;
class VirtualKeyBoard : public QMainWindow
{
    Q_OBJECT
public:
    VirtualKeyBoard(QWidget *parent = 0);
    ~VirtualKeyBoard();
    void ShowVirtualKeyBoard(MainWindow *,KoTextEditor * );
private:

    DownloadManager *manager;
    QString homeDirPath;
    QString languageFileName;
    QString newLanguage;
    int showLanguage;
    int count;
    bool switchForVirtualKeyBoard;
    bool switchForNextVirtualKeyBoardCharactors;
    KoTextEditor *showThePosition;

    //components for the virtual keyboard

    QFrame *virtualKeyBoardFrame;
    QGridLayout *virtualKeyBoardLayout;
    QComboBox *languages;
    QPushButton *numbers;
    QPushButton *virtualKeyBoardButton[80];
    QPushButton *nextSetOfCharactors;

    //components for the addition of languages

    QDialog *addingNewLanguage;
    QVBoxLayout *additionLayout;
    QLabel *stateOfAddingNewLanguage;
    QComboBox *listOfAvailableLanguages;
    QPushButton *add;


    //components for the removal of languages

    QDialog *removeLanguage;
    QVBoxLayout *removeLayout;
    QLabel *stateOfRemovingNewLanguage;
    QComboBox *listOfInstalledLanguages;
    QPushButton *remove;

    //different languages on the virtual keyboard

    QStringList virtualKeyBoardButtonValuesKannada;
    QStringList virtualKeyBoardButtonValuesTamil;
    QStringList virtualKeyBoardButtonValuesArabic;
    QStringList virtualKeyBoardButtonValuesHindi;
    QStringList virtualKeyBoardButtonValuesFinnish;
    QStringList virtualKeyBoardButtonValuesCurrentLanguage;

    QStringList containerForListOfAvailableLanguages;
    //for the numbers on the keyboard

    MainWindow *parentForTheNumbersFrame;

    QFrame *showTheNumberFrame;
    QHBoxLayout *showTheNumberLayout;
    QPushButton *showTheNumbers[11];
    QPushButton *returnToVirtualKeyBoard;

private slots:
    void displayCharactorVirtualKeyBoard();
    void showLanguagesList();
    void showNumbersOfLanguage();
    void closeNumbers();
    void displayTheRestOfTheCharactors();
    void addNewLanguage();
    void removeInstalledLanguage();
    void deleteFilesOfLanguage();
    void downloadAndAdd();
    void nowDownloadFont();
private:

    int populateTheLanguages();

};

#endif // VIRTUALKEYBOARD_H
