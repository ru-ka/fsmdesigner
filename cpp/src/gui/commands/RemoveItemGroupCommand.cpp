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
  QList<QGraphicsItem *>::iterator child_it;;
  for (child_it = selectedItems.begin(); child_it != selectedItems.end(); ++child_it) {
    itemGroup->removeFromGroup( *child_it );
  }
  relatedScene->removeItem( itemGroup );

  relatedScene->clearSelection();

  relatedScene->update();
  relatedScene->bLastCommand = false;
  relatedScene->activeGroup = NULL;
}


void  RemoveItemGroupCommand::undo() {
  qDebug() << "-------------------------";
  qDebug() << "Delete item group undo() ";
  qDebug() << "-------------------------";
  relatedScene->addItem( itemGroup );
  QList<QGraphicsItem *>::Iterator it;
  for (it = selectedItems.begin(); it != selectedItems.end(); ++it) {
    itemGroup->addToGroup( *it );
    (*it)->setSelected(false);
    (*it)->clearFocus();
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
