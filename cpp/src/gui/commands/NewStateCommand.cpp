#include "gui/commands/NewStateCommand.h"


NewStateCommand::NewStateCommand( Scene * _relatedScene,
                                  QGraphicsSceneMouseEvent * _e,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  mouseEvent(_e),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand )
{
  //-- New new GUI item to the Scene
  stateItem = new StateItem();
  // Set GUI properties.
  stateItem->setVisible( true );
  stateItem->setEnabled( true );

  //-- Create a new state to the underlying model and refer to it in the GUI item
  state = fsm->addState();
  stateItem->setModel( state );
  // The state should be added via the redo-method. The Fsm.h API currently
  // does not support the feature to generate a new state, but not to add it
  // internally yet.
  fsm->deleteState(state);

  //-- Place centered on mouse
  state->setPosition(pair<double,double>(mouseEvent->scenePos().x(),mouseEvent->scenePos().y()));
  stateItem->setPos(mouseEvent->scenePos());
}


NewStateCommand::~NewStateCommand() {
  // The command will only be destroyed in the undo-state.
  // In this state, the command has the ownership of the GUI and of the FSM
  // objects.
  delete stateItem;
  delete state;
}


void  NewStateCommand::redo(){
  qDebug() << "-------------------------";
  qDebug() << "New state item    redo() ";
  qDebug() << "-------------------------";
  // Pass the ownership of the state objects to the scene and the model.
  relatedScene->addItem( stateItem ); 
  fsm->addState( state );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  NewStateCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "New state item    undo() ";
  qDebug() << "-------------------------";
  // Get the ownership of the state objects.
  relatedScene->removeItem(   stateItem );
  fsm->deleteState( state );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}
