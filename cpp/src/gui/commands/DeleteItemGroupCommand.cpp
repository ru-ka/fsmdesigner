#include "gui/commands/DeleteItemGroupCommand.h"


DeleteItemGroupCommand::DeleteItemGroupCommand( Scene * _relatedScene,
                                                QGraphicsItemGroup * _itemGroup,
                                                QUndoCommand * _parentCommand) :
                                                QUndoCommand(_parentCommand), 
                                                relatedScene(_relatedScene),
                                                itemGroup(_itemGroup),
                                                selectedItems(_itemGroup->childItems())
{
}


DeleteItemGroupCommand::~DeleteItemGroupCommand() {
}


void  DeleteItemGroupCommand::redo() {
  // Deselect the children.
  QList<QGraphicsItem *>::iterator child_it;;
  for (child_it = selectedItems.begin(); child_it != selectedItems.end(); ++child_it) {
    itemGroup->removeFromGroup( *child_it );
    (*child_it)->setSelected( false );
    (*child_it)->clearFocus();
  }



  relatedScene->update();
}


void  DeleteItemGroupCommand::undo() {
  QList<QGraphicsItem *>::Iterator it;
  for (it = selectedItems.begin(); it != selectedItems.end(); ++it) {
    itemGroup->addToGroup( *it );
    (*it)->setSelected(false);
  }
  itemGroup->setFlags(   QGraphicsItem::ItemIsSelectable
                       | QGraphicsItem::ItemIsMovable
                       | QGraphicsItem::ItemIsFocusable
                     );
  itemGroup->setFocus();
  itemGroup->setSelected(true);

  relatedScene->update();
}
