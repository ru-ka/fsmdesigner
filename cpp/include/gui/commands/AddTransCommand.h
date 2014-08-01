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
#include "gui/items/StateItem.h"
#include "gui/items/JoinItem.h"
#include "gui/items/Transline.h"
#include "gui/items/TranslineText.h"
#include "gui/items/TrackpointItem.h"


//-- Core
#include "core/Fsm.h"

class AddTransCommand : public QUndoCommand {
  private:
    // Related objects
    Scene * relatedScene;
    Fsm   * fsm         ;
    bool    bLastCommand; // CreateItemGroupCommand?

    QList<Transline       *> transList; // Related GUI object.
    TranslineText         *  text     ; // Text.
    QList<TrackpointItem  *> trackList; // Trackpoints.
    QGraphicsItem         *  startItem;
    QGraphicsItem         *  endItem  ;


    QGraphicsItem * getIntersectingItem(QGraphicsSceneMouseEvent * e) const;
    void addTrackPointsToTransModel( Trans * trans );
    void addModelToTranslines( Trans * trans );
    void createText( Trans * trans );



  public:
    AddTransCommand( Scene          * _relatedScene,
                     QUndoCommand   * _parentCommand = NULL);

    virtual ~AddTransCommand();

    virtual void    redo();
    virtual void    undo();

    bool handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e);
    bool handleMouseMoveEvent(QGraphicsSceneMouseEvent * e);

    bool commandReady() const;

};

#endif // __ADDTRANSCOMMAND_H__
