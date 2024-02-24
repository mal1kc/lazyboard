#define kbd_tr_tr
#include <DigiKeyboard.h>

#define led_pin 1

// s1 read val:    161
// s2 read val:    336
// s3 read val:    506
// s4 read val:    658
// s5 read val:    838
// s6 read val:    1023

/*
** button layout
** | -  -  | -S3- | | -  -  |
** | -  -  | -  - | | -  -  |
** | -S2-  | -  - | | -S4-  |
** | -  -  | -  - | | -  -  |
** | -S1-  | -  - | | -S5-  |
** | -  -  | -  - | | -  -  |
** | -  -  | -S6- | | -  -  |
**
** Mod 0
**
** S1 prevMod
** S5 nextMod
** S6 ? maybe linux/windows change keys etc.
** ! TODO
** ! IMPORTANT
** S2 prevMedia  ✔
** S3 play/pauseMedia ✔
** S4 nextMedia ✔
**
*/
bool led_toggle = 0;
void LedToggle() {
  led_toggle == 1 ? digitalWrite(led_pin, LOW) : digitalWrite(led_pin, HIGH);
  led_toggle = led_toggle == 1 ? 0 : 1;
}

void setup() {
  // disable input for 2,5 second for no startup errors
  // and sending empty keystroke
  pinMode(led_pin, OUTPUT);
  DigiKeyboard.delay(2500);
  DigiKeyboard.sendKeyStroke(0);
  LedToggle();
}

// void PressInfo(int btn_id) {
//   DigiKeyboard.print("button pressed: ");
//   DigiKeyboard.print(btn_id);
//   DigiKeyboard.sendKeyStroke(KEY_ENTER);
//   LedToggle();
// }

void LedToggleNTimes(int n) {
  uint8_t i = 0;
  do {
    LedToggle();
    delay(50);
    i++;
  } while (i < n);
}

void press1() {
  // Send a key combination of CTRL + SHIFT + C
  DigiKeyboard.sendKeyStroke(KEY_C, MOD_CONTROL_LEFT | MOD_SHIFT_LEFT);
  LedToggleNTimes(3);
}

void press2() {
  DigiKeyboard.sendMediaKeyStroke(KEY_MEDIA_PREV);
  LedToggleNTimes(3);
}

void press3() {
  DigiKeyboard.sendMediaKeyStroke(KEY_MEDIA_PLAYPAUSE);
  LedToggle();
}

void press4() {
  DigiKeyboard.sendMediaKeyStroke(KEY_MEDIA_NEXT);
  LedToggleNTimes(3);
}

void press5() {
  // Send a key combination of CTRL + SHIFT + V
  DigiKeyboard.sendKeyStroke(KEY_V, MOD_CONTROL_LEFT | MOD_SHIFT_LEFT);
  LedToggleNTimes(3);
}

void press6() {
  // Send a key combination of ESC
  // esc hid code is 0x29
  DigiKeyboard.sendKeyStroke(0x29);
  LedToggle();
}

void loop() {

  int data = analogRead(1);
  int d = 0;
  if (data > 100) {
    // if button 6 pressed we do this operations
    if (data > 1022) {
      press6();
    }
    // if button 5 pressed we do this operations
    else if (data > 837) {
      press5();
    }
    // if button 4 pressed we do this operations
    else if (data > 658) {
      press4();
    }
    // if button 3 pressed we do this operations
    else if (data > 504) {
      press3();
    }
    // if button 2 pressed we do this operations
    else if (data > 333) {
      press2();
    }
    // if button 1 pressed we do this operations
    else {
      press1();
    }
  }

  if (led_toggle) {
    delay(500);
    LedToggle();
  }

  // for ignoring analog read unstabilty to waiting very short time
  DigiKeyboard.delay(50);
}
