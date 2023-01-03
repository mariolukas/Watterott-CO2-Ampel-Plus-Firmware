#ifndef HTTP_H
#define HTTP_H
#include "scheduler.h"

constexpr uint32_t HTTP_SERVER_TASK_PERIOD_MS = 10;

extern Task task_http_server;

#endif
