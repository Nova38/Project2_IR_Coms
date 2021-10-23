#ifndef IR_COMS_HPP
#define IR_COMS_HPP

class IR_Com{
    public: 
        const long interval = 125; // interval at which to blink (microseconds)

};

class IR_Reciver:public IR_Com
{
private:
    /* data */
public:
    IR_Reciver(/* args */);
    ~IR_Reciver();
};

IR_Reciver::IR_Reciver(/* args */)
{
}

IR_Reciver::~IR_Reciver()
{
}

// constants won't change:


#endif // IR_COMS_H