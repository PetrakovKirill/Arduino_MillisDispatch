/*
  format UTF-8

  Этот пример демонстрирует использование всех доступных методов класса 
  millisDispatch_t для управления вызовом пользовательских функций через 
  указанный промежуток времени.
*/

#include <millis_dispatch.h>

/* 
  Объявление экземпляра класса millisDispatch_t 
*/
millisDispatch_t <2> simpleDispatch;


uint32_t task1Period = 500;

/*
  Пользовательские функции
*/
void Task_1(void);
void Task_2(void);
void Task_3(void);


/**
 * Данная функция выполняет смену второй вызываемой функции:
 * выполняет переключение между функциями Task_2 и Task_3
 */
void Task_1(void) {
    static uint8_t counter = 0;
    static bool    flag = true;

    /* Печать собственного имени функции в консоль */
    Serial.print(__FUNCTION__);
    Serial.print(" : counter = ");
    Serial.println(counter++);

    /* Ожидаем пока счетчик досчитает до 3 */
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
    /* Печать собственного имени функции в консоль */
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
    /* Печать собственного имени функции в консоль */
    Serial.print(__FUNCTION__);

    task1Period -= 100;
    Serial.print(" : Task_1 period = ");
    Serial.println(task1Period);
    simpleDispatch.ChangePeriod(Task_1, task1Period);
}



void setup() {
    Serial.begin(9600);
    /* 
      Начальная регистрация 2 функций
    */
    simpleDispatch.AddTask(Task_1, task1Period);
    simpleDispatch.AddTask(Task_2, 900);
}

void loop() {
    /* 
      Выполнение одной итерации диспетчиризации
     */
    simpleDispatch.Dispatch();
}