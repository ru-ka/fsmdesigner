#include "gui/commands/MoveItemGroupCommand.h"


MoveItemGroupCommand::MoveItemGroupCommand( Scene * _relatedScene,
                                            QGraphicsItemGroup * _itemGroup,
                                            QPointF _oldPos,
                                            QPointF _newPos,
                                            QUndoCommand * _parentCommand) :
                                            QUndoCommand(_parentCommand), 
                                            relatedScene(_relatedScene),
                                            itemGroup(_itemGroup),
                                            oldPos(_oldPos),
                                            newPos(_newPos),
                                            bLastCommand( _relatedScene->bLastCommand )
{
}


MoveItemGroupCommand::~MoveItemGroupCommand() {
}


void  MoveItemGroupCommand::redo(){
  qDebug() << "-------------------------";
  qDebug() << "Move item group redo()   ";
  qDebug() << "-------------------------";
  itemGroup->setPos(newPos);

  this->updateTranslines();
  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  MoveItemGroupCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "Move item group undo() ";
  qDebug() << "-------------------------";
  itemGroup->setPos(oldPos);

  this->updateTranslines();
  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


//int   MoveItemGroupCommand::id() const {
//  return Id;
//}


bool  MoveItemGroupCommand::mergeWith(const QUndoCommand * command) {
  //TODO
  return false;
}

void MoveItemGroupCommand::setNewPos( QPointF _newPos ) {
  newPos = _newPos;
}

void MoveItemGroupCommand::updateTranslines() {
  // Update connected translines.
  for( auto item : itemGroup->childItems() ) {
    switch ( item ->type() ) {
      case ( FSMGraphicsItem<>::STATEITEM ) :
        dynamic_cast<StateItem*>(item)->updateTranslines();
        break;
      case ( FSMGraphicsItem<>::TRACKPOINT ) :
        dynamic_cast<TrackpointItem*>(item)->updateTranslines();
        break;
    }
  }
}
