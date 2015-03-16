/**
 * \file CreateItemGroupCommand.h
 *
 */

#ifndef __CREATEITEMGROUPCOMMAND_H__
#define __CREATEITEMGROUPCOMMAND_H__

// Includes
//--------------------

//-- Qt
#include <QtCore>
#include <QtWidgets>

//-- Gui
#include <gui/scene/Scene.h>

/**
 * \brief Create an itemGroup with this command.
 */
class CreateItemGroupCommand : public QUndoCommand {
  private:
    // Related objects
    Scene               *   relatedScene ;
    QList<QGraphicsItem *>  selectedItems;
    QGraphicsItemGroup  *   itemGroup    ;


  public:
    enum { Id = FSMDesigner::CREATEITEMGROUPCOMMAND };
    /**
     * Constructor.
     */
    CreateItemGroupCommand( Scene         * _relatedScene,
                            QList<QGraphicsItem *> _selectedItems,
                            QUndoCommand  * _parentCommand = NULL);

    /**
     * Default Destructor
     */
    virtual ~CreateItemGroupCommand();

    /** \defgroup UndoRedo Undo Redo Methods to be implemented */
    /** @{ */

    virtual int id() const;
    virtual void redo();
    virtual void undo();
    virtual bool mergeWith(const QUndoCommand * command);
    /** @} */

    QList<QGraphicsItem *> getSelectedItems() const;
};

#endif /* __CREATEITEMGROUPCOMMAND_H__ */
