/**
 * \file DeleteStateCommand.h
 *
 */

#ifndef __DELETESTATECOMMAND_H__
#define __DELETESTATECOMMAND_H__

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
class DeleteStateCommand : public QUndoCommand {
  private:
    // Related objects
    Scene     * relatedScene;
    Fsm       * fsm         ;
    bool        bLastCommand; // NewItemGroupCommand?

    QPointF     scenePos    ; // ScenePos of the item.
    int         stateID     ;

    public:
      /**
       * Constructor.
       *
       */
      DeleteStateCommand( Scene         * _relatedScene,
                          StateItem     * _guiItem,
                          QUndoCommand  * _parentCommand = NULL);

      /**
       * Default Destructor
       */
      virtual ~DeleteStateCommand();

      /** \defgroup UndoRedo Undo Redo Methods to be implemented */
      /** @{ */

      virtual void    redo();
      virtual void    undo();
      /** @} */
};

#endif /* __DELETESTATECOMMAND_H__ */
