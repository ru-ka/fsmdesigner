/**
 * \file RemoveStateCommand.h
 *
 */

#ifndef __REMOVESTATECOMMAND_H__
#define __REMOVESTATECOMMAND_H__

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
 * \brief Remove command for a state.
 */
class RemoveStateCommand : public QUndoCommand {
  private:
    // Related objects
    Scene     * relatedScene;
    StateItem * stateItem   ; // Related GUI object.
    State     * state       ; // Related fsm object.
    Fsm       * fsm         ;
    bool        bLastCommand; // NewItemGroupCommand?

    public:
      /**
       * Constructor.
       *
       */
      RemoveStateCommand( Scene         * _relatedScene,
                          StateItem     * _guiItem,
                          QUndoCommand  * _parentCommand = NULL);

      /**
       * Default Destructor
       */
      virtual ~RemoveStateCommand();

      /** \defgroup UndoRedo Undo Redo Methods to be implemented */
      /** @{ */

      virtual void    redo();
      virtual void    undo();
      /** @} */
};

#endif /* __REMOVESTATECOMMAND_H__ */
