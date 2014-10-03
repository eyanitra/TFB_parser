##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Parser
ConfigurationName      :=Debug
WorkspacePath          := "D:\Codelite.workspace\Workhome"
ProjectPath            := "D:\Codelite.workspace\Workhome\TFB_parser"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Unknown
Date                   :=10/03/14
CodeLitePath           :="C:\Program Files\CodeLite"
LinkerName             :=C:/MinGW-4.8.1/bin/g++.exe 
SharedObjectLinkerName :=C:/MinGW-4.8.1/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Parser.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW-4.8.1/bin/windres.exe 
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./lib $(IncludeSwitch)./src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-4.8.1/bin/ar.exe rcu
CXX      := C:/MinGW-4.8.1/bin/g++.exe 
CC       := C:/MinGW-4.8.1/bin/gcc.exe 
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-4.8.1/bin/as.exe 


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
Objects0=$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IntermediateDirectory)/TFB_parser.c$(ObjectSuffix) $(IntermediateDirectory)/printer.c$(ObjectSuffix) $(IntermediateDirectory)/BPPblackListRecord.c$(ObjectSuffix) $(IntermediateDirectory)/lib_DSC_bignum.c$(ObjectSuffix) $(IntermediateDirectory)/lib_DSC_hash.c$(ObjectSuffix) $(IntermediateDirectory)/lib_VF_file.c$(ObjectSuffix) $(IntermediateDirectory)/lib_VF_folder.c$(ObjectSuffix) $(IntermediateDirectory)/lib_DSC_converter.c$(ObjectSuffix) $(IntermediateDirectory)/lib_TLV_serializer.c$(ObjectSuffix) \
	



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.c$(ObjectSuffix): main.c $(IntermediateDirectory)/main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.c$(DependSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/main.c$(DependSuffix) -MM "main.c"

$(IntermediateDirectory)/main.c$(PreprocessSuffix): main.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.c$(PreprocessSuffix) "main.c"

$(IntermediateDirectory)/TFB_parser.c$(ObjectSuffix): TFB_parser.c $(IntermediateDirectory)/TFB_parser.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/TFB_parser.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/TFB_parser.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/TFB_parser.c$(DependSuffix): TFB_parser.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/TFB_parser.c$(ObjectSuffix) -MF$(IntermediateDirectory)/TFB_parser.c$(DependSuffix) -MM "TFB_parser.c"

$(IntermediateDirectory)/TFB_parser.c$(PreprocessSuffix): TFB_parser.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/TFB_parser.c$(PreprocessSuffix) "TFB_parser.c"

$(IntermediateDirectory)/printer.c$(ObjectSuffix): printer.c $(IntermediateDirectory)/printer.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/printer.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/printer.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/printer.c$(DependSuffix): printer.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/printer.c$(ObjectSuffix) -MF$(IntermediateDirectory)/printer.c$(DependSuffix) -MM "printer.c"

$(IntermediateDirectory)/printer.c$(PreprocessSuffix): printer.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/printer.c$(PreprocessSuffix) "printer.c"

$(IntermediateDirectory)/BPPblackListRecord.c$(ObjectSuffix): BPPblackListRecord.c $(IntermediateDirectory)/BPPblackListRecord.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/BPPblackListRecord.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BPPblackListRecord.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BPPblackListRecord.c$(DependSuffix): BPPblackListRecord.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/BPPblackListRecord.c$(ObjectSuffix) -MF$(IntermediateDirectory)/BPPblackListRecord.c$(DependSuffix) -MM "BPPblackListRecord.c"

$(IntermediateDirectory)/BPPblackListRecord.c$(PreprocessSuffix): BPPblackListRecord.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/BPPblackListRecord.c$(PreprocessSuffix) "BPPblackListRecord.c"

$(IntermediateDirectory)/lib_DSC_bignum.c$(ObjectSuffix): lib/DSC_bignum.c $(IntermediateDirectory)/lib_DSC_bignum.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/lib/DSC_bignum.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lib_DSC_bignum.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lib_DSC_bignum.c$(DependSuffix): lib/DSC_bignum.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lib_DSC_bignum.c$(ObjectSuffix) -MF$(IntermediateDirectory)/lib_DSC_bignum.c$(DependSuffix) -MM "lib/DSC_bignum.c"

$(IntermediateDirectory)/lib_DSC_bignum.c$(PreprocessSuffix): lib/DSC_bignum.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lib_DSC_bignum.c$(PreprocessSuffix) "lib/DSC_bignum.c"

$(IntermediateDirectory)/lib_DSC_hash.c$(ObjectSuffix): lib/DSC_hash.c $(IntermediateDirectory)/lib_DSC_hash.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/lib/DSC_hash.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lib_DSC_hash.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lib_DSC_hash.c$(DependSuffix): lib/DSC_hash.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lib_DSC_hash.c$(ObjectSuffix) -MF$(IntermediateDirectory)/lib_DSC_hash.c$(DependSuffix) -MM "lib/DSC_hash.c"

$(IntermediateDirectory)/lib_DSC_hash.c$(PreprocessSuffix): lib/DSC_hash.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lib_DSC_hash.c$(PreprocessSuffix) "lib/DSC_hash.c"

$(IntermediateDirectory)/lib_VF_file.c$(ObjectSuffix): lib/VF_file.c $(IntermediateDirectory)/lib_VF_file.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/lib/VF_file.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lib_VF_file.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lib_VF_file.c$(DependSuffix): lib/VF_file.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lib_VF_file.c$(ObjectSuffix) -MF$(IntermediateDirectory)/lib_VF_file.c$(DependSuffix) -MM "lib/VF_file.c"

$(IntermediateDirectory)/lib_VF_file.c$(PreprocessSuffix): lib/VF_file.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lib_VF_file.c$(PreprocessSuffix) "lib/VF_file.c"

$(IntermediateDirectory)/lib_VF_folder.c$(ObjectSuffix): lib/VF_folder.c $(IntermediateDirectory)/lib_VF_folder.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/lib/VF_folder.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lib_VF_folder.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lib_VF_folder.c$(DependSuffix): lib/VF_folder.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lib_VF_folder.c$(ObjectSuffix) -MF$(IntermediateDirectory)/lib_VF_folder.c$(DependSuffix) -MM "lib/VF_folder.c"

$(IntermediateDirectory)/lib_VF_folder.c$(PreprocessSuffix): lib/VF_folder.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lib_VF_folder.c$(PreprocessSuffix) "lib/VF_folder.c"

$(IntermediateDirectory)/lib_DSC_converter.c$(ObjectSuffix): lib/DSC_converter.c $(IntermediateDirectory)/lib_DSC_converter.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/lib/DSC_converter.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lib_DSC_converter.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lib_DSC_converter.c$(DependSuffix): lib/DSC_converter.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lib_DSC_converter.c$(ObjectSuffix) -MF$(IntermediateDirectory)/lib_DSC_converter.c$(DependSuffix) -MM "lib/DSC_converter.c"

$(IntermediateDirectory)/lib_DSC_converter.c$(PreprocessSuffix): lib/DSC_converter.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lib_DSC_converter.c$(PreprocessSuffix) "lib/DSC_converter.c"

$(IntermediateDirectory)/lib_TLV_serializer.c$(ObjectSuffix): lib/TLV_serializer.c $(IntermediateDirectory)/lib_TLV_serializer.c$(DependSuffix)
	$(CC) $(SourceSwitch) "D:/Codelite.workspace/Workhome/TFB_parser/lib/TLV_serializer.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/lib_TLV_serializer.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lib_TLV_serializer.c$(DependSuffix): lib/TLV_serializer.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/lib_TLV_serializer.c$(ObjectSuffix) -MF$(IntermediateDirectory)/lib_TLV_serializer.c$(DependSuffix) -MM "lib/TLV_serializer.c"

$(IntermediateDirectory)/lib_TLV_serializer.c$(PreprocessSuffix): lib/TLV_serializer.c
	@$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/lib_TLV_serializer.c$(PreprocessSuffix) "lib/TLV_serializer.c"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Debug/*$(ObjectSuffix)
	$(RM) ./Debug/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile).exe
	$(RM) "../.build-debug/Parser"


