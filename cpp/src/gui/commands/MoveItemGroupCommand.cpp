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

  relatedScene->update();
  relatedScene->bLastCommand = false;
}


void  MoveItemGroupCommand::undo(){
  qDebug() << "-------------------------";
  qDebug() << "Move item group undo() ";
  qDebug() << "-------------------------";
  itemGroup->setPos(oldPos);

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
