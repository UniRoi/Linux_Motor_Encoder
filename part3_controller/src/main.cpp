#include <iostream>
#include <thread>
#include <stdint.h>
#include <mutex>
#include <gpiod.h> // All of the GPIO stuff...
#include <unistd.h> // usleep()
#include <time.h>
#include <chrono>  // For high-precision timing
#include <pigpio.h>  // Include pigpio header

#include "speed_control.h"
#include "encoder.h"

using namespace std::chrono;

#define DBG2_PIN 25
#define DBG_PIN 17
#define SLP_PIN 5
#define FLT_PIN 4

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

static int gpio_pin = 18;   // GPIO 18 (Physical Pin 12)
static int frequency = 1000; // 1kHz PWM

void speedCtlTask(void);
void encoderUpdateTask(void);

int percentageToDutyCycle(float percentage) 
{
  if (percentage < 0.0) 
  {
      percentage = 0.0;
  } else if (percentage > 100.0) 
  {
      percentage = 100.0;
  }
  
  return static_cast<int>((percentage / 100.0) * 255);
}

void speedCtlTask(void)
{
  static int dbg_pin_state = 0;
  int16_t i16Rps = 0;

  auto previousTime = high_resolution_clock::now();

  int new_duty = 0;
  double speed_new = 0;

  while (1)
  {
    
    auto currentTime = high_resolution_clock::now();
    /* Wait for the mutex to become available. */
    {
      // std::cout << "Enter speed ctl task\n";
      std::lock_guard<std::mutex> lock(mtx);


      duration<double> elapsedTime = duration_cast<duration<double>>(currentTime - previousTime);
      
      // print elapsed time for jitter measurement
      // std::cout << elapsedTime.count() << std::endl;

      // toggle dbg pin for timing
      dbg_pin_state = !dbg_pin_state;

      // if(gpiod_line_set_value(pin_dbg, dbg_pin_state) < 0)
      // {
      //   std::cout << "Could not set value for dbg pin \n";
      // }

      // get encoder rpm directly from kernel module
      i16Rps = Encoder.GetRpm();

      speed_new = P_speed->update(targetRpm, static_cast<double>(i16Rps), 0.1 );

      new_duty = percentageToDutyCycle((speed_new / targetRpm)*100);

      std::cout << i16Rps << " " << new_duty << std::endl;

      gpioPWM(gpio_pin, new_duty);
      previousTime = currentTime;
      
      // std::cout << "leaving speed ctl task\n";
    }
    // usleep(100000);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

  // Initialize pigpio library
  if (gpioInitialise() < 0) {
      std::cerr << "pigpio initialization failed!\n";
      return -1;
  }

  gpioSetPWMfrequency(gpio_pin, frequency);
  gpioPWM(gpio_pin, 0); // start with zero duty cycle

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

  if (Encoder.init() < 0)
  {
    std::cout << "please load kerneldriver with sudo insmod and create device with sudo mknod /dev/encoder c 236 0\n";
    ret = -1;
    return ret;
    // goto cleanup;
  }

  if(gpiod_line_set_value(pin_slp, 1) < 0)
  {
    std::cout << "could not set value for slp " << std::endl; //<< u64TimeNow
  }

  P_speed = new PI_control(m_fKp, m_fTi, 0.1, 12500, 1);


  gpiod_line_request_output(pin_dbg, "polling", 0);
  gpiod_line_request_output(pin_slp, "event", 1);

  /* do setup before creating task */

  std::thread speedCtlTaskObj (speedCtlTask);
  std::thread encoderUpdateTaskObj (encoderUpdateTask);

  /* Allow the tasks to run. */
  speedCtlTaskObj.join();
  encoderUpdateTaskObj.join();

cleanup:
  // release_line:
  gpiod_line_release(pin_dbg);
  gpiod_line_release(pin_dbg2);
  gpiod_line_release(pin_flt);
  gpiod_line_release(pin_slp);

  // close_chip:
    gpiod_chip_close(chip);

  // Terminate pigpio library
  gpioTerminate();

  return ret;
}

