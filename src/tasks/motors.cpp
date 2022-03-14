#include "tasks/motors.h"

namespace // hidden
{

    // pin definition for ld293d H-bridge-driven motor
    struct Motor
    {
        uint8_t dir1; // direction 2 output enable signal pin
        uint8_t dir2; // direction 2 output enable signal pin
        uint8_t pwm;  // pwm output signal pin

        uint32_t dutycycle;
        uint8_t channel;    // pwm channel // 0-15 different channels
        uint8_t resolution; // pwm resolution // 1-16 bits
        double freq;
    };

    struct Param
    {
        struct Motor **motor;        // array of two motors
        uint32_t inputMin, inputMax; // input range
        uint32_t inputMean;          // calculated from input range
        uint8_t resolution; // pwm resolution // 1-16 bits // applied to both motors
        double freq;
    };

    // update dual motor speeds and directions
    void dualDrive_update(struct Param *param, double m1_input, double m2_input)
    {
        struct Motor **motor = param->motor;

        // set motor direction
        bool m1_dir = (m1_input < param->inputMean);
        bool m2_dir = (m2_input < param->inputMean);
        digitalWrite(motor[0]->dir1, m1_dir);
        digitalWrite(motor[0]->dir2, !m1_dir);
        digitalWrite(motor[1]->dir1, m2_dir);
        digitalWrite(motor[1]->dir2, !m2_dir);

        // set pwm control
        ledcWrite(motor[0]->channel, abs((m1_input) - (param->inputMean)));
        ledcWrite(motor[1]->channel, abs((m2_input) - (param->inputMean)));

        Serial.print("direction\n[");
        Serial.print(m1_dir);
        Serial.print(":");
        Serial.print(m2_dir);
        Serial.print("]\n");
        Serial.print("[");
        Serial.print(!m1_dir);
        Serial.print(":");
        Serial.print(!m2_dir);
        Serial.print("]\n");
        Serial.print("speed\n[");
        Serial.print(abs((m1_input) - (param->inputMean)));
        Serial.print(":");
        Serial.print(abs((m2_input) - (param->inputMean)));
        Serial.print("]\n");
        Serial.print("speed%\n[");
        Serial.print(abs((m1_input) - (param->inputMean))/(param->inputMax-param->inputMin));
        Serial.print(":");
        Serial.print(abs((m2_input) - (param->inputMean))/(param->inputMax-param->inputMin));
        Serial.print("]\n");
    }

    void getParameters(struct Param *param)
    {
        // set parameters here
        param->inputMin = 0;
        param->inputMax = 255;                                    // 16 bit resolution -> 65536 possible values
        param->inputMean = (param->inputMin + param->inputMax) / 2; // stored here as to avoid calculation every loop
        param->resolution = 8;
        param->freq = 4000;

        // allocate 2 dual motor array

        param->motor =
            (struct Motor **)malloc(2 * (sizeof *(param->motor)));
        struct Motor *motor;
        
        // first motor
        motor = (struct Motor *)malloc((sizeof *(motor)));
        motor->dir1 = 32;
        motor->dir2 = 33;
        motor->pwm = 25;
        motor->channel = 0;
        motor->resolution = param->resolution;
        motor->freq = param->freq;
        param->motor[0] = motor;

        // second motor
        motor = (struct Motor *)malloc((sizeof *(motor)));
        motor->dir1 = 26;
        motor->dir2 = 27;
        motor->pwm = 14;
        motor->channel = 1;
        motor->resolution = param->resolution;
        motor->freq = param->freq;
        param->motor[1] = motor;
    }

    void getConstraints(struct Task_Constraints *cnst)
    {
        // set constraints here
        cnst->xFirst = 0;                           /// [O] First activation
        cnst->xWcet = 0;                            /// [C] Worst-case execution time //unused?//
        cnst->xPeriod = 500 / portTICK_PERIOD_MS;   /// [T] Period
        cnst->xDeadline = 500 / portTICK_PERIOD_MS; /// [D] Deadline
    }

    void getInformation(struct Task_Information *info)
    {
        // set variables here
        info->name = "MotorTask";
        info->usStackDepth = 1000;
        info->pvParameters = (void *)1;
        info->uxPriority = 1;
        info->pvCreatedTask = NULL;
    }

    uint32_t m1_input;
    uint32_t m2_input;
    void uSetup(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;
        struct Motor **motor = param->motor;

        // first motor
        pinMode(motor[0]->dir1, OUTPUT);
        pinMode(motor[0]->dir2, OUTPUT);
        pinMode(motor[0]->pwm, OUTPUT);
        ledcSetup(motor[0]->channel, motor[0]->freq, motor[0]->resolution);
        ledcAttachPin(motor[0]->pwm, motor[0]->channel);
        
        // second motor
        pinMode(motor[1]->dir1, OUTPUT);
        pinMode(motor[1]->dir2, OUTPUT);
        pinMode(motor[1]->pwm, OUTPUT);
        ledcSetup(motor[1]->channel, motor[1]->freq, motor[1]->resolution);
        ledcAttachPin(motor[1]->pwm, motor[1]->channel);

        m1_input = param->inputMean;
        m2_input = param->inputMean;
    }

    void uLoop(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;
        m1_input +=10;
        m2_input -=50;
        m1_input = m1_input%(param->inputMax-param->inputMin);
        m2_input = m2_input%(param->inputMax-param->inputMin);
        Serial.print("[");
        Serial.print(m1_input);
        Serial.print(":");
        Serial.print(m2_input);
        Serial.print("]\n");
        dualDrive_update(param, m1_input, m2_input);
    }
}

struct Unified_Task *InitMotorTask()
{
    struct Unified_Task *uTask = (struct Unified_Task *)malloc(sizeof *uTask);
    struct Task_Information *info = (struct Task_Information *)malloc(sizeof *info);
    struct Task_Constraints *cnst = (struct Task_Constraints *)malloc(sizeof *cnst);
    struct Param *param = (struct Param *)malloc(sizeof *param);

    // init setup funtion
    uTask->uSetup = uSetup;

    // init looped function
    uTask->uLoop = uLoop;

    // init parameters
    getInformation(info);
    uTask->info = info;
    getConstraints(cnst);
    uTask->cnst = cnst;
    getParameters(param);
    uTask->param = param;
    Serial.print("Done building\n");
        delay(1000);

    return uTask;
}