/**
 * \file DeleteItemGroupCommand.h
 *
 */

#ifndef __DELETEITEMGROUPCOMMAND_H__
#define __DELETEITEMGROUPCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtGui>

//-- Gui
#include <gui/scene/Scene.h>

/**
 * \brief Delete an itemGroup with this command.
 */
class DeleteItemGroupCommand : public QUndoCommand {
  private:
    // Related objects
    Scene               *   relatedScene ;
    QList<QGraphicsItem *>  selectedItems;
    QGraphicsItemGroup  *   itemGroup    ;

    bool bLastCommand; // CreateItemGroupCommand?


  public:
    /**
     * Constructor.
     */
    DeleteItemGroupCommand( Scene         * _relatedScene,
                            QGraphicsItemGroup * _itemGroup,
                            QUndoCommand  * _parentCommand = NULL);

    /**
     * Default Destructor
     */
    virtual ~DeleteItemGroupCommand();

    /** \defgroup UndoRedo Undo Redo Methods to be implemented */
    /** @{ */

    virtual void redo();
    virtual void undo();
    /** @} */
};

#endif /* __DELETEITEMGROUPCOMMAND_H__ */
