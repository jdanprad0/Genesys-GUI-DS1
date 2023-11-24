#include "AddUndoCommand.h"
#include <QDebug>

AddUndoCommand::AddUndoCommand(QGraphicsItem *item, ModelGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicsScene(scene), _firstExecution(true) {

    _myComponentItem.graphicalComponent = nullptr;
    _myConnectionItem = nullptr;
    _myDrawingItem = nullptr;

    // filtra o tipo do item
    if (GraphicalModelComponent *component = dynamic_cast<GraphicalModelComponent *>(item)) {
        ComponentItem componentItem;

        componentItem.graphicalComponent = component;
        componentItem.initialPosition = component->pos();

        if (!component->getGraphicalInputPorts().empty() && !component->getGraphicalInputPorts().at(0)->getConnections()->empty()) {
            for (int j = 0; j < component->getGraphicalInputPorts().at(0)->getConnections()->size(); ++j) {
                componentItem.inputConnections.append(component->getGraphicalInputPorts().at(0)->getConnections()->at(j));
            }
        }
        
        for (int j = 0; j < component->getGraphicalOutputPorts().size(); ++j) {
            GraphicalComponentPort *port = component->getGraphicalOutputPorts().at(j);

            if (!port->getConnections()->empty())
                componentItem.outputConnections.append(port->getConnections()->at(0));
        }

        _myComponentItem = componentItem;
    } else if (GraphicalConnection *connection = dynamic_cast<GraphicalConnection *>(item)) {
        _myConnectionItem = connection;
    } else {
        _myDrawingItem = item;
    }

    setText(QObject::tr("Add"));
}

AddUndoCommand::~AddUndoCommand() {}

void AddUndoCommand::undo() {
    // remove o que é gráfico

    // remove as conexões individuais
    if (_myConnectionItem != nullptr)
        _myGraphicsScene->removeItem(_myConnectionItem);

    // remove os componentes
    if (_myComponentItem.graphicalComponent != nullptr) {
        for (int j = 0; j < _myComponentItem.inputConnections.size(); ++j) {
            GraphicalConnection *connection = _myComponentItem.inputConnections.at(j);
            _myGraphicsScene->removeItem(connection);
        }

        for (int j = 0; j < _myComponentItem.outputConnections.size(); ++j) {
            GraphicalConnection *connection = _myComponentItem.outputConnections.at(j);
            _myGraphicsScene->removeItem(connection);
        }

        _myGraphicsScene->removeItem(_myComponentItem.graphicalComponent);
    }

    // remove os outros itens do tipo QGraphicsItem da tela
    if (_myDrawingItem != nullptr)
        _myGraphicsScene->removeItem(_myDrawingItem);

    // agora remove o que deve ser removido do modelo

    // remove os componentes
    if (_myComponentItem.graphicalComponent != nullptr)
        _myGraphicsScene->removeComponent(_myComponentItem.graphicalComponent, true);

    // remove as conexões
    if (_myConnectionItem != nullptr) {
        GraphicalModelComponent *source = _myGraphicsScene->findGraphicalModelComponent(_myConnectionItem->getSource()->component->getId());
        GraphicalModelComponent *destination = _myGraphicsScene->findGraphicalModelComponent(_myConnectionItem->getDestination()->component->getId());

        // verifica se a conexão ainda existe, pois ela pode já ter sido removida caso fizesse parte de um componente que foi removido
        if (_myGraphicsScene->getGraphicalConnections()->contains(_myConnectionItem)) {
            // se ela existe, a remove
            _myGraphicsScene->removeGraphicalConnection(_myConnectionItem, source, destination, true);
        } else {
            // se não existe, quer dizer que a conexão faz parte de um componente que foi removido, e portando ela já foi removida
            // entao limpa sua referência
            _myConnectionItem = nullptr;
        }
    }

    // remove os outros itens do tipo QGraphicsItem
    if (_myDrawingItem != nullptr)
        _myGraphicsScene->removeDrawing(_myDrawingItem, true);

    // atualiza a cena
    _myGraphicsScene->update();
}

void AddUndoCommand::redo() {
    // adiciona tudo que é gráfico

    // adiciona o componente
    if (_myComponentItem.graphicalComponent != nullptr) {
        _myGraphicsScene->addItem(_myComponentItem.graphicalComponent);
    }

    // adiciona as conex dos componentes
    if (_myComponentItem.graphicalComponent != nullptr) {
        for (int j = 0; j < _myComponentItem.inputConnections.size(); ++j) {
            GraphicalConnection *connection = _myComponentItem.inputConnections.at(j);
            _myGraphicsScene->addItem(connection);
        }

        for (int j = 0; j < _myComponentItem.outputConnections.size(); ++j) {
            GraphicalConnection *connection = _myComponentItem.outputConnections.at(j);
            _myGraphicsScene->addItem(connection);
        }
    }

    // adiciona as conexões individuais
    if (_myConnectionItem != nullptr) {
        _myGraphicsScene->addItem(_myConnectionItem);
    }

    // adiciona os outros itens do tipo QGraphicsItem na tela
    if (_myDrawingItem != nullptr) {
        _myGraphicsScene->addItem(_myDrawingItem);
    }

    // agora adiciona o que se deve no modelo

    // adiciona componente
    if (_myComponentItem.graphicalComponent != nullptr) {
        if (!_firstExecution) {
            _myGraphicsScene->insertComponent(_myComponentItem.graphicalComponent, &_myComponentItem.inputConnections, &_myComponentItem.outputConnections, true, false, true);
        } else {
            _firstExecution = false;
        }
    }

    // realiza as conexõo entre os componentes da conexão
    if (_myConnectionItem != nullptr) {
        // verifico se a conexão ainda não existe (ela pode ser uma conexão que foi refeita no connectComponents)
        // por exemplo, pode ter uma conexão selecionada que foi eliminada anteriormente pois ela fazia parte de um componente
        // então ao refazer as conexões do componente, ela já foi religada
        if (!_myGraphicsScene->getGraphicalConnections()->contains(_myConnectionItem)) {
            GraphicalModelComponent* sourceComp = _myGraphicsScene->findGraphicalModelComponent(_myConnectionItem->getSource()->component->getId());
            GraphicalModelComponent* destComp = _myGraphicsScene->findGraphicalModelComponent(_myConnectionItem->getDestination()->component->getId());

            _myGraphicsScene->clearPorts(_myConnectionItem, sourceComp, destComp);

            _myGraphicsScene->connectComponents(_myConnectionItem, sourceComp, destComp, true);
        }
    }

    // adiciona os outros itens do tipo QGraphicsItem
    if (_myDrawingItem != nullptr)
        _myGraphicsScene->addDrawing(_myDrawingItem, true);

    // atualiza a cena
    _myGraphicsScene->update();
}
