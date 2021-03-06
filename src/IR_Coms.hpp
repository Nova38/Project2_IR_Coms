#ifndef IR_COMS_HPP
#define IR_COMS_HPP

const int sampleValSize = 8;
const int syncSize = 4;

const long interval = 125; // interval at which to blink (microseconds)

class IR_Com
{
public:
protected:
};

class IR_Reciver : public IR_Com
{
private:
    const int my_Pin;
    unsigned long base_time = 0; // will store last time LED was updated
    int sampledVals[sampleValSize] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned long[syncSize] 
    int last_state = 0;

public:
    IR_Reciver(int recPin) : my_Pin(recPin)
    {
        pinMode(recPin, INPUT_PULLUP);
        base_time = micros();
    }
    ~IR_Reciver();

    void Loop()
    {
        // unsigned long currentMicro = micros();
        // unsigned long intervalMicro = currentMicro - previousMicro;

        // if (intervalMicro >= interval)
        // {
        //     onStateChange();

        //     previousMicro = currentMicro;
        // }
        // if (currentPos == sampleValSize)
        // {

        //     onByteRec();
        //     currentPos = 0;
        // 

        int currrent_state = digitalRead(my_Pin);

        // First wait for the first bit to change to
        if(currrent_state != last_state){
           
        }
        
        // Then sync using the sync bits to the

        // Then after 4 sync bits start waiting for start bits


    }

private:
    // bool onStateChange()
    // {
    //     int sample = digitalRead(2);
    //     sampledVals[currentPos] = sample;
    //     currentPos++;
    // }

    void onByteRec()
    {
        Serial.print("\nRecived: ");
        for (int i = 0; i < sampleValSize; i++)
        {
            Serial.print(sampledVals[i]);
            sampledVals[i] = 0;
        }
    }
};


// constants won't change:


class IR_Trasmitter : public IR_Com
{
private:
    const int my_Pin;
    int currentPos = 0;
    unsigned long previousMicro = 0; // will store last time LED was updated
    int sampledVals[sampleValSize] = {0, 0, 0, 0, 0, 0, 0, 0};

public:
    IR_Trasmitter(int recPin) : my_Pin(recPin)
    {
        pinMode(recPin, INPUT_PULLUP);
    }
private:

}

#endif // IR_COMS_H