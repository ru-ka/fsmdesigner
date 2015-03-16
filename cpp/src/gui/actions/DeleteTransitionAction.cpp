/**
 * \file DeleteTransitionAction.cpp
 *
 * 
 *
 */

// Includes
//--------------------

//-- Std
#include <iostream>
#include <ostream>
#include <sstream>
using namespace std;


//-- Qt
#include <QtWidgets>
#include <QtCore>

//-- Actions
#include <gui/actions/ItemFocusedAction.h>

//-- Core
#include <core/Fsm.h>
#include <core/Trans.h>
#include <core/State.h>
#include <core/Trackpoint.h>
#include <core/Join.h>

//-- Gui
#include <gui/items/FSMGraphicsItem.h>
#include <gui/scene/Scene.h>
#include <gui/items/Transline.h>
#include <gui/items/TranslineText.h>
#include <gui/items/StateItem.h>
#include <gui/items/TrackpointItem.h>
#include <gui/items/LinkDeparture.h>
#include <gui/items/JoinItem.h>

//-- Common
#include <gui/common/SGC.h>

//-- Actions
#include <gui/actions/DeleteTrackpointAction.h>

#include "DeleteTransitionAction.h"

DeleteTransitionAction::DeleteTransitionAction(Transline * item,QUndoCommand * parentCommand) :ItemFocusedAction<Transline>(item,parentCommand) {


    //-- Delete Trackpoints Using Children actions
    //-----------------
    /*QList<TrackpointItem *> trackpoints = this->getRelatedScene()->findTransitionBaseTrackpoint(this->item->getModel());

    //-- If none, already, we are adding
    if (trackpoints.size()==0) {
        FOREACH_TRANSITION_TRACKPOINTS(item->getModel())

        DeleteTrackpointAction * deleteTrackpoint = new DeleteTrackpointAction(new TrackpointItem(trackpoint,),this);

        END_FOREACH_TRANSITION_TRACKPOINTS
    } else {

        QList<TrackpointItem *> trackpoints = this->getRelatedScene()->findTransitionBaseTrackpoint(this->item->getModel());
        while (!trackpoints.isEmpty()) {



        }
    }*/



}

DeleteTransitionAction::~DeleteTransitionAction() {

}

int DeleteTransitionAction::id() {
    return ItemFocusedAction<Transline>::id();
}
bool DeleteTransitionAction::mergeWith(const QUndoCommand * command) {
    // Call parent
    return ItemFocusedAction<Transline>::mergeWith(command);
}
void DeleteTransitionAction::redo(){


    //-- Delete Trackpoints Using action
    //-----------------
    //ItemFocusedAction<Transline>::redo();

    //ItemFocusedAction<Transline>::redo();


    //-- FIXME If the far end is a LinkDeparture, uses a delete Link Item object not delete Transition
    //---------------------------

    //-- Do try to delete
    //cout << "Redoing deleted transition with id: " << this->id() << endl;

    //-- Activate Garbage collector
    SGC::getInstance()->activate();

    // Remove from gui by propagating
    //----------------------

    //-- First find the start of the transition
    Trans * transition = (Trans*)this->item->getModel();
    Transline * first = NULL;

    QList<Transline*> translines = this->getRelatedScene()->findTransline(transition);
    while (!translines.isEmpty()) {
        Transline * cTransline = translines.takeFirst();
        if (cTransline->getStartItem()->type() == StateItem::Type) {
            first = cTransline;
            break;
        }
    }

    if (first==NULL) {
        qDebug() << "First transline not found!" << endl;
        SGC::getInstance()->collect();
        return;
    }

//    while (first->getStartItem()!=NULL && first->getStartItem()->type() != StateItem::Type) {
//        //-- Prev is a trackpoint, jump to trackpoint prev transition
//        if (FSMGraphicsItem<>::isTrackPoint(first->getStartItem())) {
//            first
//                    = FSMGraphicsItem<>::toTrackPoint(first->getStartItem())->getPreviousTransline();
//        }
//    }


    //-- get the associated state from first transline
    //  if (this->startItem==NULL) {
    StateItem * startItem = FSMGraphicsItem<>::toStateItem(first->getStartItem());

    //  }

    QGraphicsItem * endItem = NULL;

    //-- Forward and use Garbage Collector to safe delete trackpoints and such
    Transline * nextTransline = first;
    do {

        //-- If transline is linked to a trackpoint, delete the trackpoint
        if (FSMGraphicsItem<>::isTrackPoint(nextTransline->getEndItem())) {

            //-- Delete
            TrackpointItem * trackPoint = FSMGraphicsItem<>::toTrackPoint(
                    nextTransline->getEndItem());
            this->getRelatedScene()->removeItem(trackPoint);

            //SGC::getInstance()->requestDelete(trackPoint);

            //-- Next transline to consider is the prev of the next trackpoint, or none if no next trackpoint
            if (FSMGraphicsItem<>::isTrackPoint(trackPoint->getEndItem())) {
                nextTransline = FSMGraphicsItem<>::toTrackPoint(
                        trackPoint->getEndItem())->getPreviousTransline();
            } else {

                //-- Record end State item if we are on one
                //              if (this->endItem==NULL && FSMGraphicsItem<>::isStateItem(trackPoint->getEndItem())) {
//              this->endItem = FSMGraphicsItem<>::toStateItem(
//                      trackPoint->getEndItem());
                endItem = trackPoint->getEndItem();
                //              }

                //-- No more next transline
                nextTransline = NULL;

            }
        }
        //-- If transline if linked to a Link Item, delete it too if there are no more transline going to it then
        else if (nextTransline->getEndItem()->type()==LinkDeparture::Type) {

            //-- Remove Link Item
            this->getRelatedScene()->removeItem(nextTransline->getEndItem());
            //SGC::getInstance()->requestDelete(nextTransline->getEndItem());
            endItem = NULL;

            //-- Delete transline and stop there
            this->getRelatedScene()->removeItem(nextTransline);
            //SGC::getInstance()->requestDelete(nextTransline);
            nextTransline = NULL;
        }
        else {
            //-- No more next transline because no trackpoint.
            //-- Only delete transline and end there
            endItem = nextTransline->getEndItem();
            this->getRelatedScene()->removeItem(nextTransline);
            //SGC::getInstance()->requestDelete(nextTransline);
            nextTransline = NULL;

        }
    } while (nextTransline != NULL);


    // FIXME (Find out text using a search function) Remove text
    //------------------------
    QGraphicsItem * textItem = this->getRelatedScene()->findTranslineText(transition);
    if (textItem!=NULL) {
        this->getRelatedScene()->removeItem(textItem);
        SGC::getInstance()->requestDelete(textItem);
    }


    // Remove Model
    //------------------------
    this->getRelatedScene()->getFsm()->deleteTrans(transition);

    //-- Store Item as a dummy Item as Transline are always reconstructed
    //SGC::getInstance()->requestDelete(this->item);

    //-- Collect all removed items
    SGC::getInstance()->collect();



    //-- Reset item with a dummy one because the one from action might have been deleted
    this->item = new Transline(transition);

    qDebug() << "Saved model: " << QString::fromStdString(this->item->getModel()->getName());

    // Call parent
    //ItemFocusedAction<Transline>::redo();
}
void DeleteTransitionAction::undo(){


    // Model
    //----------------------
    this->getRelatedScene()->getFsm()->addTrans((Trans*)this->item->getModel());
    Trans * transition = (Trans*)this->item->getModel();

    // Gui
    // Rebuild the complete transition
    //------------

    // Get Trans & start state
    Transline * lastTransitionLine = NULL;

    State * currentState  =transition->getStartState();
    StateItem * stateItem = this->getRelatedScene()->findStateItem(currentState);

    Fsm * currentFSM = this->getRelatedScene()->getFsm();

    //-- Calculate the start and end as a trackpoints

    // Start
    Trackpoint * start = new Trackpoint(currentState->getPosition().first,
            currentState->getPosition().second, transition);

    // End
    State * endState = transition->getEndState();

    // End GUI Item
    QGraphicsItem * endItem = endState!=NULL ? this->getRelatedScene()->findStateItem(endState) : NULL;

    //-- Trace translines from start to end with trackpoints intermediaries
    Trackpoint * previous = start;
    //Trackpoint * previous = NULL;
    QGraphicsItem * previousItem = stateItem;
    FOREACH_TRANSITION_TRACKPOINTS(transition)

        // Place trackpoint
        //----------------------

        //-- Trackpoint is a link
        if (trackpoint->isLink()) {

            //-- Create Item If there is none
            LinkDeparture * linkDeparture = NULL;
            if (endItem!=NULL && endItem->type() == FSMGraphicsItem<>::LINKDEPARTURE)
                linkDeparture = dynamic_cast<LinkDeparture*>(endItem);
            else
                linkDeparture = new LinkDeparture(trackpoint,previousItem);

            //-- Position in space and on Scene
            linkDeparture->setPos(trackpoint->getPosition().first, trackpoint->getPosition().second);
            this->getRelatedScene()->addItem(linkDeparture);

            //-- Stop going further by nulling previous thing
            previous = NULL;
            previousItem = NULL;

            //-- Record as End to place last transition line
            endItem = linkDeparture;

            break;

        }
        //-- Trackpoint points to a join
        else if (trackpoint->getJoin()!=NULL) {

            //---- Don't add a trackpoint, but set End Item to the join
            //---------------------

            //-- Get Join, position, and find the graphic Item
            Join * join = trackpoint->getJoin();
            JoinItem* JoinItem = this->getRelatedScene()->findJoinItem(join);

            if (JoinItem==NULL) {
                qDebug() << "Looking for join point Graphics Item return NULL";
            }
            endItem = JoinItem;


        }
        //-- Normal case
        else {

            //-- Add trackpoint between previous and nothing (because we don't know yet)
            TrackpointItem * trackpointItem = new TrackpointItem(trackpoint,
                    previousItem, NULL);
            trackpointItem->setPos(trackpoint->getPosition().first, trackpoint->getPosition().second);
            this->getRelatedScene()->addItem(trackpointItem);

            //-- If previous is a trackpoint, this one is the next of previous
            if (FSMGraphicsItem<>::isTrackPoint(previousItem)) {
                FSMGraphicsItem<>::toTrackPoint(previousItem)->setEndItem(
                        trackpointItem);
            }

            //-- Record as previous
            previous = trackpoint;
            previousItem = trackpointItem;

        }
    END_FOREACH_TRANSITION_TRACKPOINTS


    //-- Place last transline between previous and end if we this
    // No previous means we don't want to trace between target and previous (like a link)
    if (previous != NULL) {

        //-- If Previous is a state, simply place a transline
        if (FSMGraphicsItem<>::isStateItem(previousItem)) {
            lastTransitionLine = new Transline(transition, previousItem,
                    endItem);
            lastTransitionLine->setEndItem(endItem);
            this->getRelatedScene()->addItem(lastTransitionLine);

        }
        //-- If previous is a trackpoint, the state is the end item
        else if (FSMGraphicsItem<>::isTrackPoint(previousItem)) {

            //-- Set state as next item of trackpoint
            FSMGraphicsItem<>::toTrackPoint(previousItem)->setEndItem(
                    endItem);

            //-- The last transline is the next transline of the trackpoint then
            lastTransitionLine
                    = FSMGraphicsItem<>::toTrackPoint(previousItem)->getNextTransline();

        }

        //-- Add transline text
        TranslineText * transitionLineText = new TranslineText(QString(
                transition->getName().c_str()),transition);
        transitionLineText->setPos(transition->getTextPosition().first,
                transition->getTextPosition().second);
        this->getRelatedScene()->addItem(transitionLineText);

    }


    //-- Store Item as a dummy Item as Transline are always reconstructed
    Transline * tempItem = new Transline(transition);
    this->item = tempItem;

}


