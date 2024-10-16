#include <iostream>
#include <thread>
#include <stdint.h>
#include <mutex>
#include <gpiod.h> // All of the GPIO stuff...
#include <unistd.h> // usleep()
#include <time.h>

#include "speed_control.h"
#include "encoder.h"



/* todo for linux application:
 - check if kernel module is loaded 
 - find a way for updating the time for the control loop 
 - 
 */


#define DBG2_PIN 25
#define DBG_PIN 17
#define SLP_PIN 20
#define FLT_PIN 21

#define CTL_UPDATE_TIME 100 // in ms

std::mutex mtx;// mutex for critical section

static const char *chipname = "gpiochip0";
static struct gpiod_chip *chip;
static struct gpiod_line *pin_dbg; // debug pin
static struct gpiod_line *pin_dbg2; // debug pin
static struct gpiod_line *pin_slp; // output for h-bridge sleep
static struct gpiod_line *pin_flt; // input fro h-bridge fault

encoder Encoder;

Controller *P_speed = nullptr;

static float m_fKp = 0.27;
static float m_fTi = 0.41;
static uint16_t targetRpm = 10000;

void speedCtlTask(void);
void encoderUpdateTask(void);

void speedCtlTask(void)
{
  static time_t u64LastTime = 0;
  time_t u64TimeNow = 0;
  static int dbg_pin_state = 0;
  time(&u64LastTime);

  int16_t i16Rps = 0;

  int new_duty = 0;
  double speed_new = 0;

  while (1)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    time(&u64TimeNow);
    /* Wait for the mutex to become available. */
    {
      std::cout << "Enter speed ctl task\n";
      std::lock_guard<std::mutex> lock(mtx);

      // if ((u64TimeNow - u64LastTime) > CTL_UPDATE_TIME)
      {
        // toggle dbg pin for timing
        dbg_pin_state = !dbg_pin_state;

        // if(gpiod_line_set_value(pin_dbg, dbg_pin_state) < 0)
        // {
        //   std::cout << "Could not set value for dbg pin \n";
        // }

        // get encoder rpm directly from kernel module
        i16Rps = Encoder.GetRpm();

        std::cout << "Rpm: " << i16Rps;

        // speed_new = P_speed->update(targetRpm, static_cast<double>(i16Rps), 0);

        // new_duty = (constrain(speed_new / targetRpm, 0.01, 0.99) * 100);

        std::cout << " new duty: " << new_duty << std::endl;

        /* check how to make pwm */
        // ana_out.set(new_duty);
        u64LastTime = u64TimeNow;
      }
      // std::cout << "leaving speed ctl task\n";
    }
  }
}

void encoderUpdateTask(void)
{
  static time_t u64LastTime = 0;
  time_t u64TimeNow = 0;
  int dbg_pin_state = 0;
  time(&u64LastTime);

  while (1)
  {
    // std::cout << "Enter encoder update task\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    time(&u64TimeNow);
    /* Wait for the mutex to become available. */
    {
      std::lock_guard<std::mutex> lock(mtx);

      dbg_pin_state = !dbg_pin_state;
      if(gpiod_line_set_value(pin_dbg, dbg_pin_state) < 0)
      {
        std::cout << "could not set value for dbg2 " << std::endl; //<< u64TimeNow
      }

      Encoder.updatePps();

    }
    // std::cout << "leaving encoder update task\n";
  }
}

int main(void)
{
  int ret = 0;
  // uint32_t u32TimeNow = 0;
  // eStates eStateTransition;
  // bool bFltState = false;
  // bool bResume = false;

  // int16_t i16Rps = 0;

  // int new_duty = 0;
  // double speed_new = 0;

  // u32TimeNow = millis();

  chip = gpiod_chip_open_by_name(chipname);

  if (!chip) {
    perror("Open chip failed\n");
    return -1;
  }

  pin_dbg = gpiod_chip_get_line(chip, DBG_PIN);

  if (!pin_dbg) {
    perror("Get line Dbg failed\n");
    ret = -1;
    // goto close_chip;
  }

  pin_dbg2 = gpiod_chip_get_line(chip, DBG2_PIN);

  if (!pin_dbg2) {
    perror("Get line Dbg failed\n");
    ret = -1;
    // goto close_chip;
  }

  pin_flt = gpiod_chip_get_line(chip, FLT_PIN);

  if (!pin_flt) {
    perror("Get line flt failed\n");
    ret = -1;
    // goto close_chip;
  }

  pin_slp = gpiod_chip_get_line(chip, SLP_PIN);

  if (!pin_slp) {
    perror("Get line slp failed\n");
    ret = -1;
    // goto close_chip;
  }

  // Encoder.init();
  // P_speed = new PI_control(m_fKp, m_fTi, 0.1, 12500, 1);


  gpiod_line_request_output(pin_dbg, "polling", 0);
  gpiod_line_request_output(pin_slp, "event", 1);

  /* do setup before creating task */

  std::thread speedCtlTaskObj (speedCtlTask);
  std::thread encoderUpdateTaskObj (encoderUpdateTask);

  /* Allow the tasks to run. */
  speedCtlTaskObj.join();
  encoderUpdateTaskObj.join();

  // release_line:
  // gpiod_line_release(pin_in);

  // close_chip:
  //   gpiod_chip_close(chip);

  return ret;
}

