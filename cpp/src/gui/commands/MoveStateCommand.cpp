#include "gui/commands/MoveStateCommand.h"


MoveStateCommand::MoveStateCommand( Scene * _relatedScene,
                                    StateItem * _stateItem,
                                    QUndoCommand * _parentCommand) :
                                    QUndoCommand(_parentCommand), 
                                    relatedScene(_relatedScene),
                                    stateItem(_stateItem),
                                    state( _stateItem->getModel() ),
                                    fsm( _relatedScene->getFsm() ),
                                    newPos( _stateItem->scenePos() )
{
  stateItem->setTransform(QTransform::fromTranslate( 0, 0));
  oldPos.setX( stateItem->getModel()->getPosition().first  );
  oldPos.setY( stateItem->getModel()->getPosition().second );
}


MoveStateCommand::~MoveStateCommand() {
}


void  MoveStateCommand::redo(){
  qDebug() << "redo()";
  qDebug() << "oldPos = " << oldPos;
  qDebug() << "newPos = " << newPos;
  stateItem->setPos(newPos);
  state->setPosition(std::make_pair(newPos.x(), newPos.y()));

  relatedScene->update();
}


void  MoveStateCommand::undo(){
  qDebug() << "undo()";
  qDebug() << "oldPos = " << oldPos;
  qDebug() << "newPos = " << newPos;
  stateItem->setPos(oldPos);
  state->setPosition(std::make_pair(oldPos.x(), oldPos.y()));

  relatedScene->update();
}


int   MoveStateCommand::id() const {
  return Id;
}


bool  MoveStateCommand::mergeWith(const QUndoCommand * command) {
  qDebug() << "MoveStateCommand: mergeWith()";
  // Ensure, that merely MoveStateCommands of the same item are merged.
  const MoveStateCommand *moveCommand = static_cast<const MoveStateCommand *>(command);
  StateItem * nextItem = moveCommand->getItem();
  if (stateItem != nextItem) 
    return false;

  // Merge the commands for the same item.
  newPos = nextItem->scenePos();
  return true;
}


StateItem* MoveStateCommand::getItem() const {
  return stateItem;
}
