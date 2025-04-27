
#include "CPU.hpp"

CPU::CPU() {
    //Initialize the opcode matrix
    opMatrix.resize(16*16 + 3); //3 is for the interrupt sequences
    //BRK
    opMatrix[0x00].mnemonic = "BRK";
    opMatrix[0x00].addressing_mode = IMP;
    opMatrix[0x00].cycle_op_list.push_back(&CPU::BRK_cycle2);
    opMatrix[0x00].cycle_op_list.push_back(&CPU::interrupt_seq_cycle3);
    opMatrix[0x00].cycle_op_list.push_back(&CPU::interrupt_seq_cycle4);
    opMatrix[0x00].cycle_op_list.push_back(&CPU::BRK_cycle5);
    opMatrix[0x00].cycle_op_list.push_back(&CPU::IRQ_cycle6);
    opMatrix[0x00].cycle_op_list.push_back(&CPU::IRQ_cycle7);
    opMatrix[0x00].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR INDX
    opMatrix[0x01].mnemonic = "ORA";
    opMatrix[0x01].addressing_mode = INDX;
    opMatrix[0x01].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x01].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ZP
    opMatrix[0x05].mnemonic = "ORA";
    opMatrix[0x05].addressing_mode = ZP;
    opMatrix[0x05].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x05].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x05].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ZP
    opMatrix[0x06].mnemonic = "ASL";
    opMatrix[0x06].addressing_mode = ZP;
    opMatrix[0x06].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x06].cycle_op_list.push_back(&CPU::fetch_opcode);
    //PHP
    opMatrix[0x08].mnemonic = "PHP";
    opMatrix[0x08].addressing_mode = IMP;
    opMatrix[0x08].cycle_op_list.push_back(&CPU::PHP_cycle2);
    opMatrix[0x08].cycle_op_list.push_back(&CPU::push_final_cycle);
    opMatrix[0x08].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR IMM
    opMatrix[0x09].mnemonic = "ORA";
    opMatrix[0x09].addressing_mode = IMM;
    opMatrix[0x09].cycle_op_list.push_back(&CPU::OR_IMM_cycle2);
    opMatrix[0x09].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL Accum
    opMatrix[0x0A].mnemonic = "ASL";
    opMatrix[0x0A].addressing_mode = Accum;
    opMatrix[0x0A].cycle_op_list.push_back(&CPU::ASL_Accum_cycle2);
    opMatrix[0x0A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ABS
    opMatrix[0x0D].mnemonic = "ORA";
    opMatrix[0x0D].addressing_mode = ABS;
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x0D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ABS
    opMatrix[0x0E].mnemonic = "ASL";
    opMatrix[0x0E].addressing_mode = ABS;
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x0E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BPL
    opMatrix[0x10].mnemonic = "BPL";
    opMatrix[0x10].addressing_mode = REL;
    opMatrix[0x10].cycle_op_list.push_back(&CPU::BPL_cycle2);
    opMatrix[0x10].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0x10].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0x10].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR INDY
    opMatrix[0x11].mnemonic = "ORA";
    opMatrix[0x11].addressing_mode = INDY;
    opMatrix[0x11].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x11].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ZP,X
    opMatrix[0x15].mnemonic = "ORA";
    opMatrix[0x15].addressing_mode = ZPX;
    opMatrix[0x15].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x15].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x15].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x15].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ZP,X
    opMatrix[0x16].mnemonic = "ASL";
    opMatrix[0x16].addressing_mode = ZPX;
    opMatrix[0x16].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x16].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLC
    opMatrix[0x18].mnemonic = "CLC";
    opMatrix[0x18].addressing_mode = IMP;
    opMatrix[0x18].cycle_op_list.push_back(&CPU::CLC_cycle2);
    opMatrix[0x18].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ABS,Y
    opMatrix[0x19].mnemonic = "ORA";
    opMatrix[0x19].addressing_mode = ABSY;
    opMatrix[0x19].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x19].cycle_op_list.push_back(&CPU::fetch_opcode);
    //OR ABS,X
    opMatrix[0x1D].mnemonic = "ORA";
    opMatrix[0x1D].addressing_mode = ABSX;
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::OR_final_cycle);
    opMatrix[0x1D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ASL ABS,X
    opMatrix[0x1E].mnemonic = "ASL";
    opMatrix[0x1E].addressing_mode = ABSX;
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::ASL_dummy_write);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x1E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //JSR
    opMatrix[0x20].mnemonic = "JSR";
    opMatrix[0x20].addressing_mode = ABS;
    opMatrix[0x20].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x20].cycle_op_list.push_back(&CPU::JSR_cycle3);
    opMatrix[0x20].cycle_op_list.push_back(&CPU::JSR_cycle4);
    opMatrix[0x20].cycle_op_list.push_back(&CPU::JSR_cycle5);
    opMatrix[0x20].cycle_op_list.push_back(&CPU::JSR_cycle6);
    opMatrix[0x20].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND INDX
    opMatrix[0x21].mnemonic = "AND";
    opMatrix[0x21].addressing_mode = INDX;
    opMatrix[0x21].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x21].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BIT ZP
    opMatrix[0x24].mnemonic = "BIT";
    opMatrix[0x24].addressing_mode = ZP;
    opMatrix[0x24].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x24].cycle_op_list.push_back(&CPU::BIT_final_cycle);
    opMatrix[0x24].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ZP
    opMatrix[0x25].mnemonic = "AND";
    opMatrix[0x25].addressing_mode = ZP;
    opMatrix[0x25].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x25].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x25].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROL ZP
    opMatrix[0x26].mnemonic = "ROL";
    opMatrix[0x26].addressing_mode = ZP;
    opMatrix[0x26].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x26].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x26].cycle_op_list.push_back(&CPU::ROL_dummy_write);
    opMatrix[0x26].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x26].cycle_op_list.push_back(&CPU::fetch_opcode);
    //PLP
    opMatrix[0x28].mnemonic = "PLP";
    opMatrix[0x28].addressing_mode = IMP;
    opMatrix[0x28].cycle_op_list.push_back(&CPU::pull_cycle2);
    opMatrix[0x28].cycle_op_list.push_back(&CPU::pull_cycle3);
    opMatrix[0x28].cycle_op_list.push_back(&CPU::PLP_final_cycle);
    opMatrix[0x28].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND IMM
    opMatrix[0x29].mnemonic = "AND";
    opMatrix[0x29].addressing_mode = IMM;
    opMatrix[0x29].cycle_op_list.push_back(&CPU::AND_IMM_cycle2);
    opMatrix[0x29].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROL Accum
    opMatrix[0x2A].mnemonic = "ROL";
    opMatrix[0x2A].addressing_mode = Accum;
    opMatrix[0x2A].cycle_op_list.push_back(&CPU::ROL_Accum_cycle2);
    opMatrix[0x2A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BIT ABS
    opMatrix[0x2C].mnemonic = "BIT";
    opMatrix[0x2C].addressing_mode = ABS;
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::BIT_final_cycle);
    opMatrix[0x2C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ABS
    opMatrix[0x2D].mnemonic = "AND";
    opMatrix[0x2D].addressing_mode = ABS;
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x2D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROL ABS
    opMatrix[0x2E].mnemonic = "ROL";
    opMatrix[0x2E].addressing_mode = ABS;
    opMatrix[0x2E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x2E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x2E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x2E].cycle_op_list.push_back(&CPU::ROL_dummy_write);
    opMatrix[0x2E].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x2E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BMI
    opMatrix[0x30].mnemonic = "BMI";
    opMatrix[0x30].addressing_mode = REL;
    opMatrix[0x30].cycle_op_list.push_back(&CPU::BMI_cycle2);
    opMatrix[0x30].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0x30].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0x30].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND INDY
    opMatrix[0x31].mnemonic = "AND";
    opMatrix[0x31].addressing_mode = INDY;
    opMatrix[0x31].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x31].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ZP,X
    opMatrix[0x35].mnemonic = "AND";
    opMatrix[0x35].addressing_mode = ZPX;
    opMatrix[0x35].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x35].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROL ZP,X
    opMatrix[0x36].mnemonic = "ROL";
    opMatrix[0x36].addressing_mode = ZPX;
    opMatrix[0x36].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x36].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x36].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x36].cycle_op_list.push_back(&CPU::ROL_dummy_write);
    opMatrix[0x36].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x36].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SEC
    opMatrix[0x38].mnemonic = "SEC";
    opMatrix[0x38].addressing_mode = IMP;
    opMatrix[0x38].cycle_op_list.push_back(&CPU::SEC_cycle2);
    opMatrix[0x38].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ABS,Y
    opMatrix[0x39].mnemonic = "AND";
    opMatrix[0x39].addressing_mode = ABSY;
    opMatrix[0x39].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x39].cycle_op_list.push_back(&CPU::fetch_opcode);
    //AND ABS,X
    opMatrix[0x3D].mnemonic = "AND";
    opMatrix[0x3D].addressing_mode = ABSX;
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::AND_final_cycle);
    opMatrix[0x3D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROL ABS,X
    opMatrix[0x3E].mnemonic = "ROL";
    opMatrix[0x3E].addressing_mode = ABSX;
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::ROL_dummy_write);
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::ASL_write_cycle);
    opMatrix[0x3E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //RTI
    opMatrix[0x40].mnemonic = "RTI";
    opMatrix[0x40].addressing_mode = IMP;
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle2);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle3);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle4);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle5);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::RTI_cycle6);
    opMatrix[0x40].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR INDX
    opMatrix[0x41].mnemonic = "EOR";
    opMatrix[0x41].addressing_mode = INDX;
    opMatrix[0x41].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x41].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ZP
    opMatrix[0x45].mnemonic = "EOR";
    opMatrix[0x45].addressing_mode = ZP;
    opMatrix[0x45].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x45].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x45].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LSR ZP
    opMatrix[0x46].mnemonic = "LSR";
    opMatrix[0x46].addressing_mode = ZP;
    opMatrix[0x46].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x46].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x46].cycle_op_list.push_back(&CPU::LSR_dummy_write);
    opMatrix[0x46].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x46].cycle_op_list.push_back(&CPU::fetch_opcode);
    //PHA
    opMatrix[0x48].mnemonic = "PHA";
    opMatrix[0x48].addressing_mode = IMP;
    opMatrix[0x48].cycle_op_list.push_back(&CPU::PHA_cycle2);
    opMatrix[0x48].cycle_op_list.push_back(&CPU::push_final_cycle);
    opMatrix[0x48].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR IMM
    opMatrix[0x49].mnemonic = "EOR";
    opMatrix[0x49].addressing_mode = IMM;
    opMatrix[0x49].cycle_op_list.push_back(&CPU::EOR_IMM_cycle2);
    opMatrix[0x49].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LSR Accum
    opMatrix[0x4A].mnemonic = "LSR";
    opMatrix[0x4A].addressing_mode = Accum;
    opMatrix[0x4A].cycle_op_list.push_back(&CPU::LSR_Accum_cycle2);
    opMatrix[0x4A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //JMP ABS
    opMatrix[0x4C].mnemonic = "JMP";
    opMatrix[0x4C].addressing_mode = ABS;
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::JMP_ABS_cycle3);
    opMatrix[0x4C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ABS
    opMatrix[0x4D].mnemonic = "EOR";
    opMatrix[0x4D].addressing_mode = ABS;
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x4D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LSR ABS
    opMatrix[0x4E].mnemonic = "LSR";
    opMatrix[0x4E].addressing_mode = ABS;
    opMatrix[0x4E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x4E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x4E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x4E].cycle_op_list.push_back(&CPU::LSR_dummy_write);
    opMatrix[0x4E].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x4E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BVC
    opMatrix[0x50].mnemonic = "BVC";
    opMatrix[0x50].addressing_mode = REL;
    opMatrix[0x50].cycle_op_list.push_back(&CPU::BVC_cycle2);
    opMatrix[0x50].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0x50].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0x50].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR INDY
    opMatrix[0x51].mnemonic = "EOR";
    opMatrix[0x51].addressing_mode = INDY;
    opMatrix[0x51].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x51].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ZP,X
    opMatrix[0x55].mnemonic = "EOR";
    opMatrix[0x55].addressing_mode = ZPX;
    opMatrix[0x55].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x55].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x55].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x55].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LSR ZP,X
    opMatrix[0x56].mnemonic = "LSR";
    opMatrix[0x56].addressing_mode = ZPX;
    opMatrix[0x56].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x56].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x56].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x56].cycle_op_list.push_back(&CPU::LSR_dummy_write);
    opMatrix[0x56].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x56].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLI
    opMatrix[0x58].mnemonic = "CLI";
    opMatrix[0x58].addressing_mode = IMP;
    opMatrix[0x58].cycle_op_list.push_back(&CPU::CLI_cycle2);
    opMatrix[0x58].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ABS,Y
    opMatrix[0x59].mnemonic = "EOR";
    opMatrix[0x59].addressing_mode = ABSY;
    opMatrix[0x59].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x59].cycle_op_list.push_back(&CPU::fetch_opcode);
    //EOR ABS,X
    opMatrix[0x5D].mnemonic = "EOR";
    opMatrix[0x5D].addressing_mode = ABSX;
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::EOR_final_cycle);
    opMatrix[0x5D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LSR ABS,X
    opMatrix[0x5E].mnemonic = "LSR";
    opMatrix[0x5E].addressing_mode = ABSX;
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::LSR_dummy_write);
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x5E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //RTS
    opMatrix[0x60].mnemonic = "RTS";
    opMatrix[0x60].addressing_mode = IMP;
    opMatrix[0x60].cycle_op_list.push_back(&CPU::RTS_cycle2);
    opMatrix[0x60].cycle_op_list.push_back(&CPU::RTS_cycle3);
    opMatrix[0x60].cycle_op_list.push_back(&CPU::RTS_cycle4);
    opMatrix[0x60].cycle_op_list.push_back(&CPU::RTS_cycle5);
    opMatrix[0x60].cycle_op_list.push_back(&CPU::RTS_cycle6);
    opMatrix[0x60].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC INDX
    opMatrix[0x61].mnemonic = "ADC";
    opMatrix[0x61].addressing_mode = INDX;
    opMatrix[0x61].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x61].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x61].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x61].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x61].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x61].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC ZP
    opMatrix[0x65].mnemonic = "ADC";
    opMatrix[0x65].addressing_mode = ZP;
    opMatrix[0x65].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x65].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x65].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROR ZP
    opMatrix[0x66].mnemonic = "ROR";
    opMatrix[0x66].addressing_mode = ZP;
    opMatrix[0x66].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x66].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x66].cycle_op_list.push_back(&CPU::ROR_dummy_write);
    opMatrix[0x66].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x66].cycle_op_list.push_back(&CPU::fetch_opcode);
    //PLA
    opMatrix[0x68].mnemonic = "PLA";
    opMatrix[0x68].addressing_mode = IMP;
    opMatrix[0x68].cycle_op_list.push_back(&CPU::pull_cycle2);
    opMatrix[0x68].cycle_op_list.push_back(&CPU::pull_cycle3);
    opMatrix[0x68].cycle_op_list.push_back(&CPU::PLA_final_cycle);
    opMatrix[0x68].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC IMM
    opMatrix[0x69].mnemonic = "ADC";
    opMatrix[0x69].addressing_mode = IMM;
    opMatrix[0x69].cycle_op_list.push_back(&CPU::ADC_IMM_cycle2);
    opMatrix[0x69].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROR Accum
    opMatrix[0x6A].mnemonic = "ROR";
    opMatrix[0x6A].addressing_mode = Accum;
    opMatrix[0x6A].cycle_op_list.push_back(&CPU::ROR_Accum_cycle2);
    opMatrix[0x6A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //JMP IND
    opMatrix[0x6C].mnemonic = "JMP";
    opMatrix[0x6C].addressing_mode = IND;
    opMatrix[0x6C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x6C].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x6C].cycle_op_list.push_back(&CPU::JMP_IND_cycle4);
    opMatrix[0x6C].cycle_op_list.push_back(&CPU::JMP_IND_cycle5);
    opMatrix[0x6C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC ABS
    opMatrix[0x6D].mnemonic = "ADC";
    opMatrix[0x6D].addressing_mode = ABS;
    opMatrix[0x6D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x6D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x6D].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x6D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROR ABS
    opMatrix[0x6E].mnemonic = "ROR";
    opMatrix[0x6E].addressing_mode = ABS;
    opMatrix[0x6E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x6E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x6E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x6E].cycle_op_list.push_back(&CPU::ROR_dummy_write);
    opMatrix[0x6E].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x6E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BVS
    opMatrix[0x70].mnemonic = "BVS";
    opMatrix[0x70].addressing_mode = REL;
    opMatrix[0x70].cycle_op_list.push_back(&CPU::BVS_cycle2);
    opMatrix[0x70].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0x70].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0x70].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC INDY
    opMatrix[0x71].mnemonic = "ADC";
    opMatrix[0x71].addressing_mode = INDY;
    opMatrix[0x71].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x71].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x71].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0x71].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x71].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x71].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC ZP,X
    opMatrix[0x75].mnemonic = "ADC";
    opMatrix[0x75].addressing_mode = ZPX;
    opMatrix[0x75].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x75].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x75].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x75].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROR ZP,X
    opMatrix[0x76].mnemonic = "ROR";
    opMatrix[0x76].addressing_mode = ZPX;
    opMatrix[0x76].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x76].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x76].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x76].cycle_op_list.push_back(&CPU::ROR_dummy_write);
    opMatrix[0x76].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x76].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SEI
    opMatrix[0x78].mnemonic = "SEI";
    opMatrix[0x78].addressing_mode = IMP;
    opMatrix[0x78].cycle_op_list.push_back(&CPU::SEI_cycle2);
    opMatrix[0x78].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC ABS,Y
    opMatrix[0x79].mnemonic = "ADC";
    opMatrix[0x79].addressing_mode = ABSY;
    opMatrix[0x79].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x79].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0x79].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x79].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x79].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ADC ABS,X
    opMatrix[0x7D].mnemonic = "ADC";
    opMatrix[0x7D].addressing_mode = ABSX;
    opMatrix[0x7D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x7D].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0x7D].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0x7D].cycle_op_list.push_back(&CPU::ADC_final_cycle);
    opMatrix[0x7D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //ROR ABS,X
    opMatrix[0x7E].mnemonic = "ROR";
    opMatrix[0x7E].addressing_mode = ABSX;
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::ROR_dummy_write);
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::LSR_write_cycle);
    opMatrix[0x7E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA INDX
    opMatrix[0x81].mnemonic = "STA";
    opMatrix[0x81].addressing_mode = INDX;
    opMatrix[0x81].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x81].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STY ZP
    opMatrix[0x84].mnemonic = "STY";
    opMatrix[0x84].addressing_mode = ZP;
    opMatrix[0x84].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x84].cycle_op_list.push_back(&CPU::store_Y);
    opMatrix[0x84].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ZP
    opMatrix[0x85].mnemonic = "STA";
    opMatrix[0x85].addressing_mode = ZP;
    opMatrix[0x85].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x85].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x85].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STX ZP
    opMatrix[0x86].mnemonic = "STX";
    opMatrix[0x86].addressing_mode = ZP;
    opMatrix[0x86].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x86].cycle_op_list.push_back(&CPU::store_X);
    opMatrix[0x86].cycle_op_list.push_back(&CPU::fetch_opcode);
    //DEY
    opMatrix[0x88].mnemonic = "DEY";
    opMatrix[0x88].addressing_mode = IMP;
    opMatrix[0x88].cycle_op_list.push_back(&CPU::DEY_cycle2);
    opMatrix[0x88].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TXA
    opMatrix[0x8A].mnemonic = "TXA";
    opMatrix[0x8A].addressing_mode = IMP;
    opMatrix[0x8A].cycle_op_list.push_back(&CPU::TXA_cycle2);
    opMatrix[0x8A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STY ABS
    opMatrix[0x8C].mnemonic = "STY";
    opMatrix[0x8C].addressing_mode = ABS;
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::store_Y);
    opMatrix[0x8C].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ABS
    opMatrix[0x8D].mnemonic = "STA";
    opMatrix[0x8D].addressing_mode = ABS;
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x8D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STX ABS
    opMatrix[0x8E].mnemonic = "STX";
    opMatrix[0x8E].addressing_mode = ABS;
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::store_X);
    opMatrix[0x8E].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BCC
    opMatrix[0x90].mnemonic = "BCC";
    opMatrix[0x90].addressing_mode = REL;
    opMatrix[0x90].cycle_op_list.push_back(&CPU::BCC_cycle2);
    opMatrix[0x90].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0x90].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0x90].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA (Indirect), Y
    opMatrix[0x91].mnemonic = "STA";
    opMatrix[0x91].addressing_mode = INDY;
    opMatrix[0x91].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::write_IND_Y_cycle4);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x91].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STY ZPX
    opMatrix[0x94].mnemonic = "STY";
    opMatrix[0x94].addressing_mode = ZPX;
    opMatrix[0x94].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x94].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x94].cycle_op_list.push_back(&CPU::store_Y);
    opMatrix[0x94].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ZPX
    opMatrix[0x95].mnemonic = "STA";
    opMatrix[0x95].addressing_mode = ZPX;
    opMatrix[0x95].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x95].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0x95].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x95].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STX ZPY
    opMatrix[0x96].mnemonic = "STX";
    opMatrix[0x96].addressing_mode = ZPY;
    opMatrix[0x96].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x96].cycle_op_list.push_back(&CPU::ZP_Y_cycle3);
    opMatrix[0x96].cycle_op_list.push_back(&CPU::store_X);
    opMatrix[0x96].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TYA
    opMatrix[0x98].mnemonic = "TYA";
    opMatrix[0x98].addressing_mode = IMP;
    opMatrix[0x98].cycle_op_list.push_back(&CPU::TYA_cycle2);
    opMatrix[0x98].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ABS,Y
    opMatrix[0x99].mnemonic = "STA";
    opMatrix[0x99].addressing_mode = ABSY;
    opMatrix[0x99].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::write_ABS_Y_cycle3);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x99].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TXS
    opMatrix[0x9A].mnemonic = "TXS";
    opMatrix[0x9A].addressing_mode = IMP;
    opMatrix[0x9A].cycle_op_list.push_back(&CPU::TXS_cycle2);
    opMatrix[0x9A].cycle_op_list.push_back(&CPU::fetch_opcode);
    //STA ABS,X
    opMatrix[0x9D].mnemonic = "STA";
    opMatrix[0x9D].addressing_mode = ABSX;
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::store_A);
    opMatrix[0x9D].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY IMM
    opMatrix[0xA0].mnemonic = "LDY";
    opMatrix[0xA0].addressing_mode = IMM;
    opMatrix[0xA0].cycle_op_list.push_back(&CPU::LDY_IMM_cycle2);
    opMatrix[0xA0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA IND,X
    opMatrix[0xA1].mnemonic = "LDA";
    opMatrix[0xA1].addressing_mode = INDX;
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xA1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY IMM
    opMatrix[0xA2].mnemonic = "LDX";
    opMatrix[0xA2].addressing_mode = IMM;
    opMatrix[0xA2].cycle_op_list.push_back(&CPU::LDX_IMM_cycle2);
    opMatrix[0xA2].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ZP
    opMatrix[0xA4].mnemonic = "LDY";
    opMatrix[0xA4].addressing_mode = ZP;
    opMatrix[0xA4].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA4].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xA4].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ZP
    opMatrix[0xA5].mnemonic = "LDA";
    opMatrix[0xA5].addressing_mode = ZP;
    opMatrix[0xA5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA5].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xA5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ZP
    opMatrix[0xA6].mnemonic = "LDX";
    opMatrix[0xA6].addressing_mode = ZP;
    opMatrix[0xA6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xA6].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xA6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TAY
    opMatrix[0xA8].mnemonic = "TAY";
    opMatrix[0xA8].addressing_mode = IMP;
    opMatrix[0xA8].cycle_op_list.push_back(&CPU::TAY_cycle2);
    opMatrix[0xA8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA IMM
    opMatrix[0xA9].mnemonic = "LDA";
    opMatrix[0xA9].addressing_mode = IMM;
    opMatrix[0xA9].cycle_op_list.push_back(&CPU::LDA_IMM_cycle2);
    opMatrix[0xA9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TAX
    opMatrix[0xAA].mnemonic = "TAX";
    opMatrix[0xAA].addressing_mode = IMP;
    opMatrix[0xAA].cycle_op_list.push_back(&CPU::TAX_cycle2);
    opMatrix[0xAA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ABS
    opMatrix[0xAC].mnemonic = "LDY";
    opMatrix[0xAC].addressing_mode = ABS;
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xAC].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ABS
    opMatrix[0xAD].mnemonic = "LDA";
    opMatrix[0xAD].addressing_mode = ABS;
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xAD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ABS
    opMatrix[0xAE].mnemonic = "LDX";
    opMatrix[0xAE].addressing_mode = ABS;
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xAE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BCS
    opMatrix[0xB0].mnemonic = "BCS";
    opMatrix[0xB0].addressing_mode = REL;
    opMatrix[0xB0].cycle_op_list.push_back(&CPU::BCS_cycle2);
    opMatrix[0xB0].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0xB0].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0xB0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA INDY
    opMatrix[0xB1].mnemonic = "LDA";
    opMatrix[0xB1].addressing_mode = INDY;
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xB1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ZP,X
    opMatrix[0xB4].mnemonic = "LDY";
    opMatrix[0xB4].addressing_mode = ZPX;
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xB4].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ZP,X
    opMatrix[0xB5].mnemonic = "LDA";
    opMatrix[0xB5].addressing_mode = ZPX;
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xB5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ZP,Y
    opMatrix[0xB6].mnemonic = "LDX";
    opMatrix[0xB6].addressing_mode = ZPY;
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::ZP_Y_cycle3);
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xB6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLV
    opMatrix[0xB8].mnemonic = "CLV";
    opMatrix[0xB8].addressing_mode = IMP;
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::CLV_cycle2);
    opMatrix[0xB8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ABS,Y
    opMatrix[0xB9].mnemonic = "LDA";
    opMatrix[0xB9].addressing_mode = ABSY;
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xB9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //TSX
    opMatrix[0xBA].mnemonic = "TSX";
    opMatrix[0xBA].addressing_mode = IMP;
    opMatrix[0xBA].cycle_op_list.push_back(&CPU::TSX_cycle2);
    opMatrix[0xBA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDY ABS,X
    opMatrix[0xBC].mnemonic = "LDY";
    opMatrix[0xBC].addressing_mode = ABSX;
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::LDY_fetch_data);
    opMatrix[0xBC].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDA ABS,X
    opMatrix[0xBD].mnemonic = "LDA";
    opMatrix[0xBD].addressing_mode = ABSX;
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::LDA_fetch_data);
    opMatrix[0xBD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //LDX ABS,Y
    opMatrix[0xBE].mnemonic = "LDX";
    opMatrix[0xBE].addressing_mode = ABSY;
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::LDX_fetch_data);
    opMatrix[0xBE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CPY IMM
    opMatrix[0xC0].mnemonic = "CPY";
    opMatrix[0xC0].addressing_mode = IMM;
    opMatrix[0xC0].cycle_op_list.push_back(&CPU::CPY_IMM_cycle2);
    opMatrix[0xC0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP INDX
    opMatrix[0xC1].mnemonic = "AND";
    opMatrix[0xC1].addressing_mode = INDX;
    opMatrix[0xC1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xC1].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xC1].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0xC1].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0xC1].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xC1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CPY ZP
    opMatrix[0xC4].mnemonic = "CPY";
    opMatrix[0xC4].addressing_mode = ZP;
    opMatrix[0xC4].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xC4].cycle_op_list.push_back(&CPU::CPY_final_cycle);
    opMatrix[0xC4].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP ZP
    opMatrix[0xC5].mnemonic = "CMP";
    opMatrix[0xC5].addressing_mode = ZP;
    opMatrix[0xC5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xC5].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xC5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //DEC ZP
    opMatrix[0xC6].mnemonic = "DEC";
    opMatrix[0xC6].addressing_mode = ZP;
    opMatrix[0xC6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xC6].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xC6].cycle_op_list.push_back(&CPU::DEC_dummy_write);
    opMatrix[0xC6].cycle_op_list.push_back(&CPU::DEC_write_cycle);
    opMatrix[0xC6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //INY
    opMatrix[0xC8].mnemonic = "INY";
    opMatrix[0xC8].addressing_mode = IMP;
    opMatrix[0xC8].cycle_op_list.push_back(&CPU::INY_cycle2);
    opMatrix[0xC8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP IMM
    opMatrix[0xC9].mnemonic = "CMP";
    opMatrix[0xC9].addressing_mode = IMM;
    opMatrix[0xC9].cycle_op_list.push_back(&CPU::CMP_IMM_cycle2);
    opMatrix[0xC9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //DEX
    opMatrix[0xCA].mnemonic = "DEX";
    opMatrix[0xCA].addressing_mode = IMP;
    opMatrix[0xCA].cycle_op_list.push_back(&CPU::DEX_cycle2);
    opMatrix[0xCA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CPY ABS
    opMatrix[0xCC].mnemonic = "CPY";
    opMatrix[0xCC].addressing_mode = ABS;
    opMatrix[0xCC].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xCC].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xCC].cycle_op_list.push_back(&CPU::CPY_final_cycle);
    opMatrix[0xCC].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP ABS
    opMatrix[0xCD].mnemonic = "CMP";
    opMatrix[0xCD].addressing_mode = ABS;
    opMatrix[0xCD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xCD].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xCD].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xCD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //DEC ABS
    opMatrix[0xCE].mnemonic = "DEC";
    opMatrix[0xCE].addressing_mode = ABS;
    opMatrix[0xCE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xCE].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xCE].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xCE].cycle_op_list.push_back(&CPU::DEC_dummy_write);
    opMatrix[0xCE].cycle_op_list.push_back(&CPU::DEC_write_cycle);
    opMatrix[0xCE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BNE
    opMatrix[0xD0].mnemonic = "BNE";
    opMatrix[0xD0].addressing_mode = REL;
    opMatrix[0xD0].cycle_op_list.push_back(&CPU::BNE_cycle2);
    opMatrix[0xD0].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0xD0].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0xD0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP INDY
    opMatrix[0xD1].mnemonic = "CMP";
    opMatrix[0xD1].addressing_mode = INDY;
    opMatrix[0xD1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xD1].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0xD1].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0xD1].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xD1].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xD1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP ZP,X
    opMatrix[0xD5].mnemonic = "CMP";
    opMatrix[0xD5].addressing_mode = ZPX;
    opMatrix[0xD5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xD5].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xD5].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xD5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //DEC ZPX
    opMatrix[0xD6].mnemonic = "IDEC";
    opMatrix[0xD6].addressing_mode = ZPX;
    opMatrix[0xD6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xD6].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xD6].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xD6].cycle_op_list.push_back(&CPU::DEC_dummy_write);
    opMatrix[0xD6].cycle_op_list.push_back(&CPU::DEC_write_cycle);
    opMatrix[0xD6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CLD
    opMatrix[0xD8].mnemonic = "CLD";
    opMatrix[0xD8].addressing_mode = IMP;
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::CLD_cycle2);
    opMatrix[0xD8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP ABS,Y
    opMatrix[0xD9].mnemonic = "CMP";
    opMatrix[0xD9].addressing_mode = ABSY;
    opMatrix[0xD9].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xD9].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0xD9].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xD9].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xD9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CMP ABS,X
    opMatrix[0xDD].mnemonic = "CMP";
    opMatrix[0xDD].addressing_mode = ABSX;
    opMatrix[0xDD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xDD].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0xDD].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xDD].cycle_op_list.push_back(&CPU::CMP_final_cycle);
    opMatrix[0xDD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //DEC ABS,X
    opMatrix[0xDE].mnemonic = "DEC";
    opMatrix[0xDE].addressing_mode = ABSX;
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::DEC_dummy_write);
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::DEC_write_cycle);
    opMatrix[0xDE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CPX IMM
    opMatrix[0xE0].mnemonic = "CPX";
    opMatrix[0xE0].addressing_mode = IMM;
    opMatrix[0xE0].cycle_op_list.push_back(&CPU::CPX_IMM_cycle2);
    opMatrix[0xE0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC INDX
    opMatrix[0xE1].mnemonic = "SBC";
    opMatrix[0xE1].addressing_mode = INDX;
    opMatrix[0xE1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xE1].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xE1].cycle_op_list.push_back(&CPU::IND_X_cycle4);
    opMatrix[0xE1].cycle_op_list.push_back(&CPU::IND_X_cycle5);
    opMatrix[0xE1].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xE1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CPX ZP
    opMatrix[0xE4].mnemonic = "CPX";
    opMatrix[0xE4].addressing_mode = ZP;
    opMatrix[0xE4].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xE4].cycle_op_list.push_back(&CPU::CPX_final_cycle);
    opMatrix[0xE4].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC ZP
    opMatrix[0xE5].mnemonic = "SBC";
    opMatrix[0xE5].addressing_mode = ZP;
    opMatrix[0xE5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xE5].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xE5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //INC ZP
    opMatrix[0xE6].mnemonic = "INC";
    opMatrix[0xE6].addressing_mode = ZP;
    opMatrix[0xE6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xE6].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xE6].cycle_op_list.push_back(&CPU::INC_dummy_write);
    opMatrix[0xE6].cycle_op_list.push_back(&CPU::INC_write_cycle);
    opMatrix[0xE6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //INX
    opMatrix[0xE8].mnemonic = "INX";
    opMatrix[0xE8].addressing_mode = IMP;
    opMatrix[0xE8].cycle_op_list.push_back(&CPU::INX_cycle2);
    opMatrix[0xE8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC IMM
    opMatrix[0xE9].mnemonic = "SBC";
    opMatrix[0xE9].addressing_mode = IMM;
    opMatrix[0xE9].cycle_op_list.push_back(&CPU::SBC_IMM_cycle2);
    opMatrix[0xE9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //NOP
    opMatrix[0xEA].mnemonic = "NOP";
    opMatrix[0xEA].addressing_mode = IMP;
    opMatrix[0xEA].cycle_op_list.push_back(&CPU::NOP_cycle2);
    opMatrix[0xEA].cycle_op_list.push_back(&CPU::fetch_opcode);
    //CPX ABS
    opMatrix[0xEC].mnemonic = "CPX";
    opMatrix[0xEC].addressing_mode = ABS;
    opMatrix[0xEC].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xEC].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xEC].cycle_op_list.push_back(&CPU::CPX_final_cycle);
    opMatrix[0xEC].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC ABS
    opMatrix[0xED].mnemonic = "SBC";
    opMatrix[0xED].addressing_mode = ABS;
    opMatrix[0xED].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xED].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xED].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xED].cycle_op_list.push_back(&CPU::fetch_opcode);
    //INC ABS
    opMatrix[0xEE].mnemonic = "INC";
    opMatrix[0xEE].addressing_mode = ABS;
    opMatrix[0xEE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xEE].cycle_op_list.push_back(&CPU::fetch_adh_cycle3);
    opMatrix[0xEE].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xEE].cycle_op_list.push_back(&CPU::INC_dummy_write);
    opMatrix[0xEE].cycle_op_list.push_back(&CPU::INC_write_cycle);
    opMatrix[0xEE].cycle_op_list.push_back(&CPU::fetch_opcode);
    //BEQ
    opMatrix[0xF0].mnemonic = "BEQ";
    opMatrix[0xF0].addressing_mode = REL;
    opMatrix[0xF0].cycle_op_list.push_back(&CPU::BEQ_cycle2);
    opMatrix[0xF0].cycle_op_list.push_back(&CPU::branch_cycle3);
    opMatrix[0xF0].cycle_op_list.push_back(&CPU::branch_cycle4);
    opMatrix[0xF0].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC INDY
    opMatrix[0xF1].mnemonic = "SBC";
    opMatrix[0xF1].addressing_mode = INDY;
    opMatrix[0xF1].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xF1].cycle_op_list.push_back(&CPU::IND_Y_cycle3);
    opMatrix[0xF1].cycle_op_list.push_back(&CPU::read_IND_Y_cycle4);
    opMatrix[0xF1].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xF1].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xF1].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC ZP,X
    opMatrix[0xF5].mnemonic = "SBC";
    opMatrix[0xF5].addressing_mode = ZPX;
    opMatrix[0xF5].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xF5].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xF5].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xF5].cycle_op_list.push_back(&CPU::fetch_opcode);
    //INC ZPX
    opMatrix[0xF6].mnemonic = "INC";
    opMatrix[0xF6].addressing_mode = ZPX;
    opMatrix[0xF6].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xF6].cycle_op_list.push_back(&CPU::ZP_X_cycle3);
    opMatrix[0xF6].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xF6].cycle_op_list.push_back(&CPU::INC_dummy_write);
    opMatrix[0xF6].cycle_op_list.push_back(&CPU::INC_write_cycle);
    opMatrix[0xF6].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SED
    opMatrix[0xF8].mnemonic = "SED";
    opMatrix[0xF8].addressing_mode = IMP;
    opMatrix[0xF8].cycle_op_list.push_back(&CPU::SED_cycle2);
    opMatrix[0xF8].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC ABS,Y
    opMatrix[0xF9].mnemonic = "SBC";
    opMatrix[0xF9].addressing_mode = ABSY;
    opMatrix[0xF9].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xF9].cycle_op_list.push_back(&CPU::read_ABS_Y_cycle3);
    opMatrix[0xF9].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xF9].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xF9].cycle_op_list.push_back(&CPU::fetch_opcode);
    //SBC ABS,X
    opMatrix[0xFD].mnemonic = "SBC";
    opMatrix[0xFD].addressing_mode = ABSX;
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::read_ABS_X_cycle3);
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::read_page_crossed_cycle);
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::SBC_final_cycle);
    opMatrix[0xFD].cycle_op_list.push_back(&CPU::fetch_opcode);
    //INC ABS,X
    opMatrix[0xFE].mnemonic = "INC";
    opMatrix[0xFE].addressing_mode = ABSX;
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::fetch_adl_cycle2);
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::write_ABS_X_cycle3);
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::write_page_crossed_cycle);
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::RMW_read_cycle);
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::INC_dummy_write);
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::INC_write_cycle);
    opMatrix[0xFE].cycle_op_list.push_back(&CPU::fetch_opcode);

    /** 3 interrupt sequences **/
    //Note: these are not really opcodes/instruction, they are just stored here for programming convenience
    //RESET
    opMatrix[RESET_opcode].mnemonic = "RESET";
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
    opMatrix[NMI_opcode].mnemonic = "NMI";
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
    opMatrix[IRQ_opcode].mnemonic = "IRQ";
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
    std::string mneumonic = getMnemonic(opcode);
    std::string addressingMode = addressingModeToString(getAddressingMode(opcode));
    std::cout << "Opcode: " << mneumonic << " " << addressingMode << "\n";
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

std::string CPU::getMnemonic(uint16_t opcode) {
    return opMatrix[opcode].mnemonic;
}

AddressingMode CPU::getAddressingMode(uint16_t opcode) {
    return opMatrix[opcode].addressing_mode;
}

CPU::cycle_operation CPU::getNextFunctionPtr(uint16_t opcode) {
    if (opMatrix[opcode].cycle_op_list.empty())
        return nullptr;
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

/** Arithmetic instructions **/
void CPU::ADC_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = &CPU::ADD;
    overlap_op2 = &CPU::store_ALU2A_ADC;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::SBC_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = &CPU::SUB;
    overlap_op2 = &CPU::store_ALU2A_SBC;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::ADC_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = &CPU::ADD;
    overlap_op2 = &CPU::store_ALU2A_ADC;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::SBC_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = &CPU::SUB;
    overlap_op2 = &CPU::store_ALU2A_SBC;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::ADD() {
    //Get carry value for readability
    uint8_t carry;
    if (getStatusReg(C))
        carry = 0x01;
    else
        carry = 0x00;

    ALU_result16 = static_cast<uint16_t>(A) + working_data + carry;
}

void CPU::SUB() {
    //Get inverse of carry value for readability
    uint8_t not_carry;
    if (getStatusReg(C))
        not_carry = 0x00;
    else
        not_carry = 0x01;

    ALU_result16 = static_cast<uint16_t>(A) - working_data - not_carry;
}

void CPU::store_ALU2A_ADC() {
    ALU_result = static_cast<uint8_t>(ALU_result16);

    setStatusReg(ALU_result16 > 0xFF, C);
    setStatusReg(ALU_result == 0, Z);
    setStatusReg((ALU_result & 0x80) != 0, N);
    //Checks to see if result is opposite sign of operands when they are both the same sign
    setStatusReg((ALU_result^A) & (ALU_result^working_data) & 0x80, V);

    A = ALU_result;
}

void CPU::store_ALU2A_SBC() {
    ALU_result = static_cast<uint8_t>(ALU_result16);

    setStatusReg(ALU_result16 <= 0xFF, C);
    setStatusReg(ALU_result == 0, Z);
    setStatusReg((ALU_result & 0x80) != 0, N);
    //Checks to see if result is opposite sign of operands when they are both the same sign
    setStatusReg((ALU_result^A) & (ALU_result^(~working_data)) & 0x80, V);

    A = ALU_result;
}

void CPU::INC_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data + 1;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::DEC_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data - 1;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::INC_write_cycle() {
    write(addr1, ALU_result);

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;

    curr_micro_op++;
    interrupt_poll_cycle = true;
}

void CPU::DEC_write_cycle() {
    write(addr1, ALU_result);

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;

    curr_micro_op++;
    interrupt_poll_cycle = true;
}

void CPU::INX_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = X;

    overlap_op1 = &CPU::Increment;
    overlap_op2 = &CPU::store_ALU2X_Increment;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::INY_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = Y;

    overlap_op1 = &CPU::Increment;
    overlap_op2 = &CPU::store_ALU2Y_Increment;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::DEX_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = X;

    overlap_op1 = &CPU::Decrement;
    overlap_op2 = &CPU::store_ALU2X_Increment;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::DEY_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = Y;

    overlap_op1 = &CPU::Decrement;
    overlap_op2 = &CPU::store_ALU2Y_Increment;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::Increment() {
    ALU_result = working_data + 1;
}

void CPU::Decrement() {
    ALU_result = working_data - 1;
}

void CPU::store_ALU2X_Increment() {
    X = ALU_result;

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

void CPU::store_ALU2Y_Increment() {
    Y = ALU_result;

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);
}

/** Shift instructions **/
void CPU::ASL_Accum_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = A;

    overlap_op1 = &CPU::ASL;
    overlap_op2 = &CPU::store_ALU2A_ASL;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::LSR_Accum_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = A;

    overlap_op1 = &CPU::LSR;
    overlap_op2 = &CPU::store_ALU2A_LSR;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::ROL_Accum_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = A;

    overlap_op1 = &CPU::ROL;
    overlap_op2 = &CPU::store_ALU2A_ASL;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::ROR_Accum_cycle2() {
    dummy_read();

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = A;

    overlap_op1 = &CPU::ROR;
    overlap_op2 = &CPU::store_ALU2A_LSR;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::ASL_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data << 1;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::LSR_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data >> 1;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::ROL_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data << 1;
    ALU_result = (ALU_result & 0xFE) | (getStatusReg(C) & 0x01);

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::ROR_dummy_write() {
    write(addr1, working_data);
    ALU_result = working_data >> 1;
    ALU_result = (ALU_result & 0x7F) | (static_cast<uint8_t>(getStatusReg(C)) << 7);

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::ASL_write_cycle() {
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

void CPU::LSR_write_cycle() {
    write(addr1, ALU_result);

    if (ALU_result == 0)
        setStatusReg(true, Z);
    else
        setStatusReg(false, Z);

    if ((ALU_result & 0x80) != 0)
        setStatusReg(true, N);
    else
        setStatusReg(false, N);

    //Set carry to bit 0 of original value of A
    if ((working_data & 0x01) == 0x00)
        setStatusReg(false, C);
    else
        setStatusReg(true, C);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;

    curr_micro_op++;
    interrupt_poll_cycle = true;
}

void CPU::ASL() {
    ALU_result = A << 1;
}

void CPU::LSR() {
    ALU_result = A >> 1;
}

void CPU::ROL() {
    ALU_result = A << 1;
    //Set bit 0 to C
    ALU_result = (ALU_result & 0xFE) | (getStatusReg(C) & 0x01);
}

void CPU::ROR() {
    ALU_result = A >> 1;
    //Set bit 7 to C
    ALU_result = (ALU_result & 0x7F) | (static_cast<uint8_t>(getStatusReg(C)) << 7);
}

void CPU::store_ALU2A_ASL() {
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

void CPU::store_ALU2A_LSR() {
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
    if ((working_data & 0x01) == 0x00)
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

/** Compare Instructions **/
void CPU::CMP_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = nullptr;
    overlap_op2 = &CPU::CMP_set_flags;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::CPX_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);
    overlap_op1 = nullptr;
    overlap_op2 = &CPU::CPX_set_flags;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::CPY_IMM_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    overlap_op1 = nullptr;
    overlap_op2 = &CPU::CPY_set_flags;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::CMP_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = nullptr;
    overlap_op2 = &CPU::CMP_set_flags;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::CPX_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = nullptr;
    overlap_op2 = &CPU::CPX_set_flags;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::CPY_final_cycle() {
    working_data = read(addr1);

    overlap_op1 = nullptr;
    overlap_op2 = &CPU::CPY_set_flags;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::CMP_set_flags() {
    setStatusReg(A >= working_data, C);
    setStatusReg(A == working_data, Z);
    setStatusReg(((A - working_data) & 0x80) != 0, N);
}

void CPU::CPX_set_flags() {
    setStatusReg(X >= working_data, C);
    setStatusReg(X == working_data, Z);
    setStatusReg(((X - working_data) & 0x80) != 0, N);
}

void CPU::CPY_set_flags() {
    setStatusReg(Y >= working_data, C);
    setStatusReg(Y == working_data, Z);
    setStatusReg(((Y - working_data) & 0x80) != 0, N);
}

/** Branch instructions **/
void CPU::BCC_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (getStatusReg(C)) { //Carry is set, then this becomes the final cycle of BCC
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BCS_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (!getStatusReg(C)) { //Carry is NOT set, then this becomes the final cycle of BCS
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BEQ_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (!getStatusReg(Z)) { //Zero is not set - then not equal, then this becomes the final cycle of BEQ
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BNE_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (getStatusReg(Z)) { //Zero is set so they are equal -> don't branch, then this becomes the final cycle of BNE
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BPL_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (getStatusReg(N)) { //Negative is set, then this becomes the final cycle of BPL
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BMI_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (!getStatusReg(N)) { //Negative is NOT set, then this becomes the final cycle of BMI
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BVC_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (getStatusReg(V)) { //Overflow is set, then this becomes the final cycle of BVC
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::BVS_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    working_data = read(PC++);

    interrupt_poll_cycle = false;
    curr_micro_op++;

    if (!getStatusReg(V)) { //Overflow is NOT set, then this becomes the final cycle of BVS
        curr_micro_op += 2; //Skip branching cycle and page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
}

void CPU::branch_cycle3() {
    dummy_read();

    ALU_result16 = PC + static_cast<int8_t>(working_data); //add offset

    curr_micro_op++;
    interrupt_poll_cycle = false;

    if ((PC & 0x0100) == (ALU_result16 & 0x0100)) { //page not crossed
        PC = ALU_result16;
        curr_micro_op += 1; //Skip page cross cycle
        overlap_op1 = nullptr;
        overlap_op2 = nullptr;
        interrupt_poll_cycle = true;
    }
    else {
        PC = ALU_result16 - 0x0100; //This cycle, the carry bit from adding PCL and offset should not have happened yet in cycle accurate emu
    }

}

void CPU::branch_cycle4() {
    dummy_read(PC);
    PC = ALU_result16;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    curr_micro_op++;
    interrupt_poll_cycle = true;
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

void CPU::JMP_IND_cycle4() {
    addr2 = read(addr1);
    addr1 = (addr1 & 0xFF00) | ((addr1 + 1) & 0x00FF); //Equal to addr1++ but we do not allow page to be changed (known bug in the 6502)

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::JMP_IND_cycle5() {
    addr2 |= (static_cast<uint8_t>(read(addr1)) << 8);

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::JSR_cycle3() {
    dummy_read(STACK_START + SP);

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::JSR_cycle4() {
    uint8_t PCH = (PC & 0xFF00) >> 8;
    addr2 = STACK_START + SP; //Temp Stack Pointer
    push(addr2, PCH);

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::JSR_cycle5() {
    uint8_t PCL = (PC & 0x00FF);
    push(addr2, PCL);

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::JSR_cycle6() {
    addr1 |= (static_cast<uint16_t>(read(PC++)) << 8);
    PC = addr1;
    SP = addr2 - STACK_START;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::RTS_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    dummy_read();
    PC++;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::RTS_cycle3() {
    addr1 = STACK_START + SP;
    pull(addr1); //dummy read above top of stack

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::RTS_cycle4() {
    addr2 = pull(addr1); //PCL, addr1++

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::RTS_cycle5() {
    SP = addr1 - STACK_START;
    addr2 |= static_cast<uint16_t>(read(addr1)) << 8; //PCH, Note: addr1 not incremented
    PC = addr2;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::RTS_cycle6() {
    dummy_read();
    PC++;

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

void CPU::PHA_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    dummy_read();

    working_data = A;
    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::PHP_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    dummy_read();

    working_data = status_reg | 0b00110000;
    interrupt_poll_cycle = false;
    curr_micro_op++;
}


void CPU::push_final_cycle() {
    write(STACK_START + SP, working_data);
    SP--;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::pull_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    dummy_read();
    interrupt_poll_cycle = false;
    curr_micro_op++;
}


void CPU::pull_cycle3() {
    SP++;

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::PLA_final_cycle() {
    working_data = read(STACK_START + SP);

    overlap_op1 = &CPU::load_A;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::PLP_final_cycle() {
    working_data = read(STACK_START + SP);

    overlap_op1 = &CPU::restore_flags;
    overlap_op2 = nullptr;
    interrupt_poll_cycle = true;
    curr_micro_op++;
}

void CPU::restore_flags() {
    status_reg = (status_reg & 0b00110000) | (working_data & 0b11001111);
}


/** NOP **/
void CPU::NOP_cycle2() {
    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    dummy_read();

    interrupt_poll_cycle = true;
    curr_micro_op++;
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
    dummy_read();

    if (overlap_op1 != nullptr)
        (this->*overlap_op1)();

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::interrupt_seq_cycle2() {
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
    push(addr1, status_reg & 0b11101111); //SP-- (really addr1 internally for now)

    curr_micro_op++;
    interrupt_poll_cycle = false;
}

void CPU::IRQ_cycle6() {
    SP = addr1 - STACK_START; //Store SP
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

void CPU::BRK_cycle2() {
    dummy_read();
    PC++;

    if (overlap_op2 != nullptr)
        (this->*overlap_op2)();

    interrupt_poll_cycle = false;
    curr_micro_op++;
}

void CPU::BRK_cycle5() {
    push(addr1, status_reg | 0b00010000); //SP-- (really addr1 internally for now)

    curr_micro_op++;
    interrupt_poll_cycle = false;
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
    SP = addr1 - STACK_START; //Store SP (before this cycles increment in pull call)
    addr2 |= static_cast<uint16_t>(pull(addr1)) << 8; //pull PCH
    PC = addr2;

    overlap_op1 = nullptr;
    overlap_op2 = nullptr;

    curr_micro_op++;
    interrupt_poll_cycle = true;
}

