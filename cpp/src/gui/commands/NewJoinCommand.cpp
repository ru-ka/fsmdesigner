#include "gui/commands/NewJoinCommand.h"


NewJoinCommand::NewJoinCommand( Scene * _relatedScene,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand ),
                                  startItem ( NULL ),
                                  endItem   ( NULL ),
                                  join      ( NULL )
{
}


NewJoinCommand::~NewJoinCommand() {
  //QList<Transline *>::iterator it;
  for (auto it = transList.begin(); it != transList.end(); ++it) {
    delete *it;
  }
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    delete *track_it;
  }
  if (join)
    delete join;
}


void  NewJoinCommand::redo(){
  relatedScene->addItem( startItem );
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    relatedScene->addItem( *it );
  }
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    relatedScene->addItem( *track_it );
  }
  this->fsm->addJoin( join );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  NewJoinCommand::undo(){
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    relatedScene->removeItem( *it );
  }
  QList<TrackpointItem *>::iterator track_it;
  for (track_it = trackList.begin(); track_it != trackList.end(); ++track_it) {
    relatedScene->removeItem( *track_it );
  }
  relatedScene->removeItem( startItem );
  this->fsm->deleteJoin( join );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


QGraphicsItem * NewJoinCommand::getIntersectingItem(QGraphicsSceneMouseEvent * e) const {
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


bool NewJoinCommand::handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
  // return false, if command is already ready.
  if ( commandReady() ) {
    return false;
  }

  // 1st case, there is no start item yet.
  // Intent: Add the intersecting join or state item.
  if ( startItem == NULL ) {
    join = new Join();
    startItem = new JoinItem( join );
    startItem->setPos( e->scenePos() );
    relatedScene->addItem( startItem );
    Transline * transition = new Transline( NULL, startItem, NULL );
    transition->setEndPoint( e->scenePos() );
    relatedScene->addItem( transition );
    transList.append( transition );
    return true;
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
  else if (    getIntersectingItem(e)->type() == StateItem::Type 
            || getIntersectingItem(e)->type() == JoinItem::Type ) {
    endItem = getIntersectingItem(e);
    if ( !trackList.empty() )
      trackList.last()->setEndItem( endItem );
    transList.last()->setEndItem( endItem );
    
    if ( endItem->type() == StateItem::Type )
      join->setTargetState( dynamic_cast<StateItem*>(endItem)->getModel() );
    if ( endItem->type() == JoinItem::Type )
      join->setTargetState(
          dynamic_cast<JoinItem*>(endItem)->getModel()->getTargetState() );

    this->addTrackPointsToJoinModel( );
    return true;
  }
  return false;
}


void NewJoinCommand::addTrackPointsToJoinModel( ) {
  if ( trackList.empty() )
    return;
  for (auto currentTrackpoint : trackList) {
    join->addTrackpoint( currentTrackpoint->getModel() );
    currentTrackpoint->modelChanged();
  }
}


bool NewJoinCommand::handleMouseMoveEvent(QGraphicsSceneMouseEvent * e) {
  if ( transList.empty() )
    return false;
  transList.last()->setEndPoint( e->scenePos() );
  return true;
}


bool NewJoinCommand::commandReady() const {
  return ( endItem != NULL );
}
