#define kbd_tr_tr
#include <DigiKeyboard.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

// keys added to the library in "added navigation/editing key defines";
// fallbacks so this still builds against an older library checkout
#ifndef KEY_ESC
#define KEY_ESC 0x29
#endif
#ifndef KEY_TAB
#define KEY_TAB 0x2B
#endif
#ifndef KEY_PRINTSCREEN
#define KEY_PRINTSCREEN 0x46
#endif
#ifndef KEY_ARROW_RIGHT
#define KEY_ARROW_RIGHT 0x4F
#endif
#ifndef KEY_ARROW_LEFT
#define KEY_ARROW_LEFT 0x50
#endif

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
** short press / hold (>= LONG_MS). hold S6 = switch layer (LED blinks
** layer+1 times, layer is remembered after replug)
**
**        layer 0: media             layer 1: desktop (windows)
** S1     copy    / vol- (repeat)    prev desktop (win+ctrl+left) / -
** S2     prev    / -                snip (win+shift+s) / screenshot (win+prtsc)
** S3     play    / mute             mic mute (win+alt+k) / lock (win+l)
** S4     next    / -                next desktop (win+ctrl+right) / task view
** S5     paste   / vol+ (repeat)    show desktop (win+d) / clipboard (win+v)
** S6     esc     / switch layer     esc / switch layer
*/

#define LED_PIN 1
#define BTN_PIN 1 // analog channel 1 (PB2)

// copy/paste modifiers: plain ctrl for windows apps.
// use (MOD_CONTROL_LEFT | MOD_SHIFT_LEFT) for linux terminals
#define COPY_MODS MOD_CONTROL_LEFT

// timings (ms)
#define DEBOUNCE_MS 15
#define LONG_MS 400
#define REPEAT_MS 120
#define LED_MS 80

// calibration (analogRead per button, see examples/template_read_analog.ino):
// S1 161, S2 336, S3 506, S4 658, S5 838, S6 1023
// thresholds are the midpoints between neighbours; th[0] = idle limit
const uint16_t th[] = {100, 248, 421, 582, 748, 930};

// ---- keymap ----
enum : uint8_t { A_NONE, A_KEY, A_MEDIA, A_LAYER };
#define F_REPEAT 0x80 // in Action.type: repeat while held

struct Action {
  uint8_t type, code, mods;
};

#define NO_ACT {A_NONE, 0, 0}
#define KEY_ACT(code, mods) {A_KEY, code, mods}
#define MEDIA_ACT(code) {A_MEDIA, code, 0}
#define MEDIA_REP(code) {A_MEDIA | F_REPEAT, code, 0}
#define LAYER_SWITCH {A_LAYER, 0, 0}

#define M_WIN MOD_GUI_LEFT
#define M_CTRL MOD_CONTROL_LEFT
#define M_SHIFT MOD_SHIFT_LEFT
#define M_ALT MOD_ALT_LEFT

#define LAYERS 2
#define BUTTONS 6

// [layer][button S1..S6][0 = short, 1 = hold]
const Action keymap[LAYERS][BUTTONS][2] PROGMEM = {
    // layer 0: media
    {
        {KEY_ACT(KEY_C, COPY_MODS), MEDIA_REP(KEY_MEDIA_VOLUME_DOWN)}, // S1
        {MEDIA_ACT(KEY_MEDIA_PREV), NO_ACT},                        // S2
        {MEDIA_ACT(KEY_MEDIA_PLAYPAUSE), MEDIA_ACT(KEY_MEDIA_MUTE)},      // S3
        {MEDIA_ACT(KEY_MEDIA_NEXT), NO_ACT},                        // S4
        {KEY_ACT(KEY_V, COPY_MODS), MEDIA_REP(KEY_MEDIA_VOLUME_UP)},   // S5
        {KEY_ACT(KEY_ESC, 0), LAYER_SWITCH},                    // S6
    },
    // layer 1: windows desktop
    {
        {KEY_ACT(KEY_ARROW_LEFT, M_WIN | M_CTRL), NO_ACT},             // S1
        {KEY_ACT(KEY_S, M_WIN | M_SHIFT), KEY_ACT(KEY_PRINTSCREEN, M_WIN)},  // S2
        {KEY_ACT(KEY_K, M_WIN | M_ALT), KEY_ACT(KEY_L, M_WIN)},              // S3
        {KEY_ACT(KEY_ARROW_RIGHT, M_WIN | M_CTRL), KEY_ACT(KEY_TAB, M_WIN)}, // S4
        {KEY_ACT(KEY_D, M_WIN), KEY_ACT(KEY_V, M_WIN)},                    // S5
        {KEY_ACT(KEY_ESC, 0), LAYER_SWITCH},                     // S6
    },
};

// ---- state ----
uint8_t layer = 0;
#define EEPROM_LAYER_ADDR ((uint8_t *)0)

uint8_t rawBtn = 0, stableBtn = 0;
unsigned long rawSince = 0;

uint8_t curBtn = 0; // pressed button 1..6, 0 = none
unsigned long pressStart = 0, lastRepeat = 0;
bool shortDone = false, longFired = false;

uint8_t ledToggles = 0;
bool ledOn = false;
unsigned long ledNext = 0;

// ---- led (non blocking) ----
void ledBlink(uint8_t n) {
  ledOn = false;
  digitalWrite(LED_PIN, LOW);
  ledToggles = n * 2;
  ledNext = millis();
}

void ledUpdate(unsigned long now) {
  if (ledToggles && (long)(now - ledNext) >= 0) {
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    ledToggles--;
    ledNext = now + LED_MS;
  }
}

// ---- actions ----
Action getAction(uint8_t btn, uint8_t idx) {
  Action a;
  memcpy_P(&a, &keymap[layer][btn - 1][idx], sizeof(a));
  return a;
}

void runAction(const Action &a) {
  switch (a.type & ~F_REPEAT) {
  case A_KEY:
    DigiKeyboard.sendKeyStroke(a.code, a.mods);
    ledBlink(1);
    break;
  case A_MEDIA:
    DigiKeyboard.sendMediaKeyStroke(a.code);
    ledBlink(1);
    break;
  case A_LAYER:
    layer = (layer + 1) % LAYERS;
    eeprom_update_byte(EEPROM_LAYER_ADDR, layer);
    ledBlink(layer + 1);
    break;
  }
}

// ---- buttons ----
uint8_t readButton() {
  uint16_t v = analogRead(BTN_PIN);
  if (v < th[0])
    return 0;
  for (uint8_t i = 1; i < sizeof(th) / sizeof(th[0]); i++)
    if (v < th[i])
      return i;
  return BUTTONS;
}

void onPress(uint8_t btn, unsigned long now) {
  curBtn = btn;
  pressStart = lastRepeat = now;
  longFired = false;
  // no hold action -> fire right away, no need to wait for release
  shortDone = getAction(btn, 1).type == A_NONE;
  if (shortDone)
    runAction(getAction(btn, 0));
}

void onHeld(unsigned long now) {
  if (shortDone)
    return;
  Action hold = getAction(curBtn, 1);
  if (!longFired) {
    if (now - pressStart >= LONG_MS) {
      runAction(hold);
      longFired = true;
      lastRepeat = now;
    }
  } else if ((hold.type & F_REPEAT) && now - lastRepeat >= REPEAT_MS) {
    runAction(hold);
    lastRepeat = now;
  }
}

void onRelease() {
  if (!shortDone && !longFired)
    runAction(getAction(curBtn, 0));
  curBtn = 0;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  // wait 2.5s (keeps usb polled) so the host finishes enumeration,
  // then send an empty keystroke
  DigiKeyboard.delay(2500);
  DigiKeyboard.sendKeyStroke(0);

  layer = eeprom_read_byte(EEPROM_LAYER_ADDR);
  if (layer >= LAYERS) // erased eeprom reads 0xFF
    layer = 0;
  ledBlink(layer + 1);
}

void loop() {
  DigiKeyboard.update();
  unsigned long now = millis();

  // debounce: a reading counts once it stayed the same for DEBOUNCE_MS.
  // also skips the voltage passing through lower buttons on press/release
  uint8_t b = readButton();
  if (b != rawBtn) {
    rawBtn = b;
    rawSince = now;
  } else if (b != stableBtn && now - rawSince >= DEBOUNCE_MS) {
    stableBtn = b;
    if (curBtn)
      onRelease();
    if (b)
      onPress(b, now);
  }

  if (curBtn)
    onHeld(now);

  ledUpdate(now);
  DigiKeyboard.delay(5); // sample spacing, keeps usb polled
}
