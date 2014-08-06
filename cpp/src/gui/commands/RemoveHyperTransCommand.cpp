// TODO : If requested, the constructors could be designed in a safer manner.
// This is a quick hack, since the current approach to synchronize the model
// objects with the gui objects might be replaced.
#include "gui/commands/RemoveHyperTransCommand.h"


RemoveHyperTransCommand::RemoveHyperTransCommand( 
    Scene * _relatedScene,
    HyperTransition * _guiItem,
    QUndoCommand * _parentCommand) :
    QUndoCommand(_parentCommand), 
    relatedScene(_relatedScene),
    fsm( _relatedScene->getFsm() ),
    bLastCommand( _relatedScene->bLastCommand ),
    hypertransition(_guiItem),
    hypertransModel( _guiItem->getModel() ),
    transline( _guiItem->getOutgoingTransition() )
{
}

RemoveHyperTransCommand::RemoveHyperTransCommand( 
    Scene * _relatedScene,
    Transline * _guiItem,
    QUndoCommand * _parentCommand) :
    QUndoCommand(_parentCommand), 
    relatedScene(_relatedScene),
    fsm( _relatedScene->getFsm() ),
    bLastCommand( _relatedScene->bLastCommand ),
    hypertransition( dynamic_cast<HyperTransition*>( _guiItem->getStartItem() ) ),
    hypertransModel( hypertransition->getModel() ),
    transline( _guiItem )
{
}


RemoveHyperTransCommand::~RemoveHyperTransCommand() {
}


void  RemoveHyperTransCommand::redo(){
  qDebug() << "------------------------------";
  qDebug() << "Remove hypertransition undo() ";
  qDebug() << "------------------------------";

  // remove items from scene
  relatedScene->removeItem( transline );
  relatedScene->removeItem( hypertransition );

  // remove items from model
  fsm->deleteHypertrans( hypertransModel );

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  RemoveHyperTransCommand::undo(){
  qDebug() << "------------------------------";
  qDebug() << "Remove hypertransition redo() ";
  qDebug() << "------------------------------";

  // add items to model
  fsm->addHypertrans( hypertransModel );

  // add items to scene
  relatedScene->addItem( hypertransition );
  relatedScene->addItem( transline );
  
  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
}
