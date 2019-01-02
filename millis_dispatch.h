#ifndef _MILLIS_DISPATCH_H_
#define _MILLIS_DISPATCH_H_

#include <stdint.h>
#include <Arduino.h>

typedef void (*TaskFuction_t)(void);

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
        uint32_t      milState;
        uint32_t      taskPeriod;
        TaskFuction_t TaskFuction;
    } taskList[numTask];

public:
    millisDispatch_t(void);

    /**
     * Добаление новой функции в список диспетчера
     * \param [in] newTask Указатель на пользовательсую функцию 
     *      (указатель на функцию = имя функции) 
     * \param [in] taskPeriod Период ожидания между вызовами функции
     * 
     * \return Статус типа mdstatus_t
     */
    mdstatus_t AddTask(TaskFuction_t newTask, uint32_t taskPeriod);


    /**
     * Выполнение одной итерации диспетчиризации
     */
    void Dispatch(void);


    /**
     * Изменение периода вызова уже зарегистрированной функции
     * \param [in] task Указатель на пользовательсую функцию 
     *      (указатель на функцию = имя функции) 
     * \param [in] newPeriod Новый период ожидания между вызовами функции
     * 
     * \return Статус типа mdstatus_t
     */
    mdstatus_t ChangePeriod(TaskFuction_t task, uint32_t newPeriod);


    /**
     * Удаление функции из списка диспетчиризации
     * \param [in] task Указатель на пользовательсую функцию 
     *      (указатель на функцию = имя функции) 
     * 
     * \return Статус типа mdstatus_t
     */
    mdstatus_t RemoveTask(TaskFuction_t task);
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
        if ( taskList[i].TaskFuction && 
           ( (m - taskList[i].milState) >= taskList[i].taskPeriod) ) {
            taskList[i].milState = m;
            taskList[i].TaskFuction();
        }
    }
}



template <uint8_t numTask>
mdstatus_t millisDispatch_t<numTask>::AddTask(TaskFuction_t newTask, uint32_t taskPeriod) {
    uint8_t i;
    int16_t freeTask = -1;

    if (newTask == NULL) {
        return (MILDISP_BADTASK);
    }

    for (i = 0; i < numTask; i++) {
        if ( (freeTask < 0) && (taskList[i].TaskFuction == NULL) ) {
            freeTask = i;
        } else if (taskList[i].TaskFuction == newTask) {
            return (MILDISP_DUBLTASK);
        }
    }
    
    if (freeTask < 0) {
        return (MILDISP_OVF_TASK);
    }

    /* atomic modification */
    cli();
        taskList[freeTask].TaskFuction = newTask;
        taskList[freeTask].taskPeriod  = taskPeriod;
        taskList[freeTask].milState    = millis();
    sei();
    return (MILDISP_OK);
}



template <uint8_t numTask>
mdstatus_t millisDispatch_t<numTask>::RemoveTask(TaskFuction_t task) {
    uint8_t i;

    for (i = 0; i < numTask; i++) {
        if (taskList[i].TaskFuction == task) {
            break;
        }
    }
    if (i >= numTask) {
        return (MILDISP_BADTASK);
    }

    /* atomic modification */
    cli();
        taskList[i].TaskFuction = NULL;
        taskList[i].taskPeriod  = 0;
        taskList[i].milState    = 0;
    sei();
    return (MILDISP_OK);
}



template <uint8_t numTask>
mdstatus_t millisDispatch_t<numTask>::ChangePeriod(TaskFuction_t task, uint32_t newPeriod) {
    uint8_t i;
    for (i = 0; i < numTask; i++) {
        if (taskList[i].TaskFuction == task) {
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