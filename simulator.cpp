#include "simulator.h"

Simulator::Simulator() : memory(), regisFile(){}

void Simulator::fetch(){
    word selectedPC = 0;

    if(M.icode == IJXX && !M.Cnd){
        selectedPC = M.valA;
    }else if(W.icode == IRET){
        selectedPC = W.valM;
    }else{
        selectedPC = F.predPC;
    }


    bool imem_error = false;

    byte split = 0;
    if(memory.get(selectedPC, &split)){
        imem_error = true;
    }

    byte align_regis = 0;
    imem_error = !memory.get(selectedPC + 1, &align_regis);

    byte rA = align_regis >> 4;
    byte rB = align_regis & 0xf;


    byte icode = split >> 4;
    if(imem_error) icode = INOP;
    byte ifun = split & 0xf;

    bool needValC = false;
    if(icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ
            || icode == IJXX || icode == ICALL){
        needValC = true;
    }

    bool needRegis = false;
    if(icode == IRRMOVQ || icode == IOPQ || icode == IPUSHQ
            || icode == IPOPQ || icode == IIRMOVQ || icode == IRMMOVQ || icode == IMRMOVQ){
        needRegis = true;
    }

    word valC = 0;
    if(needRegis) imem_error = !memory.getWord(selectedPC + 2, &valC);
    else imem_error = !memory.getWord(selectedPC + 1, &valC);

    word incrementPC = selectedPC + 1 + needRegis + 8 * needValC;

    bool instrValid = false;
    if(icode >= 0 && icode <= 0xB){
        switch (icode) {
        case IOPQ:
            instrValid = (ifun >= 0 && ifun <= 3);break;
        case IJXX:
        case IRRMOVQ:
            instrValid = (ifun >= 0 && ifun <= 6);break;
        default:
            instrValid = (ifun == 0);
        }
    }
    
    

    byte stat = AOK;
    if(icode == IHALT) stat = HLT;
    if(imem_error) stat = ADR;
    if(!instrValid) stat = INS;

    word predictPC = 0;
    if(icode == IJXX || icode == ICALL) predictPC = valC;
    else predictPC = incrementPC;

    D.in_stat = stat;
    D.in_icode = icode;
    D.in_ifun = ifun;
    D.in_rA = rA;
    D.in_rB = rB;
    D.in_valC = valC;
    D.in_valP = incrementPC;

    F.in_predPC = predictPC;
    
}

void Simulator::decode()
{
    byte dstE = 0xF;
    if(D.icode == IRRMOVQ || D.icode == IIRMOVQ || D.icode == IOPQ){
        dstE = D.rB;
    }else if(D.icode == IPUSHQ || D.icode == IPOPQ || D.icode == ICALL || D.icode == IRET){
        dstE = RRSP;
    }else{
        dstE = 0xF;
    }

    byte dstM = 0xF;
    if(D.icode == IMRMOVQ || D.icode == IPOPQ){
        dstM = D.rA;
    }else{
        dstM = 0xF;
    }

    byte srcA = 0;
    if(D.icode == IRRMOVQ || D.icode == IRMMOVQ || D.icode == IOPQ || D.icode == IPUSHQ){
        srcA = D.rA;
    }else if(D.icode == IPOPQ || D.icode == IRET){
        srcA = RRSP;
    }else{
        srcA = 0xF;
    }

    byte srcB = 0;
    if(D.icode == IOPQ || D.icode == IRRMOVQ || D.icode == IMRMOVQ){
        srcB = D.rB;
    }else if(D.icode == IPUSHQ || D.icode == IPOPQ || D.icode == ICALL || D.icode == IRET){
        srcB = RRSP;
    }else{
        srcB = 0xF;
    }

    word d_rvalA = regisFile.get(srcA);
    word d_rvalB = regisFile.get(srcB);

    word d_valA = 0;
    if(D.icode == ICALL || D.icode == IJXX) d_valA = D.valP;
    else if(srcA == e.dstE) d_valA = e.valE;
    else if(srcA == M.dstM) d_valA = m.valM;
    else if(srcA == M.dstE) d_valA = M.valE;
    else if(srcA == W.dstM) d_valA = W.valM;
    else if(srcA == W.dstE) d_valA = W.valE;
    else d_valA = d_rvalA;

    word d_valB = 0;
    if(srcB == e.dstE) d_valB = e.valE;
    else if(srcB == M.dstM) d_valB = m.valM;
    else if(srcB == M.dstE) d_valB = M.valE;
    else if(srcB == W.dstM) d_valB = W.valM;
    else if(srcB == W.dstE) d_valB = W.valE;
    else d_valB = d_rvalB;

    d.srcA = srcA;
    d.srcB = srcB;

    E.in_stat = D.stat;
    E.in_icode = D.icode;
    E.in_ifun = D.ifun;
    E.in_valC = D.valC;
    E.in_valA = d_valA;
    E.in_valB = d_valB;
    E.in_dstE = dstE;
    E.in_dstM = dstM;
    E.in_srcA = srcA;
    E.in_srcB = srcB;
}

void Simulator::execute()
{
    bool setCC = false;
    if(E.icode == IOPQ) setCC = true;

    word ALU_A = 0;
    if(E.icode == IRRMOVQ || E.icode == IOPQ) ALU_A = E.valA;
    else if(E.icode == IIRMOVQ || E.icode == IRMMOVQ || E.icode == IMRMOVQ) ALU_A = E.valC;
    else if(E.icode == ICALL || E.icode == IPUSHQ) ALU_A = -8;
    else if(E.icode == IRET || E.icode == IPOPQ) ALU_A = 8;

    word ALU_B = 0;
    if(E.icode == IRMMOVQ || E.icode == IMRMOVQ || E.icode == IOPQ || E.icode == ICALL ||
            E.icode == IPUSHQ || E.icode == IRET || E.icode == IPOPQ) ALU_B = E.valB;
    else if(E.icode == IRRMOVQ || E.icode == IIRMOVQ) ALU_B = 0;

    word ALU_fun = 0;
    if(E.icode == IOPQ) ALU_fun = E.ifun;
    else ALU_fun = ALUADD;

    word valE = 0;
    switch (ALU_fun) {
    case ALUADD: valE = ALU_B + ALU_A; break;
    case ALUSUB: valE = ALU_B - ALU_A; break;
    case ALUAND: valE = ALU_B & ALU_A; break;
    case ALUXOR: valE = ALU_B ^ ALU_A; break;
    }

    if(setCC){
        if(valE == 0) E.CC[ZF] = 1;
        else E.CC[ZF] = 0;

        if(valE < 0) E.CC[SF] = 1;
        else E.CC[SF] = 0;

        if((ALU_A < 0 == ALU_B < 0) && (valE < 0 != ALU_A < 0)) E.CC[OF] = 1;
        else E.CC[OF] = 0;
    }


    bool cnd = false;
    if(E.ifun == 0) cnd = true;
    if(E.ifun == 1 && ((E.CC[SF] ^ E.CC[OF]) || E.CC[ZF])) cnd = true;
    if(E.ifun == 2 && (E.CC[SF] ^ E.CC[OF])) cnd = true;
    if(E.ifun == 3 && E.CC[ZF]) cnd = true;
    if(E.ifun == 4 && !E.CC[ZF]) cnd = true;
    if(E.ifun == 5 && !(E.CC[SF] ^ E.CC[OF])) cnd = true;
    if(E.ifun == 6 && !((E.CC[SF] ^ E.CC[OF]) || E.CC[ZF])) cnd = true;
    if(E.icode == IOPQ && !control.setCC) cnd = false;

    

    byte dstE = E.dstE;
    if(E.icode == IIRMOVQ && !cnd) dstE = 0xF;

    e.valE = valE;
    e.dstE = dstE;
    e.cnd = cnd;

    M.in_stat = E.stat;
    M.in_icode = E.icode;
    M.in_Cnd = cnd;
    M.in_valE = valE;
    M.in_valA = E.valA;
    M.in_dstE = dstE;
    M.in_dstM = E.dstM;
}

void Simulator::memo()
{
    bool memRead = false;
    if(M.icode == IMRMOVQ || M.icode == IPOPQ || M.icode == IRET) memRead = true;

    bool memWrite = false;
    if(M.icode == IRMMOVQ || M.icode == IPUSHQ || M.icode == ICALL) memWrite = true;

    if(memRead && memWrite) std::cout << "read and write together";

    word addr = 0;
    if(M.icode == IRMMOVQ || M.icode == IPUSHQ || M.icode == ICALL || M.icode == IMRMOVQ) addr = M.valE;
    if(M.icode == IPOPQ || M.icode == IRET) addr = M.valA;

    word valM = 0;
    bool mem_error = false;
    if(memRead){
        mem_error = !memory.getWord(addr, &valM);
    }

    if(memWrite){
        mem_error = !memory.setWord(addr, M.valA);
    }

    byte stat = AOK;
    if(mem_error) stat = ADR;
    else stat = M.stat;

    m.stat = stat;
    m.valM = valM;

    W.in_stat = stat;
    W.in_icode = M.icode;
    W.in_valE = M.valE;
    W.in_valM = valM;
    W.in_dstE = M.dstE;
    W.in_dstM = M.dstM;
}

void Simulator::writeback()
{
    regisFile.set(W.dstE, W.valE);
    regisFile.set(W.dstM, W.valM);
}

void Simulator::controlLogicUpdate()
{
    control.F_stall = (E.icode == IMRMOVQ || E.icode == IPOPQ) &&
                        (E.dstM == d.srcA || E.dstM == d.srcB) ||
                        (D.icode == IRET || E.icode == IRET || M.icode == IRET);

    control.D_stall = (E.icode == IMRMOVQ || E.icode == IPOPQ)
            && (E.dstM == d.srcA || E.dstM == d.srcB);

    control.D_bubble = (E.icode == IJXX && !e.cnd) ||
            (!((E.icode == IMRMOVQ || E.icode == IPOPQ)
             && (E.dstM == d.srcA || E.dstM == d.srcB))
             && (D.icode == IRET || E.icode == IRET || M.icode == IRET));

    control.E_bubble = (E.icode == IJXX && !e.cnd)
            || ((E.icode == IMRMOVQ || E.icode == IPOPQ)
                && (E.dstM == d.srcA || E.dstM == d.srcB));

    control.setCC = (E.icode == IOPQ) &&
            m.stat == AOK && W.stat == AOK;

    control.M_bubble = !(m.stat == AOK && W.stat == AOK);

    control.W_stall = !(W.stat == AOK);
}

void Simulator::tick()
{
    cicle++;

    if(!control.W_stall){
        W.update();
        writeback();
    }
    if(control.M_bubble){
        M.bubble();
    }else{
        M.update();
    }
    memo();
    if(control.E_bubble){
        E.bubble();
    }else{
        E.update();
    }
    execute();
    if(control.D_stall){
        
    }else if(control.D_bubble){
        D.bubble();
    }else{
        D.update();
    }
    decode();
    controlLogicUpdate();
    if(!control.F_stall){
        F.update();
        fetch();
    }
    controlLogicUpdate();

    print();
}

void Simulator::load(std::string path){
    init();
    std::ifstream input;
    input.open(path.c_str());
    bool isOpen = input.is_open();
    int curAddr = 0;
    while(input){
        std::string line;
        std::getline(input, line);
        if(line[0] == '.'){
            curAddr = strtol(line.c_str() + 1, NULL, 16);
        }
        else{
            int length = line.length();
            for(int i = 0; i < length; i += 2){
                std::string num = "00";
                num[0] = line[i];
                num[1] = line[i + 1];
                memory.set(curAddr, (byte)strtol(num.c_str(), NULL, 16));
                curAddr++;
            }
        }
    }
}

void Simulator::print()
{
    
    std::cout << F.predPC << std::endl;
    std::cout << std::hex << (int)D.stat << (int)D.icode << (int)D.ifun << (int)D.rA << (int)D.rB << D.valC << D.valP << std::endl;
    std::cout << std::hex << (int)E.stat << (int)E.icode << (int)E.ifun << E.valC << E.valA << E.valB << (int)E.dstE << (int)E.dstM << (int)E.srcA << (int)E.srcB << std::endl;
    std::cout << std::hex << (int)M.stat << (int)M.icode << M.Cnd << M.valE << M.valA << (int)M.dstE << (int)M.dstM << std::endl;
    std::cout << std::hex << (int)W.stat << (int)W.icode << W.valE << W.valM << (int)W.dstE << (int)W.dstM << std::endl;

}

void Simulator::init()
{
    cicle = 0;
    control.init();
    F.init();
    D.init();
    E.init();
    M.init();
    W.init();
}
