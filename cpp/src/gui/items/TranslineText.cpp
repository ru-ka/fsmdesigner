/*
 * TranslineText.cpp
 *
 *  Created on: Oct 21, 2010
 *      Author: rleys
 */
// Includes
//--------------

//-- Std
#include <iostream>
using namespace std;

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Core
#include <core/Core.h>
#include <core/Fsm.h>
#include <core/State.h>
#include <core/Trans.h>
#include <core/Trackpoint.h>

//-- Gui
#include <gui/scene/Scene.h>
#include <gui/items/FSMGraphicsItem.h>
#include <gui/items/FSMGraphicsTextItem.h>
#include <gui/items/Transline.h>

#include "TranslineText.h"

TranslineText::TranslineText(QString qs,Trans * transition) :
		FSMGraphicsTextItem(qs, NULL) {

	this->name = qs;
	this->transition = transition;

	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
	this->setFlag(QGraphicsItem::ItemIsFocusable,true);
	this->setZValue(200000);

}

TranslineText::~TranslineText(){

}


Trans * TranslineText::getTransition() {
    return this->transition;
}

bool TranslineText::recordText() {


	this->transition->setName(this->toPlainText().toStdString());
	return true;

}

void TranslineText::paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget) {

	painter->setRenderHint(QPainter::Antialiasing);

	QGraphicsTextItem::paint(painter, option, widget);
}

QVariant TranslineText::itemChange(GraphicsItemChange change,
		const QVariant & value) {

	// Selection Change
	//---------------------
	if (change == QGraphicsItem::ItemSelectedHasChanged) {

		if (value.toBool() == true) {

			this->setDefaultTextColor(Qt::red);


		} else {

			// Unselect
			this->setDefaultTextColor(Qt::black);
			this->setOpacity(1.0);


		}
	} else if (change == QGraphicsItem::ItemPositionHasChanged) {
		this->recordPosition();
	}

	return FSMGraphicsTextItem::itemChange(change, value);

}



bool TranslineText::recordPosition() {

	this->transition->setTextPosition( this->pos().x(),this->pos().y());


	return true;
}

void TranslineText::modelChanged() {

    if (this->getTransition()!=NULL) {

        // Update text
        this->setPlainText(QString::fromStdString(this->getTransition()->getName()));

        // Add an underline if default
        if (this->getTransition()->isDefault()) {
            this->setPlainText(tr("<u>")+this->toPlainText()+tr("</u>"));
        }

    }

}

QRectF TranslineText::boundingRect () {
	return QGraphicsTextItem::boundingRect();
}
