/*
  format UTF-8

    Цель данного примера показать работу всех доступных методов 
  класса millisDispatch_t:

    mdstatus_t AddTask(TaskFunction_t newTask, uint32_t taskPeriod);
        * Добаление новой функции в список диспетчера
        * \param [in] newTask Указатель на пользовательскую функцию 
        *      (указатель на функцию = имя функции) 
        * \param [in] taskPeriod Период ожидания между вызовами функции
        * 
        * \return Статус типа mdstatus_t



    void Dispatch(void);
        * Выполнение одной итерации диспетчеризации


    mdstatus_t ChangePeriod(TaskFunction_t task, uint32_t newPeriod);
        * Изменение периода вызова уже зарегистрированной функции
        * \param [in] task Указатель на пользовательскую функцию 
        *      (указатель на функцию = имя функции) 
        * \param [in] newPeriod Новый период ожидания между вызовами функции
        * 
        * \return Статус типа mdstatus_t


    mdstatus_t RemoveTask(TaskFunction_t task);
        * Удаление функции из списка диспетчиризации
        * \param [in] task Указатель на пользовательскую функцию 
        *      (указатель на функцию = имя функции) 
        * 
        * \return Статус типа mdstatus_t

        Список констант типа mdstatus_t, которые возвращают методы класса 
    millisDispatch_t (AddTask, ChangePeriod и RemoveTask):
        MILDISP_OK          Операция завершина успешно
        MILDISP_OVF_TASK    Достигнуто максимальное кол-во зарегистрированных функций
        MILDISP_BADTASK     Не верная функция
        MILDISP_DUBLTASK    Попытка повторной регистрации
*/

#include <millis_dispatch.h>

/* 
    При объявлении экземпляра класса необходимо указать 
  параметр шаблона типа uint8_t, который задает максимально 
  возможное кличество регистрируемых в диспетчере функций.
  В данном примере этот параметр = 2, т.е. в диспетчере можно 
  зарегистрировать не более 2-х функций. 
*/
millisDispatch_t <2> simpleDispatch;


static uint32_t task1Period = 500;


void Task_1(void);
void Task_2(void);
void Task_3(void);


/**
 *  Данная функция выполняет смену второй вызываемой функции:
 * каждую 3-ю итерацию выполняет переключение между функциями 
 * Task_2 и Task_3
 */
void Task_1(void) {
    static uint8_t counter = 0;
    static bool    flag = true;

    /* Печать собственного имени функции в консоль */
    Serial.print(__FUNCTION__);
    Serial.print(" : counter = ");
    Serial.println(counter++);

    if (counter > 3) {
        if (flag) {
            /* Переключение на функцию Task_3 */
            simpleDispatch.RemoveTask(Task_2);
            simpleDispatch.AddTask(Task_3, 900);
        } else {
            /* Переключение на функцию Task_2 */
            simpleDispatch.RemoveTask(Task_3);
            simpleDispatch.AddTask(Task_2, 900);
        }
        flag = !flag;
        counter = 0;
    }
}


/**
 * Данная функция каждый вызов меняет период вызова Task_1
 * на +100ms
 */
void Task_2(void) {
    Serial.print(__FUNCTION__);

    task1Period += 100;
    Serial.print(" : Task_1 period = ");
    Serial.println(task1Period);
    simpleDispatch.ChangePeriod(Task_1, task1Period);
}


/**
 * Данная функция каждый вызов меняет период вызова Task_1
 * на -100ms
 */
void Task_3(void) {
    Serial.print(__FUNCTION__);

    task1Period -= 100;
    Serial.print(" : Task_1 period = ");
    Serial.println(task1Period);
    simpleDispatch.ChangePeriod(Task_1, task1Period);
}



void setup() {
    Serial.begin(9600);
    /* 
      Начальная регистрация 2-х функций.
    */
    simpleDispatch.AddTask(Task_1, task1Period);
    simpleDispatch.AddTask(Task_2, 900);
}

void loop() {
    /* 
        Выполнение одной итерации диспетчеризации.
      Данный метод проходит по списку зарегистрированных функций,
      если время периода истекло диспетчер вызовет эту функцию.
     */
    simpleDispatch.Dispatch();
}