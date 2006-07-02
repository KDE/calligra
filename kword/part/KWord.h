/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWORD_H
#define KWORD_H

/// The kword-global namespace for all KWord related things.
class KWord {
public:
    /// what should happen when the frame is full (too small for its contents)
    enum FrameBehavior {
        AutoExtendFrameBehavior,    ///< Make the frame bigger to fit the contents
        AutoCreateNewFrameBehavior, ///< Create a new frame on the next page
        IgnoreContentFrameBehavior  ///< Ignore the content and clip it
    };
    /// What happens on new page
    enum NewFrameBehavior {
        ReconnectNewFrame,  ///< On new page reconnect a copied from to the flow
        NoFollowupFrame,    ///< On new page don't create a new page
        CopyNewFrame        ///< On new page create a copy-frame and show that.
    };
    enum AnchorType {
        //TODO
// regarding anchoring of footnotes:
//14:06 < elvstone> in the best of worlds i would like 3 alternatives; 1) To anchor at bottom of page. 2) To anchor at bottom of last frame in frameset. 3) To anchor at bottom of frame.
    /* ideally the following properties could be given to any floating frame:
       Position: (y)
        Top of frame
        Top of paragraph
        Above current line
        At insertion point
        Below current line
        Bottom of paragraph
        Bottom of frame
        Absolute
       Alignment: (x)
        Left
        Right
        Center
        Closest to binding
        Further from binding
        Absolute */

    };

    /// The behavior other frames' text has when this property is set on a frame.
    enum RunAroundSide {
        BiggestRunAroundSide,   ///< Run other text around the side that has the most space
        LeftRunAroundSide,      ///< Run other text around the left side of the frame
        RightRunAroundSide      ///< Run other text around the right side of the frame
    };

    /// Each text frame set can be catogorized in one of these items
    enum TextFrameSetType {
        FirstPageHeaderTextFrameSet, ///< The frameSet that holds the header for the first page
        OddPagesHeaderTextFrameSet,  ///< The frameSet that holds the headers for the odd pages
        EvenPagesHeaderTextFrameSet, ///< The frameSet that holds the headers for the even pages
        FirstPageFooterTextFrameSet, ///< The frameSet that holds the footer for the first page
        OddPagesFooterTextFrameSet,  ///< The frameSet that holds the footers for the odd pages
        EvenPagesFooterTextFrameSet, ///< The frameSet that holds the footers for the even pages
        MainTextFrameSet,   ///< The frameset that holds all the frames for the main text area
        OtherTextFrameSet   ///< Any other text frameset not managed by the auto-frame layout
    };

    /// used in KWPageSettings to determine if, and what kind of header/footer to use
    enum HeaderFooterType {
        HFTypeNone,       ///< Don't show the frames
        HFTypeEvenOdd,    ///< Show different content for even and odd pages
        HFTypeUniform,    ///< Show the same content for each page
        HFTypeSameAsFirst ///< Show the same content for each page, including the first page
    };

};

#endif
