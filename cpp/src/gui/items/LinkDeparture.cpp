/*
 * LinkDeparture.cpp
 *
 *  Created on: Oct 21, 2010
 *      Author: rleys
 */

// Includes
//---------------

//-- Std
#include <iostream>
#include <cmath>
#include <typeinfo>
using namespace std;

//-- Qt
#include <QtWidgets>
#include <QtCore>


//-- Core
#include <core/Fsm.h>
#include <core/Core.h>
#include <core/State.h>
#include <core/Trackpoint.h>
#include <core/Trans.h>

//-- Gui
#include <gui/scene/Scene.h>
#include <gui/items/TrackpointItem.h>
#include <gui/items/LinkArrival.h>
#include <gui/items/Transline.h>
#include <gui/items/StateItem.h>

//-- Undo
#include <gui/actions/DeleteLinkDepartureAction.h>




#include "LinkDeparture.h"
LinkDeparture::LinkDeparture(Trackpoint * model,QGraphicsItem * startItem) :
		TrackpointItem(model,startItem) {

	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);

	// Prepare Base shapte Painting path
	//-----------------------------
	qreal height=15;

	this->shapePath.moveTo(0,0);
	this->shapePath.addEllipse(-height/2,-height/2,height,height);

	this->setPath(this->shapePath);

	// Infos from model
	//-------------------------
	if (this->getModel()!=NULL) {
    qDebug() << "LinkDeparture: Model != NULL-> color should be set.";
    //qDebug() << this->getModel()->getTargetLink()->getColor();


    //-- Color: Local one or link one ?
      unsigned int targetColor = this->getModel()->getTargetLink() !=NULL ? this->getModel()->getTargetLink()->getColor() : this->getModel()->getColor();

      if (targetColor>0){

          this->setBrush(QBrush(QColor::fromRgb(targetColor)));

      } else if(this->getModel()->getTargetLink() !=NULL) {

          // Take in predefined list
          this->setBrush(QBrush(LinkArrival::getDefaultLinkColor(this->getModel()->getTargetLink()->getId())));
      }


	}
}


LinkDeparture::~LinkDeparture() {
}


void LinkDeparture::modelChanged() {
  TrackpointItem::modelChanged();
}

void LinkDeparture::preparePath() {

	//-- Start with base path
	QPainterPath shapePath = this->shapePath;

	// Update direction
	// Determine orientation using transition and previous element
	//---------

	// Rotate if we are left of first point of previous transline
	/*if (this->model!=NULL && this->previousTransline!=NULL &&
		this->x()<this->previousTransline->getStartItem()->x()) {
		this->setRotation(180);
		this->tr
	} else
		this->setRotation(0);*/


	// Prepare text
	//-----------------

	// Text
	QString txt(((Trans*)this->getModel()->getTransition())->getEndState()->getName().c_str());
	QSize textSize = QApplication::fontMetrics().size(Qt::TextSingleLine,txt);


	// Place text
	QRectF rect = shapePath.boundingRect();
	qreal alignWidth = rect.width()-textSize.width();
	shapePath.addText(alignWidth/2,rect.height()+textSize.height(),QApplication::font("Purisa"),txt);

	//-- Set new path
	this->setPath(shapePath);
}


QGraphicsScene * LinkDeparture::getScene() {
	return this->scene();
}

void LinkDeparture::setPreviousTransline(Transline * previousTransline) {

	TrackpointBaseItem::setPreviousTransline(previousTransline);

	// Update direction
	// Determine orientation using transition and previous element
	//---------
	this->preparePath();
}

void LinkDeparture::setEndItem(QGraphicsItem * item) {
	return;
}

void LinkDeparture::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget) {


	// Let parent paint
	painter->setRenderHint(QPainter::Antialiasing);
	QGraphicsPathItem::paint(painter,option,widget);
}

void LinkDeparture::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
	// Delegate
	TrackpointItem::mouseMoveEvent(event);

	this->preparePath();
}

void LinkDeparture::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {

	// Delegate to parent
	QGraphicsPathItem::mouseReleaseEvent(event);

	// Record position
	if (this->recordPosition()==true) {
		// Redraw
		//scene()->update();
		//((View*) this->scene()->views()[0])->redraw();
	}
}

QVariant LinkDeparture::itemChange(GraphicsItemChange change,
		const QVariant & value) {

	// Added to Scene
	//---------------------
	if (change == QGraphicsItem::ItemSceneHasChanged && this->scene()!=NULL) {
		//-- Need a start item
		if (this->startItem!=NULL && FSMGraphicsItem<>::isStateItem(this->startItem)) {

			// Tooltip (if we have a model)
			//-----------------------------
			if (this->getModel()!=NULL)
				this->setToolTip(QString().append(
						FSMGraphicsItem<>::toStateItem(this->startItem)->getModel()->getName().c_str())
						.append(" -> ")
						.append(((Trans*)this->getModel()->getTransition())->getEndState()->getName().c_str()));

			// Update Painting path
			//-----------------------------
			this->preparePath();

		}
	} // EOF added to scene

	return TrackpointItem::itemChange(change, value);

}

void LinkDeparture::selected() {


}

void LinkDeparture::deselected() {
	this->setOpacity(1.0);
}

bool LinkDeparture::recordPosition() {

	this->model->setPosition(make_pair(this->pos().x(),this->pos().y()));

	return true;
}
