/***********************************************************************
 *                                                                      *
 * (C) 2006, Frank Lemke, Computer Architecture Group,                  *
 * University of Mannheim, Germany                                      *
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program; if not, write to the Free Software          *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 *
 * USA                                                                  *
 *                                                                      *
 * For informations regarding this file contact                         *
 *			      office@mufasa.informatik.uni-mannheim.de *
 *                                                                      *
 ***********************************************************************/

/*!
 \file trackpoint.h
 \brief Trackpoint

 \author Frank Lemke
 \date 2006-02-27
 */

#ifndef TRACKPOINT_H_
#define TRACKPOINT_H_

// Includes
//---------------

//-- Std
#include <utility>
#include <string>
#include <cstdlib>

using namespace std;

//-- Core
class TransitionBase;
class Join;
class Link;

/*!
 \brief Trackpoint-object

 First of all it is a point of a Transition. But it is also
 possible to connect it to a Link and provides the functionality
 to expand this transitionpoint to a hierarchical-state.
 */
class Trackpoint {

    protected:

        /// The transition this trackpoint is related to
        TransitionBase *         transition;

        /// The link id that is targetting (0 means not a link)
        Link * targetLink;

        /// The Join that is targetted
        Join *   join;

        /// The color of the trackpoint
        unsigned int            color;

        /// Position of the trackpoint
        pair<double, double> position;


    public:

        Trackpoint(double posx, double posy,TransitionBase * transition = NULL);
        virtual ~Trackpoint();


        /** \defgroup Properties */
        /** @{ */

        TransitionBase * getTransition();

        void setTransition(TransitionBase * transition);

        /**
         * @warning Not a reference! Modifying the result will not modify the source
         * @return Position pair (first = x, second = y)
         */
        pair<double,double> getPosition();

        void setPosition(pair<double,double> newPosition);

        unsigned int getColor();

        void setColor(unsigned int color);

        void setTargetLink(Link* targetLink);

        Link* getTargetLink();

        /**
         *
         * @return true if this trackpoint is targeting a link
         */
        bool isLink();

        bool isJoin() const;

        /**
         *
         * @param joinId The ID of the targeted join, 0 means not targeting a join
         */
        void setJoin(Join *);

        /**
         *
         * @return ID of targeted Join, 0 means not targeting a join
         */
        Join * getJoin();

        /** @} */

        /**
         *
         * @return Trackpoint* - Pointer to the Trackpoint after this one, or NULL if none
         * @see{Trans#deleteTrackpoint}
         */
        Trackpoint* remove();

};

#endif
