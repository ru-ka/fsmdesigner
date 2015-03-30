#include "gui/commands/NewStateCommand.h"
#include "gui/scene/Scene.h"


NewStateCommand::NewStateCommand( Scene * _relatedScene,
                                  QGraphicsSceneMouseEvent * _e,
                                  QUndoCommand * _parentCommand) :
                                  QUndoCommand(_parentCommand), 
                                  relatedScene(_relatedScene),
                                  fsm( _relatedScene->getFsm() ),
                                  bLastCommand( _relatedScene->bLastCommand ),
                                  scenePos( _e->scenePos() ),
                                  stateId( 0 )
{
}


NewStateCommand::~NewStateCommand() {
}


void  NewStateCommand::redo(){
  qDebug() << "-------------------------";
  qDebug() << "New state item    redo() ";
  qDebug() << "-------------------------";
  //-- New GUI item for the Scene.
  StateItem * stateItem = new StateItem();
  //-- Set GUI properties.
  stateItem->setVisible( true );
  stateItem->setEnabled( true );

  //-- Create a new state to the underlying model and refer to it in the GUI item
  //int next_state_id = fsm->getNextStateId(); // -- for debugging?
  State * state = fsm->createNextState();
  stateItem->setModel( state );

  //-- Place centered on mouse
  state->setPosition(pair<double,double>( scenePos.x(),scenePos.y() ) );
  stateItem->setPos( scenePos );

  //-- Pass the ownership of the state objects to the scene and the model.
  relatedScene->addItem( stateItem ); 
  State * statePointer = fsm->addState( state );
  Q_ASSERT( ( stateId == state->getId() ) || ( stateId == 0 ) );
  stateId = state->getId();
  Q_ASSERT( statePointer != NULL );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  NewStateCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "New state item    undo() ";
  qDebug() << "-------------------------";
  // Get the ownership of the state objects.
  State * state = fsm->getStateByID( stateId );
  StateItem * stateItem = relatedScene->findStateItem( state );
  relatedScene->removeItem(   stateItem );
  fsm->removeState( state );

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;

  delete stateItem;
  delete state;
}
