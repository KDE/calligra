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

#ifndef KSPREADSTYLECLUSTER_H
#define KSPREADSTYLECLUSTER_H

#include <qobject.h>
#include <qvaluestack.h>

#include <koffice_export.h>

class KSpreadStyle;
class KSpreadSheet;
class KSpreadRange;

namespace KSpread {

class StyleClusterQuad;
class StyleClusterTester;

// class StyleManipulator;

/**
 * The StyleCluster class is an owner of a Quad tree made up of
 * StyleClusterQuad's.  It contains various functions to search the
 * quad tree and insert a new style for a cell at a given x,y.
 * 
 * Internally it always maintains the quad tree at a minimum size
 * possible.  Note that inserting ( @ref setStyle ) can be fairly expensive,
 * creating and deleting up to 8 objects in total.  So use the (yet to be
 * written) insert range functions to change large amounts of styles at a time.
 * 
 * Note that the style returned here is <i>not</i> necessarily
 * the final style for the cell, because the whole column and whole row
 * may have a style that need to be applied first.  Any settings in
 * those styles override the settings here.
 * 
 * See the StyleClusterQuad class for more implementation details.
 *
 * Testing
 * =======
 *   If you make any changes to the code, make sure to run the test suite.
 * To do this, run kspread and make a new empty document.  Then press ctrl+shift+T
 * and chose "cell style test".  Run that, and check that all the tests pass.
 *
 * If you add functionality, make sure to add new tests to the StyleclusterTest class
 * 
 * Manager
 * =======
 *   To be done - write a manager for this class for easier mass insertion.
 * 
 * @see StyleClusterQuad
 * @see StyleClusterTester
 * 
 * @author John Tapsell
 * @author Raphael Langerhorst
 */
class KSPREAD_EXPORT StyleCluster : public QObject
{
  Q_OBJECT

  protected:
    StyleClusterQuad* m_topQuad;
    KSpreadSheet* m_sheet;
    //To be written
    //friend class StyleManipulator;
    friend class StyleClusterTester;
    /**
     * An internally used function.  Takes a list of quads, each one being a child of the
     * one underneath it, with the last one being a pointer to m_topQuad.
     * Then it goes up this list and checks whether it can be simplified, and
     * does so if it's possible.
     * 
     * @param path This stack will be destroyed in this function. Make a copy if you want to keep it.
     */
    void simplify( QValueStack<StyleClusterQuad**> &path );

    /**
     * Return the quad being used at x,y.
     */
    StyleClusterQuad* lookupNode(int x, int y);
  public:
    StyleCluster(KSpreadSheet* sheet);

    ~StyleCluster();

    /**
     * Example usage: A cell should be changed to bold.
     * First, the current style is looked up with lookup(),
     * then a new KSpreadStyle is created based on the
     * returned style and set to bold. Then the style
     * should be checked against existing styles (use existing
     * one and delete your created one).
     * Now use your style to insert it into x,y.
     *
     * @param x zero-indexed column of the cell to set the style for
     * @param y zero-indexed row of the cell to set the style for
     * @param style The style to set it to.  Must be non-null.  To 'remove'
     *              a style, set the style to the sheet default style or something.
     */
    void setStyle( int x, int y, KSpreadStyle * style);

//     /**
//      * Practically same usage as above, but use a range to apply the style.
//      * TODO - implement
//      */
//     void insert( const KSpreadRange & range, const KSpreadStyle * style);

    /**
     * If you intend to modify this returned style, create
     * a new one based on the returned style.
     *
     * @param x zero-indexed column of the cell to set the style for
     * @param y zero-indexed row of the cell to set the style for
     * @return The style that this cell is using.  Note that this may
     *         not be the ultimate style that the cell is using because
     *         you have any column style and row style overrides this.
     * @see insert
     */
    const KSpreadStyle& lookup(int x, int y);

};

}

#endif
