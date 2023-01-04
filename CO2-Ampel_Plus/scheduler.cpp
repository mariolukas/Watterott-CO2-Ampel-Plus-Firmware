
#include "scheduler.h"
#include <TaskScheduler.h>

namespace TS {
namespace {
class SchedulerSingleton {
 public:
  static Scheduler& getInstance() {
    static Scheduler instance;
    return instance;
  }

 private:
  SchedulerSingleton() {}

 public:
  SchedulerSingleton(SchedulerSingleton const&) = delete;
  void operator=(SchedulerSingleton const&) = delete;
};
}  // namespace

Scheduler& get_scheduler() {
  return SchedulerSingleton::getInstance();
}
}  // namespace TS
