#include "gui/commands/CreateItemGroupCommand.h"


CreateItemGroupCommand::CreateItemGroupCommand( Scene * _relatedScene,
                                                QList<QGraphicsItem *> _selectedItems,
                                                QUndoCommand * _parentCommand) :
                                                QUndoCommand(_parentCommand), 
                                                relatedScene(_relatedScene),
                                                selectedItems(_selectedItems)
{
  itemGroup = NULL;
}


CreateItemGroupCommand::~CreateItemGroupCommand() {
}


void  CreateItemGroupCommand::redo() {
  Q_ASSERT( itemGroup == NULL );
  itemGroup = relatedScene->createItemGroup( selectedItems );
  Q_ASSERT( itemGroup != NULL );
  itemGroup->setFlags(   QGraphicsItem::ItemIsSelectable
                       | QGraphicsItem::ItemIsMovable
                       | QGraphicsItem::ItemIsFocusable
                     );
  itemGroup->setFocus();
  itemGroup->setSelected(true);
  // Just select the itemGroup and deselect its children
  QList<QGraphicsItem *>::Iterator it;
  for (it = selectedItems.begin(); it != selectedItems.end(); ++it) {
    (*it)->setSelected(false);
  }

  relatedScene->update();
}


void  CreateItemGroupCommand::undo() {
  relatedScene->destroyItemGroup( itemGroup );
  itemGroup = NULL;
  // The child items could be queried by itemGroup->childItems(),
  // but since the command is responsible for generating the itemGroup,
  // its item-set MUST contain the same items as the itemGroup->childItems()
  // set.
  QList<QGraphicsItem *>::iterator child_it;;
  for (child_it = selectedItems.begin(); child_it != selectedItems.end(); ++child_it) {
    (*child_it)->setSelected(false);
    (*child_it)->clearFocus();
  }

  relatedScene->update();
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
  Q_ASSERT( itemGroup != NULL );
  selectedItems.removeAt( selectedItems.indexOf( itemGroup ) );

  const_cast<CreateItemGroupCommand *>(groupCommand)->undo(); //Hackish
  this->undo();

  // Create the flattened itemGroup.
  this->redo();

  return true;
}


QList<QGraphicsItem *> CreateItemGroupCommand::getSelectedItems() const {
  return selectedItems;
}
