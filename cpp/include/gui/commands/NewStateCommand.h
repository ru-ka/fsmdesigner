/**
 * \file NewStateCommand.h
 *
 */

#ifndef __NEWSTATECOMMAND_H__
#define __NEWSTATECOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
//#include <gui/scene/Scene.h>
class Scene;

//-- Items
#include "gui/items/StateItem.h"

//-- Core
#include "core/Fsm.h"

/**
 * \brief New command for a state.
 */
class NewStateCommand : public QUndoCommand {
  private:
    // Related objects
    Scene     * relatedScene;
    Fsm       * fsm         ;
    bool        bLastCommand; // NewItemGroupCommand?

    StateItem * stateItem   ; // Related GUI object.
    State     * state       ; // Related fsm object.

    //QGraphicsSceneMouseEvent * mouseEvent;
    QPointF     scenePos;

    public:
      /**
       * Constructor.
       *
       */
      NewStateCommand(  Scene         * _relatedScene,
                        QGraphicsSceneMouseEvent * _e,
                        QUndoCommand  * _parentCommand = NULL);

      /**
       * Default Destructor
       */
      virtual ~NewStateCommand();

      /** \defgroup UndoRedo Undo Redo Methods to be implemented */
      /** @{ */

      virtual void    redo();
      virtual void    undo();
      /** @} */
};

#endif /* __NEWSTATECOMMAND_H__ */
