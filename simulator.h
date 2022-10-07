#pragma once

#include <iostream>
#include <fstream>
#include "memory.h"
#include "regisfile.h"

struct F_T{
    long predPC;

    long in_predPC;

    void update(){
        predPC = in_predPC;
    }
    void init(){
        predPC = 0;

        in_predPC = 0;
    }
};

struct D_T{
    byte stat;
    byte icode;
    byte ifun;
    byte rA;
    byte rB;
    long valC;
    long valP;

    byte in_stat;
    byte in_icode;
    byte in_ifun;
    byte in_rA;
    byte in_rB;
    long in_valC;
    long in_valP;

    void update(){
        stat = in_stat;
        icode = in_icode;
        ifun = in_ifun;
        rA = in_rA;
        rB = in_rB;
        valC = in_valC;
        valP = in_valP;
    }
    void bubble(){
        stat = AOK;
        icode = INOP;
        ifun = 0;
        rA = rB = 0xF;
        valC = valP = 0;
    }
    void init(){
        bubble();
        in_stat = AOK;
        in_icode = INOP;
        in_ifun = 0;
        in_rA = in_rB = 0xF;
        in_valC = in_valP = 0;
    }
    
};

struct d_T{
    byte srcA;
    byte srcB;
};

struct E_T{
    byte stat;
    byte icode;
    byte ifun;
    long valC;
    long valA;
    long valB;
    byte dstE;
    byte dstM;
    long srcA;
    long srcB;

    byte in_stat;
    byte in_icode;
    byte in_ifun;
    long in_valC;
    long in_valA;
    long in_valB;
    byte in_dstE;
    byte in_dstM;
    long in_srcA;
    long in_srcB;


    bool CC[3] = {0, 0, 0};

    void bubble(){
        stat = AOK;
        icode = INOP;
        ifun = 0;
        dstE = dstM = 0xF;
        valA = valB = valC = srcA = srcB = 0;
    }
    void update(){
        stat = in_stat;
        icode = in_icode;
        ifun = in_ifun;
        valC = in_valC;
        valA = in_valA;
        valB = in_valB;
        dstE = in_dstE;
        dstM = in_dstM;
        srcA = in_srcA;
        srcB = in_srcB;
    }
    void init(){
        bubble();
        in_stat = AOK;
        in_icode = INOP;
        in_ifun = 0;
        in_dstE = in_dstM = 0xF;
        in_valA = in_valB = in_valC = in_srcA = in_srcB = 0;
    }
};

struct e_T{
    word valE;
    byte dstE;
    bool cnd;
};

struct M_T{
    byte stat;
    byte icode;
    bool Cnd;
    long valE;
    long valA;
    byte dstE;
    byte dstM;

    byte in_stat;
    byte in_icode;
    bool in_Cnd;
    long in_valE;
    long in_valA;
    byte in_dstE;
    byte in_dstM;

    void bubble(){
        stat = AOK;
        icode = INOP;
        Cnd = false;
        valA = valE = 0;
        dstM = dstE = 0xF;
    }

    void update(){
        stat = in_stat;
        icode = in_icode;
        Cnd = in_Cnd;
        valA = in_valA;
        valE = in_valE;
        dstM = in_dstM;
        dstE = in_dstE;
    }

    void init(){
        bubble();
        in_stat = AOK;
        in_icode = INOP;
        in_Cnd = false;
        in_valA = in_valE = 0;
        in_dstM = in_dstE = 0xF;
    }

};

struct m_T{
    word valM;

    byte stat = AOK;
};

struct W_T{
    byte stat;
    byte icode;
    long valE;
    long valM;
    byte dstE;
    byte dstM;

    byte in_stat;
    byte in_icode;
    long in_valE;
    long in_valM;
    byte in_dstE;
    byte in_dstM;

    void update(){
        stat = in_stat;
        icode = in_icode;
        valE = in_valE;
        valM = in_valM;
        dstE = in_dstE;
        dstM = in_dstM;
    }
    void init(){
        stat = AOK;
        icode = INOP;
        valE = valM = 0;
        dstE = dstM = 0xF;

        in_stat = AOK;
        in_icode = INOP;
        in_valE = in_valM = 0;
        in_dstE = in_dstM = 0xF;
    }
    
};

struct ControlLogic{
    bool F_stall;
    bool D_stall;
    bool D_bubble;
    bool E_bubble;
    bool M_bubble;
    bool W_stall;
    bool setCC;

    void init(){
        F_stall = false;
        D_stall = false;
        D_bubble = false;
        E_bubble = false;
        M_bubble = false;
        W_stall = false;
        setCC = false;
    }
};

class Simulator
{
public:
    Memory memory;
    RegisFile regisFile;

    F_T F;
    D_T D;
    E_T E;
    M_T M;
    W_T W;

    d_T d;
    e_T e;
    m_T m;

    ControlLogic control;

    int cicle;

    Simulator();
    void controlLogicUpdate();
    void init();

    void fetch();
    void decode();
    void execute();
    void memo();
    void writeback();

    void load(std::string path);
    void tick();
    void print();
};

