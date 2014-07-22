#include "gui/commands/CreateItemGroupCommand.h"


CreateItemGroupCommand::CreateItemGroupCommand( Scene * _relatedScene,
                                                QList<QGraphicsItem *> _selectedItems,
                                                QUndoCommand * _parentCommand) :
                                                QUndoCommand(_parentCommand), 
                                                relatedScene(_relatedScene),
                                                selectedItems(_selectedItems)
{
  itemGroup = relatedScene->createItemGroup( selectedItems );
  Q_ASSERT( itemGroup != NULL );

  // Remove the items from the item group, in oder to add them through the redo-method
  // consistently.
  QList<QGraphicsItem *>::iterator child_it;;
  for (child_it = selectedItems.begin(); child_it != selectedItems.end(); ++child_it) {
    itemGroup->removeFromGroup( *child_it );
  }
}


CreateItemGroupCommand::~CreateItemGroupCommand() {
  relatedScene->destroyItemGroup( itemGroup );
}


void  CreateItemGroupCommand::redo() {
  qDebug() << "-------------------------";
  qDebug() << "Create item group redo() ";
  qDebug() << "-------------------------";
  QList<QGraphicsItem *>::Iterator it;
  for (it = selectedItems.begin(); it != selectedItems.end(); ++it) {
    (*it)->setSelected(false);
    (*it)->clearFocus();
    itemGroup->addToGroup( *it );
  }
  itemGroup->setFlags(   QGraphicsItem::ItemIsSelectable
                       | QGraphicsItem::ItemIsMovable
                       | QGraphicsItem::ItemIsFocusable
                     );
  // Just select the itemGroup and deselect its children

  itemGroup->setFocus();
  itemGroup->setSelected(true);

  relatedScene->update();
  relatedScene->bLastCommand = true;
  relatedScene->activeGroup = itemGroup;
}


void  CreateItemGroupCommand::undo() {
  qDebug() << "-------------------------";
  qDebug() << "Create item group undo() ";
  qDebug() << "-------------------------";
  // The child items could be queried by itemGroup->childItems(),
  // but since the command is responsible for generating the itemGroup,
  // its item-set MUST contain the same items as the itemGroup->childItems()
  // set.
  QList<QGraphicsItem *>::iterator child_it;;
  for (child_it = selectedItems.begin(); child_it != selectedItems.end(); ++child_it) {
    itemGroup->removeFromGroup( *child_it );
    (*child_it)->setSelected(false);
    (*child_it)->clearFocus();
  }

  relatedScene->update();
  relatedScene->bLastCommand = false;
  relatedScene->activeGroup = NULL;
}


int   CreateItemGroupCommand::id() const {
  return Id;
}


/*
 *  This is a hackish approach.
 *  The reason for this is, the redo()-method of each pushed command is
 *  executed by the undoStack.
 *
 *  DoubleCheck if merging is desirable.
 */
bool  CreateItemGroupCommand::mergeWith(const QUndoCommand * command) {
  const CreateItemGroupCommand *groupCommand =
    static_cast<const CreateItemGroupCommand*>(command);
  // Append all recently selected graphic items.
  selectedItems.append( groupCommand->getSelectedItems() );
  // Remove the destroyed groupItem.
  selectedItems.removeAt( selectedItems.indexOf( itemGroup ) );

  this->undo();

  // Create the flattened itemGroup.
  this->redo();

  return true;
}


QList<QGraphicsItem *> CreateItemGroupCommand::getSelectedItems() const {
  return selectedItems;
}
