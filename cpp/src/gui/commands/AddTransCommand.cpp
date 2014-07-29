#include "gui/commands/AddTransCommand.h"


AddTransCommand::AddTransCommand( Scene * _relatedScene,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  bLastCommand( _relatedScene->bLastCommand )
{
}


AddTransCommand::~AddTransCommand() {
  // TODO: is the model updated correctly by the existing code base?
  QList<Transline *>::iterator it;
  for (it = transList.begin(); it != transList.end(); ++it) {
    delete *it;
  }
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
  text->setPos( transList.first()->scenePos() );
  relatedScene->addItem( text );
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


bool AddTransCommand::addTransLine ( Transline  * transItem ) {
  if ( transItem == NULL )
    return false;
  transList.append( transItem );
  return true;
}

bool AddTransCommand::addTransText ( TranslineText  * textItem ) {
  if ( textItem == NULL )
    return false;
  text = textItem;
  return true;
}

bool AddTransCommand::addTrackPoint( TrackpointItem * trackItem ) {
  if ( trackItem == NULL )
    return false;
  trackList.append( trackItem );
  return true;
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


bool AddTransCommand::handleMouseEvent(QGraphicsSceneMouseEvent * e) {
}
