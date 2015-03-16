/**
 * \file NewLinkCommand.h
 *
 */

#ifndef __NEWLINKCOMMAND_H__
#define __NEWLINKCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
#include <gui/scene/Scene.h>

//-- Items
#include "gui/items/StateItem.h"
#include "gui/items/Transline.h"
#include "gui/items/LinkArrival.h"
#include "gui/items/LinkDeparture.h"


//-- Core
#include "core/Fsm.h"
#include "core/Link.h"
#include "core/Trackpoint.h"
#include "core/Trans.h"

class NewLinkCommand : public QUndoCommand {
  private:
    // Related objects
    Scene * relatedScene;
    Fsm   * fsm         ;
    bool    bLastCommand; // CreateItemGroupCommand?

    StateItem     * startItem ;
    Transline     * startTrans;
    Trans         * transModel;
    Trackpoint    * trackPoint;
    LinkDeparture * linkDepart;
    StateItem     * endItem   ;
    Transline     * endTrans  ;
    LinkArrival   * linkArrival;
    Link          * link      ;
    bool            finished  ;



    QGraphicsItem * getIntersectingItem(QGraphicsSceneMouseEvent * e) const;
    void addTrackPointsToJoinModel();


  public:
    NewLinkCommand( Scene          * _relatedScene,
                     QUndoCommand   * _parentCommand = NULL);

    virtual ~NewLinkCommand();

    virtual void    redo();
    virtual void    undo();

    bool handleMouseReleaseEvent(QGraphicsSceneMouseEvent * e);
    bool handleMouseMoveEvent(QGraphicsSceneMouseEvent * e);

    bool commandReady() const;

};

#endif // __NEWLINKCOMMAND_H__
