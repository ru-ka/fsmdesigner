#include "gui/commands/MoveStateCommand.h"

MoveStateCommand::MoveStateCommand( Scene * _relatedScene,
                                    StateItem * _stateItem,
                                    QUndoCommand * _parentCommand) :
                                    QUndoCommand(_parentCommand), 
                                    relatedScene(_relatedScene),
                                    fsm( _relatedScene->getFsm() ),
                                    movedStateId( _stateItem->getModel()->getId() ),
                                    newPos( _stateItem->scenePos() ),
                                    bLastCommand( _relatedScene->bLastCommand )
{
  oldPos.setX( _stateItem->getModel()->getPosition().first  );
  oldPos.setY( _stateItem->getModel()->getPosition().second );
}


MoveStateCommand::~MoveStateCommand() {
}


void  MoveStateCommand::redo(){
  State * state = fsm->getStateByID( movedStateId );
  StateItem * stateItem = relatedScene->findStateItem( state );
  stateItem->setPos(newPos);
  state->setPosition(std::make_pair(newPos.x(), newPos.y()));
  stateItem->modelChanged();

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  MoveStateCommand::undo(){
  State * state = fsm->getStateByID( movedStateId );
  StateItem * stateItem = relatedScene->findStateItem( state );
  stateItem->setPos(oldPos);
  state->setPosition(std::make_pair(oldPos.x(), oldPos.y()));
  stateItem->modelChanged();

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}


int   MoveStateCommand::id() const {
  return Id;
}


bool  MoveStateCommand::mergeWith(const QUndoCommand * command) {
  // Ensure, that merely MoveStateCommands of the same item are merged.
  const MoveStateCommand *moveCommand = static_cast<const MoveStateCommand *>(command);
  int nextItemID = moveCommand->getItemID();
  if (movedStateId != nextItemID) 
    return false;

  // Merge the commands for the same item.
  State * state = fsm->getStateByID( movedStateId );
  StateItem * nextItem = relatedScene->findStateItem( state );
  newPos = nextItem->scenePos();
  return true;
}


int MoveStateCommand::getItemID() const {
  return movedStateId;
}
