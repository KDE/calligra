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
  private:
    KSpreadStyle* m_style;
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

    /**
     * Returns the number of m_{top,bottom}{left,right} pointers that are null
     */
    int numNullChildren();

  
    typedef enum { Simple = 0, Quad = 1} QuadType;
    
    QuadType m_type;
  
        
    StyleClusterQuad* m_topLeft;
    StyleClusterQuad* m_topRight;
    StyleClusterQuad* m_bottomLeft;
    StyleClusterQuad* m_bottomRight;

    KSpreadStyle* getStyle();
    void setStyle(KSpreadStyle * style);
};

KSpreadStyle *StyleClusterQuad::getStyle() {
  return m_style;
}
void StyleClusterQuad::setStyle(KSpreadStyle * style) {
  if(m_style && m_style->release()) {
    delete m_style;
  }
  m_style = style;
  if(m_style)
    m_style->addRef();
}
    

StyleClusterQuad::StyleClusterQuad()
: m_style(NULL),
  m_type(Simple),
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
  m_topLeft = m_topRight = m_bottomLeft = m_bottomRight = 0;
}

int StyleClusterQuad::numNullChildren() {
  int num_children = 0;
  if(!m_topLeft) num_children++;
  if(!m_topRight) num_children++;
  if(!m_bottomRight) num_children++;
  if(!m_bottomLeft ) num_children++;
  return num_children;
}

StyleCluster::StyleCluster(KSpreadSheet* sheet)
: m_sheet(sheet)
{
  Q_ASSERT(sheet); if(!sheet) return;
  // create quad tree
  // don't use KSpreadFormat, it seems to be deprecated anyway...
  m_topQuad = new StyleClusterQuad();
  m_topQuad->setStyle(sheet->doc()->styleManager()->defaultStyle());
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
// If a Quad has a Simple, then it must also have a null.  If temporarily it doesn't, then the 
// Simple must be deleted, and made a null and the Quad given that style.
void StyleCluster::insert( int x, int y, KSpreadStyle * style)
{
  Q_ASSERT(m_topQuad);
  
  StyleClusterQuad** current_node = &m_topQuad;
  StyleClusterQuad* last_node = NULL;
  int x_offset = 0;
  int y_offset = 0;
  int quad_size = KS_Max_Quad;
  
  if( m_topQuad->m_type == StyleClusterQuad::Simple ) { 
    if( style == m_topQuad->getStyle() )
      return;
    else
      m_topQuad->m_type = StyleClusterQuad::Quad;
  }
  
  //let's keep track of the path we went down, so we can go up as well
  // note that we store pointers to pointers 
  QValueStack<StyleClusterQuad**> path;
  
  while (true)
  {
    Q_ASSERT (current_node);
    Q_ASSERT( *current_node);
    //The below should be true.  It is obviously true from the code, so feel free to comment out
    Q_ASSERT( (*current_node)->m_type == StyleClusterQuad::Quad );
    Q_ASSERT( quad_size > 0); // we can't have a quad of width 1!

    // For the next few lines, 'quad_size' is the size of the child node - i.e. the new current_node
    quad_size /= 2;
    
    last_node = *current_node;
    
    path.push(current_node);
    
    if( x - x_offset < quad_size ) {
      if( y - y_offset < quad_size ) {
        current_node = &((*current_node)->m_topLeft);
      }
      else {
        current_node = &((*current_node)->m_bottomLeft);
        y_offset += quad_size;
      }
    } else {
      if( y - y_offset < quad_size ) {
        current_node = &((*current_node)->m_topRight);
        x_offset += quad_size;
      }
      else {
        current_node = &((*current_node)->m_bottomRight);
        y_offset += quad_size;
        x_offset += quad_size;
      }
    }
    
    //quad_size is now the size (size of width, and size of height) of current_node
    
    //Now we have gone down one step.  The parent is a quad, but the current node
    //may be null, in which case our style is the style of the parent, or it's Simple,
    //in which case we need to check whether we to subdivide, or it's a quad, in which case
    //we don't do anything until we loop again around this while loop and go down into that quad.
    
    if( !*current_node ) { // We know the parent is a Quad now
      //Okay, so we are using the style of the parent.
      //If we are not down to a single cell, then we will have to create a quad for current node,
      //and go down into it continously until there are no more quads to create.
      
      Q_ASSERT( last_node->m_type == StyleClusterQuad::Quad );
      
      //The whole of this section is already this style.  No need to do anything.
      if( style == last_node->getStyle() ) return;

      if(quad_size == 1) {  //We are now on a single cell
	int num_null_children_in_parent = last_node->numNullChildren();

	Q_ASSERT(last_node->getStyle() != NULL);
	
        Q_ASSERT(num_null_children_in_parent > 0);
	if(num_null_children_in_parent == 1) {// We are the only one using the style info in parent, so just change the m_style in parent

	  last_node->setStyle(style);
	  simplify(path);
	} else {  //someone else in the parent is using the style info in parent, so we have to create our own child
          (*current_node) = new StyleClusterQuad(); //defaults to a Simple
          (*current_node)->setStyle(style);
	}
        return;
      }
      
      *current_node = new StyleClusterQuad(); //defaults to a Simple
      (*current_node)->setStyle(last_node->getStyle());

      //we will go into this next time round the loop
      // so make it a quad
      (*current_node)->m_type = StyleClusterQuad::Quad;

      if(last_node->numNullChildren() == 0) {
        last_node->setStyle(0); //nothing is using this anymore
      }

    } else if( (*current_node)->m_type == StyleClusterQuad::Simple ) { 
      if( style == (*current_node)->getStyle() )
        return;
      if(quad_size == 1) {
	
        //So we are a simple cell
	//
        if( last_node->getStyle() == style) {
          delete (*current_node);  //style is released in the destructor
          *current_node = NULL;
          //Now it may be that there are no other children in the last_node, so the style is for all 4 of its children quads
	  
	  simplify(path);
        
	} else if (last_node->getStyle() == 0) {
	  //We are the _only_ child that is Simple.  Can't happen for quad_size==1
	  Q_ASSERT(false);
	  return;
	} else { 
	  //The style on the parent differs from us, so there's nothing more we can do
          (*current_node)->setStyle(style);
	}
	return; //we are on quad_size ==1, so a single cell. no point continuing.	
      }
      //else make this a quad, and go inside it
      (*current_node)->m_type = StyleClusterQuad::Quad;
    } // else it's a quad, and we will go into it on the next time round the while loop
  }
  
  return;
  
}

void StyleCluster::simplify(  QValueStack<StyleClusterQuad**> path ) { 
  StyleClusterQuad** current_node;
  StyleClusterQuad* last_node = NULL;

  if( path.isEmpty()) return;
  current_node = path.pop();
  if( !path.isEmpty() && path.top())
    last_node = *(path.top());
  
  Q_ASSERT( current_node && *current_node);

  if((*current_node)->m_bottomLeft && (*current_node)->m_bottomLeft->m_type == StyleClusterQuad::Simple && (*current_node)->m_bottomLeft->getStyle() == (*current_node)->getStyle()) {
    delete (*current_node)->m_bottomLeft;
    (*current_node)->m_bottomLeft = 0;
  }
  if((*current_node)->m_bottomRight && (*current_node)->m_bottomRight->m_type == StyleClusterQuad::Simple && (*current_node)->m_bottomRight->getStyle() == (*current_node)->getStyle()) {
    delete (*current_node)->m_bottomRight;
    (*current_node)->m_bottomRight = 0;
  }
  if((*current_node)->m_topLeft && (*current_node)->m_topLeft->m_type == StyleClusterQuad::Simple && (*current_node)->m_topLeft->getStyle() == (*current_node)->getStyle()) {
    delete (*current_node)->m_topLeft;
    (*current_node)->m_topLeft = 0;
  }
  if((*current_node)->m_topRight && (*current_node)->m_topRight->m_type == StyleClusterQuad::Simple && (*current_node)->m_topRight->getStyle() == (*current_node)->getStyle()) {
    delete (*current_node)->m_topRight;
    (*current_node)->m_topRight = 0;
  }

  
  if((*current_node)->numNullChildren() == 4) { //we can simplify - all children in the quad are pointing to use.
    (*current_node)->m_type = StyleClusterQuad::Simple;
	  
    if(!last_node) {
      
      Q_ASSERT( m_topQuad == *current_node );

      return;
    }
    if(last_node->getStyle() == (*current_node)->getStyle()) {
      //Parent has the same style, so delete us, then go back up recursively
      delete (*current_node);
      *current_node = 0;
      simplify(path);
    } else if(last_node->getStyle() == 0 ) {
      //the parent has only quads, and we are now the only simple one
      //so delete us, and set the style for the parent
      last_node->setStyle( (*current_node)->getStyle() );
      delete (*current_node);
      *current_node = 0; 
    } else if(last_node->numNullChildren() == 0) {
      //The style in the parent is different, but we are the only one using that style
      last_node->setStyle( (*current_node)->getStyle() );
      delete (*current_node);
      (*current_node) = 0;
      simplify(path);
    } else {
      //The style in the parent is different, and it has children using that style, so there is nothing we can do.
    }
  }
}

const KSpreadStyle& StyleCluster::lookup(int x, int y) {
  return *(lookupNode(x,y)->getStyle());
}

StyleClusterQuad* StyleCluster::lookupNode(int x, int y) {
  //walk over quad-tree
  // see gnumeric sheet-style.c  cell_tile_apply_pos(...)
  //  these implementations rather differ though
  
  Q_ASSERT(m_topQuad);
  
  StyleClusterQuad* current_node = m_topQuad;
  StyleClusterQuad* last_node = NULL;
  int x_offset = 0;
  int y_offset = 0;
  int quad_size = KS_Max_Quad;
  
  while ( current_node && current_node->m_type != StyleClusterQuad::Simple )
  {
    last_node = current_node;
    quad_size /= 2;
    if( x - x_offset < quad_size ) {
      if( y - y_offset < quad_size ) {
        current_node = current_node->m_topLeft;
      }
      else {
        current_node = current_node->m_bottomLeft;
        y_offset += quad_size;
      }
    } else {
      if( y - y_offset < quad_size ) {
        current_node = current_node->m_topRight;
        x_offset += quad_size;
      }
      else {
        current_node = current_node->m_bottomRight;
        y_offset += quad_size;
        x_offset += quad_size;
      }
    }
  }
  
  if( !current_node ) return last_node;
  
  return current_node;
}

}

#include "stylecluster.moc"
