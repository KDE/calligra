/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006-2009,2011 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CREATORCONTROLLER_H
#define CREATORCONTROLLER_H

// lib
#include "kastencontrollers_export.h"
// Kasten gui
#include <abstractxmlguicontroller.h>

class KXMLGUIClient;


namespace Kasten2
{

class ModelCodecManager;
class AbstractDocumentStrategy;


class KASTENCONTROLLERS_EXPORT CreatorController : public AbstractXmlGuiController
{
  Q_OBJECT

  public:
    CreatorController( ModelCodecManager* modelCodecManager,
                       AbstractDocumentStrategy* documentStrategy,
                       KXMLGUIClient* guiClient );
    virtual ~CreatorController();

  public: // AbstractXmlGuiController API
    virtual void setTargetModel( AbstractModel* model );

  private Q_SLOTS:
    void onNewActionTriggered();
    void onNewFromClipboardActionTriggered();
    void onNewFromGeneratorActionTriggered();

  private:
    ModelCodecManager* mModelCodecManager;
    AbstractDocumentStrategy* mDocumentStrategy;
};

}

#endif
