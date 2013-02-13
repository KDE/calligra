/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "modelcodecviewmanager.h"

// lib
#include <filesystem/modelencoderfilesystemexporterconfigeditorfactory.h>
#include <abstractmodelstreamencoderconfigeditorfactory.h>
#include <abstractmodelstreamencoderconfigeditor.h>
#include <abstractmodelstreamencoder.h>
#include <abstractmodelexporterconfigeditorfactory.h>
#include <abstractmodelexporterconfigeditor.h>
#include <abstractmodelexporter.h>
#include <abstractmodeldatageneratorconfigeditorfactory.h>
#include <abstractmodeldatageneratorconfigeditor.h>
#include <abstractmodeldatagenerator.h>


namespace Kasten2
{

ModelCodecViewManager::ModelCodecViewManager() {}

void ModelCodecViewManager::setEncoderConfigEditorFactories( const QList<AbstractModelStreamEncoderConfigEditorFactory*>& factoryList )
{
    qDeleteAll( mEncoderFactoryList );
    mEncoderFactoryList = factoryList;

    qDeleteAll( mExporterFactoryList );
    mExporterFactoryList.clear();

    foreach( AbstractModelStreamEncoderConfigEditorFactory* factory, mEncoderFactoryList )
        mExporterFactoryList << new ModelEncoderFileSystemExporterConfigEditorFactory( factory );
}

void ModelCodecViewManager::setExporterConfigEditorFactories( const QList<AbstractModelExporterConfigEditorFactory*>& factoryList )
{
    qDeleteAll( mExporterFactoryList );
    mExporterFactoryList = factoryList;
}

void ModelCodecViewManager::setGeneratorConfigEditorFactories( const QList<AbstractModelDataGeneratorConfigEditorFactory*>& factoryList )
{
    qDeleteAll( mGeneratorFactoryList );
    mGeneratorFactoryList = factoryList;
}

AbstractModelStreamEncoderConfigEditor* ModelCodecViewManager::createConfigEditor( AbstractModelStreamEncoder* encoder ) const
{
    AbstractModelStreamEncoderConfigEditor* result = 0;

    foreach( const AbstractModelStreamEncoderConfigEditorFactory* factory, mEncoderFactoryList )
    {
        result = factory->tryCreateConfigEditor( encoder );
        if( result )
            break;
    }

    return result;
}

AbstractModelExporterConfigEditor* ModelCodecViewManager::createConfigEditor( AbstractModelExporter* exporter ) const
{
    AbstractModelExporterConfigEditor* result = 0;

    foreach( const AbstractModelExporterConfigEditorFactory* factory, mExporterFactoryList )
    {
        result = factory->tryCreateConfigEditor( exporter );
        if( result )
            break;
    }

    return result;
}

AbstractModelDataGeneratorConfigEditor* ModelCodecViewManager::createConfigEditor( AbstractModelDataGenerator* generator ) const
{
    AbstractModelDataGeneratorConfigEditor* result = 0;

    foreach( const AbstractModelDataGeneratorConfigEditorFactory* factory, mGeneratorFactoryList )
    {
        result = factory->tryCreateConfigEditor( generator );
        if( result )
            break;
    }

    return result;
}

ModelCodecViewManager::~ModelCodecViewManager()
{
    qDeleteAll( mEncoderFactoryList );
    qDeleteAll( mExporterFactoryList );
    qDeleteAll( mGeneratorFactoryList );
}

}
