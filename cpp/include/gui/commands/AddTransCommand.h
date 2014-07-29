/**
 * \file AddTransCommand.h
 *
 */

#ifndef __ADDTRANSCOMMAND_H__
#define __ADDTRANSCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtGui>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/Transline.h"
#include "gui/items/TranslineText.h"
#include "gui/items/TrackpointItem.h"

//-- Core
#include "core/Fsm.h"

class AddTransCommand : public QUndoCommand {
  private:
    // Related objects
    Scene      * relatedScene;
    bool         bLastCommand; // CreateItemGroupCommand?

    QList<Transline       *> transList; // Related GUI object.
    TranslineText         *  text     ; // Text.
    QList<TrackpointItem  *> trackList; // Trackpoints.

    QGraphicsItem * getIntersectingItem(QGraphicsSceneMouseEvent * e) const;



  public:
    AddTransCommand( Scene     * _relatedScene,
                     QUndoCommand   * _parentCommand = NULL);

    virtual ~AddTransCommand();

    virtual void    redo();
    virtual void    undo();

    bool handleMouseEvent(QGraphicsSceneMouseEvent * e);

    // Provide setter in order to add trackpoints and translines.
    bool addTransLine ( Transline      * transItem);
    bool addTransText ( TranslineText  * transText);
    bool addTrackPoint( TrackpointItem * trackItem);
};

#endif // __ADDTRANSCOMMAND_H__
