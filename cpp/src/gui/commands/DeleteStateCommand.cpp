#include "gui/commands/DeleteStateCommand.h"


DeleteStateCommand::DeleteStateCommand( Scene * _relatedScene,
                                        StateItem * _guiItem,
                                        QUndoCommand * _parentCommand) :
                                        QUndoCommand(_parentCommand), 
                                        relatedScene(_relatedScene),
                                        fsm( _relatedScene->getFsm() ),
                                        bLastCommand( _relatedScene->bLastCommand ),
                                        scenePos( _guiItem->scenePos() ),
                                        stateID( _guiItem->getModel()->getId() )
{
}


DeleteStateCommand::~DeleteStateCommand() {
}


void  DeleteStateCommand::redo(){
  qDebug() << "-------------------------";
  qDebug() << "Remove state item undo() ";
  qDebug() << "-------------------------";
  State * state = fsm->getStateByID( stateID );
  StateItem * stateItem = relatedScene->findStateItem( state );
  relatedScene->removeItem( stateItem );
  delete stateItem;

  State * removed_state = fsm->removeState( state );
  Q_ASSERT( removed_state );
  delete removed_state;

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  DeleteStateCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "Remove state item redo() ";
  qDebug() << "-------------------------";
  //-- New GUI item for the Scene.
  StateItem * stateItem = new StateItem();
  //-- Set GUI properties.
  stateItem->setVisible( true );
  stateItem->setEnabled( true );

  //-- Create a new state to the underlying model and refer to it in the GUI item
  //int next_state_id = fsm->getNextStateId(); -- for debugging?
  State * state = fsm->createNextState();
  stateItem->setModel( state );

  //-- Place the as the previous item in the scene.
  state->setPosition( pair<double,double>( scenePos.x(), scenePos.y() ) );
  stateItem->setModel( state );

  //-- Pass the ownership of the state objects to the scene and the data model.
  relatedScene->addItem( stateItem );
  State * statePointer = fsm->addState( state );
  Q_ASSERT( statePointer != NULL );

  Q_ASSERT( state->getId() == stateID );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}
