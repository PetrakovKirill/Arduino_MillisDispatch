/*
  format UTF-8

  Этот пример демонстрирует использование класса millisDispatch_t для
  управления вызовом пользовательских функций через указанный промежуток 
  времени, а также для организации отложенного вызова функции
*/

#include <millis_dispatch.h>

/* 
  Объявление экземпляра класса millisDispatch_t
*/
millisDispatch_t <3> simpleDispatch;


/**
 * Основной цикл выполняющийся с периодом 500 ms
 * просто печатае свое имя и инкрементирует счетчик
*/
void MainTask(void) {
    static uint32_t counter = 0;
    Serial.print(__FUNCTION__);
    Serial.print(" : counter = ");
    Serial.println(counter++);
}


/**
 * Процедура отложенного вызова, которая уведомляет о 
 * срабатывании прерывания
*/
void DefferedInterruptHandler(void) {
    static uint32_t counter = 0;

    /* Проверка изменения состояния пина */
    if (digitalRead(2) != LOW) {
        /* Если состояние изменилось, необходимо 
           выйти из функции, но перед этим необходимо
           удалить отложенный обработчик из диспетчера */
        goto just_exit;
    }

    Serial.print("Ext Interrupt Counter = ");
    Serial.println(counter++);

just_exit:
    /* Так как необходимо выполнить процедуру 1 раз
       удаляем процедуру из списка диспетчиризации */
    simpleDispatch.RemoveTask(DefferedInterruptHandler);
}


/**
 * Основной обработчик внешего прерывания
 */
void InterruptHandler(void) {
    /* Регистрируем отложенный обработчик, для подавления
       дребезга заложим задержку в 50 ms */
    simpleDispatch.AddTask(DefferedInterruptHandler, 50);
}


void setup() {
    Serial.begin(9600);

    /* 
      Подтягиваем пин 2 к питанию (вход INT0)
     */
    pinMode(2, INPUT_PULLUP);
    /* 
      Регистрируем процедуру основного цикла
    */
    simpleDispatch.AddTask(MainTask, 500);

    /* 
      Регистрируем обработчик прерывания Arduino
      для INT0, по спаду
     */
    attachInterrupt(0, InterruptHandler, FALLING);
}

void loop() {
    /* 
      Выполнение одной итерации диспетчиризации
     */
    simpleDispatch.Dispatch();
}