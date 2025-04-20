
#include "CPU.hpp"

CPU::CPU() {
    //Initialize the opcode matrix
    opMatrix.resize(16*16 + 3); //3 is for the interrupt sequences
    //OR INDX
    opMatrix[0x01].pneumonic = "ORA";
    opMatrix[0x01].addressing_mode = INDX;
    opMatrix[0x01].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ZP
    opMatrix[0x05].pneumonic = "ORA";
    opMatrix[0x05].addressing_mode = ZP;
    opMatrix[0x05].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x05].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x05].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ZP
    opMatrix[0x06].pneumonic = "ASL";
    opMatrix[0x06].addressing_mode = ZP;
    opMatrix[0x06].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::RMW_set_Z_N_C_write_cycle);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR IMM
    opMatrix[0x09].pneumonic = "ORA";
    opMatrix[0x09].addressing_mode = IMM;
    opMatrix[0x09].cycle_op_list.push_back(&CPU::OR_IMM_cycle2);
    opMatrix[0x09].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL Accum
    opMatrix[0x0A].pneumonic = "ASL";
    opMatrix[0x0A].addressing_mode = Accum;
    opMatrix[0x0A].cycle_op_list.push_back(&CPU::ASL_Accum_cycle2);
    opMatrix[0x0A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ABS
    opMatrix[0x0D].pneumonic = "ORA";
    opMatrix[0x0D].addressing_mode = ABS;
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ABS
    opMatrix[0x0E].pneumonic = "ASL";
    opMatrix[0x0E].addressing_mode = ABS;
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::RMW_set_Z_N_C_write_cycle);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR INDY
    opMatrix[0x11].pneumonic = "ORA";
    opMatrix[0x11].addressing_mode = INDY;
    opMatrix[0x11].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ZP,X
    opMatrix[0x15].pneumonic = "ORA";
    opMatrix[0x15].addressing_mode = ZPX;
    opMatrix[0x15].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x15].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x15].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x15].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ZP,X
    opMatrix[0x16].pneumonic = "ASL";
    opMatrix[0x16].addressing_mode = ZPX;
    opMatrix[0x16].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::RMW_set_Z_N_C_write_cycle);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLC
    opMatrix[0x18].pneumonic = "CLC";
    opMatrix[0x18].addressing_mode = IMP;
    opMatrix[0x18].cycle_op_list.push_back(&CPU::CLC_cycle2);
    opMatrix[0x18].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ABS,Y
    opMatrix[0x19].pneumonic = "ORA";
    opMatrix[0x19].addressing_mode = ABSY;
    opMatrix[0x19].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ABS,X
    opMatrix[0x1D].pneumonic = "ORA";
    opMatrix[0x1D].addressing_mode = ABSX;
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ABS,X
    opMatrix[0x1E].pneumonic = "ASL";
    opMatrix[0x1E].addressing_mode = ABSX;
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::RMW_set_Z_N_C_write_cycle);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND INDX
    opMatrix[0x21].pneumonic = "AND";
    opMatrix[0x21].addressing_mode = INDX;
    opMatrix[0x21].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BIT ZP
    opMatrix[0x24].pneumonic = "BIT";
    opMatrix[0x24].addressing_mode = ZP;
    opMatrix[0x24].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x24].cycle_op_list.push_back(&CPU::BIT_final_cycle);
    opMatrix[0x24].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ZP
    opMatrix[0x25].pneumonic = "AND";
    opMatrix[0x25].addressing_mode = ZP;
    opMatrix[0x25].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x25].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x25].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND IMM
    opMatrix[0x29].pneumonic = "AND";
    opMatrix[0x29].addressing_mode = IMM;
    opMatrix[0x29].cycle_op_list.push_back(&CPU::AND_IMM_cycle2);
    opMatrix[0x29].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BIT ABS
    opMatrix[0x2C].pneumonic = "BIT";
    opMatrix[0x2C].addressing_mode = ABS;
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::BIT_final_cycle);
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ABS
    opMatrix[0x2D].pneumonic = "AND";
    opMatrix[0x2D].addressing_mode = ABS;
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND INDY
    opMatrix[0x31].pneumonic = "AND";
    opMatrix[0x31].addressing_mode = INDY;
    opMatrix[0x31].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ZP,X
    opMatrix[0x35].pneumonic = "AND";
    opMatrix[0x35].addressing_mode = ZPX;
    opMatrix[0x35].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SEC
    opMatrix[0x38].pneumonic = "SEC";
    opMatrix[0x38].addressing_mode = IMP;
    opMatrix[0x38].cycle_op_list.push_back(&CPU::SEC_cycle2);
    opMatrix[0x38].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ABS,Y
    opMatrix[0x39].pneumonic = "AND";
    opMatrix[0x39].addressing_mode = ABSY;
    opMatrix[0x39].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ABS,X
    opMatrix[0x3D].pneumonic = "AND";
    opMatrix[0x3D].addressing_mode = ABSX;
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //RTI
    opMatrix[0x40].pneumonic = "RTI";
    opMatrix[0x40].addressing_mode = IMP;
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle2);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle3);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle4);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle5);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle6);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR INDX
    opMatrix[0x41].pneumonic = "EOR";
    opMatrix[0x41].addressing_mode = INDX;
    opMatrix[0x41].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ZP
    opMatrix[0x45].pneumonic = "EOR";
    opMatrix[0x45].addressing_mode = ZP;
    opMatrix[0x45].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x45].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x45].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR IMM
    opMatrix[0x49].pneumonic = "EOR";
    opMatrix[0x49].addressing_mode = IMM;
    opMatrix[0x49].cycle_op_list.push_back(&CPU::EOR_IMM_cycle2);
    opMatrix[0x49].cycle_op_list.push_back(&CPU::fetch_opcode);
    //JMP ABS
    opMatrix[0x4C].pneumonic = "JMP";
    opMatrix[0x4C].addressing_mode = ABS;
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::JMP_ABS_cycle3);
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ABS
    opMatrix[0x4D].pneumonic = "EOR";
    opMatrix[0x4D].addressing_mode = ABS;
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR INDY
    opMatrix[0x51].pneumonic = "EOR";
    opMatrix[0x51].addressing_mode = INDY;
    opMatrix[0x51].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ZP,X
    opMatrix[0x55].pneumonic = "EOR";
    opMatrix[0x55].addressing_mode = ZPX;
    opMatrix[0x55].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x55].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x55].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x55].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLI
    opMatrix[0x58].pneumonic = "CLI";
    opMatrix[0x58].addressing_mode = IMP;
    opMatrix[0x58].cycle_op_list.push_back(&CPU::CLI_cycle2);
    opMatrix[0x58].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ABS,Y
    opMatrix[0x59].pneumonic = "EOR";
    opMatrix[0x59].addressing_mode = ABSY;
    opMatrix[0x59].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ABS,X
    opMatrix[0x5D].pneumonic = "EOR";
    opMatrix[0x5D].addressing_mode = ABSX;
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SEI
    opMatrix[0x78].pneumonic = "SEI";
    opMatrix[0x78].addressing_mode = IMP;
    opMatrix[0x78].cycle_op_list.push_back(&CPU::SEI_cycle2);
    opMatrix[0x78].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA INDX
    opMatrix[0x81].pneumonic = "STA";
    opMatrix[0x81].addressing_mode = INDX;
    opMatrix[0x81].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STY ZP
    opMatrix[0x84].pneumonic = "STY";
    opMatrix[0x84].addressing_mode = ZP;
    opMatrix[0x84].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x84].cycle_op_list.push_back(&CPU::store_Y);
    opMatrix[0x84].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ZP
    opMatrix[0x85].pneumonic = "STA";
    opMatrix[0x85].addressing_mode = ZP;
    opMatrix[0x85].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x85].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x85].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STX ZP
    opMatrix[0x86].pneumonic = "STX";
    opMatrix[0x86].addressing_mode = ZP;
    opMatrix[0x86].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x86].cycle_op_list.push_back(&CPU::store_X);
    opMatrix[0x86].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TXA
    opMatrix[0x8A].pneumonic = "TXA";
    opMatrix[0x8A].addressing_mode = IMP;
    opMatrix[0x8A].cycle_op_list.push_back(&CPU::TXA_cycle2);
    opMatrix[0x8A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STY ABS
    opMatrix[0x8C].pneumonic = "STY";
    opMatrix[0x8C].addressing_mode = ABS;
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::store_Y);
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ABS
    opMatrix[0x8D].pneumonic = "STA";
    opMatrix[0x8D].addressing_mode = ABS;
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STX ABS
    opMatrix[0x8E].pneumonic = "STX";
    opMatrix[0x8E].addressing_mode = ABS;
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::store_X);
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA (Indirect), Y
    opMatrix[0x91].pneumonic = "STA";
    opMatrix[0x91].addressing_mode = INDY;
    opMatrix[0x91].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::write_IND_Y_cycle4);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STY ZPX
    opMatrix[0x94].pneumonic = "STY";
    opMatrix[0x94].addressing_mode = ZPX;
    opMatrix[0x94].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x94].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x94].cycle_op_list.push_back(&CPU::store_Y);
    opMatrix[0x94].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ZPX
    opMatrix[0x95].pneumonic = "STA";
    opMatrix[0x95].addressing_mode = ZPX;
    opMatrix[0x95].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x95].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x95].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x95].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STX ZPY
    opMatrix[0x96].pneumonic = "STX";
    opMatrix[0x96].addressing_mode = ZPY;
    opMatrix[0x96].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x96].cycle_op_list.push_back(&CPU::ZP_Y_cycle3);
    opMatrix[0x96].cycle_op_list.push_back(&CPU::store_X);
    opMatrix[0x96].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TYA
    opMatrix[0x98].pneumonic = "TYA";
    opMatrix[0x98].addressing_mode = IMP;
    opMatrix[0x98].cycle_op_list.push_back(&CPU::TYA_cycle2);
    opMatrix[0x98].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ABS,Y
    opMatrix[0x99].pneumonic = "STA";
    opMatrix[0x99].addressing_mode = ABSY;
    opMatrix[0x99].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::write_ABS_Y_cycle3);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TXS
    opMatrix[0x9A].pneumonic = "TXS";
    opMatrix[0x9A].addressing_mode = IMP;
    opMatrix[0x9A].cycle_op_list.push_back(&CPU::TXS_cycle2);
    opMatrix[0x9A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ABS,X
    opMatrix[0x9D].pneumonic = "STA";
    opMatrix[0x9D].addressing_mode = ABSX;
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY IMM
    opMatrix[0xA0].pneumonic = "LDY";
    opMatrix[0xA0].addressing_mode = IMM;
    opMatrix[0xA0].cycle_op_list.push_back(&CPU::LDY_IMM_cycle2);
    opMatrix[0xA0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA IND,X
    opMatrix[0xA1].pneumonic = "LDA";
    opMatrix[0xA1].addressing_mode = INDX;
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY IMM
    opMatrix[0xA2].pneumonic = "LDX";
    opMatrix[0xA2].addressing_mode = IMM;
    opMatrix[0xA2].cycle_op_list.push_back(&CPU::LDX_IMM_cycle2);
    opMatrix[0xA2].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ZP
    opMatrix[0xA4].pneumonic = "LDY";
    opMatrix[0xA4].addressing_mode = ZP;
    opMatrix[0xA4].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA4].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xA4].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ZP
    opMatrix[0xA5].pneumonic = "LDA";
    opMatrix[0xA5].addressing_mode = ZP;
    opMatrix[0xA5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA5].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xA5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ZP
    opMatrix[0xA6].pneumonic = "LDX";
    opMatrix[0xA6].addressing_mode = ZP;
    opMatrix[0xA6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA6].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xA6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TAY
    opMatrix[0xA8].pneumonic = "TAY";
    opMatrix[0xA8].addressing_mode = IMP;
    opMatrix[0xA8].cycle_op_list.push_back(&CPU::TAY_cycle2);
    opMatrix[0xA8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA IMM
    opMatrix[0xA9].pneumonic = "LDA";
    opMatrix[0xA9].addressing_mode = IMM;
    opMatrix[0xA9].cycle_op_list.push_back(&CPU::LDA_IMM_cycle2);
    opMatrix[0xA9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TAX
    opMatrix[0xAA].pneumonic = "TAX";
    opMatrix[0xAA].addressing_mode = IMP;
    opMatrix[0xAA].cycle_op_list.push_back(&CPU::TAX_cycle2);
    opMatrix[0xAA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ABS
    opMatrix[0xAC].pneumonic = "LDY";
    opMatrix[0xAC].addressing_mode = ABS;
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ABS
    opMatrix[0xAD].pneumonic = "LDA";
    opMatrix[0xAD].addressing_mode = ABS;
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ABS
    opMatrix[0xAE].pneumonic = "LDX";
    opMatrix[0xAE].addressing_mode = ABS;
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA INDY
    opMatrix[0xB1].pneumonic = "LDA";
    opMatrix[0xB1].addressing_mode = INDY;
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ZP,X
    opMatrix[0xB4].pneumonic = "LDY";
    opMatrix[0xB4].addressing_mode = ZPX;
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ZP,X
    opMatrix[0xB5].pneumonic = "LDA";
    opMatrix[0xB5].addressing_mode = ZPX;
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ZP,Y
    opMatrix[0xB6].pneumonic = "LDX";
    opMatrix[0xB6].addressing_mode = ZPY;
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::ZP_Y_cycle3);
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLV
    opMatrix[0xB8].pneumonic = "CLV";
    opMatrix[0xB8].addressing_mode = IMP;
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::CLV_cycle2);
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ABS,Y
    opMatrix[0xB9].pneumonic = "LDA";
    opMatrix[0xB9].addressing_mode = ABSY;
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TSX
    opMatrix[0xBA].pneumonic = "TSX";
    opMatrix[0xBA].addressing_mode = IMP;
    opMatrix[0xBA].cycle_op_list.push_back(&CPU::TSX_cycle2);
    opMatrix[0xBA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ABS,X
    opMatrix[0xBC].pneumonic = "LDY";
    opMatrix[0xBC].addressing_mode = ABSX;
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ABS,X
    opMatrix[0xBD].pneumonic = "LDA";
    opMatrix[0xBD].addressing_mode = ABSX;
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ABS,Y
    opMatrix[0xBE].pneumonic = "LDX";
    opMatrix[0xBE].addressing_mode = ABSY;
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLD
    opMatrix[0xD8].pneumonic = "CLD";
    opMatrix[0xD8].addressing_mode = IMP;
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::CLD_cycle2);
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SED
    opMatrix[0xFD].pneumonic = "SED";
    opMatrix[0xFD].addressing_mode = IMP;
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::SED_cycle2);
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::fetch_opcode);

    /** 3 interrupt sequences **/
    //Note: these are not really opcodes/instruction, they are just stored here for programming convenience
    //RESET
    opMatrix[RESET_opcode].pneumonic = "RESET";
    opMatrix[RESET_opcode].addressing_mode = INTERRUPT;
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::waste_cycle);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::fetch_RESET_vector_LSB);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::fetch_RESET_vector_MSB);
    opMatrix[RESET_opcode].cycle_op_list.push_back(&CPU::fetch_opcode);
    //NMI
    opMatrix[NMI_opcode].pneumonic = "NMI";
    opMatrix[NMI_opcode].addressing_mode = INTERRUPT;
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle1);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle2);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle3);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle4);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle5);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::NMI_cycle6);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::NMI_cycle7);
    opMatrix[NMI_opcode].cycle_op_list.push_back(&CPU::fetch_opcode);
    //IRQ
    opMatrix[IRQ_opcode].pneumonic = "IRQ";
    opMatrix[IRQ_opcode].addressing_mode = INTERRUPT;
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle1);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle2);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle3);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle4);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::interrupt_seq_cycle5);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::IRQ_cycle6);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::IRQ_cycle7);
    opMatrix[IRQ_opcode].cycle_op_list.push_back(&CPU::fetch_opcode);
}

CPU::~CPU() {
    //TODO
}

void CPU::setStatusReg(bool set, Flags flag) {
    if (set)
        status_reg |= (1 << flag);
    else
        status_reg &= ~(1 << flag);
}

bool CPU::getStatusReg(Flags flag)  {
    return (status_reg & (1 << flag));
}

void CPU::connect2Bus(SystemBus *bus_ptr) {
    system_bus_ptr = bus_ptr;
}

uint8_t CPU::read(uint16_t address) {
    return system_bus_ptr->read(address);
}

void CPU::write(uint16_t address, uint8_t data) {
    system_bus_ptr->write(address, data);
}

/** Helper functions for logging and stepMode **/
void CPU::cpu_dump() {
    //Output the opcode or interrupt currently being executed
    std::string pneumonic = getPneumonic(opcode);
    std::string addressingMode = addressingModeToString(getAddressingMode(opcode));
    std::cout << "Opcode: " << pneumonic << " " << addressingMode << "\n";
    //Output PC
    std::cout << "PC: $"
              << std::hex
              << std::uppercase
              << std::setw(4)
              << std::setfill('0')
              << (int)PC
              << "\n";
    //Output status register
    std::cout << "P: ";
    if (getStatusReg(N)) std::cout << "N ";
    else std::cout << "n ";
    if (getStatusReg(V)) std::cout << "V ";
    else std::cout << "v ";
    std::cout << "- ";
    if (getStatusReg(B)) std::cout << "B ";
    else std::cout << "b ";
    if (getStatusReg(D)) std::cout << "D ";
    else std::cout << "d ";
    if (getStatusReg(I)) std::cout << "I ";
    else std::cout << "i ";
    if (getStatusReg(Z)) std::cout << "Z ";
    else std::cout << "z ";
    if (getStatusReg(C)) std::cout << "C\n";
    else std::cout << "c\n";
    //Output A
    std::cout << "A: $"
          << std::hex
          << std::setw(2)
          << std::setfill('0')
          << (int)A
          << "\n";

    std::cout << "X: $"
              << std::hex
              << std::setw(2)
              << std::setfill('0')
              << (int)X
              << "\n";

    std::cout << "Y: $"
              << std::hex
              << std::setw(2)
              << std::setfill('0')
              << (int)Y
              << "\n";

    std::cout << "SP: $"
              << std::hex
              << std::setw(2)
              << std::setfill('0')
              << (int)SP
              << "\n";

    //Reset output format to decimal
    std::cout << std::dec;
}

std::string CPU::addressingModeToString(AddressingMode addressing_mode) {
    switch (addressing_mode) {
        case Accum: return "Accumulator";
        case IMM:   return "Immediate";
        case ABS:   return "Absolute";
        case ZP:    return "Zero Page";
        case ZPX:   return "Zero Page, X";
        case ZPY:   return "Zero Page, Y";
        case ABSX:  return "Absolute, X";
        case ABSY:  return "Absolute, Y";
        case IMP:   return "Implied";
        case REL:   return "Relative";
        case INDX:  return "Indexed Indirect (Indirect, X)";
        case INDY:  return "Indirect Indexed (Indirect), Y";
        case IND:   return "Indirect";
        case INTERRUPT: return "interrupt sequence";
        default:    return "Unknown";
    }
}

/** Helper functions to access opMatrix lookup table**/

std::string CPU::getPneumonic(uint16_t opcode) {
    return opMatrix[opcode].pneumonic;
}

AddressingMode CPU::getAddressingMode(uint16_t opcode) {
    return opMatrix[opcode].addressing_mode;
}

CPU::cycle_operation CPU::getNextFunctionPtr(uint16_t opcode) {
    return opMatrix[opcode].cycle_op_list[curr_micro_op];
}

size_t CPU::getListSize(uint16_t opcode) {
    return opMatrix[opcode].cycle_op_list.size();
}

/****/

void CPU::fetch_opcode() {
    opcode = read(PC);
    PC++;

    if (overlap_op1 != nullptr)
        (this->*overlap_op1)();

    interrupt_poll_cycle = false;
    curr_micro_op = 0;
}

void CPU::dummy_read() {
    read(PC);
}

void CPU::dummy_read(uint16_t address) {
    read(address);
}

void CPU::waste_cycle() {
    setStatusReg(true, I);
    curr_micro_op++;
    interrupt_poll_cycle = false;
}
/****/

/** Addressing Modes **/
void CPU::fetch_adl_cycle2() {
    addr1 = static_cast<uint16_t>(read(PC++));
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();
    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::fetch_adh_cycle3() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8);
    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::ZP_X_cycle3() {
    dummy_read(addr1);
    addr1 = (addr1 + X) & 0x00FF; //8 bit wrapping
    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::ZP_Y_cycle3() {
    dummy_read(addr1);
    addr1 = (addr1 + Y) & 0x00FF;
    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::read_ABS_X_cycle3() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8); //Fetch adh, combine with adl
    uint16_t old_addr1 = addr1;
    addr1 += X;

    if ((old_addr1 & 0x0100) == (addr1 & 0x0100)) //page crossing did not occur
        curr_micro_op++; //Extra incrementation to skip the dummy read that occurs in the extra cycle that occurs if there IS a page cross

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::read_ABS_Y_cycle3() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8); //Fetch adh, combine with adl
    uint16_t old_addr1 = addr1;
    addr1 += Y;

    if ((old_addr1 & 0x0100) == (addr1 & 0x0100)) //page crossing did not occur
        curr_micro_op++; //Extra incrementation to skip the dummy read that occurs in the extra cycle that occurs if there IS a page cross

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::write_ABS_X_cycle3() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8); //Fetch adh, combine with adl
    uint16_t old_addr1 = addr1;
    addr1 += X;

    if ((old_addr1 & 0x0100) != (addr1 & 0x0100)) //page crossed
        page_crossed = true;

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::write_ABS_Y_cycle3() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8); //Fetch adh, combine with adl
    uint16_t old_addr1 = addr1;
    addr1 += Y;

    if ((old_addr1 & 0x0100) != (addr1 & 0x0100)) //page crossed
        page_crossed = true;

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::read_page_crossed_cycle() {
    dummy_read(addr1 - 0x0100); //To be accurate, the 6502 would not have added the carry bit to adh yet during this dummy read which is why we subtract 0x0100

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::write_page_crossed_cycle() {
    if (page_crossed)
        dummy_read(addr1 - 0x0100);
    else
        dummy_read(addr1);

    page_crossed = false; //reset
    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::IND_X_cycle4() {
    addr2 = static_cast<uint16_t>(read(addr1)); //Fetch adl

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::IND_X_cycle5() {
    addr2 |= (static_cast<uint16_t>(read(addr1+1)) << 8); //Fetch adh
    addr1 = addr2; //The next cycle (for whatever instr is using IND, X addressing) uses addr1

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::IND_Y_cycle3() {
    addr2 = static_cast<uint16_t>(read(addr1)); //gives lsb of BASE address

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::read_IND_Y_cycle4() {
    addr2 |= (static_cast<uint16_t>(read(addr1+1)) << 8); //fetch msb of base address
    uint16_t old_addr2 = addr2;
    addr2 += Y;

    if ((old_addr2 & 0x0100) == (addr2 & 0x0100)) //page crossing did not occur
        curr_micro_op++; //Extra incrementation to skip the dummy read that occurs in the extra cycle that occurs

    addr1 = addr2;

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::write_IND_Y_cycle4() {
    addr2 |= (static_cast<uint16_t>(read(addr1+1)) << 8); //fetch msb of base address
    uint16_t old_addr2 = addr2;
    addr2 += Y;

    if ((old_addr2 & 0x0100) != (addr2 & 0x0100)) //page crossed
        page_crossed = true;

    addr1 = addr2;

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::RMW_read_cycle() {
    working_data = read(addr1);

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::RMW_set_Z_N_C_write_cycle() {
    write(addr1, ALU_result);

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);

    //Set carry to bit 7 of original value of A
    if ((working_data & 0x80) == 0x00)
        setStatusReg(false, C);
    else
        setStatusReg(true, C);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;

    curr_micro_op++;
    interrupt_poll_cycle = true;
}

/** Data Movement (access) **/
void CPU::LDA_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::LDA_fetch_data() {
    working_data = read(addr1);

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_A() {
    A = working_data;

    //Set Zero flag
    if (working_data == 0x00)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    //Set Negative flag
    if ((working_data & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

void CPU::LDX_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::LDX_fetch_data() {
    working_data = read(addr1);

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_X() {
    X = working_data;

    //Set Zero flag
    if (working_data == 0x00)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    //Set Negative flag
    if ((working_data & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

void CPU::LDY_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = &CPU::load_Y;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::LDY_fetch_data() {
    working_data = read(addr1);

    overlap_op1 = &CPU::load_Y;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_Y() {
    Y = working_data;

    //Set Zero flag
    if (working_data == 0x00)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    //Set Negative flag
    if ((working_data & 0x80) != 0x00)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

/** Data Movement (store ops) **/
void CPU::store_A() {
    write(addr1, A);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::store_X() {
    write(addr1, X);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::store_Y() {
    write(addr1, Y);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

/** Data Movement (transfer) **/
void CPU::TAX_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = A;

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TXA_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = X;

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TAY_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = A;

    overlap_op1 = &CPU::load_Y;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TYA_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = Y;

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TSX_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = SP;

    overlap_op1 = &CPU::load_X;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::TXS_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    //Note: working_data is only modified after we finish overlap_op2 (if there is one), b/c overlap_op2 could in theory need to use a value it set in working_data (example: ADC Abs)
    working_data = X;

    overlap_op1 = &CPU::load_SP;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::load_SP() {
    SP = working_data;
    //Don't set Z and N flags
}

/** Shift instructions **/
void CPU::ASL_Accum_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = A;

    overlap_op1 = &CPU::ASL;
    overlap_op2 = &CPU::store_ALU2A_set_Z_N_C;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::ASL_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data << 1;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::ASL() {
    ALU_result = A << 1;
}

void CPU::store_ALU2A_set_Z_N_C() {
    A = ALU_result;

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);

    //Set carry to bit 7 of original value of A
    if ((working_data & 0x80) == 0x00)
        setStatusReg(false, C);
    else
        setStatusReg(true, C);
}



/** Bitwise instructions **/
void CPU::AND_IMM_cycle2() {
    working_data = read(PC++);

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::AND;
    overlap_op2 = &CPU::store_ALU2A;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::AND_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = &CPU::AND;
    overlap_op2 = &CPU::store_ALU2A;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::OR_IMM_cycle2() {
    working_data = read(PC++);

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::OR;
    overlap_op2 = &CPU::store_ALU2A_set_Z_N;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::OR_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = &CPU::OR;
    overlap_op2 = &CPU::store_ALU2A_set_Z_N;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::EOR_IMM_cycle2() {
    working_data = read(PC++);

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::EOR;
    overlap_op2 = &CPU::store_ALU2A_set_Z_N;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::EOR_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = &CPU::EOR;
    overlap_op2 = &CPU::store_ALU2A_set_Z_N;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::BIT_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = &CPU::BIT;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::AND() {
    ALU_result = A & working_data;

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

void CPU::OR() {
    ALU_result = A | working_data;
}

void CPU::EOR() {
    ALU_result = A ^ working_data;
}

void CPU::BIT() {
    ALU_result = A & working_data;

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    setStatusReg((working_data & 0x80), N);
    setStatusReg((working_data & 0x40), V);
}


void CPU::store_ALU2A() {
    A = ALU_result;
}

void CPU::store_ALU2A_set_Z_N() {
    A = ALU_result;

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}


/** FLAG instructions **/
void CPU::CLC_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_carry;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::clear_carry() {
    setStatusReg(false, C);
}

void CPU::CLI_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_interrupt_disable;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::clear_interrupt_disable() {
    setStatusReg(false, I);
}

void CPU::CLV_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_overflow;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::clear_overflow() {
    setStatusReg(false, V);
}

void CPU::CLD_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::clear_decimal;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::clear_decimal() {
    setStatusReg(false, D);
}

void CPU::SEC_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::set_carry;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}


void CPU::set_carry() {
    setStatusReg(true, C);
}

void CPU::SEI_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::set_interrupt_disable;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::set_interrupt_disable() {
    setStatusReg(true, I);
}

void CPU::SED_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    overlap_op1 = &CPU::set_decimal;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::set_decimal() {
    setStatusReg(true, D);
}

/** Jump Instructions **/
void CPU::JMP_ABS_cycle3() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8);
    PC = addr1;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

/** Stack **/
void CPU::push(uint16_t& address, uint8_t val) {
    write(address, val);
    address--;
}

uint8_t CPU::pull(uint16_t& address) {
    return read(address++);
}

/** Interrupts **/
void CPU::fetch_RESET_vector_LSB() {
    PC = static_cast<uint16_t>(read(0xFFFC));

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::fetch_RESET_vector_MSB() {
    uint16_t MSB = static_cast<uint16_t>(read(0xFFFD));
    PC |= (MSB << 8);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::interrupt_seq_cycle1() {
    std::cout <<"HERE\n";
    dummy_read();

    if (overlap_op1 != nullptr)
        (this->*overlap_op1)();

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::interrupt_seq_cycle2() {
    std::cout <<"HERE\n";
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::interrupt_seq_cycle3() {
    auto pch = static_cast<uint8_t>((PC & 0xFF00) >> 8);
    addr1 = STACK_START + SP;
    push(addr1, pch); //SP-- (really addr1 internally for now)

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::interrupt_seq_cycle4() {
    auto pcl = static_cast<uint8_t>(PC & 0x00FF);
    push(addr1, pcl); //SP-- (really addr1 internally for now)

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::interrupt_seq_cycle5() {
    push(addr1, status_reg); //SP-- (really addr1 internally for now)

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::IRQ_cycle6() {
    SP = addr1; //Store SP
    addr1 = static_cast<uint16_t>(read(0xFFFE));

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::IRQ_cycle7() {
    auto MSB = static_cast<uint16_t>(read(0xFFFF));
    addr1 |= (MSB << 8);
    PC = addr1;

    setStatusReg(true, I);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::NMI_cycle6() {
    SP = addr1; //Store SP
    addr1 = static_cast<uint16_t>(read(0xFFFA));

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::NMI_cycle7() {
    auto MSB = static_cast<uint16_t>(read(0xFFFB));
    addr1 |= (MSB << 8);
    PC = addr1;

    setStatusReg(true, I);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::RTI_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::RTI_cycle3() {
    dummy_read(STACK_START + SP);
    addr1 = (STACK_START + SP + 1); //SP++

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::RTI_cycle4() {
    working_data = pull(addr1); //SP++, pull P

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::RTI_cycle5() {
    addr2 = static_cast<uint16_t>(pull(addr1)); //SP++, pull PCL
    status_reg = working_data; //Restore P actually happens this cycle

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::RTI_cycle6() {
    SP = addr1; //Store SP (before this cycles increment in pull call)
    addr2 |= static_cast<uint16_t>(pull(addr1)) << 8; //pull PCH
    PC = addr2;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;

    curr_micro_op++;
    interrupt_poll_cycle = true;
}

