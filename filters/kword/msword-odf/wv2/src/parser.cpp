/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "parser.h"
#include "olestream.h"
#include "handlers.h"

using namespace wvWare;

Parser::Parser( OLEStorage* storage, OLEStreamReader* wordDocument ) :
    m_inlineHandler( new InlineReplacementHandler ), m_subDocumentHandler( new SubDocumentHandler ),
    m_tableHandler( new TableHandler ), m_pictureHandler( new PictureHandler ),
    m_textHandler( new TextHandler ),m_graphicsHandler( new GraphicsHandler ), m_ourInlineHandler( true ),
    m_ourSubDocumentHandler( true ), m_ourTableHandler( true ), m_ourPictureHandler( true ),
    m_ourTextHandler( true ), m_ourGraphicsHandler ( true ), m_storage( storage ),
    m_wordDocument( wordDocument ), m_okay( true )
{
    if ( !storage || !wordDocument ) {
        m_okay = false;
        return;
    }

    // Let the "real" constructor find out which additional
    // streams it needs, as only it knows the FIB!
}

Parser::~Parser()
{
    // In case we're still using the default handlers we have
    // to clean them up. Messy, but we shouldn't use some smart
    // pointer here, as it's very restrictive for the user of
    // this library (std::auto_ptr would make it impossible to
    // use MI, SharedPtr would potentially lead to circular references).
    if ( m_ourInlineHandler )
        delete m_inlineHandler;
    if ( m_ourSubDocumentHandler )
        delete m_subDocumentHandler;
    if ( m_ourTableHandler )
        delete m_tableHandler;
    if ( m_ourPictureHandler )
        delete m_pictureHandler;
    if ( m_ourTextHandler )
        delete m_textHandler;
    if( m_ourGraphicsHandler)
        delete m_graphicsHandler;

    // Don't forget to close everything properly here
    delete m_wordDocument;
    m_storage->close();
    delete m_storage;
}

void Parser::setInlineReplacementHandler( InlineReplacementHandler* handler )
{
    setHandler<InlineReplacementHandler>( handler, &m_inlineHandler, m_ourInlineHandler );
}

void Parser::setSubDocumentHandler( SubDocumentHandler* handler )
{
    setHandler<SubDocumentHandler>( handler, &m_subDocumentHandler, m_ourSubDocumentHandler );
}

void Parser::setTableHandler( TableHandler* handler )
{
    setHandler<TableHandler>( handler, &m_tableHandler, m_ourTableHandler );
}

void Parser::setPictureHandler( PictureHandler* handler )
{
    setHandler<PictureHandler>( handler, &m_pictureHandler, m_ourPictureHandler );
}

void Parser::setTextHandler( TextHandler* handler )
{
    setHandler<TextHandler>( handler, &m_textHandler, m_ourTextHandler );
}

void Parser::setGraphicsHandler( GraphicsHandler* handler )
{
    setHandler<GraphicsHandler>( handler, &m_graphicsHandler, m_ourGraphicsHandler );
}
