#include "gui/commands/DeleteStateCommand.h"


DeleteStateCommand::DeleteStateCommand( Scene * _relatedScene,
                                        StateItem * _guiItem,
                                        QUndoCommand * _parentCommand) :
                                        QUndoCommand(_parentCommand), 
                                        relatedScene(_relatedScene),
                                        stateItem(_guiItem),
                                        state( _guiItem->getModel() ),
                                        fsm( _relatedScene->getFsm() )
{
}


DeleteStateCommand::~DeleteStateCommand() {
}


void  DeleteStateCommand::redo(){
  relatedScene->removeItem(   stateItem );
  fsm->deleteState( state );

  relatedScene->update();
}


void  DeleteStateCommand::undo(){
  relatedScene->addItem(   stateItem );
  fsm->addState( state );

  relatedScene->update();
}
