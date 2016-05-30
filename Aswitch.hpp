/* Aswitch.hpp -- make an analog pin behave like a pushbutton
   Author: Alex Shroyer
*/
#pragma once
#include <Arduino.h>

class Aswitch{
public:
    /* @param p: analog-enabled pin
       @param f: either `analogRead` or `touchRead`
    */
    Aswitch(uint8_t p,int(*f)(uint8_t)):pin(p),func(f),threshold(200){}

    void update(void){
        int value=func(pin);
        static int idx{0};
        buf[++idx%BUFLEN]=value;// update ring buffer
        if(accept_next){
            accept_next=false;
            double average=get_avg();
            current_state=(average-value>threshold)||!(value-average>threshold);
            hilo=prev_state&&!current_state;// falling edge reset in hl()
            lohi=current_state&&!prev_state;// rising edge reset in lh()
        }
        prev_state=current_state;
    }

    bool hl(void){return reset_edge(hilo);}
    bool lh(void){return reset_edge(lohi);}
    //int get_thresh(void){return threshold;}
    //void set_thresh(int new_thresh){threshold=new_thresh;}

private:
    bool reset_edge(bool &edge){
        bool t=edge;
        edge=false;
        accept_next=true;
        return t;
    }

    double get_avg(void){
        double sum{0};
        for(uint8_t i=0;i<BUFLEN;++i){sum+=buf[i];}
        return sum/BUFLEN;
    }

    uint8_t pin;
    int (*func)(uint8_t);
    int threshold;
    static const uint8_t BUFLEN{7};
    int buf[BUFLEN]{0};
    bool current_state,prev_state,hilo,lohi;
    bool accept_next{true};
};
