################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DefaultValues.cpp \
../src/SimpleAnalyzerApp.cpp 

OBJS += \
./src/DefaultValues.o \
./src/SimpleAnalyzerApp.o 

CPP_DEPS += \
./src/DefaultValues.d \
./src/SimpleAnalyzerApp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DTETLIBRARY -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-2.8 -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 `wx-config --cxxflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


