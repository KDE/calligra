/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>

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
#ifndef KOOPENPANE_H
#define KOOPENPANE_H

#include <kjanuswidget.h>

class KoCustomDocumentCreator;
class KConfig;
class KoTemplateGroup;
class KoOpenPanePrivate;
class KInstance;

class KoOpenPane : public KJanusWidget
{
  Q_OBJECT

  public:
    KoOpenPane(QWidget *parent, KInstance* instance, const QString& templateType = QString::null);
    virtual ~KoOpenPane();

//     void addCustomDocumentCreator(KoCustomDocumentCreator *cdc);

  protected slots:
    void showOpenFileDialog();

  signals:
    void openExistingFile(const QString&);
    void openTemplate(const QString&);

  protected:
//     void addTemplatesPanel(KoTemplateGroup *group);

  private:
    KoOpenPanePrivate* d;
};

/// Interface for the application specific CustomDocumentCreator panel.
class KoCustomDocumentCreator
{
public:
    KoCustomDocumentCreator(KoOpenPane *parent, KConfig *config);
    virtual ~KoCustomDocumentCreator() = 0;

    // some signals / slots to signal enter and OK pressed.
    // also we should add an OK button at the bottom.

    // note we can't extend QObject here as the implementing class will surely
    // be a widget (we actually expect it to be one!).  So we have to do
    // signal slots via the virtual methods way.
};

#endif //KOOPENPANE_H
