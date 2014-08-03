// Includes
//---------------

//-- Std
#include <iostream>
using namespace std;

// -- Qt
#include <QtGui>
#include <QtScript/QScriptEngine>
#include <QtCore>

//-- Items
#include <gui/items/FSMGraphicsItem.h>
#include <gui/items/StateItem.h>
#include <gui/items/TrackpointItem.h>
#include <gui/items/Transline.h>
#include <gui/items/TranslineText.h>
#include <gui/items/LinkArrival.h>
#include <gui/items/LinkDeparture.h>
#include <gui/items/JoinItem.h>
#include <gui/items/HyperTransition.h>

//-- Undo
#include <gui/actions/DeleteJoinAction.h>
#include <gui/actions/DeleteLinkArrivalAction.h>
#include <gui/actions/DeleteTransitionAction.h>
#include <gui/actions/DeleteHyperTransitionAction.h>

//-- Commands
//-- State
#include <gui/commands/NewStateCommand.h>
#include <gui/commands/RemoveStateCommand.h>
#include <gui/commands/MoveStateCommand.h>
//-- ItemGroup
#include <gui/commands/CreateItemGroupCommand.h>
#include <gui/commands/RemoveItemGroupCommand.h>
#include <gui/commands/MoveItemGroupCommand.h>
//-- Trans
#include <gui/commands/NewTransCommand.h>
//-- HyperTrans
#include <gui/commands/NewHyperTransCommand.h>
//-- Join
#include <gui/commands/NewJoinCommand.h>
//-- Link
#include <gui/commands/NewLinkCommand.h>

//-- Others
#include <gui/common/GUIUtils.h>

//-- Verification
#include <gui/verify/FSMVerificator.h>
#include <verification/Verificator.h>
#include <verification/StateOutputsRule.h>
#include <verification/VerificatorRule.h>

//-- Core
#include <core/FSMDesigner.h>
#include <core/Fsm.h>
#include <core/Core.h>
#include <core/State.h>
#include <core/Link.h>
#include <core/Join.h>
#include <core/Trackpoint.h>
#include <core/Trans.h>

using namespace FSMDesigner;

#include "Scene.h"
Scene::Scene(Fsm * fsm, QObject *parent) :
  QGraphicsScene(-5000,-5000,10000,10000,parent),
  bLastCommand( false ),
  activeGroup( NULL ),
  activeTransCommand( NULL ),
  activeHyperTransCommand( NULL ),
  activeJoinCommand( NULL ),
  activeLinkCommand( NULL )
{

  this->setBackgroundBrush(Qt::white);
  //this->setBackgroundBrush(QPixmap());
  //this->setStickyFocus(true);

  // Variables
  //-------------
  this->fsm = fsm;

  //-- Place
  this->placeMode = FSMDesigner::NONE;
  this->placeLock = FSMDesigner::UNLOCKED;

  // Variables
  //------------------
  this->placeLinkEndState   = NULL;
  this->placeLinkStartState = NULL;

  // Verification
  //---------------
  this->setFSMVerificator(new FSMVerificator(this));

  connect( this, SIGNAL(selectionChanged()), this, SLOT(receivedSelectionChanged()));
}

void Scene::initializeScene() {

  // Clean scene
  //----------
  this->clearSelection();
  this->clear();


  // Draw FSM
  //------------------

  //-- Get current FSM to draw
  Fsm * currentFSM = this->fsm;

  // Prepare some data
  //------------------------------

  //-- Created items map (id<->object), and Join map
  map<unsigned int, StateItem*> statesMap;
  map<unsigned int, JoinItem* > joinsMap;

  // Place Joins
  //--------------------
  FOREACH_JOIN(this->fsm)
    //-- Prepare Item
    JoinItem * joinItem = new JoinItem((*it).second);
    joinsMap[(*it).first] = joinItem;
    this->addItem( joinItem );
  END_FOREACH_JOIN

  // Loop over states until no more
  //-------------------
  FOREACH_STATE(currentFSM)

    // Get Current State
    State * currentState = state;
    if (currentState == NULL) {
      qDebug() << "currentState == NULL. Should never occur, if the fsm is correct.";
      continue;
    }

    //-- Create a GUI state if necessary
    StateItem * stateItem = statesMap.find(currentState->getId())!=statesMap.end() ? statesMap[currentState->getId()] : NULL;
    if (stateItem == NULL) {
      stateItem = new StateItem(currentState);
      statesMap[currentState->getId()] = stateItem;
      this->addItem(stateItem);
    }

    // Create transitions from this item
    //-----------
    /*
    Transline * lastTransitionLine = NULL;
    list<Trans*> stateStartingTransitions =
        currentState->getStartingTransitions();
    list<Trans*>::iterator i;
    for (i = stateStartingTransitions.begin(); i
        != stateStartingTransitions.end(); i++) {

      // Get Trans
      Trans * transition = *i;
      lastTransitionLine = NULL;

      // End
      //currentFSM->getStatebyID(transition->end);
      //State * endState = currentFSM->getCurrentState();
      //currentFSM->getStatebyID(currentState->sid);
      State * endState = transition->getEndState();

      //-- Find/create end Gui item
      StateItem * endStateItem = statesMap.find(endState->getId())!=statesMap.end() ? statesMap[endState->getId()] : NULL;
      if (endStateItem == NULL) {
        endStateItem = new StateItem(endState);
        statesMap[endState->getId()] = endStateItem;
        this->addItem(endStateItem);
      }

      //-- Use Undo to place on the scene
      Transline * lineToAdd = new Transline((TransitionBase*)transition);
      DeleteTransitionAction * undoTransition = new DeleteTransitionAction(lineToAdd);
      //undoTransition->setStartItem(stateItem);
      //undoTransition->setEndItem(endStateItem);
      undoTransition->setRelatedScene(this);
      undoTransition->undo();
      delete undoTransition;
      delete lineToAdd;

    } // ENd OF transitions loop
    */


  END_FOREACH_STATE
  //-- END LOOP OVER STATES
  //
  FOREACH_JOIN(this->fsm)
    //-- Prepare Item
    Q_ASSERT( joinsMap.find( (*it).first ) != joinsMap.end() );
    JoinItem * startItem = joinsMap[(*it).first];
    auto endStateId = join->getTargetState()->getId();
    Q_ASSERT( statesMap.find( endStateId ) != statesMap.end() );
    QGraphicsItem * endItem   = statesMap[ endStateId ];
    auto trackpoints = join->getTrackpoints();
    if ( trackpoints.size() == 0 ) {
      Transline * currentTransItem = new Transline( NULL, startItem, endItem );
      this->addItem( currentTransItem );
    } else {
      // ------------------------------
      // CASE 1 : endItem == trackpoint
      // ------------------------------
      if ( !trackpoints.back()->isJoin() ) {
        QList<Transline      *> transList;
        Transline * currentTransItem = NULL;
        currentTransItem = new Transline( NULL, startItem, NULL );
        transList.append( currentTransItem );
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          TrackpointItem * item = new TrackpointItem( currentPoint );
          trackList.append( item );
          item->setPreviousTransline( currentTransItem );
          // 2) Update current TransItem and create additional TranslineItems.
          currentTransItem->setEndItem( item );
          currentTransItem = new Transline( NULL, item, NULL );
          transList.append( currentTransItem );
          item->setNextTransline( currentTransItem );
        }
        currentTransItem->setEndItem( endItem );
        // 4) Add items to scene.
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
      // ------------------------------
      // CASE 2 : endItem == join
      // ------------------------------
      else {
        Q_ASSERT( trackpoints.back()->isJoin() );
        auto endJoinId = trackpoints.back()->getJoin()->getId();
        Q_ASSERT( joinsMap.find( endJoinId ) != joinsMap.end() );
        endItem = joinsMap[endJoinId];
        QList<Transline      *> transList;
        Transline * currentTransItem = NULL;
        currentTransItem = new Transline( NULL, startItem, NULL );
        transList.append( currentTransItem );
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          if ( !currentPoint->isJoin() ) {
            TrackpointItem * item = new TrackpointItem( currentPoint );
            trackList.append( item );
            item->setPreviousTransline( currentTransItem );
            // 2) Update current TransItem and create additional TranslineItems.
            currentTransItem->setEndItem( item );
            currentTransItem = new Transline( NULL, item, NULL );
            transList.append( currentTransItem );
            item->setNextTransline( currentTransItem );
          }
        }
        currentTransItem->setEndItem( endItem );
        // 4) Add items to scene.
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
    }
  END_FOREACH_JOIN

  // TODO: Implement a function? or a command? for this task?
  // Iterate over the transitions and create the required gui-items.
  // TODO: Since the underlying data structures are not carefully chosen, it is
  // hard to control the translation from the gui structure to the backend
  // structure. Simplify the data structures and improve the maintainability.
  //
  //
  // IMPORTANT!!!
  // THIS CODE SHOULD EXPLICITLY ADDRESS ALL CASES INDUCED BY THE INEFFICIENT
  // DATA STRUCTURE.
  // THIS CODE IS NOT FOR MAINTAINABILITY.
  // THIS CODE WILL CHANGE LATER ON WITH MORE TIME - HOPEFULLY.
  // OTHERWISE, HAVE FUN WITH IT ;).
  //
  auto transitions = fsm->getTransitions();
  for ( auto trans : transitions ) {
    qDebug() << "Constructing transition.";
    // Since the underlying data structure is a mess, implement all cases
    // seperatly.
    // TODO: Redesign the data structure or live with its negative impacts!
    auto startStateId = trans.second->getStartState()->getId();
    Q_ASSERT( statesMap.find(startStateId) != statesMap.end() );
    QGraphicsItem * startItem = statesMap[startStateId];
    auto endStateId   = trans.second->getEndState()->getId();
    Q_ASSERT ( statesMap.find(endStateId) != statesMap.end() );
    QGraphicsItem * endItem = statesMap[endStateId];

    // Get the start/end items.
    auto trackpoints  = trans.second->getTrackpoints();
    qDebug() << "trans.second->getName() = " << trans.second->getName().c_str();
    qDebug() << "trackpoints.size() = " << trackpoints.size();

    if ( trackpoints.size() == 0 ) {
      Transline * currentTransItem = 
        new Transline( trans.second, startItem, endItem );
        this->addItem( currentTransItem );
        TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        text->setPos( trans.second->getTextPosition().first,
                      trans.second->getTextPosition().second );
    } else if ( trackpoints.size() == 1 ) {
      qDebug() << "trackpoints->size() == 1";
      qDebug() << "Not implemented yet.";
      // -------------------------
      // CASE 1: NORMAL TRACKPOINT
      // -------------------------
      if ( !trackpoints.front()->isJoin() && !trackpoints.front()->isLink() ) {
        QList<Transline      *> transList;
        Transline * currentTransItem = NULL;
        currentTransItem = new Transline( trans.second, startItem, NULL );
        transList.append( currentTransItem );
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          TrackpointItem * item = new TrackpointItem( currentPoint );
          trackList.append( item );
          item->setPreviousTransline( currentTransItem );
          // TODO: is it important to set start/end items of trackpoints?
          // 2) Update current TransItem and create additional TranslineItems.
          currentTransItem->setEndItem( item );
          currentTransItem = new Transline( trans.second, item, NULL );
          transList.append( currentTransItem );
          item->setNextTransline( currentTransItem );
        }
        currentTransItem->setEndItem( endItem );
        // 3) Create transition text.
        TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        text->setPos( trans.second->getTextPosition().first,
                      trans.second->getTextPosition().second );
        // 4) Add items to scene.
        this->addItem( text );
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
      // -----------------------------
      // END CASE 1: NORMAL TRACKPOINT
      // -----------------------------
      // -----------------------
      // CASE 2: JOIN TRACKPOINT
      // -----------------------
      else if ( trackpoints.front()->isJoin() ) {
        qDebug() << "trackpoints->front()->isJoin(). Not implemeted yet.";
        // 1 Get JoinItem.
        auto joinId = trackpoints.front()->getJoin()->getId();
        Q_ASSERT( joinsMap.find(joinId) != joinsMap.end() );
        JoinItem * joinItem = joinsMap[joinId];
        Transline * currentTransItem = 
          new Transline( trans.second, startItem, joinItem );
        // 2) Create transition text.
        TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        text->setPos( trans.second->getTextPosition().first,
                      trans.second->getTextPosition().second );
        // 3) Add transline and text to scene.
        this->addItem( currentTransItem );
        this->addItem( text );
      }
      // --------------------------
      // END CASE 2: JOIN TRACKPOINT
      // --------------------------
      // -----------------------
      // CASE 3: LINK TRACKPOINT
      // -----------------------
      else if ( trackpoints.front()->isLink() ) {
        qDebug() << "trackpoints->front()->isLink(). Not implemeted yet.";
        LinkDeparture * linkItem = new LinkDeparture( trackpoints.front(), startItem );
        Transline * currentTransItem = 
          new Transline( trans.second, startItem, linkItem );
        this->addItem( linkItem );
        this->addItem( currentTransItem );
      }
      // ---------------------------
      // END CASE 3: LINK TRACKPOINT
      // ---------------------------
    } else {
      // ---------------------------------
      // CASE 1: STATE to STATE transition
      // ---------------------------------
      if ( !trackpoints.front()->isJoin() && !trackpoints.front()->isLink()
          && !trackpoints.back()->isJoin() && !trackpoints.back()->isJoin() ) {
        QList<Transline      *> transList;
        Transline * currentTransItem = NULL;
        currentTransItem = new Transline( trans.second, startItem, NULL );
        transList.append( currentTransItem );
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          TrackpointItem * item = new TrackpointItem( currentPoint );
          trackList.append( item );
          item->setPreviousTransline( currentTransItem );
          // TODO: is it important to set start/end items of trackpoints?
          // 2) Update current TransItem and create additional TranslineItems.
          currentTransItem->setEndItem( item );
          currentTransItem = new Transline( trans.second, item, NULL );
          transList.append( currentTransItem );
          item->setNextTransline( currentTransItem );
        }
        currentTransItem->setEndItem( endItem );
        // 3) Create transition text.
        TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        text->setPos( trans.second->getTextPosition().first,
                      trans.second->getTextPosition().second );
        // 4) Add items to scene.
        this->addItem( text );
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
      // -------------------------------------
      // END CASE 1: STATE to STATE transition
      // -------------------------------------



      // ----------------------------------------
      // CASE 2: STATE to JOIN or LINK transition
      // ----------------------------------------
      else if ( !trackpoints.front()->isJoin() && !trackpoints.front()->isLink()
         && ( trackpoints.back()->isJoin() || trackpoints.back()->isLink() ) ) {
        qDebug() << "State to join or link transition";
        QList<Transline      *> transList;
        Transline * currentTransItem = NULL;
        currentTransItem = new Transline( trans.second, startItem, NULL );
        transList.append( currentTransItem );
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          if ( currentPoint->isJoin() || currentPoint->isLink() )
            break;
          TrackpointItem * item = new TrackpointItem( currentPoint );
          trackList.append( item );
          item->setPreviousTransline( currentTransItem );
          // TODO: is it important to set start/end items of trackpoints?
          // 2) Update current TransItem and create additional TranslineItems.
          currentTransItem->setEndItem( item );
          currentTransItem = new Transline( trans.second, item, NULL );
          transList.append( currentTransItem );
          item->setNextTransline( currentTransItem );
        }
        // Adjust enditem in the local scope.
        QGraphicsItem * endItem = NULL;
        if ( trackpoints.back()->isJoin() )
          endItem = joinsMap[trackpoints.back()->getJoin()->getId()];
        else {
          endItem = new LinkDeparture( trackpoints.back() );
          this->addItem( endItem );
          qDebug() << "LinkDeparture = " << endItem;
        }

        currentTransItem->setEndItem( endItem );
        // 3) Create transition text.
        TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        text->setPos( trans.second->getTextPosition().first,
                      trans.second->getTextPosition().second );
        // 4) Add items to scene.
        this->addItem( text );
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
      // --------------------------------------------
      // END CASE 2: STATE to JOIN or JOIN transition
      // --------------------------------------------
      


      // --------------------------------
      // CASE 3: JOIN to STATE transition
      // --------------------------------
      else if ( trackpoints.front()->isJoin()
          && !trackpoints.back()->isJoin() && !trackpoints.back()->isLink() ) {
        qDebug() << "join or link to state transition";
        QList<Transline      *> transList;
        Transline * currentTransItem = NULL;
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          if ( currentPoint->isJoin() ) {
            auto joinId = currentPoint->getJoin()->getId();
            Q_ASSERT( joinsMap.find( joinId ) != joinsMap.end() );
            startItem = joinsMap[joinId];
            currentTransItem = new Transline( trans.second, startItem, NULL );
            transList.append( currentTransItem );
          } else {
            TrackpointItem * item = new TrackpointItem( currentPoint );
            trackList.append( item );
            item->setPreviousTransline( currentTransItem );
            // 2) Update current TransItem and create additional TranslineItems.
            currentTransItem->setEndItem( item );
            currentTransItem = new Transline( trans.second, item, NULL );
            transList.append( currentTransItem );
            item->setNextTransline( currentTransItem );
          }
        }
        currentTransItem->setEndItem( endItem );
        // 3) Create transition text.
        TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        text->setPos( trans.second->getTextPosition().first,
                      trans.second->getTextPosition().second );
        // 4) Add items to scene.
        this->addItem( text );
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
      // ------------------------------------
      // END CASE 3: JOIN to STATE transition
      // ------------------------------------



      // -------------------------------
      // CASE 4: JOIN to JOIN transition
      // -------------------------------
      else if ( trackpoints.front()->isJoin() && trackpoints.back()->isJoin() ) {
        qDebug() << "join or link to join or link transition";
        auto joinStartID = trackpoints.front()->getJoin()->getId();
        auto joinEndId   = trackpoints.back() ->getJoin()->getId();
        Q_ASSERT( joinsMap.find( joinStartID ) != joinsMap.end() );
        Q_ASSERT( joinsMap.find( joinEndId   ) != joinsMap.end() );
        startItem = joinsMap[ joinStartID ];
        endItem   = joinsMap[ joinEndId   ];

        Transline * currentTransItem = NULL;
        currentTransItem = new Transline( trans.second, startItem, NULL );

        QList<Transline      *> transList;
        transList.append( currentTransItem );
        QList<TrackpointItem *> trackList;
        for ( auto currentPoint : trackpoints ) {
          if ( !currentPoint->isJoin() ) {
            TrackpointItem * item = new TrackpointItem( currentPoint );
            trackList.append( item );
            item->setPreviousTransline( currentTransItem );
            // TODO: is it important to set start/end items of trackpoints?
            // 2) Update current TransItem and create additional TranslineItems.
            currentTransItem->setEndItem( item );
            currentTransItem = new Transline( trans.second, item, NULL );
            transList.append( currentTransItem );
            item->setNextTransline( currentTransItem );
          }
        }
        currentTransItem->setEndItem( endItem );
        // 3) Create transition text.
        //TranslineText * text = new TranslineText( trans.second->getName().c_str(), trans.second );
        //text->setPos( trans.second->getTextPosition().first,
        //              trans.second->getTextPosition().second );
        // 4) Add items to scene.
        //this->addItem( text );
        for ( auto item : trackList ) {
          this->addItem( item );
        }
        for ( auto item : transList ) {
          this->addItem( item );
        }
      }
      // -------------------------------------------
      // END CASE 4: JOIN or LINK to JOIN transition
      // -------------------------------------------
      //
      else {
        qDebug() << "Not implemented yet. Most likely invalid state.";
        Q_ASSERT( false );
      }
    }
  }
  // End Transitions.

  // Loop over links entrances
  //----------------------------
  FOREACH_LINKS(currentFSM)

        //-- Use Undo Class to place
        LinkArrival * linkArrival = new LinkArrival(link,statesMap[link->getTargetState()->getId()]);
        DeleteLinkArrivalAction * undoLinkArrival = new DeleteLinkArrivalAction(linkArrival);
        undoLinkArrival->setRelatedScene(this);
        undoLinkArrival->undo();

  END_FOREACH_LINKS

  // Place Hypertransitions
    //---------------------------
    FOREACH_HYPERTRANSITIONS(this->fsm)

        //-- Prepare Item
        HyperTransition * hypertransitionItem = new HyperTransition(hypertransition);

        //-- Use undo to place on the scene
        DeleteHyperTransitionAction * undoHypertransition = new DeleteHyperTransitionAction(hypertransitionItem);
        undoHypertransition->setRelatedScene(this);
        undoHypertransition->undo();
        delete undoHypertransition;

    END_FOREACH_HYPERTRANSITIONS

    this->placeMode = FSMDesigner::CHOOSE;

}

QUndoStack * Scene::getUndoStack() {
  return &(this->undoStack);
}

/**
 * Propagate Undo Redo
 */
void Scene::undo() {
    this->setPlaceMode(FSMDesigner::NONE);
    this->undoStack.undo();
    this->setPlaceMode(FSMDesigner::CHOOSE);
}

/**
 * Propagate Undo Redo
 */
void Scene::redo() {
    this->setPlaceMode(FSMDesigner::NONE);
    this->undoStack.redo();
    this->setPlaceMode(FSMDesigner::CHOOSE);
}

void Scene::addToToPlaceStack(QGraphicsItem * item) {

  //-- If the stack is not empty,
  //---------------------------

  //-- Apply rules to items

  // Not visible and not enabled
  this->addItem(item);
  item->setVisible(false);
  item->setEnabled(false);


  //-- Add to stack
  this->toPlaceStack.push_front(item);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *e) {

  QGraphicsScene::mousePressEvent(e);

  //-- First element has to be placed, removed from stack and reenabled
  if (this->toPlaceStack.size() > 0) {

    qDebug() << "Mouse Press, releasing to be placed element:"
        << e->scenePos();

    //-- If the first is a transline, we don't really want to place it
    //-- Invisible elements are not yet to be considered. They will be made visible after first movement
    if (!this->toPlaceStack.first()->isVisible()) {

    } else if (this->toPlaceStack.first()->type() == Transline::Type) {

      QGraphicsItem * tr = this->toPlaceStack.takeFirst();

      //this->toPlaceStack.first()->setVisible(false);

      //-- only ensure the line is enabled
      tr->setEnabled(true);


      //-- If the transline does not lead to anywhere, and don't be long to a trackpoint delete
      /*
      if (FSMGraphicsItem<>::toTransline(tr)->getEndItem()==NULL &&
          !FSMGraphicsItem<>::isTrackPoint(FSMGraphicsItem<>::toTransline(tr)->getStartItem() )) {
        delete tr;
      }
      */
    } else {

      // Get and remove
      QGraphicsItem * first = this->toPlaceStack.takeFirst();

      // Reenable
      first->setEnabled(true);

      // Position
      this->placeUnderMouse(first, e->scenePos());
    }
  }
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
  qDebug() << "-----------------";
  qDebug() << "mouseReleaseEvent";
  qDebug() << "-----------------";

  if ( this->placeMode == CHOOSE ) {
    // Handle item groups
    if ( this->selectedItems().size() == 0 && bLastCommand ) {
      undoStack.undo();
      e->setAccepted(true);
    } else if ( this->selectedItems().size() == 0 && activeGroup &&
        !e->isAccepted() ) {
      RemoveItemGroupCommand * groupCommand =
        new RemoveItemGroupCommand( this, activeGroup );
      undoStack.push( groupCommand );
      e->setAccepted(true);
    } else if ( this->selectedItems().size() == 1 &&
        oldPos != this->selectedItems().first()->pos() ) {
      qDebug() << "Move detected.";
      this->moveItem();
      e->setAccepted(true);
    } else if ( this->selectedItems().size() ==1 && bLastCommand &&
        this->selectedItems().first()->type() != QGraphicsItemGroup::Type ) {
      undoStack.undo();
    } else if (this->selectedItems().size() > 1) { // Create an itemGroup
      // TODO: Do not split up translines in several items. This requires an
      // major update of the internal data structures. But for now,
      // create an item group, if there are other items than trackpoints or
      // translines in the selection.
      bool bCreateGroup = false;
      QList<QGraphicsItem *>::iterator it;
      QList<QGraphicsItem *> currentItems = this->selectedItems();
      cout << endl;
      for ( it = currentItems.begin(); it != currentItems.end(); ++it) {
        qDebug() << "&(*it) = " << &(*it);
        qDebug() << "*it = " << (*it);
        if ( (*it)->type() != FSMGraphicsItem<>::TRANSLINE &&
             (*it)->type() != FSMGraphicsItem<>::TRANSLINETEXT &&
             (*it)->type() != FSMGraphicsItem<>::TRACKPOINT ) {
          bCreateGroup = true;
          break;
        }
      }
      if ( bCreateGroup ) {
        CreateItemGroupCommand * groupCommand =
          new CreateItemGroupCommand( this, this->selectedItems() );
        undoStack.push(groupCommand);
        e->setAccepted(true);
      }
      // ENDTODO
    }
  }


  // Placing if not in choose mode
  //------------------
  if (this->placeMode != CHOOSE) {

    // Do action depending on place mode
    switch (this->placeMode) {

    // State
    //----------------------
    case STATE:
      {
        // Generate the appropriate command and pass it to the undostack. 
        NewStateCommand *stateCommand = new NewStateCommand( this, e );
        undoStack.push( stateCommand );
        // Close interaction
        this->setPlaceMode( CHOOSE );
      }
      break;
      //-- END OF STATE -------//
    case TRANS:
      {
        qDebug() << "placeTransitionStack.size() = " << placeTransitionStack.size();
        if ( activeTransCommand == NULL ) {
          activeTransCommand = new NewTransCommand( this );
          activeTransCommand->handleMouseReleaseEvent(e);
        } else {
          activeTransCommand->handleMouseReleaseEvent(e);
          if ( activeTransCommand->commandReady() ) {
            undoStack.push( activeTransCommand );
            activeTransCommand = NULL;
            this->setPlaceMode( CHOOSE );
          }
        }
      }
      break;
      //-- END OF TRANS -------//


        // Link
        //------------------------------
    case LINKDEPARTURE: {
        if ( activeLinkCommand == NULL ) {
          activeLinkCommand = new NewLinkCommand( this );
          activeLinkCommand->handleMouseReleaseEvent(e);
        } else {
          activeLinkCommand->handleMouseReleaseEvent(e);
          if ( activeLinkCommand->commandReady() ) {
            undoStack.push( activeLinkCommand );
            activeLinkCommand = NULL;
            this->setPlaceMode( CHOOSE );
          }
        }
      }
      break;
      //-- END OF Link -------------//

    // Place a JointPoint
    //--------------------------
    case JOIN: {
      {
        if ( activeJoinCommand == NULL ) {
          activeJoinCommand = new NewJoinCommand( this );
          activeJoinCommand->handleMouseReleaseEvent(e);
        } else {
          activeJoinCommand->handleMouseReleaseEvent(e);
          if ( activeJoinCommand->commandReady() ) {
            undoStack.push( activeJoinCommand );
            activeJoinCommand = NULL;
            this->setPlaceMode( CHOOSE );
          }
        }
      }
      break;
    }
    //-- EOF JoinItem

    // Place an HyperTransition
    //---------------------------------
    case HYPERTRANS: {
      if ( activeHyperTransCommand == NULL ) {
        activeHyperTransCommand = new NewHyperTransCommand( this );
        activeHyperTransCommand->handleMouseReleaseEvent(e);
      } else {
        activeHyperTransCommand->handleMouseReleaseEvent(e);
        if ( activeHyperTransCommand->commandReady() ) {
          undoStack.push( activeHyperTransCommand );
          activeHyperTransCommand =  NULL;
          if ( placeLock == FSMDesigner::UNLOCKED )
            setPlaceMode( FSMDesigner::CHOOSE );
        }
      }
      break;
    }

    }



    //-- EOF Place Mode --------------//

  }

  // To Place elements Stack
  //------------------------------



  //-- Parent job
  // Only now, to avoid selection update on an item whose model has not been prepared
  QGraphicsScene::mouseReleaseEvent(e);

}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* e) {
  // TODO: Is this the right place to dispatch the event, or should a function
  // help to structure the dispatch?
  if ( activeHyperTransCommand )
    activeHyperTransCommand->handleMouseMoveEvent(e);
  if ( activeTransCommand )
    activeTransCommand->handleMouseMoveEvent(e);
  if ( activeJoinCommand )
    activeJoinCommand->handleMouseMoveEvent(e);
  if ( activeLinkCommand )
    activeLinkCommand->handleMouseMoveEvent(e);
  //----------------- Mouse Following stacks --------------//

  // Mouse following
  //---------------------------------

  if (this->toPlaceStack.size() > 0) {

    QGraphicsItem * stackFirst = this->toPlaceStack.first();
    stackFirst->setVisible(true);
    stackFirst->setEnabled(true);
    this->placeUnderMouse(stackFirst, e->scenePos());
  }


  QGraphicsScene::mouseMoveEvent(e);
}

void Scene::placeUnderMouse(QGraphicsItem * item, QPointF mousePosition) {

  /*item->setPos(mousePosition.x() - (item->boundingRect().width() / 2),
   mousePosition.y() - (item->boundingRect().height() / 2));*/

  //-- If we have a transline, we only want to position the endpoint
  if (item->type() == Transline::Type) {

    Transline * line = dynamic_cast<Transline*> (item);
    line->setEndPoint(mousePosition);
  } else {
    item->setPos(mousePosition);
  }

}


void Scene::keyReleaseEvent(QKeyEvent * keyEvent) {
  qDebug() << "-----------------";
  qDebug() << "keyReleaseEvent";
  qDebug() << "-----------------";
  // TODO: (QUICKHACK) move to the right place, 
  if(keyEvent->key() == Qt::Key_Escape) {
    if (bLastCommand) {
      undoStack.undo();
    } else if (activeGroup ) {
      RemoveItemGroupCommand * groupCommand =
        new RemoveItemGroupCommand( this, activeGroup );
      undoStack.push( groupCommand );
    }
  }
  // ENDTODO

  // Call Parent
  //-------------------
  QGraphicsScene::keyReleaseEvent(keyEvent);

  //qDebug() << "Krelease in scene widget: " << keyEvent->isAccepted();


  //-- IF event was already accepted, don't process it
  if (keyEvent->isAccepted())
    return;

  // ESC sets place mode to choose
  //---------------
  if (keyEvent->key() == Qt::Key_Escape && this->getPlaceMode() != CHOOSE) {
    this->setPlaceMode(CHOOSE);

    // TODO: free activeCommands.
    if ( activeTransCommand ) {
      activeTransCommand->undo();
      delete activeTransCommand;
      activeTransCommand = NULL;
    }
    if ( activeHyperTransCommand ) {
      activeHyperTransCommand->undo();
      delete activeHyperTransCommand;
      activeHyperTransCommand = NULL;
    }
    if ( activeJoinCommand ) {
      activeJoinCommand->undo();
      delete activeJoinCommand;
      activeJoinCommand = NULL;
    }

  }
  // Super deletes items
  //---------------------
  else if (keyEvent->key() == Qt::Key_Delete) {

    QList<QGraphicsItem*> selectedItems = this->selectedItems();
    // TODO:
    for (QList<QGraphicsItem*>::iterator it = selectedItems.begin(); it
        < selectedItems.end(); it++) {

      //--  Check item is still on the scene
      if ((*it)->scene() == NULL) {
        continue;
      }

      if ( (*it)->type() == StateItem::Type ) {
        // TODO: delete associated translines.
        RemoveStateCommand *stateCommand = new RemoveStateCommand( this,
            dynamic_cast<StateItem*>(*it) );
        undoStack.push(stateCommand);
      }

      /*
      QList<QUndoCommand*> undoCommands;
      if ((*it)->type() == StateItem::Type) {

        undoCommands = dynamic_cast<StateItem*> (*it)->remove();

      } else if ((*it)->type() == Transline::Type) {

        undoCommands = dynamic_cast<Transline*> (*it)->remove();

      } else if ((*it)->type() == LinkDeparture::Type) {

        undoCommands = dynamic_cast<LinkDeparture*> (*it)->remove();

      } else if ((*it)->type() == TrackpointItem::Type) {

        undoCommands = dynamic_cast<TrackpointItem*> (*it)->remove();

      } else if ((*it)->type() == LinkArrival::Type) {

        undoCommands = dynamic_cast<LinkArrival*> (*it)->remove();

      } else if ((*it)->type() == HyperTransition::Type) {

                undoCommands = dynamic_cast<HyperTransition*> (*it)->remove();

            } else if ((*it)->type() == JoinItem::Type) {

                undoCommands = dynamic_cast<JoinItem*> (*it)->remove();

            }

      //-- Add gathered Undo commands to stack
      for (QList<QUndoCommand*>::iterator cit = undoCommands.begin(); cit
          < undoCommands.end(); cit++) {
        this->undoStack.push(*cit);
      }
      */

      // Only do once
      break;

    }

  } 
}


Fsm * Scene::getFsm() {
  return this->fsm;
}


void Scene::setFsm(Fsm * fsm) {

  this->fsm = fsm;
}

#define CLEAN_POINTER_QLIST(list)\
    while (!list.isEmpty()) \
       delete list.takeFirst();

void Scene::setPlaceMode(FSMDesigner::Item mode) {
  qDebug() << "-----------------";
  qDebug() << "setPlaceMode";
  qDebug() << "-----------------";

  // -- Quick hack.
  // -- TODO: develop a fool proof approach
  if (activeTransCommand != NULL) {
    undoStack.push(activeTransCommand);
    activeTransCommand = NULL;
  }
  // END - quick hack.


  //-- If back to choose requested, then perform cleaning and such
  //-------------------------------------
  if (mode==FSMDesigner::CHOOSE) {

    //-- Restore any cursor change
//    QApplication::restoreOverrideCursor();

    //-- Clear selected elements
    this->clearSelection();



    // Clean to place stack
    //-----------------------------------

    //-- Clean things to be placed as they will never come up
    while (!this->toPlaceStack.isEmpty()) {

      QGraphicsItem* firstItem = this->toPlaceStack.takeFirst();
      //-- Transline that belong to a trackpoint are removed by trackpoint
      if (FSMGraphicsItem<>::isTransline(firstItem) &&
        FSMGraphicsItem<>::isTrackPoint(FSMGraphicsItem<>::toTransline(firstItem)->getStartItem())) {

      } else
        delete firstItem;
    }
    //CLEAN_POINTER_QLIST(this->toPlaceStack)

    // Clean Transition place stack
    //---------------------------------------

    //-- Clear Trackpoints that were in placement
    while (!this->placeTransitionStack.isEmpty()) {

      QGraphicsItem* firstItem = this->placeTransitionStack.takeFirst();
      //-- Transline that belong to a trackpoint are removed by trackpoint
      if (FSMGraphicsItem<>::isTrackPoint(firstItem)) {
        this->removeItem(firstItem);
        //delete firstItem;
      }
    }
    if ( activeTransCommand != NULL ) {
      delete activeTransCommand;
      activeTransCommand = NULL;
    }

    // Reset variables
    //---------------------
    this->placeLinkStartState = NULL;
    this->placeLinkEndState = NULL;
  }


  //-- If In Lock mode, don't change back to choose and repeat
  if (this->getPlaceModeLock() != FSMDesigner::LOCKED ) {
    //-- Record
    this->placeMode = mode;
    emit modeChanged(); 
  }

  //-- Make some preparation
  switch (this->placeMode) {

    case LINKDEPARTURE: {
        //-- Change cursor
//        QApplication::setOverrideCursor(Qt::CrossCursor);

        //-- Highligh all the states that don't already have a link
        this->clearSelection();
        QList<QGraphicsItem*> items = this->items();
        for (QList<QGraphicsItem*>::iterator it = items.begin(); it
                < items.end(); it++) {

            if ((*it)->type() == StateItem::Type) {
                (*it)->setSelected(true);
            }

        }

        break;
    }
    default:
      break;
  }
}

LinkArrival * Scene::placeLinkToState(StateItem * endState, Link * link) {

  // Find An eventual LinkArrival to this state, using the StateItem incoming transitions
  //------------------------------
  LinkArrival * linkArrival = endState->findLinkArrival();


  //-- Create GUI Item if not found
  if (linkArrival==NULL) {
      linkArrival = new LinkArrival(link);

    //-- Add to scene
    //this->addItem(LinkArrival);

    //-- Create transition Line to state
    Transline * transLine = new Transline(NULL, linkArrival, endState);
    linkArrival->setNextTransline(transLine);
    this->addItem(transLine);

  }


  return linkArrival;
}

void Scene::alignSelectionVertical() {

  qreal firstPosx = -1;

  //-- Foreach selection
  QList<QGraphicsItem*> selectedItems = this->selectedItems();
  for (QList<QGraphicsItem*>::iterator it = selectedItems.begin(); it
      < selectedItems.end(); it++) {

    //-- Only states and trackpoints
    if ((*it)->type() != StateItem::Type && (*it)->type() != TrackpointItem::Type )
      continue;

    //-- Record X first position
    if (firstPosx == -1) {
      firstPosx = (*it)->pos().x();
    } else {
      //-- If already, give it to the others
      (*it)->setPos(firstPosx, (*it)->y());
    }

  }

}


void Scene::verify() {

  if (this->getFSMVerificator()!=NULL) {
    this->getFSMVerificator()->reset();
  }

    Verificator * verificator = new Verificator();
    verificator->addRule(new StateOutputsRule());
    verificator->verify(this->fsm,this);

    delete verificator;


}


void Scene::enteredRule(VerificatorRule * rule) {

}

void Scene::ruleSuccessful(VerificatorRule * rule) {

}

void Scene::ruleFailed(VerificatorRule * rule,QList<RuleError*>& errors) {

    qDebug() << "Rule failed: " << rule->getName();

    // Map Errors to Elements
    //--------------------------------
    for (int i = 0 ; i<errors.size(); i++) {

        // Pick Error
        RuleError * error = errors.at(i);

        // Create an FSMVerifyError
        FSMVerifyError * guiError = new FSMVerifyError(FSMVerifyError::DEFAULT);
        guiError->setMessage(error->getMessage());

        // Map To GUI Item
        //-----------------------
        switch (error->getConcernedObjects().front().first) {

            //-- State Item
            case FSMDesigner::STATE: {

                if (error->getConcernedObjects().front().second==NULL)
                    break;

                // Find back and Set
                StateItem * stateItem = this->findStateItem(static_cast<State*>(error->getConcernedObjects().front().second));
                if (stateItem!=NULL) {
                    guiError->addConcernedItem(stateItem);
                    stateItem->addVerificationError(guiError);
                }

                break;
            }
            default:

                break;

        }


    }


}

QList<Transline *> Scene::findTransline(Trans * transitionModel) {

  QList<QGraphicsItem*> allItems = this->items();
  QList<Transline *> result;

  //-- Search for the transline
  for (QList<QGraphicsItem*>::iterator it = allItems.begin();it!=allItems.end();it++) {

    if (FSMGraphicsItem<>::isTransline((*it))
        && FSMGraphicsItem<>::toTransline(*it)->getModel()!= NULL
        && FSMGraphicsItem<>::toTransline(*it)->getModel()==transitionModel) {
      result+=(FSMGraphicsItem<>::toTransline(*it));
    }
  }

  return result;

}

QList<TrackpointItem *> Scene::findTransitionBaseTrackpoint(TransitionBase * transitionBaseModel) {

    QList<QGraphicsItem*> allItems = this->items();
    QList<TrackpointItem *> result;

    //-- Search for the transline
    for (QList<QGraphicsItem*>::iterator it = allItems.begin();it!=allItems.end();it++) {

        TransitionBase * itemTransitionModel = NULL;
        if (FSMGraphicsItem<>::isTrackPoint((*it))
                && FSMGraphicsItem<>::toTrackPoint(*it)->getModel()!= NULL
                && FSMGraphicsItem<>::toTrackPoint(*it)->getModel()->getTransition()==transitionBaseModel) {
            result+=FSMGraphicsItem<>::toTrackPoint(*it);
        }
    }

    return result;

}

TranslineText * Scene::findTranslineText(Trans * transitionModel) {

    QList<QGraphicsItem*> allItems = this->items();
    TranslineText * result = NULL;

    //-- Search for the transline text
    for (QList<QGraphicsItem*>::iterator it = allItems.begin();it!=allItems.end();it++) {
        if (FSMGraphicsItem<>::isTranslineText((*it))
                && FSMGraphicsItem<>::toTranslineText(*it)->getTransition()!= NULL
                && FSMGraphicsItem<>::toTranslineText(*it)->getTransition()==transitionModel) {
            result=(FSMGraphicsItem<>::toTranslineText(*it));
            break;
        }
    }

    return result;
}

StateItem * Scene::findStateItem(State * stateModel) {

  QList<QGraphicsItem*> allItems = this->items();
  QList<Transline *> result;

  //-- Search for the transline
  for (QList<QGraphicsItem*>::iterator it = allItems.begin();it!=allItems.end();it++) {

    if (FSMGraphicsItem<>::isStateItem((*it))
        && FSMGraphicsItem<>::toStateItem(*it)->getModel()!= NULL
        && FSMGraphicsItem<>::toStateItem(*it)->getModel()==stateModel) {
      return FSMGraphicsItem<>::toStateItem(*it);
    }
  }

  return NULL;

}

JoinItem * Scene::findJoinItem(Join * joinModel) {

    QList<QGraphicsItem*> allItems = this->items();
    JoinItem * result;

    //-- Search for the transline
    for (QList<QGraphicsItem*>::iterator it = allItems.begin();it!=allItems.end();it++) {

        if (FSMGraphicsItem<>::isJoinItem((*it))
                && FSMGraphicsItem<>::toJoinItem(*it)->getModel()!= NULL
                && FSMGraphicsItem<>::toJoinItem(*it)->getModel()==joinModel) {
            return FSMGraphicsItem<>::toJoinItem(*it);
        }
    }

    return NULL;

}

QList<LinkDeparture*> Scene::findLinkDepartures(Link * link) {

  QList<QGraphicsItem*> allItems = this->items();
  QList<LinkDeparture *> result;

  //-- Search for the LinkDepartures
  for (QList<QGraphicsItem*>::iterator it = allItems.begin();it!=allItems.end();it++) {

    if (FSMGraphicsItem<>::isLinkDeparture((*it))
        && FSMGraphicsItem<>::toLinkDeparture(*it)->getModel()!= NULL
        && FSMGraphicsItem<>::toLinkDeparture(*it)->getModel()->getTargetLink() == link) {
      result << FSMGraphicsItem<>::toLinkDeparture(*it);
    }
  }

  return result;
}


void Scene::receivedSelectionChanged() {
  qDebug() << "received SelectionChanged()";
  selectionChanged = true;
}


void Scene::moveItem() {
  qDebug() << "moveItem() ";
  Q_ASSERT( this->selectedItems().size() == 1 );
  QGraphicsItem * currentItem = this->selectedItems().first();
  switch( currentItem->type() ) {
    case ( QGraphicsItemGroup::Type ) : {
      MoveItemGroupCommand * moveCommand =
        new MoveItemGroupCommand( this,
            dynamic_cast<QGraphicsItemGroup*>(currentItem),
            oldPos, currentItem->pos() );
      undoStack.push( moveCommand );
      break;
    }
    default: {
      qDebug() << "moveItem() default case";
      this->moveItem(currentItem);
    }
  }
}


void Scene::moveItem(QGraphicsItem * item) {
  qDebug() << "moveItem(QGraphicsItem * item) ";
  switch( item->type() ) {
    case ( FSMGraphicsItem<>::STATEITEM ) : {
      qDebug() << "moveItem() STATEITEM case";
      MoveStateCommand * moveCommand =
        new MoveStateCommand( this, dynamic_cast<StateItem *>(item));
      undoStack.push( moveCommand );
      break;
    }
    default:
      qDebug() << "moveItem() default case";
  }
}


void Scene::setPlaceModeSlot(FSMDesigner::Item mode) {
  qDebug() << "-----------------";
  qDebug() << "setPlaceMode slot";
  qDebug() << "-----------------";

  // Undo unused CreateItemGroupCommands.
  if (bLastCommand) {
    undoStack.undo();
  } else if (activeGroup) {
    RemoveItemGroupCommand * groupCommand =
      new RemoveItemGroupCommand( this, activeGroup );
    undoStack.push( groupCommand );
  }
  this->setPlaceMode( mode );
}
