#include "MoveUndoCommand.h"
#include "ModelGraphicsScene.h"

MoveUndoCommand::MoveUndoCommand(QList<QGraphicsItem*> item, ModelGraphicsScene *scene, QList<QPointF> &oldPos, QList<QPointF> &newPos, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicalItem(item), _myGraphicsScene(scene), _myOldPos(oldPos), _myNewPos(newPos) {
    setText(QObject::tr("Move"));

    for (int i = 0; i < oldPos.size(); i++) {
        qreal x1 = oldPos.at(i).x();
        qreal x2 = oldPos.at(i).y();

        if (x1 == 0 || x2 == 0) {
            int a = 1;
        }
    }

    for (int i = 0; i < newPos.size(); i++) {
        qreal x1 = newPos.at(i).x();
        qreal x2 = newPos.at(i).y();

        if (x1 == 0 || x2 == 0) {
            int a = 1;
        }
    }
}

MoveUndoCommand::~MoveUndoCommand() {}

void MoveUndoCommand::undo() {

    for (int i = 0; i < _myGraphicalItem.size(); i++) {
        QPointF oldPos = _myOldPos[i];
         _myGraphicalItem[i]->setPos(oldPos);

        if (GraphicalModelComponent *component = dynamic_cast<GraphicalModelComponent *> (_myGraphicalItem[i])) {
            component->setOldPosition(oldPos);
        } else {
            _myGraphicsScene->insertOldPositionItem(_myGraphicalItem[i], oldPos);
        }
    }

    // atualiza a cena
    _myGraphicsScene->update();
}

void MoveUndoCommand::redo() {
    for (int i = 0; i < _myGraphicalItem.size(); i++) {
        QPointF newPos = _myNewPos[i];
        _myGraphicalItem[i]->setPos(newPos);
    }

    // atualiza a cena
    _myGraphicsScene->update();
}
