/*
 * FSMGraphicsTextItem.h
 *
 *  Created on: Oct 21, 2010
 *      Author: rleys
 */

#ifndef FSMGRAPHICSTEXTITEM_H_
#define FSMGRAPHICSTEXTITEM_H_

// Includes
//--------------

//-- Gui
#include <FSMGraphicsItem.h>

//-- Qt
#include <QtCore>
#include <QtWidgets>

/**
 * This class is a QGraphicsTextItem that implements non editable
 * text display behaviour, combined with editable behaviour
 * when double clicked
 */
class FSMGraphicsTextItem : public QGraphicsTextItem , public FSMGraphicsItem<QString> {

	Q_OBJECT

public:
	/// Default constructor
	FSMGraphicsTextItem(QString &qs, QGraphicsItem* parent=NULL);
	virtual ~FSMGraphicsTextItem();

	/// Does nothing per default
	virtual bool recordPosition();

  virtual QRectF  boundingRect () const;

public Q_SLOTS:

	/**
	 * Starts editing the text
	 */
	virtual void startEditing();

	/**
	 * Stop editing the text
	 */
	virtual void stopEditing();


protected:

    /// Store start time of editing to filter unselection events based on time
    QDateTime * editorStartTime;



	/// Overriden to detect selection changes and thus painting parameters
	virtual QVariant itemChange ( GraphicsItemChange change, const QVariant & value );

	/// Activate editor mode
	virtual void 	mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event );

	/// Moving object should not move propagated selected objects
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

	/// Adjust size in dynamic
	virtual void keyPressEvent ( QKeyEvent * event );

	/**
	 * Edit on F2
	 * @param event
	 */
	virtual void keyReleaseEvent(QKeyEvent * event);

	/**
	 *
	 * @param event
	 */
	virtual void	focusOutEvent ( QFocusEvent * event );


	/// record the new text for the model
	/// Does nothing per default
	/// @return true if something recorded, false if not
	virtual bool recordText()  ;


};

#endif /* FSMGRAPHICSTEXTITEM_H_ */
