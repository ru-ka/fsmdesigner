#include "gui/commands/NewTransCommand.h"


NewTransCommand::NewTransCommand( Scene * _relatedScene,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand ),
                                  text      ( NULL ),
                                  startItem ( NULL ),
                                  endItem   ( NULL ),
                                  trans     ( NULL )
{
}


NewTransCommand::~NewTransCommand() {
  //QList<Transline *>::iterator it;
  for (auto it = transList.begin(); it != transList.end(); ++it) {
    delete *it;
  }
  if (text)
    delete text;
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    delete *track_it;
  }
  delete trans;
}


void  NewTransCommand::redo(){
  qDebug() << "Adding transline items. Transline items.size() = " << transList.size();
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    relatedScene->addItem( *it );
  }
  qDebug() << "Adding text"; 
  if ( text != NULL ) {
    relatedScene->addItem( text );
  }
  qDebug() << "Adding trackpoint items"; 
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    relatedScene->addItem( *track_it );
  }
  this->fsm->addTrans( trans );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  NewTransCommand::undo(){
  qDebug() << "Before removing translineItems.";
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    relatedScene->removeItem( *it );
  }
  qDebug() << "Before removing text.";
  if ( text )
    relatedScene->removeItem( text );
  QList<TrackpointItem *>::iterator track_it;
  qDebug() << "Before removing trackpoints.";
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    relatedScene->removeItem( *track_it );
  }
  this->fsm->deleteTrans( trans );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


QGraphicsItem * NewTransCommand::getIntersectingItem(QGraphicsSceneMouseEvent * e) const {
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


bool NewTransCommand::handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
  // return false, if command is already ready.
  if ( commandReady() ) {
    return false;
  }

  // 1st case, there is no start item yet.
  // Intent: Add the intersecting join or state item.
  if ( startItem == NULL ) {
    if ( getIntersectingItem(e) == NULL )
      return false;
    if ( getIntersectingItem(e)->type() == StateItem::Type ) {
      startItem = getIntersectingItem(e);
      Transline * transition = new Transline( NULL, startItem, NULL );
      transition->setEndPoint( e->scenePos() );
      qDebug() << "transition->boundingRect() = " << transition->boundingRect();
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
    transList.last()->setEndItem( item );
    Transline * transition = new Transline( NULL, item, NULL );
    transition->setEndPoint( e->scenePos() );
    relatedScene->addItem( transition );
    // Connect the translines properly.
    item->setPreviousTransline( transList.last() );
    item->setNextTransline( transition );
    // Finally, append transition to transList.
    transList.append( transition );
    return true;
  }
  // 3rd case, a start item is set, any number of trackpoints are added
  // and the intersecting item is a state item.
  // Intent: Add end item. Finish the outstanding transline.
  else if ( getIntersectingItem(e)->type() == StateItem::Type ) {
    endItem = getIntersectingItem(e);
    if ( !trackList.empty() )
      trackList.last()->setEndItem( endItem );
    transList.last()->setEndItem( endItem );
    if ( startItem->type() == StateItem::Type ) {
      trans = fsm->addTrans(
          dynamic_cast<StateItem*>( startItem )->getModel(),
          dynamic_cast<StateItem*>( endItem   )->getModel() );
      this->addTrackPointsToTransModel( );
      this->addModelToTranslines( );
      this->createText( );
    }
  }
  // 4th case, a start item is set, any number of trackpoints are added
  // and the intersecting item is a join item.
  // Intent: Add end item. Finish the outstanding transline.
  else if ( getIntersectingItem(e)->type() == JoinItem::Type ) {
    endItem = getIntersectingItem(e);
    if ( !trackList.empty() )
      trackList.last()->setEndItem( endItem );
    transList.last()->setEndItem( endItem );
    State * targetState = dynamic_cast<JoinItem *>(endItem)->getModel()->getTargetState();
    if ( startItem->type() == StateItem::Type ) {
      trans = fsm->addTrans(
          dynamic_cast<StateItem*>( startItem )->getModel(), targetState );
      this->addTrackPointsToTransModel( );
      Trackpoint * joinPoint = new Trackpoint(0,0);
      joinPoint->setJoin( dynamic_cast<JoinItem *>(endItem)->getModel() );
      trans->appendTrackpoint( joinPoint );
      this->addModelToTranslines( );
      this->createText( );
    }

  } else {
    return false;
  }
  return false;
}


void NewTransCommand::addTrackPointsToTransModel( ) {
  QList<TrackpointItem *>::iterator it;
  for (it = trackList.begin(); it != trackList.end(); ++it) {
    trans->appendTrackpoint( (*it)->getModel() );
    (*it)->modelChanged();
  }
}

void NewTransCommand::addModelToTranslines( ) {
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
   (*it)->setModel( trans );
  }
}


void NewTransCommand::createText( ) {
  // TODO: where to place the Transline text?
  text = new TranslineText( QString( trans->getName().c_str() ), trans );
  QPointF textPos = transList.first()->boundingRect().center();
  textPos.setX( textPos.x() - text->boundingRect().width()/2 );
  textPos.setY( textPos.y() - text->boundingRect().height()/2 );
  text->setPos( textPos );
}


bool NewTransCommand::handleMouseMoveEvent(QGraphicsSceneMouseEvent * e) {
  if ( transList.empty() )
    return false;
  transList.last()->setEndPoint( e->scenePos() );
  return true;
}


bool NewTransCommand::commandReady() const {
  return ( endItem != NULL );
}


















/*
        } else if (joinToState) {

          //-- Update join model to target the state
          State * targetState = FSMGraphicsItem<>::toStateItem(this->placeTransitionStack.front())->getModel();
          FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.back())->getModel()->setTargetState(targetState);

          //-- Update eventual transition going to this join to the new target
          QList<Transline*>  incoming = FSMGraphicsItem<>::toJoinItem(this->placeTransitionStack.back())->getIncomingTransitions();
          for (QList<Transline*>::iterator it = incoming.begin();it != incoming.end(); it++) {
            ((Trans *)(*it)->getModel())->setEndState(FSMGraphicsItem<>::toStateItem(this->placeTransitionStack.front())->getModel());
          }

        }
        */


/*
else if (stateToJoin) {

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
        */









/*
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
