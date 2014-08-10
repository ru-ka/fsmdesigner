#include "gui/commands/RemoveItemGroupCommand.h"


RemoveItemGroupCommand::RemoveItemGroupCommand( Scene * _relatedScene,
                                                QGraphicsItemGroup * _itemGroup,
                                                QUndoCommand * _parentCommand) :
                                                QUndoCommand(_parentCommand), 
                                                relatedScene(_relatedScene),
                                                itemGroup(_itemGroup),
                                                selectedItems(_itemGroup->childItems()),
                                                bLastCommand( _relatedScene->bLastCommand )
{
}


RemoveItemGroupCommand::~RemoveItemGroupCommand() {
}


void  RemoveItemGroupCommand::redo() {
  qDebug() << "-------------------------";
  qDebug() << "Delete item group redo() ";
  qDebug() << "-------------------------";
  // Deselect the children.
  for( auto item : selectedItems ) {
    itemGroup->removeFromGroup( item );
  }
  relatedScene->clearSelection();

  relatedScene->removeItem( itemGroup );

  relatedScene->update();
  relatedScene->bLastCommand = false;
  relatedScene->activeGroup = NULL;
}


void  RemoveItemGroupCommand::undo() {
  qDebug() << "-------------------------";
  qDebug() << "Delete item group undo() ";
  qDebug() << "-------------------------";
  relatedScene->addItem( itemGroup );

  for( auto item : selectedItems ) {
    itemGroup->addToGroup( item );
    item->setSelected( false );
    item->clearFocus();
  }
  itemGroup->setFlags(   QGraphicsItem::ItemIsSelectable
                       | QGraphicsItem::ItemIsMovable
                       | QGraphicsItem::ItemIsFocusable
                     );
  itemGroup->setFocus();
  itemGroup->setSelected(true);

  relatedScene->update();
  relatedScene->bLastCommand = this->bLastCommand;
  relatedScene->activeGroup = itemGroup;
}
