//
//  Disassembler.cpp
//  
//
//  Created by Greg Norman on 27/1/2023.
//

#include "Disassembler.hpp"
extern char Global_working_directory[FILENAME_MAX];
TimerClass MyTimerDissassembler;

// final formatting for assembled commands
void finalise_command(char *&command_for_prompt,                        // current instruction being assembled
                      Converted_Assembly_Code &OutputAssemblyCode,      // assembled file contents
                      const char *Description,                          // comment explaining instruction (optional)
                      uint16_t Address_Upper_16_bits,                   // upper 16 bits of flash address
                      uint32_t device_mem_size,                         // total flash memory - not implemented
                      uint16_t n );                                     // opcode

bool dissassembleDataEntry(uint64_t &file_pos,
                           Converted_Assembly_Code &OutputAssemblyCode,
                           std::vector<unsigned char> &OutputFileContents,
                           PIC18F_FULL_IS &Instruction_Set,
                           bool bDisplayAssembly,
                           bool bDisplayBinContents);

static void ReadMCUHeader(const char * HeaderFile,PIC18F_FULL_IS &Instruction_Set)
{
    if(HeaderFile == NULL)
    {
        return;
    }
    clearIncludedFileDirectories();
    
    char fileDir[FILENAME_MAX];
    
    if(HeaderFile[0]!='/')  // relative directory
        snprintf(fileDir,sizeof(fileDir),"%s/%s",Global_working_directory,HeaderFile);
    else                    // absolute directory
        snprintf(fileDir,sizeof(fileDir),"%s",HeaderFile);
    
    Copy_Over_Binary_File(fileDir,Instruction_Set);
    
}

void WriteAssemblyCodeToFile(Converted_Assembly_Code &OutputAssemblyCode, const char *HeaderFile,bool bDisplayBinContents, bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{
    char output_dir[FILENAME_MAX];
    snprintf(output_dir,sizeof(output_dir),"%s/DisassembledCode.asm",Global_working_directory);
    FILE *fp = fopen(output_dir,"w");
    
    // MCU header
    if(HeaderFile != NULL)
    {
        if(HeaderFile[0] != '/')
        {
            fprintf(fp,"#include \"%s\"\n",HeaderFile);
        }
        else
        {
            fprintf(fp,"#include <%s>\n",HeaderFile);
        }
    }
    uint32_t functionCounter = 0;
    for (uint64_t  pos = 0;pos<OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM.size();pos++)
    {
        
        if(bDisplayBinContents)
        {
            std::cout<<OutputAssemblyCode.Address[pos] << " ";
        }
        if(bDisplayAssembly)
        {
            std::cout<< OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos]<<std::endl;
        }
        else if(bDisplayBinContents)
        {
            std::cout<<std::endl;
        }
        
        fprintf(fp,"%s\n",OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str());
        
        if((strstr(OutputAssemblyCode.ASSEMBLY_CODE_FULL_PROGRAM[pos].c_str(),"RET"))//RETFIE, RETURN
           ||(functionCounter==0))
        {
            // print a list of special function registers used in the function
            fprintf(fp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n;");
            std::string SFRs = grabFSRList(functionCounter);
            functionCounter++;
            fprintf(fp,"%s",SFRs.c_str());
            fprintf(fp,"\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n");
        }
        
    }
    fclose(fp);

}

void Disassemble(const char *fileDir, const char* HeaderFile, bool bDisplayBinContents,bool bDisplayAssembly,PIC18F_FULL_IS &Instruction_Set)
{

    MyTimerDissassembler.updateTimerReference();
    
    std::vector<unsigned char> hexFileContents;                 // holds contents of input file (".hex")
    
    Copy_FIRMWARE_FILE_to_Buffer(fileDir,hexFileContents);
    
    Converted_Assembly_Code OutputAssemblyCode;                 // holds contents of output file (".asm")
        
    clearEQU();
    
    ReadMCUHeader(HeaderFile,Instruction_Set);
    
    for(uint64_t file_pos=0; file_pos < hexFileContents.size() ; )
    {
        if(hexFileContents[file_pos]==':')  // first character on a new line
        {
            file_pos++; // move past colon
            
            if(dissassembleDataEntry(file_pos, OutputAssemblyCode,hexFileContents,Instruction_Set, bDisplayAssembly,bDisplayBinContents) == true)
            {
                break;
            }
        }
        else
        {
            file_pos++;
        }
    }
    HighlightFSRs(OutputAssemblyCode);
    
    markTableReads(OutputAssemblyCode,Instruction_Set);

    AddAllComments(OutputAssemblyCode);
    
    markAllAsciiData(OutputAssemblyCode);
    
    addLabelsToAssemblyCode(OutputAssemblyCode);

    WriteAssemblyCodeToFile(OutputAssemblyCode,HeaderFile,bDisplayBinContents, bDisplayAssembly,Instruction_Set);
    
    std::cout << MyTimerDissassembler.CheckDuration() << "  seconds\n";
}


uint32_t Convert_num_to_hex( std::vector<unsigned char> input,uint64_t &vec_start, uint8_t return_len)
{
    uint32_t return_value=0;
    for(int counter=0;counter<return_len;counter++)
    {
        char tempstring[10];
        snprintf(tempstring,sizeof(tempstring),"%c",input[vec_start+counter]);

        return_value=return_value*0x10;
        return_value+=strtol(tempstring,NULL,16);
    }
    vec_start+=return_len;

    return return_value;

}
bool dissassembleDataEntry(uint64_t &file_pos,
                           Converted_Assembly_Code &OutputAssemblyCode,
                           std::vector<unsigned char> &OutputFileContents,
                           PIC18F_FULL_IS &Instruction_Set,
                           bool bDisplayAssembly,
                           bool bDisplayBinContents)
{
    static bool final_byte_is_double_word = false;
    static uint16_t Address_Upper_16_bits = 0x00;

    // intel hex file header format
    // :DBADDRTDATADATADATADATACS
    uint8_t number_of_bytes=       0xff &   Convert_num_to_hex(OutputFileContents,file_pos,2);
    uint32_t address =                      Convert_num_to_hex(OutputFileContents,file_pos,4);
    uint8_t type =                 0xff &   Convert_num_to_hex(OutputFileContents,file_pos,2);

    if(type==0x00)
    {
        static bool first_byte = true;
        if(first_byte==true)
        {
            first_byte=false;
            add_ORG_to_asm_file(address,OutputAssemblyCode);
        }

        number_of_bytes*=2;

        std::vector<uint8_t> Data_bytes(number_of_bytes);
        for(int counter=0;counter<number_of_bytes;counter++)
        {
            Data_bytes[counter] = OutputFileContents[file_pos];
           
            file_pos++;
        }

        if(Address_Upper_16_bits==Instruction_Set.Device_ID_Address)
        {
            deal_with_non_program_data(Data_bytes,"DEVICE_ID:",address,Address_Upper_16_bits,OutputAssemblyCode,false);
        }
        else if(Address_Upper_16_bits==Instruction_Set.Config_Address)
        {
            deal_with_non_program_data(Data_bytes,"CONFIG_BITS:",address,Address_Upper_16_bits,OutputAssemblyCode,false);
        }
        else if(Address_Upper_16_bits==Instruction_Set.EEPROM_START_ADDR)
        {
            deal_with_non_program_data(Data_bytes,"EEPROM_DIRECTIVE:",address,Address_Upper_16_bits,OutputAssemblyCode,true);
        }
        else
        {
            generate_asm(Data_bytes,address,OutputFileContents,file_pos, final_byte_is_double_word,OutputAssemblyCode,Instruction_Set,bDisplayAssembly,bDisplayBinContents,Address_Upper_16_bits);
        }
    }
    if(type==0x01)  //End of File
    {
        return true;
    }
    if(type==0x04)  //Extended Linear Address
    {
        Address_Upper_16_bits = Convert_num_to_hex(OutputFileContents,file_pos,4);
    }
    
    return false;
}


