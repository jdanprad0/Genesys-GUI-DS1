#include "AddUndoCommand.h"
#include "ModelGraphicsView.h"

AddUndoCommand::AddUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), myGraphicalModelComponent(gmc), myGraphicsScene(scene), firstExecution(true) {
    initialPosition = QPointF(gmc->scenePos().x(), gmc->scenePos().y() + gmc->getHeight()/2);

    std::string position = "position=(x=" + std::to_string(gmc->scenePos().x()) + ", y=" + std::to_string(gmc->scenePos().y()) + ")";

    myGraphicsScene->update();
    setText(QObject::tr("Add %1")
                .arg(QString::fromStdString("name=" + gmc->getComponent()->getName() + ", " + position)));
}

AddUndoCommand::~AddUndoCommand() {}

void AddUndoCommand::undo() {
    //remove in model
    myGraphicsScene->removeModelComponentInModel(myGraphicalModelComponent);

    //limpa as conexoes
    myGraphicsScene->clearConnectionsComponent(myGraphicalModelComponent);

    //graphically
    myGraphicsScene->removeItem(myGraphicalModelComponent);
    myGraphicsScene->getGraphicalModelComponents()->removeOne(myGraphicalModelComponent);

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::COMPONENT, myGraphicalModelComponent);
    dynamic_cast<ModelGraphicsView*> (myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

    myGraphicsScene->update();
}

void AddUndoCommand::redo() {
    //graphically
    myGraphicsScene->addItem(myGraphicalModelComponent);
    myGraphicsScene->getGraphicalModelComponents()->append(myGraphicalModelComponent);

    if (!firstExecution) {
        //add in model
        myGraphicsScene->getSimulator()->getModels()->current()->insert(myGraphicalModelComponent->getComponent());

        //refaz as conexões exceto na inicializacao do objeto
        //myGraphicsScene->reconnectConnectionsOnRedoComponent(myGraphicalModelComponent);
    } else {
        firstExecution = false;
    }

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, myGraphicalModelComponent);
    dynamic_cast<ModelGraphicsView*> (myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

    myGraphicsScene->update();
}
