#include "gui/commands/NewHyperTransCommand.h"


NewHyperTransCommand::NewHyperTransCommand( Scene * _relatedScene,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand( _parentCommand ), 
                                  relatedScene( _relatedScene ),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand ),
                                  hypertransition( NULL ),
                                  hypertransModel( NULL ),
                                  transline( NULL ),
                                  targetState( NULL )
{
}


NewHyperTransCommand::~NewHyperTransCommand() {
  if (hypertransition)
    delete hypertransition;
  if (hypertransModel)
    delete hypertransModel;
  if (transline)
    delete transline      ;
  // Do not delete the target state, since the command does not have the
  // ownership of that object.
}


void  NewHyperTransCommand::redo(){
  // add items to model
  fsm->addHypertrans( hypertransModel );

  // add items to scene
  relatedScene->addItem( hypertransition );
  relatedScene->addItem( transline );
  
  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  NewHyperTransCommand::undo(){
  // remove items from scene
  relatedScene->removeItem( transline );
  relatedScene->removeItem( hypertransition );

  // remove items from model
  fsm->deleteHypertrans( hypertransModel );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


QGraphicsItem * NewHyperTransCommand::getIntersectingItem(QGraphicsSceneMouseEvent * e) const {
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


bool NewHyperTransCommand::handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
  if ( hypertransition == NULL ) {
    // Create new model. Do not attach the model to the fsm data structure yet.
    hypertransModel = new Hypertrans();
    hypertransModel->setPosition(
      pair<double,double>(e->scenePos().x(), e->scenePos().y() ) );
    //-- It is cleaner to modify the model at the redo command. On the
    //other hand, the current fsm-api throws an exception, if the command is
    //undone and deleted without calling redo() before.
    //fsm->addHypertrans( hypertransModel );
    // Create a new hyper transition item.
    hypertransition = new HyperTransition( hypertransModel );
    hypertransition->setPos( e->scenePos() );
    // Create a new transline between the hyper transition and the target state.
    transline = new Transline( NULL, hypertransition, NULL );
    transline->setEndPoint( e->scenePos() );
    // Show the gui items to the user by adding them to the active scene.
    relatedScene->addItem( hypertransition );
    relatedScene->addItem( transline );
    relatedScene->update();
    return true;
  } else if ( targetState == NULL && getIntersectingItem(e) ) {
    if ( getIntersectingItem(e)->type() == FSMGraphicsItem<>::STATEITEM ) {
      targetState = dynamic_cast<StateItem *>( getIntersectingItem(e) );
      transline->setEndItem( targetState );
      hypertransModel->setTargetState( targetState->getModel() );
      return true;
    } else {
      return false;
    }
  }
  return false;
}


bool NewHyperTransCommand::handleMouseMoveEvent(QGraphicsSceneMouseEvent * e) {
  if ( transline == NULL )
    return false;
  transline->setEndPoint( e->scenePos() );
  return true;
}


bool NewHyperTransCommand::commandReady() const {
  return (targetState != NULL);
}
