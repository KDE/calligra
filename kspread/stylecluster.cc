/* This file is part of the KDE project
   Copyright (C) 2005 The KSpread Team
                              www.koffice.org/kspread

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include "stylecluster.h"

#include "kspread_global.h"
#include "kspread_doc.h"
#include "kspread_style_manager.h"
#include "kspread_style.h"
#include "kspread_sheet.h"
#include "kspread_util.h"

#include <qvaluestack.h>

namespace KSpread {

class StyleClusterQuad
{
  public:
  
    /**
     * Initializes m_type to Simple and everything else to NULL.
     * You should set style, type and the child quads to your needs!
     */
    StyleClusterQuad();
    
    /**
     * Releases m_style, deletes child quads, if m_type is Quad
     */
    ~StyleClusterQuad();
  
    typedef enum { Simple = 0, Quad = 1} QuadType;
    
    QuadType m_type;
  
    KSpreadStyle* m_style;
        
    StyleClusterQuad* m_topLeft;
    StyleClusterQuad* m_topRight;
    StyleClusterQuad* m_bottomLeft;
    StyleClusterQuad* m_bottomRight;
};

StyleClusterQuad::StyleClusterQuad()
: m_type(Simple),
  m_style(NULL),
  m_topLeft(NULL),
  m_topRight(NULL),
  m_bottomLeft(NULL),
  m_bottomRight(NULL)
{
}

StyleClusterQuad::~StyleClusterQuad()
{
  if (m_style && m_style->release())
  {
    delete m_style;
    m_style = NULL;
  }
  
  if (m_type == Quad)
  {
    if (m_topLeft)
      delete m_topLeft;
    if (m_topRight)
      delete m_topRight;
    if (m_bottomLeft)
      delete m_bottomLeft;
    if (m_bottomRight)
      delete m_bottomRight;
  }
}

StyleCluster::StyleCluster(KSpreadSheet* sheet)
: m_sheet(sheet)
{
  // create quad tree
  // don't use KSpreadFormat, it seems to be deprecated anyway...
  m_topQuad = new StyleClusterQuad();
  m_topQuad->m_style = sheet->doc()->styleManager()->defaultStyle();
}


StyleCluster::~StyleCluster()
{
  if (m_topQuad)
  {
    delete m_topQuad;
    m_topQuad=NULL;
  }
}
// A Simple will never have the same style as the parent.  Instead it will be null.
// If a Quad has a Simple, then it must also have a null.  If it does, then the 
// Simple must be deleted, and made a null and the Quad given that style.
void StyleCluster::insert( int x, int y, KSpreadStyle * style)
{
  Q_ASSERT(m_topQuad);
  
  StyleClusterQuad** current_node = &m_topQuad;
  StyleClusterQuad* last_node = NULL;
  int x_offset = 0;
  int y_offset = 0;
  int half_quad_size = KS_Max_Quad/2;
  
  if( m_topQuad->m_type == StyleClusterQuad::Simple ) { 
    if( style == m_topQuad->m_style )
      return;
    else
      m_topQuad->m_type = StyleClusterQuad::Quad;
  }
  
  //let's keep track of the path we went down, so we can go up as well
  // note that we store pointers to pointers 
  QValueStack<StyleClusterQuad**> path;
  
  while (true)
  {
    //The below should be true.  It is obviously true from the code, so feel free to comment out
    Q_ASSERT( (*current_node)->m_type == StyleClusterQuad::Quad );
  
    last_node = *current_node;
    
    path.push(current_node);
    
    if( x - x_offset < half_quad_size ) {
      if( y - y_offset < half_quad_size ) {
        current_node = &((*current_node)->m_topLeft);
      }
      else {
        current_node = &((*current_node)->m_bottomLeft);
        y_offset += half_quad_size;
      }
    } else {
      if( y - y_offset < half_quad_size ) {
        current_node = &((*current_node)->m_topRight);
        x_offset += half_quad_size;
      }
      else {
        current_node = &((*current_node)->m_bottomRight);
        y_offset += half_quad_size;
        x_offset += half_quad_size;
      }
    }
    
    if( !*current_node ) { // We know the parent is a Quad now
      Q_ASSERT( last_node->m_type == StyleClusterQuad::Quad );
      
      if( style == last_node->m_style) return;
      
      *current_node = new StyleClusterQuad(); //defaults to a Simple
      
      if(half_quad_size == 0) {
        (*current_node)->m_style = style;
        style->addRef();
        return;
      }
      (*current_node)->m_type = StyleClusterQuad::Quad;
      if ((*current_node)->m_style)
      {
        if ((*current_node)->m_style->release())
          delete (*current_node)->m_style;
      }
      (*current_node)->m_style = last_node->m_style;
      style->addRef();
    } else if( (*current_node)->m_type == StyleClusterQuad::Simple ) { 
      if( style == (*current_node)->m_style )
        return;
      if(half_quad_size == 0) {
        (*current_node)->m_style = style;
        style->addRef();
        
        if( last_node->m_style == style) {
          delete (*current_node);  //style is released in the destructor
          *current_node = NULL;
        } else {
          while( last_node && last_node->m_topLeft != NULL && last_node->m_topRight != NULL &&
              last_node->m_bottomLeft != NULL && last_node->m_bottomRight != NULL ) {
              
              //if the parent quad is full, and this is a simple node, we delete this node
              //and set the style of the parent node accordingly.
              
              last_node->m_style = style;
              style->addRef();
              
              //always make sure that reference
              // counting does not drop to 0 if we need
              // it still, so addRef() BEFORE release()
              // (or delete node) for the same style
              
              delete (*current_node);
              *current_node = NULL;
              
              //move up one layer
              current_node = path.pop();
              last_node = *(path.top());
          }
        }
        
        return;
      }
      (*current_node)->m_type = StyleClusterQuad::Quad;
    } // else it's a quad, and we will go into it on the next time round the while loop
    
    half_quad_size /= 2;
  }
  
  return;
  
}

const KSpreadStyle& StyleCluster::lookup(int x, int y)
{
  //walk over quad-tree
  // see gnumeric sheet-style.c  cell_tile_apply_pos(...)
  //  these implementations rather differ though
  
  Q_ASSERT(m_topQuad);
  
  StyleClusterQuad* current_node = m_topQuad;
  StyleClusterQuad* last_node = NULL;
  int x_offset = 0;
  int y_offset = 0;
  int half_quad_size = KS_Max_Quad;
  
  while ( current_node && current_node->m_type != StyleClusterQuad::Simple )
  {
    last_node = current_node;
    half_quad_size /= 2;
    if( x - x_offset < half_quad_size ) {
      if( y - y_offset < half_quad_size ) {
        current_node = current_node->m_topLeft;
      }
      else {
        current_node = current_node->m_bottomLeft;
        y_offset += half_quad_size;
      }
    } else {
      if( y - y_offset < half_quad_size ) {
        current_node = current_node->m_topRight;
        x_offset += half_quad_size;
      }
      else {
        current_node = current_node->m_bottomRight;
        y_offset += half_quad_size;
        x_offset += half_quad_size;
      }
    }
  }
  if( !current_node ) return *(last_node->m_style);

  return *(current_node->m_style);
}

}

#include "stylecluster.moc"
