/**
 * \file AddStateCommand.h
 *
 */

#ifndef __ADDSTATECOMMAND_H__
#define __ADDSTATECOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtGui>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/StateItem.h"

//-- Core
#include "core/Fsm.h"

/**
 * \brief Add command for a state.
 */
class AddStateCommand : public QUndoCommand {
  private:
    // Related objects
    Scene     * relatedScene;
    StateItem * stateItem   ; // Related GUI object.
    State     * state       ; // Related fsm object.
    Fsm       * fsm         ;

    QGraphicsSceneMouseEvent * mouseEvent;

    public:
      /**
       * Constructor.
       *
       */
      AddStateCommand(  Scene         * _relatedScene,
                        QGraphicsSceneMouseEvent * _e,
                        QUndoCommand  * _parentCommand = NULL);

      /**
       * Default Destructor
       */
      virtual ~AddStateCommand();

      /** \defgroup UndoRedo Undo Redo Methods to be implemented */
      /** @{ */

      virtual void    redo();
      virtual void    undo();
      /** @} */
};

#endif /* __ADDSTATECOMMAND_H__ */
