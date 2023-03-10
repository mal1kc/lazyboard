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

void PressInfo(int btn_id) {
  DigiKeyboard.print("button pressed: ");
  DigiKeyboard.print(btn_id);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  LedToggle();
}

void LedToggleNTimes(int n) {
  uint8_t i = 0;
  do {
    LedToggle();
    delay(50);
    i++;
  } while (i < n);
}

void loop() {

  int data = analogRead(1);
  int d = 0;
  if (data > 100) {
    // if button 6 pressed we do this operations
    if (data > 1022) {
      PressInfo(6);
    }
    // if button 5 pressed we do this operations
    else if (data > 837) {
      PressInfo(5);
    }
    // if button 4 pressed we do this operations
    else if (data > 658) {

      DigiKeyboard.sendMediaKeyStroke(KEY_MEDIA_NEXT);
      // TODO unkown reason causes to pause media after pressing prev-next
      DigiKeyboard.sendMediaKeyStroke(
          KEY_MEDIA_PLAYPAUSE); // temporary solution
      LedToggleNTimes(3);
      // PressInfo(4);

    }
    // if button 3 pressed we do this operations
    else if (data > 504) {
      DigiKeyboard.sendMediaKeyStroke(KEY_MEDIA_PLAYPAUSE);
      LedToggleNTimes(2);
      // PressInfo(3);
    }
    // if button 2 pressed we do this operations
    else if (data > 333) {
      DigiKeyboard.sendMediaKeyStroke(KEY_MEDIA_PREV);
      // TODO unkown reason causes to pause media after pressing prev-next
      DigiKeyboard.sendMediaKeyStroke(
          KEY_MEDIA_PLAYPAUSE); // temporary solution
      LedToggleNTimes(3);
      // PressInfo(2);
    }
    // if button 1 pressed we do this operations
    else {
      PressInfo(1);
    }
  }

  if (led_toggle) {
    delay(500);
    LedToggle();
  }

  // for ignoring analog read unstabilty to waiting very short time
  DigiKeyboard.delay(50);
}
