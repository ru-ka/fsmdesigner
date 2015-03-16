/**
 * \file NewHyperTransCommand.h
 *
 */

#ifndef __NEWHYPERTRANSCOMMAND_H__
#define __NEWHYPERTRANSCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/Transline.h"
#include "gui/items/TranslineText.h"
#include "gui/items/StateItem.h"
#include "gui/items/HyperTransition.h"

//-- Core
#include "core/Fsm.h"

class NewHyperTransCommand : public QUndoCommand {
  private:
    // Related objects
    Scene * relatedScene;
    Fsm   * fsm         ;
    bool    bLastCommand; // CreateItemGroupCommand?

    HyperTransition * hypertransition;
    Hypertrans      * hypertransModel;
    Transline       * transline      ;
    StateItem       * targetState    ;

    QGraphicsItem * getIntersectingItem(QGraphicsSceneMouseEvent * e) const;

  public:
    NewHyperTransCommand( Scene     * _relatedScene,
                     QUndoCommand   * _parentCommand = NULL);

    virtual ~NewHyperTransCommand();

    virtual void    redo();
    virtual void    undo();

    // Returns, whether the event changed the command or not.
    bool handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e);
    bool handleMouseMoveEvent(QGraphicsSceneMouseEvent * e);

    bool commandReady() const;

};

#endif // __NEWHYPERTRANSCOMMAND_H__
