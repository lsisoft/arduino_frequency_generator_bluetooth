#include <HardwareSerial.h>
#include <USBAPI.h>
#include "math.h"

class FrequencyCalculator
{
    static const int nscalers = 5;
    static constexpr int scalers[nscalers] = {1, 8, 64, 256, 1024};
//    int master = pow(2, 24);
    int reg = 65536;

    int log_master = 24;
    double log_scalers[5];
    int log_reg = log(reg) / log(2);

public:
    FrequencyCalculator()
    {
        for (int i = 0; i < nscalers; ++i)
            log_scalers[i] = log(scalers[i]) / log(2);

    }

    struct RegCountDuty
    {
        int count_reg, duty_reg;
//        friend std::ostream& operator<<(std::ostream& os, const RegCountDuty& duty)
//        {
//            os << "reg: " << duty.reg << " duty: " << duty.duty;
//            return os;
//        }
    };

    RegCountDuty frequency(double freq, double duty) const
    {
        double log_freq = log(freq) / log(2);
        double possible_log_scaner = log_master - log_reg - log_freq;

        int idx = 0;
        while (idx < nscalers - 1 && possible_log_scaner > log_scalers[idx])
            ++idx;

//        std::cout << freq << " ls " << log_scalers[idx];

        int count_reg = round(pow(2, 24 - log_scalers[idx]) / freq);

        double gen_frequency = pow(2, 24 - log_scalers[idx]) / count_reg;
//        std::cout << " reg " << count_reg << " g " << gen_frequency;

        int duty_reg = round(count_reg * duty);
//        std::cout << " duty_reg " << duty_reg;
//        std::cout << std::endl;

        return RegCountDuty{count_reg, duty_reg};
    }
};
constexpr int FrequencyCalculator::scalers[nscalers];

#define MAX_INPUT 90

char inputBuffer[MAX_INPUT + 1]; // Handles up to 90 bytes in a c-style string, with a null character termination.
char outputBuffer[MAX_INPUT * 2 + 1]; // Handles up to 90 bytes in a c-style string, with a null character termination.

void readSerialChars();
void processBuffer(char* buffer);

void readSerialChars()
{
    if (Serial.available() > 0)
    {
        char input = Serial.read();
        static int s_len; // static variables default to 0
        if (s_len >= MAX_INPUT)
        {
            // Have received already the maximum number of characters
            // Ignore all new input until line termination occurs
        }
        else if (input != '\n' && input != '\r')
            inputBuffer[s_len++] = input;
        else
        {
            inputBuffer[s_len] = 0;
            Serial.print("RECEIVED MSG: ");
            Serial.println(inputBuffer);

            processBuffer(inputBuffer);

            s_len = 0;
            inputBuffer[0] = 0;
        }
    }
}

double currentFreq = 5;
double currentDuty = .1;
double targetFreq = currentFreq;
double targetDuty = currentDuty;
double speedFreq = 0;
double speedDuty = 0;

void processBuffer(char* buffer)
{
    //F30000D10S100
    unsigned int freq, duty, speedSeconds;
    if (sscanf(buffer, "F%uD%uS%u", &freq, &duty, &speedSeconds) != 3)
    {
        sprintf(outputBuffer, "Expecting F30000D10S100, received %s", buffer);
        Serial.println(outputBuffer);

        if (duty > 15)
            duty = 15;

        targetFreq = freq;
        targetDuty = duty / 100.;
        double speedMicro = speedSeconds * 1e6;

        if (targetFreq != currentFreq)
            speedFreq = (targetFreq - currentFreq) / speedMicro;
        else
            speedFreq = 0;

        if (targetDuty != currentDuty)
            speedDuty = (targetDuty - currentDuty) / speedMicro;
        else
            speedDuty = 0;
    }
}

FrequencyCalculator frequencyCalculator;
void setup()
{
    Serial.begin(9600);

    inputBuffer[0] = 0;

    Serial.println("Begin:");
}

unsigned long lastTime = -1;
void loop()
{
    readSerialChars();

    //for speed calculation
    unsigned long currentTime = micros();
    if (currentTime < lastTime) // after ~50 days there is a reset in micros
    {
        lastTime = currentTime;
    }
    else
    {
        const double sliceTime = 100e3;

        if (currentTime - lastTime < sliceTime) // 100 millisecond
        {
            double delta = max(currentTime - lastTime, 2 * sliceTime);// prevent errors

            if (speedFreq != 0)
            {
                currentFreq += speedFreq * delta;
                if (signbit(targetFreq - currentFreq) != signbit(speedFreq))
                {
                    currentFreq = targetFreq;
                    speedFreq = 0;
                }
            }
            if (speedDuty != 0)
            {
                currentDuty += speedDuty * delta;
                if (signbit(targetDuty - currentDuty) != signbit(speedDuty))
                {
                    currentDuty = targetDuty;
                    speedDuty = 0;
                }
            }

            if (speedFreq != 0 || speedDuty != 0)
            {
                auto regs = frequencyCalculator.frequency(currentFreq, currentDuty);

                sprintf(outputBuffer, "freq %f duty %f count_reg %d duty_reg %d", currentFreq, currentDuty, regs.count_reg, regs.duty_reg);
                Serial.println(outputBuffer);
            }

            lastTime = currentTime;
        }
    }
}