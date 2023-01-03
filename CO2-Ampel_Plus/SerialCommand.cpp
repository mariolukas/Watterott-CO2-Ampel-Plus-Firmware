#include "SerialCommand.h"
#include <Arduino.h>
#include <CircularBuffer.h>
#include "DeviceConfig.h"
#include "core_cm0plus.h"
#include "errno.h"

constexpr char COMMAND_TERMINATOR = ';';
constexpr size_t _SERIAL_BUFFER_SIZE = 1024;
CircularBuffer<char, _SERIAL_BUFFER_SIZE> serial_buffer;

constexpr size_t _MAX_COMMAND_LENGTH = 16;
constexpr size_t _MAX_ARGUMENT_LENGTH = 32;
constexpr size_t _MAX_VALUE_LENGTH = 128;

typedef struct {
  char command[_MAX_COMMAND_LENGTH];
  char argument[_MAX_ARGUMENT_LENGTH];
  char value[_MAX_VALUE_LENGTH];
} command_sentence_t;

typedef struct command_argument_t {
  const char* name;
  String (*get_handler)(command_argument_t*, int, char*);
  String (*set_handler)(command_argument_t*, int, char*);
} command_argument_t;

constexpr size_t COMMAND_VAL_MAX_STR_LEN = 70;

int parse_bool(const char* value, bool* result) {
  char* endptr;
  errno = 0;

  int val = strtol(value, &endptr, 10);
  if (endptr == value || errno == ERANGE || (!(val == 0 || val == 1))) {
    Serial.print("Error parsing value string for bool: '");
    Serial.print(value);
    Serial.println("' valid values: [0, 1]");
  } else {
    *result = static_cast<bool>(val);
    return 0;
  }
  return 1;
}

int parse_int(const char* value, int* result) {
  char* endptr;
  errno = 0;

  int val = strtol(value, &endptr, 10);
  if (endptr == value || errno == ERANGE) {
    Serial.print("Error parsing value string for int: '");
    Serial.print(value);
    Serial.println("'");
  } else {
    *result = static_cast<int>(val);
    return 0;
  }
  return 1;
}

int parse_float(const char* value, float* result) {
  char* endptr;
  errno = 0;

  float val = strtof(value, &endptr);
  if (endptr == value || errno == ERANGE) {
    Serial.print("Error parsing value string for float: '");
    Serial.print(value);
    Serial.println("'");
  } else {
    *result = static_cast<float>(val);
    return 0;
  }
  return 1;
}

template <typename T, size_t VALUE_MAX_SIZE>
int parse_string(const char* value, T (&result)[VALUE_MAX_SIZE]) {
  if (strnlen(value, VALUE_MAX_SIZE) >= VALUE_MAX_SIZE) {
    Serial.print("String value too long: '");
    Serial.print(value);
    Serial.print("' max ");
    Serial.print(String(VALUE_MAX_SIZE - 1));
    Serial.print(" characters!");
  } else {
    strncpy(result, value, VALUE_MAX_SIZE);
    return 0;
  }
  return 1;
}

void print_var(const char* name, String value) {
  Serial.print(name);
  Serial.print(": '");
  Serial.print(value);
  Serial.println("'");
}

String get_handler(command_argument_t* arguments,
                   int argument_index,
                   char* value) {
  if (arguments[argument_index].get_handler == nullptr) {
    Serial.print("'get' not available for '");
    Serial.print(arguments[argument_index].name);
    Serial.println("'");
    return "";
  }

  String result =
      arguments[argument_index].get_handler(arguments, argument_index, value);
  print_var(arguments[argument_index].name, result);
  return result;
}

String get_buzzer(command_argument_t* arguments,
                  int argument_index,
                  char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.buzzer_enabled);
}

String get_change_count(command_argument_t* arguments,
                        int argument_index,
                        char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.change_count);
}

String get_wifi_ssid(command_argument_t* arguments,
                     int argument_index,
                     char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.wifi_ssid);
}

String get_wifi_password(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.wifi_password);
}

String get_ap_password(command_argument_t* arguments,
                       int argument_index,
                       char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.ap_password);
}

String get_mqtt_broker_port(command_argument_t* arguments,
                            int argument_index,
                            char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.mqtt_broker_port);
}

String get_mqtt_broker_address(command_argument_t* arguments,
                               int argument_index,
                               char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.mqtt_broker_address);
}

String get_mqtt_topic(command_argument_t* arguments,
                      int argument_index,
                      char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.mqtt_topic);
}

String get_ampel_name(command_argument_t* arguments,
                      int argument_index,
                      char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.ampel_name);
}

String get_temperature_offset(command_argument_t* arguments,
                              int argument_index,
                              char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.temperature_offset);
}

String get_mqtt_username(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.mqtt_username);
}

String get_mqtt_password(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.mqtt_password);
}

String get_mqtt_format(command_argument_t* arguments,
                       int argument_index,
                       char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.mqtt_format);
}

String get_light_enabled(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.light_enabled);
}

String get_led_brightness(command_argument_t* arguments,
                          int argument_index,
                          char* value) {
  device_config_t cfg = config_get_values();
  return String(cfg.led_brightness);
}

String set_handler(command_argument_t* arguments,
                   int argument_index,
                   char* value) {
  if (arguments[argument_index].set_handler == nullptr) {
    Serial.print("'set' not available for '");
    Serial.print(arguments[argument_index].name);
    Serial.println("'");
    return "";
  }
  String result =
      arguments[argument_index].set_handler(arguments, argument_index, value);
  print_var(arguments[argument_index].name, result);
  return result;
}

String set_buzzer(command_argument_t* arguments,
                  int argument_index,
                  char* value) {
  device_config_t cfg = config_get_values();
  if (parse_bool(value, &cfg.buzzer_enabled) != 0)
    return String("Error setting buzzer!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.buzzer_enabled);
}

String set_wifi_ssid(command_argument_t* arguments,
                     int argument_index,
                     char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.wifi_ssid) != 0)
    return String("Error setting wifi_ssid!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.wifi_ssid);
}

String set_wifi_password(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.wifi_password) != 0)
    return String("Error setting wifi_password!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.wifi_password);
}

String set_ap_password(command_argument_t* arguments,
                       int argument_index,
                       char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.ap_password) != 0)
    return String("Error setting ap_password!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.ap_password);
}

String set_mqtt_broker_port(command_argument_t* arguments,
                            int argument_index,
                            char* value) {
  device_config_t cfg = config_get_values();
  if (parse_int(value, &cfg.mqtt_broker_port) != 0)
    return String("Error setting mqtt_broker_port!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.mqtt_broker_port);
}

String set_mqtt_broker_address(command_argument_t* arguments,
                               int argument_index,
                               char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.mqtt_broker_address) != 0)
    return String("Error setting mqtt_broker_address!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.mqtt_broker_address);
}

String set_mqtt_topic(command_argument_t* arguments,
                      int argument_index,
                      char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.mqtt_topic) != 0)
    return String("Error setting mqtt_topic!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.mqtt_topic);
}

String set_ampel_name(command_argument_t* arguments,
                      int argument_index,
                      char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.ampel_name) != 0)
    return String("Error setting ampel_name!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.ampel_name);
}

String set_temperature_offset(command_argument_t* arguments,
                              int argument_index,
                              char* value) {
  device_config_t cfg = config_get_values();
  if (parse_float(value, &cfg.temperature_offset) != 0)
    return String("Error setting temperature_offset!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.temperature_offset);
}

String set_mqtt_username(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.mqtt_username) != 0)
    return String("Error setting mqtt_username!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.mqtt_username);
}

String set_mqtt_password(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  if (parse_string(value, cfg.mqtt_password) != 0)
    return String("Error setting mqtt_password!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.mqtt_password);
}

String set_mqtt_format(command_argument_t* arguments,
                       int argument_index,
                       char* value) {
  device_config_t cfg = config_get_values();
  if (parse_int(value, &cfg.mqtt_format) != 0)
    return String("Error setting mqtt_format!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.mqtt_format);
}

String set_light_enabled(command_argument_t* arguments,
                         int argument_index,
                         char* value) {
  device_config_t cfg = config_get_values();
  if (parse_bool(value, &cfg.light_enabled) != 0)
    return String("Error setting light_enabled!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.light_enabled);
}

String set_led_brightness(command_argument_t* arguments,
                          int argument_index,
                          char* value) {
  device_config_t cfg = config_get_values();
  if (parse_int(value, &cfg.led_brightness) != 0)
    return String("Error setting led_brightness!");
  config_set_values(cfg);
  cfg = config_get_values();
  return String(cfg.led_brightness);
}

String reboot_handler(command_argument_t* arguments,
                      int argument_index,
                      char* value) {
  Serial.println("Rebooting in 1 second!");
  delay(1000);
  NVIC_SystemReset();

  return "";
}

command_argument_t commands[] = {{"get", &get_handler, nullptr},
                                 {"set", &set_handler, nullptr},
                                 {"reboot", &reboot_handler, nullptr}};

command_argument_t arguments[] = {
    {"buzzer", &get_buzzer, &set_buzzer},
    {"change_count", &get_change_count, nullptr},
    {"wifi_ssid", &get_wifi_ssid, &set_wifi_ssid},
    {"wifi_password", &get_wifi_password, &set_wifi_password},
    {"ap_password", &get_ap_password, &set_ap_password},
    {"mqtt_broker_port", &get_mqtt_broker_port, &set_mqtt_broker_port},
    {"mqtt_broker_address", &get_mqtt_broker_address, &set_mqtt_broker_address},
    {"mqtt_topic", &get_mqtt_topic, &set_mqtt_topic},
    {"ampel_name", &get_ampel_name, &set_ampel_name},
    {"temperature_offset", &get_temperature_offset, &set_temperature_offset},
    {"mqtt_username", &get_mqtt_username, &set_mqtt_username},
    {"mqtt_password", &get_mqtt_password, &set_mqtt_password},
    {"mqtt_format", &get_mqtt_format, &set_mqtt_format},
    {"light_enabled", &get_light_enabled, &set_light_enabled},
    {"led_brightness", &get_led_brightness, &set_led_brightness},
};

int find_index(const char* argument,
               command_argument_t* collection,
               size_t len) {
  for (size_t i = 0; i <= len; i++) {
    if (strcmp(argument, collection[i].name) == 0)
      return static_cast<int>(i);
  }
  return -1;
}

String command_handler(command_sentence_t command_sentence) {
  int command_index = find_index(command_sentence.command, commands,
                                 sizeof(commands) / sizeof(command_argument_t));
  int argument_index =
      find_index(command_sentence.argument, arguments,
                 sizeof(arguments) / sizeof(command_argument_t));

  if (command_index == -1) {
    Serial.println();
    Serial.print("Command not found: '");
    Serial.print(command_sentence.command);
    Serial.println("' Valid commands:");
    for (unsigned int i = 0;
         i < (sizeof(commands) / sizeof(command_argument_t)); i++) {
      Serial.print("  ");
      Serial.println(commands[i].name);
    }
  }

  if (argument_index == -1) {
    Serial.println();
    Serial.println("Argument not found! Valid arguments:");
    for (size_t i = 0; i < (sizeof(arguments) / sizeof(command_argument_t));
         i++) {
      Serial.print("  ");
      Serial.println(arguments[i].name);
    }
  }

  if (command_index == -1 || argument_index == -1) {
    return "";
  }

  // we are using 'get_handler' to store the generic command handler
  if (commands[command_index].get_handler == nullptr) {
    Serial.print("'");
    Serial.print(commands[command_index].name);
    Serial.println("' has no handler configured.");
    return "";
  }

  return (commands[command_index].get_handler)(arguments, argument_index,
                                               command_sentence.value);
}

void token_handler(char* destination, char* source, size_t len) {
  char* token = strtok(source, " ");
  if (token != NULL)
    strncpy(destination, token, len);
}

void serial_handler() {
  bool terminated = false;
  while (Serial.available() > 0 && !serial_buffer.isFull()) {
    char character = static_cast<char>(Serial.read());
    serial_buffer.push(character);
    if (character == ';') {
      terminated = true;
      break;
    }
  }

  if (terminated == true) {
    char command_buffer[_SERIAL_BUFFER_SIZE] = {};
    for (auto i = 0; i < serial_buffer.capacity; i++) {
      char character = serial_buffer.shift();
      if (character == COMMAND_TERMINATOR) {
        command_buffer[i] = 0;
        break;
      }
      command_buffer[i] = character;
    }

    if (strnlen(command_buffer, _SERIAL_BUFFER_SIZE) > 0) {
      command_sentence_t command_sentence = {};
      token_handler(command_sentence.command, command_buffer,
                    _MAX_COMMAND_LENGTH);
      token_handler(command_sentence.argument, NULL, _MAX_ARGUMENT_LENGTH);
      token_handler(command_sentence.value, NULL, _MAX_VALUE_LENGTH);
      command_handler(command_sentence);
    }
  }
}

Task task_serial_handler(10 * TASK_MILLISECOND, -1, &serial_handler, &ts);
