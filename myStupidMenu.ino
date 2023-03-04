#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C main_lcd(0x38, 16, 2);

TaskHandle_t button_Handler, menu;

#define ENTER_BIT 0x01
#define EXIT_BIT 0x02
#define UP_BIT 0x04
#define DOWN_BIT 0x08

struct menuItem {
  int Name;
  int Next;
  int Previous;
  int Parent;
  int Child;
  uint8_t Select;
  String Text;
};

menuItem Menu1 = {
  /*Name*/ 1,
  /*Next*/ 2,
  /*Previous*/ 0,
  /*Parent*/ 0,
  /*Child*/ 0,
  /*Select*/ 0,
  /*Text*/ "First"
};

menuItem Menu2 = {
  /*Name*/ 2,
  /*Next*/ 0,
  /*Previous*/ 1,
  /*Parent*/ 0,
  /*Child*/ 0,
  /*Select*/ 0,
  /*Text*/ "Second"
};

void setup() {
  Serial.begin(115200);

  xTaskCreatePinnedToCore(button_Handler_code, "Task0", 10000, NULL, 1, &button_Handler, 1);
  vTaskDelay(pdMS_TO_TICKS(100));
  xTaskCreatePinnedToCore(menu_code, "Task1", 10000, NULL, 1, &menu, 1);
  vTaskDelay(pdMS_TO_TICKS(100));
}

void loop() {}

void menu_code(void *pvParameters) {
  main_lcd.init();
  main_lcd.backlight();
  menuItem present_Menu = Menu1;
  BaseType_t xResult;
  uint32_t ulNotifiedValue;
  for (;;) {
    main_lcd.setCursor(1, 0);                   /*Ставим курсор на позицию 1*/
    main_lcd.print(present_Menu.Text);          /* Пишем .Text выбранного меню */

    xResult = xTaskNotifyWait(pdFALSE,          /* Не очищать биты на входе. */
                              ULONG_MAX,        /* Очистка всех бит на выходе. */
                              &ulNotifiedValue, /* Сохраняет значение оповещения. */
                              pdMS_TO_TICKS(500));
    if (xResult == pdPASS) {
      if ((ulNotifiedValue & UP_BIT) != 0) {
        present_Menu = Menu1;
        main_lcd.clear();
      }
      if ((ulNotifiedValue & DOWN_BIT) != 0) {
        present_Menu = Menu2;
        main_lcd.clear();
      }
      if ((ulNotifiedValue & ENTER_BIT) != 0) {
        //
        //main_lcd.clear();
      }
      if ((ulNotifiedValue & EXIT_BIT) != 0) {
        //
        //main_lcd.clear();
      }
    }
    //Serial.println(ulNotifiedValue);
  }
}

void button_Handler_code(void *pvParameters) {  //CORE 1
  pinMode(15, INPUT);                           // Down
  pinMode(4, INPUT);                            // Up
  pinMode(16, INPUT);                           // Parent
  pinMode(17, INPUT);                           // Child
  bool send_once = 0;
  for (;;) {
    if (digitalRead(15) == 1 && send_once == 0) {
      send_once = 1;
      Serial.println("Down");
      xTaskNotify(menu, DOWN_BIT, eSetBits);
    }
    if (digitalRead(4) == 1 && send_once == 0) {
      send_once = 1;
      Serial.println("Up");
      xTaskNotify(menu, UP_BIT, eSetBits);
    }
    if (digitalRead(16) == 1 && send_once == 0) {
      send_once = 1;
      Serial.println("Parent");
    }
    if (digitalRead(17) == 1 && send_once == 0) {
      send_once = 1;
      Serial.println("Child");
    }
    if (digitalRead(15) == 0 && digitalRead(4) == 0 && digitalRead(16) == 0 && digitalRead(17) == 0 && send_once == 1) {
      send_once = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
