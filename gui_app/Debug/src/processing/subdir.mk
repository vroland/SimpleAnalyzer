################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/processing/Analyzer.cpp \
../src/processing/MeshProcessor.cpp \
../src/processing/ObjectData.cpp \
../src/processing/utils.cpp 

OBJS += \
./src/processing/Analyzer.o \
./src/processing/MeshProcessor.o \
./src/processing/ObjectData.o \
./src/processing/utils.o 

CPP_DEPS += \
./src/processing/Analyzer.d \
./src/processing/MeshProcessor.d \
./src/processing/ObjectData.d \
./src/processing/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/processing/%.o: ../src/processing/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DTETLIBRARY -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-2.8 -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 `wx-config --cxxflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


