/**
 * \file NewJoinCommand.h
 *
 */

#ifndef __NEWJOINCOMMAND_H__
#define __NEWJOINCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

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
#include "core/Join.h"

class NewJoinCommand : public QUndoCommand {
  private:
    // Related objects
    Scene * relatedScene;
    Fsm   * fsm         ;
    bool    bLastCommand; // CreateItemGroupCommand?

    QList<Transline       *> transList; // Related GUI object.
    QList<TrackpointItem  *> trackList; // Trackpoints.
    QGraphicsItem         *  startItem;
    QGraphicsItem         *  endItem  ;
    Join                  *  join     ;


    QGraphicsItem * getIntersectingItem(QGraphicsSceneMouseEvent * e) const;
    void addTrackPointsToJoinModel();


  public:
    NewJoinCommand( Scene          * _relatedScene,
                     QUndoCommand   * _parentCommand = NULL);

    virtual ~NewJoinCommand();

    virtual void    redo();
    virtual void    undo();

    bool handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e);
    bool handleMouseMoveEvent(QGraphicsSceneMouseEvent * e);

    bool commandReady() const;

};

#endif // __NEWJOINCOMMAND_H__
