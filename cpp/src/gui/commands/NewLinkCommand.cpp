// TODO: double check visibility. Double check linkDepart and linkArrival
// connections.
#include "gui/commands/NewLinkCommand.h"


NewLinkCommand::NewLinkCommand( Scene * _relatedScene,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand ),
                                  startItem ( NULL ),
                                  startTrans( NULL ),
                                  transModel( NULL ),
                                  trackPoint( NULL ),
                                  linkDepart( NULL ),
                                  endItem   ( NULL ),
                                  endTrans  ( NULL ),
                                  linkArrival(NULL ),
                                  link      ( NULL ),
                                  finished  ( false)
{
  QApplication::setOverrideCursor(Qt::CrossCursor);
}


NewLinkCommand::~NewLinkCommand() {
  if ( startTrans )
    delete startTrans;
  if ( transModel )
    delete transModel;
  if ( trackPoint)
    delete trackPoint;
  if ( linkDepart )
    delete linkDepart;
  if ( endTrans )
    delete endTrans;
  if ( linkArrival )
    delete linkArrival;
  // Restore the cursor, if the command gets canceled prematurely.
  if ( !linkDepart )
    QApplication::restoreOverrideCursor();
}


void  NewLinkCommand::redo(){
  qDebug() << "NewLinkCommand: redo()";
  this->fsm->addTrans( transModel );
  this->fsm->addLink( link );
  relatedScene->addItem( startTrans );
  relatedScene->addItem( linkDepart );
  relatedScene->addItem( endTrans   );
  relatedScene->addItem( linkArrival);

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  NewLinkCommand::undo(){
  qDebug() << "NewLinkCommand: undo()";
  relatedScene->removeItem( startTrans );
  relatedScene->removeItem( linkDepart );
  relatedScene->removeItem( endTrans   );
  relatedScene->removeItem( linkArrival);
  this->fsm->deleteLink( link );
  this->fsm->deleteTrans( transModel );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


QGraphicsItem * NewLinkCommand::getIntersectingItem(QGraphicsSceneMouseEvent * e) const {
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


bool NewLinkCommand::handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
  // return false, if command is already ready.
  if ( commandReady() ) {
    return false;
  }

  // 1st case, there is no start item yet.
  // Intent: Assign the intersecting state item to the startItem.
  if ( startItem == NULL ) {
    qDebug() << " Selecting start item.";
    QGraphicsItem * intersectingItem = getIntersectingItem(e);
    if ( intersectingItem != NULL && intersectingItem->type() == StateItem::Type ) {
      startItem = dynamic_cast<StateItem *>(intersectingItem);
      startItem->setSelected(false);
      return true;
    } else {
      return false;
    }
  } 
  // 2nd case, a start item is set, but there is no end item yet.
  // Intent: Assign the intersecting state item to the end item.
  // Prepare the link placement.
  else if ( endItem == NULL ) {
    qDebug() << " Selecting end item.";
    QGraphicsItem * intersectingItem = getIntersectingItem(e);
    if ( intersectingItem != NULL && intersectingItem->type() == StateItem::Type
         && intersectingItem != startItem ) {
      endItem = dynamic_cast<StateItem *>(intersectingItem);
      endItem->setSelected(false);
      link = this->fsm->getLinkbyGoal( endItem->getModel() );
      if ( link == NULL )
        link = this->fsm->addLink( endItem->getModel(),
                                   e->scenePos().x(), e->scenePos().y() );
      transModel = this->fsm->addTrans( startItem->getModel()->getId(),
                                        endItem  ->getModel()->getId() );
      trackPoint = transModel->appendTrackpoint( e->scenePos().x(),
                                                 e->scenePos().y() );
      trackPoint->setTargetLink( link );
      qDebug() << " Placing linkDepart.";
      linkDepart = new LinkDeparture( trackPoint, startItem );
      linkDepart->setPos( e->scenePos() );
      relatedScene->addItem( linkDepart );
      startTrans = new Transline( transModel, startItem, linkDepart );
      linkDepart->setPreviousTransline( startTrans );
      relatedScene->addItem( startTrans );
      QApplication::restoreOverrideCursor();
      return true;
    } else {
      return false;
    }
  } 
  // 3rd case, place linkDeparture. 
  else if ( !linkArrival ) {
    qDebug() << " Placing linkArrival.";
    linkArrival = new LinkArrival ( link, endItem );
    endTrans    = new Transline   ( NULL, linkArrival, endItem );
    linkArrival->setNextTransline( endTrans );
    relatedScene->addItem( linkArrival );
    relatedScene->addItem( endTrans );
  }
  // 4th case, place linkArrival.
  else if ( linkArrival && !finished ) {
    qDebug() << " Finished placement.";
    finished = true;
  }
  return false;
}


bool NewLinkCommand::handleMouseMoveEvent(QGraphicsSceneMouseEvent * e) {
  if ( linkArrival ) {
    linkArrival->setPos( e->scenePos() );
    return true;
  } else if ( linkDepart ) {
    linkDepart->setPos( e->scenePos() );
    return true;
  }
  return false;
}


bool NewLinkCommand::commandReady() const {
  return finished;
}
