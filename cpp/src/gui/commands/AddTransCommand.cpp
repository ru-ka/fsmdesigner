#include "gui/commands/AddTransCommand.h"


AddTransCommand::AddTransCommand( Scene * _relatedScene,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand ),
                                  text      ( NULL ),
                                  startItem ( NULL ),
                                  endItem   ( NULL )
{
}


AddTransCommand::~AddTransCommand() {
  // TODO: is the model updated correctly by the existing code base?
  QList<Transline *>::iterator it;
  // TODO: Most likely, merely the first transline should be deleted, since
  // the other translines are freed by their owners namely the trackpoint items?
  // Answer: Nope, currently not. Nice pointer mess so far ;).
  for (it = transList.begin(); it != transList.end(); ++it) {
    delete *it;
  }
  if (text)
    delete text;
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    delete *track_it;
  }
}


void  AddTransCommand::redo(){
  qDebug() << "Adding transline items. Transline items.size() = " << transList.size();
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    relatedScene->addItem( *it );
  }
  qDebug() << "Adding text"; 
  if ( text != NULL ) {
    // TODO: find a good position for the text item.
    text->setPos( transList.first()->scenePos() );
    relatedScene->addItem( text );
  }
  qDebug() << "Adding trackpoint items"; 
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    relatedScene->addItem( *track_it );
  }

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  AddTransCommand::undo(){
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    relatedScene->removeItem( *it );
  }
  relatedScene->removeItem( text );
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    relatedScene->removeItem( *it );
  }

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


QGraphicsItem * AddTransCommand::getIntersectingItem(QGraphicsSceneMouseEvent * e) const {
  // NOTE: Copied from scene.cpp. TODO: Make function nice. What to do, if
  // several stateItems are under the cursor? Pop up a selection dialog?
  //-- Get Item under
  QList<QGraphicsItem*> itemsUnder = relatedScene->items(e->scenePos(),
      Qt::IntersectsItemShape, Qt::AscendingOrder);

  // Wipe transline if on top and already started the transaction
  //if (this->placeTransitionStack.size() > 0 && itemsUnder.size() > 0
  if (itemsUnder.size() > 0
      && itemsUnder.front()->type() == Transline::Type) {
    itemsUnder.pop_front();
  }

  QGraphicsItem* itemUnder =
      itemsUnder.size() > 0 ? itemsUnder.front() : NULL;

  return itemUnder;
}


bool AddTransCommand::handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
  // TODO: is the case JOIN to JOIN legal?
  // In the old FSMDesigner, this case is not described, but could occur.
  
  // return false, if command is already ready.
  if ( commandReady() ) {
    return false;
  }

  // 1st case, there is no start item yet.
  // Intent: Add the intersecting join or state item.
  if ( startItem == NULL ) {
    if ( getIntersectingItem(e) == NULL )
      return false;
    if ( getIntersectingItem(e)->type() == StateItem::Type ||
         getIntersectingItem(e)->type() == JoinItem::Type ) {
      startItem = getIntersectingItem(e);
      Transline * transition = new Transline( NULL, startItem, NULL );
      transition->setEndPoint( e->scenePos() );
      relatedScene->addItem( transition );
      transList.append( transition );
      return true;
    } else {
      return false;
    }
  } 
  // 2nd case, a start item is set and there is no item under the cursor.
  // Intent: add a trackpoint.
  else if ( getIntersectingItem(e) == NULL ) {
    // Add a trackpoint. Set the new trackpoint as endItem of the previous
    // trackpoint.
    TrackpointItem * item = NULL;
    if ( trackList.isEmpty() ) {
      item = new TrackpointItem( 
        new Trackpoint(e->scenePos().x(), e->scenePos().y(), NULL),
          startItem, NULL);
    } else {
      item = new TrackpointItem(
        new Trackpoint(e->scenePos().x(), e->scenePos().y(), NULL),
          trackList.last(), NULL);
      trackList.last()->setEndItem( item );
    }
    item->setPos ( e->scenePos() );
    relatedScene->addItem( item );
    trackList.append( item );
    // Take care of the translines.
    transList.last()->setDrawArrow(false);
    transList.last()->setEndItem( item );
    Transline * transition = new Transline( NULL, item, NULL );
    transition->setEndPoint( e->scenePos() );
    relatedScene->addItem( transition );
    transList.append( transition );
    return true;
  }
  // 3rd case, a start item is set, any number of trackpoints are added
  // and the intersecting item is a state item.
  // Intent: Add end item. Finish the outstanding transline.
  else if ( getIntersectingItem(e)->type() == StateItem::Type ) {
    endItem = getIntersectingItem(e);
    trackList.last()->setEndItem( endItem );
    transList.last()->setEndItem( endItem );
    // stateToState
    if ( startItem->type() == StateItem::Type ) {
      Trans * transition = fsm->addTrans(
          dynamic_cast<StateItem*>( startItem )->getModel(),
          dynamic_cast<StateItem*>( endItem   )->getModel() );
      this->addTrackPointsToModel( transition );
      this->createText( transition );
    }
    // joinToState
    else if ( startItem->type() == JoinItem::Type ) {
      // TODO: implement
      Q_ASSERT( false );
    }
  }
  // 4th case, a start item is set, any number of trackpoints are added
  // and the intersecting item is a join item.
  // Intent: Add end item. Finish the outstanding transline.
  else if ( getIntersectingItem(e)->type() == JoinItem::Type ) {
    endItem = getIntersectingItem(e);
    trackList.last()->setEndItem( endItem );
    transList.last()->setEndItem( endItem );
    // stateToJoin
    if ( startItem->type() == StateItem::Type ) {
      //this->createText( transition );
      // TODO: implement
      Q_ASSERT( false );
    }
    // joinToJoin
    else if ( startItem->type() == JoinItem::Type ) {
      // TODO: implement
      Q_ASSERT( false );
    }
    // TODO: implement
    Q_ASSERT( false );
  }
}


void AddTransCommand::addTrackPointsToModel( Trans * trans ) {
  QList<TrackpointItem *>::iterator it;
  for (it = trackList.begin(); it != trackList.end(); ++it) {
    trans->appendTrackpoint( (*it)->getModel() );
    (*it)->modelChanged();
  }
}


void AddTransCommand::createText( Trans * trans ) {
  text = new TranslineText( QString( trans->getName().c_str() ), trans );
  text->setPos( transList.first()->scenePos() );
}


bool AddTransCommand::handleMouseMoveEvent(QGraphicsSceneMouseEvent * e) {
  if ( transList.empty() )
    return false;
  transList.last()->setEndPoint( e->scenePos() );
  return true;
}


bool AddTransCommand::commandReady() const {
  return ( endItem != NULL );
}


















/*
 //-- Transition from state to state
      //-----------------
      if (stateToState || joinToState || stateToJoin) {

          //-- Keep added/referenced transition
          Trans * transition= NULL;

        //---- Add to model depending on type
        //---------------------
        if (stateToState) {

          //-- Add Transition
            transition = this->fsm->addTrans(
              dynamic_cast<StateItem*> (this->placeTransitionStack.back())->getModel(),
              dynamic_cast<StateItem*> (this->placeTransitionStack.front())->getModel());

        } else if (joinToState) {

          //-- Update join model to target the state
          State * targetState = FSMGraphicsItem<>::toStateItem(this->placeTransitionStack.front())->getModel();
          FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.back())->getModel()->setTargetState(targetState);

          //-- Update eventual transition going to this join to the new target
          QList<Transline*>  incoming = FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.back())->getIncomingTransitions();
          for (QList<Transline*>::iterator it = incoming.begin();it != incoming.end(); it++) {
            ((Trans *)(*it)->getModel())->setEndState(FSMGraphicsItem<>::toStateItem(this->placeTransitionStack.front())->getModel());
          }

        } else if (stateToJoin) {

          //-- Target is target of the joint
          State * targetState = FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.front())->getModel()->getTargetState();
          int joinId = FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.front())->getModel()->getId();

          //-- Add a transition pointing to the join point destination
          transition = this->fsm->addTrans(dynamic_cast<StateItem*> (this->placeTransitionStack.back())->getModel(),targetState);

          //-- Add one last trackpoint representing the join before the join graphic item
          TrackpointItem * joinTrackpoint = new TrackpointItem(new Trackpoint(0,0,NULL));
          joinTrackpoint->getModel()->setJoin(FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.front())->getModel());
          //this->placeTransitionStack.insert(this->placeTransitionStack.indexOf(this->placeTransitionStack.front())-1,joinTrackpoint);
          this->placeTransitionStack.insert(1,joinTrackpoint);

          qDebug() << "Join target: " << targetState->getId() << " is id: " << joinTrackpoint->getModel()->getJoin()->getId();

        }


        //---- Trackpoints (Go from end to start because of stacking)
        //-------------------
        Transline* lastTransline = NULL;
        for (int i = this->placeTransitionStack.size() - 1; i >= 0; i--) {

          //-- Add Trackpoint model to transition
          if (FSMGraphicsItem<>::isTrackPoint(
              this->placeTransitionStack[i])) {

            TrackpointItem * trackPoint =
                FSMGraphicsItem<>::toTrackPoint(
                    this->placeTransitionStack[i]);

            //-- Add to transition if we are placing a transition
            if (stateToJoin|| stateToState) {

              transition->appendTrackpoint(
                  trackPoint->getModel());
            }
            //-- Add to JoinItem if we are starting on a Join
            else if (joinToState) {
              FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.back())->getModel()->addTrackpoint(trackPoint->getModel());
            }

            //-- If trackpoint points to join, it is not designed to go on the scene
            if (trackPoint->getModel()->getJoin()==NULL) {
              trackPoint->modelChanged();
              lastTransline = trackPoint->getPreviousTransline();
            }
          }

        } // End trackpoints --//

        //-- If no last transline, we have to create a new one
        if (lastTransline == NULL) {
          lastTransline = new Transline(transition,
              this->placeTransitionStack.back(), this->placeTransitionStack.front());
          this->addItem(lastTransline);
//          activeTransCommand->addTransLine( lastTransline );
        }

*/
