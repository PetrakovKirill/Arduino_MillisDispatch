#ifndef _MILLIS_DISPATCH_H_
#define _MILLIS_DISPATCH_H_

/*
The MIT License (MIT)

Copyright (c) "2019" "Petrakov Kirill"
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>
#include <Arduino.h>

typedef void (*TaskFunction_t)(void);

/**
 * Возможные состояния, возващаемые методами класса millisDispatch_t
 */
typedef enum {
    MILDISP_OK = 0,     /* Операция завершина успешно */
    MILDISP_OVF_TASK,   /* Достигнуто максимальное кол-во зарегистрированных функций */
    MILDISP_BADTASK,    /* Не верная функция */
    MILDISP_DUBLTASK    /* Попытка повторной регистрации */
} mdstatus_t;


template <uint8_t numTask>
class millisDispatch_t {
private:
    struct mdtask_t {
        uint32_t       milState;
        uint32_t       taskPeriod;
        TaskFunction_t TaskFunction;
    } taskList[numTask];

public:
    millisDispatch_t(void);

    /**
     * Добаление новой функции в список диспетчера
     * \param [in] newTask Указатель на пользовательскую функцию 
     *      (указатель на функцию = имя функции) 
     * \param [in] taskPeriod Период ожидания между вызовами функции
     * 
     * \return Статус типа mdstatus_t
     */
    mdstatus_t AddTask(TaskFunction_t newTask, uint32_t taskPeriod);


    /**
     * Выполнение одной итерации диспетчеризации
     */
    void Dispatch(void);


    /**
     * Изменение периода вызова уже зарегистрированной функции
     * \param [in] task Указатель на пользовательскую функцию 
     *      (указатель на функцию = имя функции) 
     * \param [in] newPeriod Новый период ожидания между вызовами функции
     * 
     * \return Статус типа mdstatus_t
     */
    mdstatus_t ChangePeriod(TaskFunction_t task, uint32_t newPeriod);


    /**
     * Удаление функции из списка диспетчеризации
     * \param [in] task Указатель на пользовательскую функцию 
     *      (указатель на функцию = имя функции) 
     * 
     * \return Статус типа mdstatus_t
     */
    mdstatus_t RemoveTask(TaskFunction_t task);
};



/* **************************************************************
 *********************** Реализация Методов ***********************
   ************************************************************** */

template <uint8_t numTask>
millisDispatch_t<numTask>::millisDispatch_t(void) {
    memset((void *)this, 0x00, sizeof(millisDispatch_t));
}



template <uint8_t numTask>
void millisDispatch_t<numTask>::Dispatch(void) {
    uint8_t  i;
    uint32_t m;

    for(i = 0; i < numTask; ++i) {
        m = millis();
        if ( taskList[i].TaskFunction && 
           ( (m - taskList[i].milState) >= taskList[i].taskPeriod) ) {
            taskList[i].milState = m;
            taskList[i].TaskFunction();
        }
    }
}



template <uint8_t numTask>
mdstatus_t millisDispatch_t<numTask>::AddTask(TaskFunction_t newTask, uint32_t taskPeriod) {
    uint8_t i;
    int16_t freeTask = -1;

    if (newTask == NULL) {
        return (MILDISP_BADTASK);
    }

    for (i = 0; i < numTask; i++) {
        if ( (freeTask < 0) && (taskList[i].TaskFunction == NULL) ) {
            freeTask = i;
        } else if (taskList[i].TaskFunction == newTask) {
            return (MILDISP_DUBLTASK);
        }
    }
    
    if (freeTask < 0) {
        return (MILDISP_OVF_TASK);
    }

    /* atomic modification */
    cli();
        taskList[freeTask].TaskFunction = newTask;
        taskList[freeTask].taskPeriod   = taskPeriod;
        taskList[freeTask].milState     = millis();
    sei();
    return (MILDISP_OK);
}



template <uint8_t numTask>
mdstatus_t millisDispatch_t<numTask>::RemoveTask(TaskFunction_t task) {
    uint8_t i;

    for (i = 0; i < numTask; i++) {
        if (taskList[i].TaskFunction == task) {
            break;
        }
    }
    if (i >= numTask) {
        return (MILDISP_BADTASK);
    }

    /* atomic modification */
    cli();
        taskList[i].TaskFunction = NULL;
        taskList[i].taskPeriod  = 0;
        taskList[i].milState    = 0;
    sei();
    return (MILDISP_OK);
}



template <uint8_t numTask>
mdstatus_t millisDispatch_t<numTask>::ChangePeriod(TaskFunction_t task, uint32_t newPeriod) {
    uint8_t i;
    for (i = 0; i < numTask; i++) {
        if (taskList[i].TaskFunction == task) {
            break;
        }
    }
    
    if (i >= numTask) {
        return (MILDISP_BADTASK);
    }

    /* atomic modification */
    cli();
        taskList[i].taskPeriod  = newPeriod;
    sei();
    return (MILDISP_OK);
}

#endif /* _MILLIS_DISPATCH_H_ */