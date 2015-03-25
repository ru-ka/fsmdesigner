#include "gui/commands/RemoveStateCommand.h"


RemoveStateCommand::RemoveStateCommand( Scene * _relatedScene,
                                        StateItem * _guiItem,
                                        QUndoCommand * _parentCommand) :
                                        QUndoCommand(_parentCommand), 
                                        relatedScene(_relatedScene),
                                        stateItem(_guiItem),
                                        state( _guiItem->getModel() ),
                                        fsm( _relatedScene->getFsm() ),
                                        bLastCommand( _relatedScene->bLastCommand )
{
}


RemoveStateCommand::~RemoveStateCommand() {
}


void  RemoveStateCommand::redo(){
  qDebug() << "-------------------------";
  qDebug() << "Remove state item undo() ";
  qDebug() << "-------------------------";
  relatedScene->removeItem(   stateItem );
  fsm->removeState( state );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  RemoveStateCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "Remove state item redo() ";
  qDebug() << "-------------------------";
  relatedScene->addItem(   stateItem );
  fsm->addState( state );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}
