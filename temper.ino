/*
 * Скетч написан Arduinology
 * 
 * https://github.com/arduinologyz
 * itkudrin@gmail.com
 * 
 * Применяйте и модифицируйте свободно при условии упоминания автора и размещения ссылки https://github.com/arduinologyz.
 */
 
#include <WS2812FX.h>


//Количество светодиодов в полоске
#define LED_COUNT 8
//Куда подключается вход светодиодной полоски
#define LED_PIN 13

//Массив для хранения сменяющихся цветов
int32_t leds[LED_COUNT];

//Переменные цветов
int32_t color_cold;
int32_t color_normal;
int32_t color_hot;

//Переменные граничных значений температур
int hot_value = 0;
int room_value = 500;
int cold_value = 700;

//Инициализируем библиотеку со светодиодами
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Пин, куда подключается вывод датчика температуры
const int temperPin = 14;

//Стартовая настройка системы
void setup() {

  //Зададим граничные значения температуры. Чем выше температура, тем ниже считываемые с датчика показания - обратная зависимость.

  //Горячо
 hot_value = 350;

  //Средне
 room_value = 450;

 //Холодно
 cold_value = 550;

  //Обнулим значения уветов полоски
  for (int i=0;i<LED_COUNT;i++) leds[i] = BLACK;

  //Цвет холода
  color_cold = ws2812fx.Color(0,0,255);
  
  //Цвет комнатной температуры (средний)
  color_normal = ws2812fx.Color(0,255,0);

  //Цвет, когда жарко
  color_hot = ws2812fx.Color(255,0,0);

  //Запускаем библиотеку светодиодов
  ws2812fx.init();
  ws2812fx.setBrightness(70);
  //ws2812fx.setSpeed(30000);
  //ws2812fx.setColor(GREEN);
  //ws2812fx.setMode(FX_MODE_STATIC);
  //ws2812fx.start();

  //Стартуем возможность выводить информацию с системы на UART (и далее через последовательный порт в подключенный компьютер)
  Serial.begin(115200);

  //Установим пин датчика температуры на вход
  pinMode(temperPin,INPUT);
}

void loop() {


  //Считаем значение температуры
  int temp = analogRead(temperPin);

  //Проверим границы, если выходит за них - установим граничные значения показаний
  if (temp>cold_value) temp = cold_value;
  if (temp<hot_value) temp = hot_value;


  //Переменная процента смешения цветов от холода к среднему, от среднего к жаркому
  int percent;
  
  //Компоненты цветов для смешения в процентном сотношении
  uint8_t r1,r2,r3,g1,g2,g3,b1,b2,b3;

  //Посчитаем процент для смешения цветов
  if (temp<room_value){
     
     r1 = 255; g1 = 0; b1 = 0;
     r2 = 0; g2 = 255; b2 = 0;
     percent = (double)(temp-hot_value)*100/(room_value-hot_value);
  } else {
     r1 = 0; g1 = 255; b1 = 0;
     r2 = 0; g2 = 0; b2 = 255;
     percent = (double)(temp-room_value)*100/(cold_value-room_value);
  }

  //Рассчитаем компоненты итогового цвета
  r3 = (uint32_t)(100-percent)*r1/100 + (uint32_t)percent*r2/100;
  g3 = (uint32_t)(100-percent)*g1/100 + (uint32_t)percent*g2/100;
  b3 = (uint32_t)(100-percent)*b1/100 + (uint32_t)percent*b2/100;

  //Запишем последнее измерение температуры в массив, сместив старые значения на единицу
  for (int i=0;i<(LED_COUNT-1);i++) leds[i] = leds[i+1];
  leds[LED_COUNT-1] = ws2812fx.Color(r3,g3,b3);

  //Запишем значения в светодиодную ленту
  for (int i=0;i<LED_COUNT;i++) ws2812fx.setPixelColor(i,leds[i]);

  //Покажем цвета
  ws2812fx.show();
  ws2812fx.service();
  
  //Выведем информацию о значении температуры на подключенный компьютер
  Serial.println(temp);

  //Обеспечим задержку в 0.3 секунды для наглядности заполнения
  delay(300);
}
