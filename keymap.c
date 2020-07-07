/* Copyright 2015-2017 Jannik Wibker
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#ifdef CONSOLE_ENABLE
#include <print.h>
#endif

enum leds {
	LED00, LED01, LED02,
	LED03, LED04, LED05,
	LED06, LED07, LED08,
	LED09, LED10, LED11,
	LED12, LED13
};

enum layers {
	/* Linux layers */
	_L_BASE, _L_LOWER, _L_RAISE, _L_ADJUST,

	/* MacOS layers */
	_M_BASE, _M_LOWER, _M_RAISE, _M_ADJUST
};

enum keycodes {
  /* toggle additional information via rgb leds ON/OFF */
	INF_TGL = SAFE_RANGE,

  /* switch from Linux layout to MacOS layout */
	L_TO_M,
	/* switch from MacOs layout to Linux layout */
	M_TO_L,

	/* Linux layer switches */
	L_BASE, L_LOWER, L_RAISE, L_ADJUST,

	L_D_BASE, L_D_LOWER, L_D_RAISE, L_D_ADJUST,

	/* MacOS layer switches */
	M_BASE, M_LOWER, M_RAISE, M_ADJUST,

	M_D_BASE, M_D_LOWER, M_D_RAISE, M_D_ADJUST,
};

typedef union {
  uint32_t raw;
  struct {
    bool info_mode :1;
    bool linux_mode :1;
  };
} user_config_t;

user_config_t user_config;

static uint16_t idle_timer = 0;
static uint8_t halfmin_counter = 0;
static bool led_on = true;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	/* Linux - Base
	 * ,-----------------------------------------------------------------------------------.
	 * | Esc  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   [  |  ]   |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |Adjust| Ctrl | Super|  Alt | Raise|    Space    | Lower|AltGr | Caps |   -  |   =  |
	 * `-----------------------------------------------------------------------------------'
   */
	[_L_BASE] = LAYOUT_planck_mit(
		_CTRL_ESC,    KC_Q,    KC_W,     KC_E,    KC_R,    KC_T, KC_Y, KC_U,    KC_I,    KC_O,        KC_P,      KC_BSPC,
		_L_TAB_LOWER, KC_A,    KC_S,     KC_D,    KC_F,    KC_G, KC_H, KC_J,    KC_K,    KC_L,        KC_LBRC,   KC_RBRC,
		KC_LSFT,      KC_Z,    KC_X,     KC_C,    KC_V,    KC_B, KC_N, KC_M,    KC_COMM, KC_DOT,      KC_SLSH,   KC_ENT,
		KC_LCTL,      KC_LGUI, L_ADJUST, KC_LALT, L_RAISE, KC_SPC,     L_LOWER, KC_RALT, _L_CAPSLOCK, _GUI_MINS, _CTL_EQL),

	/* Linux Lower
	 * ,-----------------------------------------------------------------------------------.
	 * | Ctrl |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Tab  |  \*  |   ^  |   -  |   =  |   \  | Left | Down |  Up  |Right |   ;  |  '   |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift| Val- | Val+ |  Ins | Home | P Up |P Down| End  |   ,  |   .  |   /  |Delete|
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |Adjust| Ctrl | Super|  Alt | Raise|    Enter    |Lower |AltGr | Caps | Left | Right|
	 * `-----------------------------------------------------------------------------------'
   */
	[_L_LOWER] = LAYOUT_planck_mit(
		KC_RCTL, KC_1,         KC_2,    KC_3,         KC_4,     KC_5,    KC_6,    KC_7,    KC_8,    KC_9,     KC_0,    KC_BSPC,
		KC_TRNS, _L_BACKSLASH, KC_GRV,  _L_ASTERISKS, _L_SLASH, KC_BSLS, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,  KC_SCLN, KC_QUOT,
		KC_TRNS, RGB_VAD,      RGB_VAI, KC_INS,       KC_HOME,  KC_PGDN, KC_PGUP, KC_END,  KC_COMM, KC_DOT,   KC_SLSH, KC_DEL,
		KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS,      KC_TRNS,  KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,  KC_LEFT, KC_RIGHT),

	/* Linux Raise
	 * ,-----------------------------------------------------------------------------------.
	 * | Ctrl |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  | F10  | F11  |
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Super|Vol Up|  Up  |VolDwn| Play |      |      |      |      |      |   [  |  ]   |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift| Left | Down | Right| Mute |  Esc |Enter | End  |   ,  |   .  |   /  | F12  |
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |Adjust| Ctrl | Super|  Alt | Raise|  Backspace  |Lower |AltGr | Caps | Hue+ | Sat+ |
	 * `-----------------------------------------------------------------------------------'
   */
	[_L_RAISE] = LAYOUT_planck_mit(
		KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,    KC_F10,  KC_F11,
		KC_LGUI, KC_VOLD, KC_UP,   KC_VOLU, KC_MPLY, KC_GRV,  KC_MINS, KC_EQL,  KC_SCLN, KC_QUOT,  KC_LBRC, KC_RBRC,
		KC_TRNS, KC_LEFT, KC_DOWN, KC_RGHT, KC_MUTE, KC_ESC,  KC_ENT,  KC_BSLS, KC_COMM, KC_DOT,   KC_SLSH, KC_F12,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BSPC,          KC_TRNS, KC_TRNS, KC_TRNS,  RGB_HUI, RGB_SAI),

	/* Linux Adjust
	 * ,-----------------------------------------------------------------------------------.
	 * | Debug| Btn1 | M Up | Btn2 |Scrll+|Delete|   /  |   1  |   2  |   3  |   -  | Reset|
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Btn3 |M Left|M Down|MRight|Scrll-|  Tab |   *  |   4  |   5  |   6  |   +  | Forw |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift| RGB T| Cycle| InfT |  Mac |  Esc | Bksp |   7  |   8  |   9  |Enter | Bckw |
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |Adjust| Ctrl | Super|  Alt | Raise|    Space    |Lower |   0  |   .  |AltGr | Adjst|
	 * `-----------------------------------------------------------------------------------'
   */
	[_L_ADJUST] = LAYOUT_planck_mit(
		DEBUG,  KC_BTN1, KC_MS_U,					  KC_BTN2, KC_WH_U, RESET,  KC_PSLS,  KC_P1,   KC_P2,   KC_P3,    KC_PMNS, RESET,
		KC_BTN3,  KC_MS_L, KC_MS_D,					KC_MS_R, KC_WH_D, KC_TAB, KC_PAST,  KC_P4,   KC_P5,   KC_P6,    KC_PPLS, _FORWARDS,
		KC_TRNS, RGB_TOG, RGB_MODE_FORWARD, INF_TGL, L_TO_M,  KC_ESC, KC_BSPC,  KC_P7,   KC_P8,   KC_P9,    KC_PENT, _BACKWARDS,
		KC_TRNS, KC_TRNS, KC_TRNS,					KC_TRNS, KC_TRNS, KC_SPC,           KC_TRNS, KC_P0,   KC_PDOT,  KC_RALT, L_ADJUST),

	/* MacOS - Base
	 * ,-----------------------------------------------------------------------------------.
	 * | Esc  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Tab  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   [  |  ]   |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |  Fn  | Ctrl |  Alt | Super| Raise|    Space    | Lower|AltGr | Caps |   -  |   =  |
	 * `-----------------------------------------------------------------------------------'
   */
	[_M_BASE] = LAYOUT_planck_mit(
		_CTRL_ESC,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T, KC_Y, KC_U,    KC_I,    KC_O,        KC_P,      KC_BSPC,
		_M_TAB_LOWER, KC_A,    KC_S,    KC_D,    KC_F,    KC_G, KC_H, KC_J,    KC_K,    KC_L,        KC_LBRC,   KC_RBRC,
		KC_LSFT,      KC_Z,    KC_X,    KC_C,    KC_V,    KC_B, KC_N, KC_M,    KC_COMM, KC_DOT,      KC_SLSH,   KC_ENT,
		_M_FN,        KC_LCTL, KC_LALT, KC_LGUI, M_RAISE, KC_SPC,     M_LOWER, KC_RALT, _M_CAPSLOCK, _GUI_MINS, _CTL_EQL),

	/* MacOS Lower
	 * ,-----------------------------------------------------------------------------------.
	 * | Ctrl |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Tab  |  \*  |   ^  |   -  |   =  |   \  | Left | Down |  Up  |Right |   ;  |  '   |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift| Val- | Val+ |  Ins | Home | P Up |P Down| End  |   ,  |   .  |   /  |Delete|
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |  Fn  | Ctrl |  Alt | Super| Raise|    Enter    |Lower |AltGr | Caps | Left | Right|
	 * `-----------------------------------------------------------------------------------'
   */
	[_M_LOWER] = LAYOUT_planck_mit(
		KC_RCTL, 			KC_1,         KC_2,    KC_3,         KC_4,     KC_5,    KC_6,    KC_7,    KC_8,    KC_9,     KC_0,    KC_BSPC,
		_M_TAB_LOWER, _M_BACKSLASH, KC_GRV,  _M_ASTERISKS, _M_SLASH, KC_BSLS, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,  KC_SCLN, KC_QUOT,
		KC_TRNS, 			RGB_VAD,      RGB_VAI, KC_INS,       KC_HOME,  KC_PGDN, KC_PGUP, KC_END,  KC_COMM, KC_DOT,   KC_SLSH, KC_DEL,
		KC_TRNS, 			KC_TRNS,      KC_TRNS, KC_TRNS,      KC_TRNS,  KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,  KC_LEFT, KC_RIGHT),

	/* MacOS Raise
	 * ,-----------------------------------------------------------------------------------.
	 * | Ctrl |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |  F7  |  F8  |  F9  | F10  | F11  |
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Super|Vol Up|  Up  |VolDwn| Play |      |      |      |      |      |   [  |  ]   |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift| Left | Down | Right| Mute |  Esc |Enter | End  |   ,  |   .  |   /  | F12  |
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |  Fn  | Ctrl |  Alt | Super| Raise|  Backspace  |Lower |AltGr | Caps | Hue+ | Sat+ |
	 * `-----------------------------------------------------------------------------------'
   */
	[_M_RAISE] = LAYOUT_planck_mit(
		KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,    KC_F10,     KC_F11,
		KC_LGUI, KC_VOLD, KC_UP,   KC_VOLU, KC_MPLY, KC_GRV,  KC_MINS, KC_EQL,  KC_SCLN, KC_QUOT,  KC_LBRC,    KC_RBRC,
		KC_TRNS, KC_LEFT, KC_DOWN, KC_RGHT, KC_MUTE, KC_ESC,  KC_ENT,  KC_BSLS, KC_COMM, KC_DOT,   KC_SLSH,    KC_F12,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_BSPC,          KC_TRNS, KC_TRNS, KC_TRNS,  RGB_HUI,    RGB_SAI),

  /* MacOS Adjust
	 * ,-----------------------------------------------------------------------------------.
	 * | Debug| Btn1 | M Up | Btn2 |Scrll+|Delete|   /  |   1  |   2  |   3  |   -  | Reset|
	 * |------+------+------+------+------+-------------+------+------+------+------+------|
	 * | Btn3 |M Left|M Down|MRight|Scrll-|  Tab |   *  |   4  |   5  |   6  |   +  | Forw |
	 * |------+------+------+------+------+------|------+------+------+------+------+------|
	 * | Shift| RGB T| Cycle| InfT | Linux|  Esc | Bksp |   7  |   8  |   9  |Enter | Bckw |
	 * |------+------+------+------+------+------+------+------+------+------+------+------|
	 * |  Fn  | Ctrl |  Alt | Super| Raise|    Space    |Lower |   0  |   .  |AltGr | Adjst|
	 * `-----------------------------------------------------------------------------------'
   */
	[_M_ADJUST] = LAYOUT_planck_mit(
		DEBUG,   KC_BTN1, KC_MS_U,          KC_BTN2, KC_WH_U, KC_DEL, KC_PSLS,  KC_P1,   KC_P2,   KC_P3,    KC_PMNS, RESET,
		KC_BTN3, KC_MS_L, KC_MS_D,          KC_MS_R, KC_WH_D, KC_TAB, KC_PAST,  KC_P4,   KC_P5,   KC_P6,    KC_PPLS, _FORWARDS,
		KC_TRNS, RGB_TOG, RGB_MODE_FORWARD, INF_TGL, M_TO_L,  KC_ESC, KC_BSPC,  KC_P7,   KC_P8,   KC_P9,    KC_PENT, _BACKWARDS,
		KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_SPC,           KC_TRNS, KC_P0,   KC_PDOT,  KC_RALT, M_ADJUST)

};

void set_led(uint8_t r, uint8_t g, uint8_t b, uint8_t i) {
  if (i < RGBLED_NUM) {
		rgblight_setrgb_at(r, g, b, i);
		// #ifdef CONSOLE_ENABLE
		// 	printf("set_led:%d,%d,%d,%d", r, g, b, i);
		// #endif
    // led[i].r = r;
    // led[i].g = g;
    // led[i].b = b;
  }
}

void set_rgblight_by_layer(layer_state_t state) {
	switch(biton32(state)) {
		case _L_BASE:
		case _M_BASE: {
			#ifdef CONSOLE_ENABLE
				print("layer:base\n");
			#endif
			set_led(LAYER_BASE_COLORS, LED13);
			rgblight_set_effect_range(LED00, 14);
		} break;
		case _L_LOWER:
		case _M_LOWER: {
			#ifdef CONSOLE_ENABLE
				print("layer:lower\n");
			#endif
			rgblight_set_effect_range(LED00, 8);
			set_led(LAYER_LOWER_COLORS, LED08);
			set_led(LAYER_LOWER_COLORS, LED09);
			set_led(LAYER_LOWER_COLORS, LED10);
			set_led(LAYER_LOWER_COLORS, LED11);
			set_led(LAYER_LOWER_COLORS, LED12);
			set_led(LAYER_LOWER_COLORS, LED13);
		} break;
		case _L_RAISE:
		case _M_RAISE: {
			#ifdef CONSOLE_ENABLE
				print("layer:raise\n");
			#endif
			rgblight_set_effect_range(LED00, 8);
			set_led(LAYER_RAISE_COLORS, LED08);
			set_led(LAYER_RAISE_COLORS, LED09);
			set_led(LAYER_RAISE_COLORS, LED10);
			set_led(LAYER_RAISE_COLORS, LED11);
			set_led(LAYER_RAISE_COLORS, LED12);
			set_led(LAYER_RAISE_COLORS, LED13);
		} break;
		case _L_ADJUST:
		case _M_ADJUST: {
			#ifdef CONSOLE_ENABLE
				print("layer:adjst\n");
			#endif
			rgblight_set_effect_range(LED00, 8);
			set_led(LAYER_ADJUST_COLORS, LED08);
			set_led(LAYER_ADJUST_COLORS, LED09);
			set_led(LAYER_ADJUST_COLORS, LED10);
			set_led(LAYER_ADJUST_COLORS, LED11);
			set_led(LAYER_ADJUST_COLORS, LED12);
			#ifdef OS_INDICATOR
				if(!user_config.linux_mode) {
					set_led(MACOS_COLORS, LED13);
				} else {
					set_led(LAYER_ADJUST_COLORS, LED13);
				}
			#else
				set_led(LAYER_ADJUST_COLORS, LED13);
			#endif
		} break;
	}
	rgblight_set();
}

void suspend_power_down_user(void) {
    // rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_user(void) {
    // rgb_matrix_set_suspend_state(false);
}

// loading the eeprom data
void keyboard_post_init_user(void) {
  user_config.raw = eeconfig_read_user();

  if(user_config.info_mode) {
    // TODO: what to do here?
  }
}

// EEPROM is getting reset
void eeconfig_init_user(void) {
  user_config.raw = 0;
  user_config.linux_mode = true; // linux mode by default
  eeconfig_update_user(user_config.raw);
}

void matrix_init_user(void) {
}

void matrix_scan_user(void) {
  /* idle_timer needs to be set one time */
  if(idle_timer == 0) idle_timer = timer_read();

  #ifdef BACKLIGHT_ENABLE
    if(led_on && timer_elapsed(idle_timer) > 30000) {
      halfmin_counter++;
      idle_timer = timer_read();
    }

    if(led_on && halfmin_counter >= BACKLIGHT_TIMEOUT * 2) {
			backlight_disable();
      led_on = false;
      halfmin_counter = 0;
    }
  #endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  if(record->event.pressed) {
    #ifdef BACKLIGHT_ENABLE
      if(led_on == false) {
				backlight_enable();
        led_on = true;
      }
    #endif
    idle_timer = timer_read();
    halfmin_counter = 0;
  }

	switch (keycode) {

    case L_LOWER:
			if(record->event.pressed) {
				layer_on(_L_LOWER);
				#ifdef CONSOLE_ENABLE
					print("L_LOWER down\n");
				#endif
			} else {
				layer_off(_L_LOWER);
				#ifdef CONSOLE_ENABLE
					print("L_LOWER up\n");
				#endif
			}
			update_tri_layer(_L_LOWER, _L_RAISE, _L_ADJUST);
			return false;

		case L_RAISE:
			if(record->event.pressed) {
				layer_on(_L_RAISE);
				#ifdef CONSOLE_ENABLE
					print("L_RAISE down\n");
				#endif
			} else {
				layer_off(_L_RAISE);
				#ifdef CONSOLE_ENABLE
					print("L_RAISE up\n");
				#endif
			}
			update_tri_layer(_L_LOWER, _L_RAISE, _L_ADJUST);
			return false;

    case L_ADJUST:
			if(record->event.pressed) {
				#ifdef CONSOLE_ENABLE
					print("L_ADJST down\n");
				#endif
				layer_on(_L_ADJUST);
			} else {
				#ifdef CONSOLE_ENABLE
					print("L_ADJST up\n");
				#endif
				layer_off(_L_ADJUST);
			}
			return false;


		case M_LOWER:
			if(record->event.pressed) {
				#ifdef CONSOLE_ENABLE
					print("M_LOWER down\n");
				#endif
				layer_on(_M_LOWER);
			} else {
				#ifdef CONSOLE_ENABLE
					print("M_LOWER up\n");
				#endif
				layer_off(_M_LOWER);
			}
			update_tri_layer(_M_LOWER, _M_RAISE, _M_ADJUST);
			return false;
				
		case M_RAISE:
			if(record->event.pressed) {
				#ifdef CONSOLE_ENABLE
					print("M_RAISE down\n");
				#endif
				layer_on(_M_RAISE);
			} else {
				#ifdef CONSOLE_ENABLE
					print("M_RAISE up\n");
				#endif
				layer_off(_M_RAISE);
			}
			update_tri_layer(_M_LOWER, _M_RAISE, _M_ADJUST);
			return false;

    case M_ADJUST:
			if(record->event.pressed) {
				#ifdef CONSOLE_ENABLE
					print("M_ADJST down\n");
				#endif
				layer_on(_M_ADJUST);
			} else {
				#ifdef CONSOLE_ENABLE
					print("M_ADJST up\n");
				#endif
				layer_off(_M_ADJUST);
			}
			return false;

		case RGB_HUI:
		case RGB_HUD:
		case RGB_SAI:
		case RGB_SAD:
		case RGB_VAI:
		case RGB_VAD: {
			if(record->event.pressed) {

				process_rgb(keycode, record);

				rgblight_set_effect_range(LED00, 0);
				rgblight_sethsv_range(
					rgblight_get_hue(),
					rgblight_get_sat(),
					rgblight_get_val(),
					LED00,
					LED13
				);

				#ifdef CONSOLE_ENABLE
					print("COLOR_UPDATE:showcasing new color\n");
				#endif
			}
		} return false;


		/* Info Toggle */
		case INF_TGL: {
			if(record->event.pressed) {
				user_config.info_mode = !user_config.info_mode;
				eeconfig_update_user(user_config.raw);
				rgblight_set_effect_range(LED00, user_config.info_mode ? 8 : 14);
				#ifdef CONSOLE_ENABLE
					print("INF_TGL");
					if(user_config.info_mode) print("on\n");
					else 											print("off\n");
				#endif
			}
		} break;

		/* Switch from Linux to MacOS */
		case L_TO_M: {
			if(record->event.pressed) {
				#ifdef CONSOLE_ENABLE
					print("LAYOUT: Mac\n");
				#endif
				user_config.linux_mode = false;
				set_single_persistent_default_layer(_M_BASE); // TODO: integrate in user_config?
			}
		} break;

		/* Switch from MacOS to Linux */
		case M_TO_L: {
			if(record->event.pressed) {
				#ifdef CONSOLE_ENABLE
					print("LAYOUT: Linux\n");
				#endif
				user_config.linux_mode = true;
				set_single_persistent_default_layer(_L_BASE);
			}
		} break;
	}


	return true;
}

layer_state_t layer_state_set_user(layer_state_t state) { // TODO: add correct colors
  #ifdef LAYER_INDICATOR
    if(user_config.info_mode) {
			#ifdef CONSOLE_ENABLE
				print("info_mode:on");
			#endif
      set_rgblight_by_layer(state);
    } else {
			#ifdef CONSOLE_ENABLE
				print("info_mode:off\n");
			#endif
		}
  #endif
  return state;
}

void oneshot_layer_changed_user(uint8_t layer) {

	if(layer == _L_LOWER || layer == _M_LOWER) {
		#ifdef CONSOLE_ENABLE
			print("modtap:shift down\n");
		#endif
		register_code(KC_LSFT);
		// activate shift
	}

	if(!layer) {
		// deactivate shift and other things
		#ifdef CONSOLE_ENABLE
			print("modtap:shift up\n");
		#endif
		unregister_code(KC_LSFT);
	}
}

bool led_update_user(led_t led_state) {
  #ifdef CAPSLOCK_INDICATOR
    if(led_state.caps_lock) {
      set_led(CAPS_COLORS, LED07);
			rgblight_set();
      return false;
    } else {
      return true;
    }
  #endif
}
