/**
 * \file MoveStateCommand.h
 *
 */

#ifndef __MOVESTATECOMMAND_H__
#define __MOVESTATECOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/StateItem.h"

//-- Core
#include "core/Fsm.h"

/**
 * \brief Move command for a state.
 */
class MoveStateCommand : public QUndoCommand {
  private:
    // Related objects
    Scene     * relatedScene;
    Fsm       * fsm         ;

    int         movedStateId;

    QPointF     oldPos      ;
    QPointF     newPos      ;

    bool        bLastCommand; // CreateItemGroupCommand?


  public:
    enum { Id = FSMDesigner::MOVESTATECOMMAND };
    /**
     * Constructor.
     *
     */
    MoveStateCommand( Scene         * _relatedScene,
                      StateItem     * _stateItem,
                      QUndoCommand  * _parentCommand = NULL);

    /**
     * Default Destructor
     */
    virtual ~MoveStateCommand();

    /** \defgroup UndoRedo Undo Redo Methods to be implemented */
    /** @{ */

    virtual int id() const;
    virtual void redo();
    virtual void undo();
    virtual bool mergeWith(const QUndoCommand * command);
    /** @} */

    int getItemID()   const;
};

#endif /* __MOVESTATECOMMAND_H__ */
