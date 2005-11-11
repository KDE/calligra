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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

/**
 * This class represents one quad in a quad tree.
 * There will always be at least one instance of this for each StyleCluster
 * class, referenced in StyleCluster::m_topQuad
 * 
 * A quad will either be simple, in which case case all cells that it encloses
 * are of m_style, or it will be a Quad.
 * 
 * If the quad is of type Quad, then each of the 4 StyleClusterQuad pointers
 * m_topLeft, m_topRight, m_bottomLeft, m_bottomRight will either point to a
 * new StyleClusterQuad, or it will be null.
 * 
 * If one or more of the pointers is null, then that is equivalent to it pointing
 * to a StyleClusterQuad of type Simple, with the same style.
 * 
 * Note that if any of the pointers point to a StyleClusterQuad of type Simple, then
 * that implies:
 *   * There is at least one pointer in the parent that is null, since otherwise
 *     the Simple child StyleClusterQuad could itself be optimised away to null.
 *   * The style of the Simple child differs from the parent for the same reason.
 * 
 * The code should (and does at time of writting) always maintain this, and uses this
 * useful fact to have an efficent self minimilization algorithm.  Upon insertion, the
 * quad tree will automatically simplify itself to the minimal structure.
 * 
 */
class StyleClusterQuad
{
  private:
    Style* m_style;
  public:
  
    /**
     * You should set style and the child quads to your needs!
     */
    inline StyleClusterQuad();
    
    /**
     * Releases m_style, deletes child quads, if m_type is Quad
     */
    inline ~StyleClusterQuad();

    /**
     * Returns the number of m_{top,bottom}{left,right} pointers that are null
     * Does not use the isSimple variable, so safe to call if isSimple is
     * invalid.
     */
    inline int numNullChildren();

    /**
     * Are the following 4 pointers all null?  For speed efficency.. I hope.
     * This needs to be kept in sync manually.
     */
    bool m_isSimple;

    StyleClusterQuad* m_topLeft;
    StyleClusterQuad* m_topRight;
    StyleClusterQuad* m_bottomLeft;
    StyleClusterQuad* m_bottomRight;

    Style* getStyle() { return m_style; }
    inline void setStyle(Style * style);
    /**
     * Create a new StyleClusterQuad for one of it's null children
     */
    inline void makeChild(StyleClusterQuad **child);
};

void StyleClusterQuad::makeChild(StyleClusterQuad **child) {
  Q_ASSERT(!*child);
  Q_ASSERT(child == &m_topLeft || child == &m_topRight ||
      child == &m_bottomLeft || child == &m_bottomRight);
      
  *child = new StyleClusterQuad();
  (*child)->setStyle(getStyle());
  m_isSimple = false; //Make sure the parent knows it has a non-null child now.

  if(numNullChildren() == 0) {
    setStyle(0); //nothing is using this anymore
  }
}
void StyleClusterQuad::setStyle(Style * style) {
  if(m_style && m_style->release()) {
    delete m_style;
  }
  m_style = style;
  if(m_style)
    m_style->addRef();
}

StyleClusterQuad::StyleClusterQuad()
: m_style(NULL),
  m_isSimple(true),
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
  
  if (!m_isSimple) //there are non-null pointers
  {
    if (m_topLeft)
      delete m_topLeft;
    if (m_topRight)
      delete m_topRight;
    if (m_bottomLeft)
      delete m_bottomLeft;
    if (m_bottomRight)
      delete m_bottomRight;
    m_topLeft = m_topRight = m_bottomLeft = m_bottomRight = 0;
  }
}

int StyleClusterQuad::numNullChildren() {
  int num_children = 0;
  if(!m_topLeft) num_children++;
  if(!m_topRight) num_children++;
  if(!m_bottomRight) num_children++;
  if(!m_bottomLeft ) num_children++;
  return num_children;
}

//end of StyleClusterQuad
//start of StyleCluster

StyleCluster::StyleCluster(Sheet* sheet)
: m_sheet(sheet)
{
  Q_ASSERT(sheet); if(!sheet) return;
  // create quad tree
  // don't use Format, it seems to be deprecated anyway...
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

void StyleCluster::setStyle( const Range & range, Style * style)
{
  QValueStack< Range > ranges;
  ranges.push(range);
  
  Range current_range;
  
  while(!ranges.isEmpty()) {
    current_range = ranges.pop();
    
    
    StyleClusterQuad** current_node = &m_topQuad;
    StyleClusterQuad* last_node = NULL;
    int x_offset = 0;
    int y_offset = 0;
    int quad_size = KS_Max_Quad;
    
    int range_width = current_range.startCol()- current_range.endCol();
    int range_height = current_range.startRow()- current_range.endRow();
    int max_quad_size_wanted = (range_width > range_height)?range_width:range_height;
    
    while( x_offset != current_range.startCol() || 
           y_offset != current_range.startRow() ||
           quad_size > max_quad_size_wanted ) {
      //Note this function changes most of its parameters
      stepDownOne(current_node, current_range.startCol(), x_offset, current_range.startRow(), y_offset, quad_size);
      if(*current_node == NULL) {
        last_node->makeChild(current_node);
      }
    }
    (*current_node)->setStyle( style );
    
    //FIXME - finish this function
    if( quad_size < range_width ) {
      //ranges.push( new );
    }
  } 
}

// A Simple (m_isSimple) will never have the same style as the parent.  Instead, if the styles
// are the same, the instance wouldn't exist and the parent would point to null.
// If a Quad has a Simple, then it must also have a null.  If temporarily it doesn't, then the 
// Simple must be deleted, and made a null and the Quad given that style.
void StyleCluster::setStyle( int x, int y, Style * style)
{
  Q_ASSERT(m_topQuad);
  
  StyleClusterQuad** current_node = &m_topQuad;
  StyleClusterQuad* last_node = NULL;
  int x_offset = 0;
  int y_offset = 0;
  int quad_size = KS_Max_Quad;

  if( m_topQuad->m_isSimple && style == m_topQuad->getStyle() )
      return;

  //let's keep track of the path we went down, so we can go up as well
  // note that we store pointers to pointers 
  QValueStack<StyleClusterQuad**> path;
  
  while (true)
  {
    Q_ASSERT (current_node);
    Q_ASSERT( *current_node);
    Q_ASSERT( quad_size > 0); // we can't have a quad of width 1!
    
    path.push(current_node);
    last_node = *current_node;
    
    //Note this function modifies its arguments
    stepDownOne(current_node, x, x_offset, y, y_offset, quad_size);
    
    //quad_size is now the size (size of width, and size of height) of current_node
    //Now we have gone down one step.  The current node may be null, in which case
    //our style is the style of the parent, or it's Simple,
    //in which case we need to check whether we to subdivide, or it's a quad, in which case
    //we don't do anything until we loop again around this while loop and go down into that quad.
    
    if( !*current_node ) { 
      //Okay, so we are using the style of the parent.
      //If we are not down to a single cell, then we will have to create a quad for current node,
      //and go down into it continously until there are no more quads to create.
      
      //The whole of this section is already this style.  No need to do anything.
      if( style == last_node->getStyle() ) return;

      if(quad_size == 1) {  //We are now on a single cell
	int num_null_children_in_parent = last_node->numNullChildren();

	Q_ASSERT(last_node->getStyle() != NULL);
	
        Q_ASSERT(num_null_children_in_parent > 0);
	if(num_null_children_in_parent == 1) {// We are the only one using the style info in parent, so just change the m_style in parent

	  last_node->setStyle(style);
	  simplify(path);
          Q_ASSERT( !last_node->m_isSimple);
	} else {  //someone else in the parent is using the style info in parent, so we have to create our own child
          last_node->makeChild(current_node);
	}
        return;
      }
      
      last_node->makeChild(current_node);

    }
  }
  
  return;
  
}

void StyleCluster::simplify(  QValueStack<StyleClusterQuad**> &path ) { 
  StyleClusterQuad** current_node;
  StyleClusterQuad* last_node = NULL;

  while (true) {
  
    if( path.isEmpty()) return;
    current_node = path.pop();
    if( !path.isEmpty() && path.top())
        last_node = *(path.top());
    
    Q_ASSERT( current_node && *current_node);
    
    if((*current_node)->m_bottomLeft && (*current_node)->m_bottomLeft->m_isSimple && (*current_node)->m_bottomLeft->getStyle() == (*current_node)->getStyle()) {
        delete (*current_node)->m_bottomLeft;
        (*current_node)->m_bottomLeft = 0;
    }
    if((*current_node)->m_bottomRight && (*current_node)->m_bottomRight->m_isSimple && (*current_node)->m_bottomRight->getStyle() == (*current_node)->getStyle()) {
        delete (*current_node)->m_bottomRight;
        (*current_node)->m_bottomRight = 0;
    }
    if((*current_node)->m_topLeft && (*current_node)->m_topLeft->m_isSimple && (*current_node)->m_topLeft->getStyle() == (*current_node)->getStyle()) {
        delete (*current_node)->m_topLeft;
        (*current_node)->m_topLeft = 0;
    }
    if((*current_node)->m_topRight && (*current_node)->m_topRight->m_isSimple && (*current_node)->m_topRight->getStyle() == (*current_node)->getStyle()) {
        delete (*current_node)->m_topRight;
        (*current_node)->m_topRight = 0;
    }
    
    
    if((*current_node)->numNullChildren() == 4) { //we can simplify - all children in the quad are pointing to use.
      (*current_node)->m_isSimple = true;
            
      if(!last_node) {
        
        Q_ASSERT( m_topQuad == *current_node );
    
        return;
      }
      if(last_node->getStyle() == (*current_node)->getStyle()) {
        //Parent has the same style, so delete us, then go back up recursively
        delete (*current_node);
        *current_node = 0;
      } else if(last_node->getStyle() == 0 ) {
        //the parent has only quads, and we are now the only simple one
        //so delete us, and set the style for the parent
        last_node->setStyle( (*current_node)->getStyle() );
        delete (*current_node);
        *current_node = 0; 
        return;
      } else if(last_node->numNullChildren() == 0) {
        //The style in the parent is different, but we are the only one using that style
        last_node->setStyle( (*current_node)->getStyle() );
        delete (*current_node);
        (*current_node) = 0;
      } else {
        //The style in the parent is different, and it has children using that style, so there is nothing we can do.
        return;
      }
    }
  }
}

const Style& StyleCluster::lookup(int x, int y) {
  return *(lookupNode(x,y)->getStyle());
}

void StyleCluster::stepDownOne(StyleClusterQuad **& current_node, int x, int & x_offset, int y, int & y_offset, int & quad_size) {

  quad_size /= 2;
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
}

StyleClusterQuad* StyleCluster::lookupNode(int x, int y) {
  //walk over quad-tree
  // see gnumeric sheet-style.c  cell_tile_apply_pos(...)
  //  these implementations rather differ though
  
  Q_ASSERT(m_topQuad);
  
  StyleClusterQuad** current_node = &m_topQuad;
  StyleClusterQuad* last_node = NULL;
  int x_offset = 0;
  int y_offset = 0;
  int quad_size = KS_Max_Quad;
  
  while ( *current_node && !(*current_node)->m_isSimple )
  {
    last_node = *current_node;
    //Below function modifies most of its parameters!
    stepDownOne(current_node, x, x_offset, y, y_offset, quad_size);
  }
  
  if( !(*current_node) ) return last_node;
  
  return *current_node;
}

}

#include "stylecluster.moc"
