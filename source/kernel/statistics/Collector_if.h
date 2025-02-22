/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Collector_if.h
 * Author: rafael.luiz.cancian
 *
 * Created on 14 de Agosto de 2018, 14:16
 */

#ifndef COLLECTOR_IF_H
#define COLLECTOR_IF_H

#include <string>
#include <functional>

/*!
 */
typedef std::function<void(double, double) > CollectorAddValueHandler;

template<typename Class>
CollectorAddValueHandler setCollectorAddValueHandler(void (Class::*function)(double, double), Class * object) {
	return std::bind(function, object, std::placeholders::_1, std::placeholders::_2);
}

typedef std::function<void() > CollectorClearHandler;

template<typename Class>
CollectorClearHandler setCollectorClearHandler(void (Class::*function)(), Class * object) {
	return std::bind(function, object);
}

/*!
 * Interface for collecting values of a single stochastic variable.  Values collected can be used as base for statistical analysis. 
 */
class Collector_if {
public:
	virtual void clear() = 0;
	virtual void addValue(double value, double weight=1) = 0;
	virtual double getLastValue() = 0;
	virtual unsigned long numElements() = 0;
public:
	virtual void setAddValueHandler(CollectorAddValueHandler addValueHandler) = 0;
	virtual void setClearHandler(CollectorClearHandler clearHandler) = 0;
};

#endif /* COLLECTOR_IF_H */

