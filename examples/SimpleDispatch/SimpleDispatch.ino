/*
  format UTF-8

  Этот пример демонстрирует простое использование класса millisDispatch_t для
  управления вызовом пользовательских функций через указанный промежуток 
  времени.
*/

#include <millis_dispatch.h>

const char *str = "This is user Task_";

/* 
  Объявление экземпляра класса millisDispatch_t требует параметр шаблона, 
  этот параметр принимает число типа uint8_t, которе задет максимальное
  количество регистрируемых в диспетчере функций. В текущем примере
  этот параметр = 3 
*/
millisDispatch_t <3> simpleDispatch;

/* 
  Следующие функции: Task_1, Task_2, Task_3 - простые задачи, 
  в которых должен исполняться пользовательский код.
  Внимание!!! Все регистрируемые пользовательские функции должны быть
  следующей сигнатуры: void Имя_Функции(void) {} 
*/

void Task_1(void) {
    Serial.print(str);
    Serial.println('1');
}

void Task_2(void) {
    Serial.print(str);
    Serial.println('2');
}

void Task_3(void) {
    static uint32_t counter = 0;
    Serial.print(str);
    Serial.print('3');
    Serial.print(" : counter = ");
    Serial.println(counter++);
}



void setup() {
    Serial.begin(9600);

    /* 
      Метот AddTask регистрирует пользовательскую задачу, 
      которая будет выполнятся с заданным периодом
      Первый аргумент - указатель на функцию (название функции), которую 
        необходимо вызвать (Функция должна иметь сигнатуру описанную выше)
      Второй аргумент - период выполнения регистрируемой функции в ms
    */
    simpleDispatch.AddTask(Task_1, 1200);
    simpleDispatch.AddTask(Task_2, 3333);
    simpleDispatch.AddTask(Task_3, 600 );
}

void loop() {
    /* 
      Выполнение одной итерации диспетчиризации
     */
    simpleDispatch.Dispatch();
}