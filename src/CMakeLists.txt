cmake_minimum_required(VERSION 3.19)
project(disPicable)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

#source files
target_sources(${PROJECT_NAME} PRIVATE DisassemblerRamTracker.cpp )
target_sources(${PROJECT_NAME} PRIVATE ArgumentParser.cpp )
target_sources(${PROJECT_NAME} PRIVATE AssembleDirectives.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerTableReads.cpp)
target_sources(${PROJECT_NAME} PRIVATE MacroEngine.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassembleDirectives.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssemblerArguments.cpp)
target_sources(${PROJECT_NAME} PRIVATE Disassembler.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerLabels.cpp)
target_sources(${PROJECT_NAME} PRIVATE BasicFileEditing.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssemblerMachineCodeWriter.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassembleDoubleWords.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssemblerFilePreprocessing.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerFunctionStacks.cpp)
target_sources(${PROJECT_NAME} PRIVATE EQUs.cpp)
target_sources(${PROJECT_NAME} PRIVATE Assembler.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssemblerAddressAndChecksum.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerInstructionProcessor.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerComments.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssemblerInstructionProcessor.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssembleSingleWord.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerParameters.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerModifyByAddress.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssembleDoubleWord.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssembleDisassembleCommonFunctions.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassembleSingleword.cpp)
target_sources(${PROJECT_NAME} PRIVATE AssemblerLabels.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerFileWriter.cpp)
target_sources(${PROJECT_NAME} PRIVATE DisassemblerHexFileParser.cpp)

