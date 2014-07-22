#include "gui/commands/DeleteStateCommand.h"


DeleteStateCommand::DeleteStateCommand( Scene * _relatedScene,
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


DeleteStateCommand::~DeleteStateCommand() {
}


void  DeleteStateCommand::redo(){
  qDebug() << "-------------------------";
  qDebug() << "Delete state item undo() ";
  qDebug() << "-------------------------";
  relatedScene->removeItem(   stateItem );
  fsm->deleteState( state );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  DeleteStateCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "Delete state item redo() ";
  qDebug() << "-------------------------";
  relatedScene->addItem(   stateItem );
  fsm->addState( state );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}
