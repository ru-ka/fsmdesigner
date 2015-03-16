/**
 * \file RemoveItemGroupCommand.h
 *
 */

#ifndef __REMOVEITEMGROUPCOMMAND_H__
#define __REMOVEITEMGROUPCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
#include <gui/scene/Scene.h>

/**
 * \brief Delete an itemGroup with this command.
 */
class RemoveItemGroupCommand : public QUndoCommand {
  private:
    // Related objects
    Scene               *   relatedScene ;
    QGraphicsItemGroup  *   itemGroup    ;
    QList<QGraphicsItem *>  selectedItems;

    bool bLastCommand; // CreateItemGroupCommand?


  public:
    /**
     * Constructor.
     */
    RemoveItemGroupCommand( Scene              * _relatedScene,
                            QGraphicsItemGroup * _itemGroup,
                            QUndoCommand       * _parentCommand = NULL);

    /**
     * Default Destructor
     */
    virtual ~RemoveItemGroupCommand();

    /** \defgroup UndoRedo Undo Redo Methods to be implemented */
    /** @{ */

    virtual void redo();
    virtual void undo();
    /** @} */
};

#endif /* __REMOVEITEMGROUPCOMMAND_H__ */
