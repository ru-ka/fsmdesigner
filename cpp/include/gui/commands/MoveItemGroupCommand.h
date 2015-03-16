/**
 * \file MoveItemGroupCommand.h
 *
 */

#ifndef __MOVEITEMGROUPCOMMAND_H__
#define __MOVEITEMGROUPCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/StateItem.h"
#include "gui/items/TrackpointItem.h"

//-- Core
#include "core/Fsm.h"

/**
 * \brief Move command.
 */
class MoveItemGroupCommand : public QUndoCommand {
  private:
    // Related objects
    Scene               * relatedScene;
    QGraphicsItemGroup  * itemGroup   ;

    QPointF               oldPos      ;
    QPointF               newPos      ;

    bool                  bLastCommand; // CreateItemGroupCommand?


  public:
//    enum { Id = FSMDesigner::MOVEITEMGROUPCOMMAND }; //TODO
    /**
     * Constructor.
     *
     */
    MoveItemGroupCommand( Scene               * _relatedScene,
                          QGraphicsItemGroup  * _itemGroup,
                          QPointF               _oldPos,
                          QPointF               _newPos = QPointF(0,0),
                          QUndoCommand        * _parentCommand = NULL);

    /**
     * Default Destructor
     */
    virtual ~MoveItemGroupCommand();

    void updateTranslines();
    void setNewPos( QPointF _newPos);

    /** \defgroup UndoRedo Undo Redo Methods to be implemented */
    /** @{ */

//    virtual int id() const;
    virtual void redo();
    virtual void undo();
    virtual bool mergeWith(const QUndoCommand * command);
    /** @} */

};

#endif /* __MOVEITEMGROUPCOMMAND_H__ */
