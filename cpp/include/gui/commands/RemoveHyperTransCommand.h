/**
 * \file RemoveHyperTransCommand.h
 *
 */

#ifndef __REMOVEHYPERTRANSCOMMAND_H__
#define __REMOVEHYPERTRANSCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtGui>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/HyperTransition.h"
#include "gui/items/Transline.h"

//-- Core
#include "core/Fsm.h"

/**
 * \brief Remove command for a hypertransition.
 */
class RemoveHyperTransCommand : public QUndoCommand {
  private:
    // Related objects
    Scene     * relatedScene;
    Fsm       * fsm         ;
    bool        bLastCommand; // NewItemGroupCommand?

    HyperTransition * hypertransition;
    Hypertrans      * hypertransModel;
    Transline       * transline      ;

    public:
      /**
       * Constructor.
       *
       */
      RemoveHyperTransCommand( Scene           * _relatedScene,
                               HyperTransition * _guiItem,
                               QUndoCommand    * _parentCommand = NULL);

      RemoveHyperTransCommand( Scene           * _relatedScene,
                               Transline       * _guiItem,
                               QUndoCommand    * _parentCommand = NULL);

      /**
       * Default Destructor
       */
      virtual ~RemoveHyperTransCommand();

      /** \defgroup UndoRedo Undo Redo Methods to be implemented */
      /** @{ */

      virtual void    redo();
      virtual void    undo();
      /** @} */
};

#endif /* __REMOVEHYPERTRANSCOMMAND_H__ */
