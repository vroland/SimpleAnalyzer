################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/fileIO/Exporter.cpp \
../src/fileIO/Importer.cpp 

OBJS += \
./src/fileIO/Exporter.o \
./src/fileIO/Importer.o 

CPP_DEPS += \
./src/fileIO/Exporter.d \
./src/fileIO/Importer.d 


# Each subdirectory must supply rules for building sources it contributes
src/fileIO/%.o: ../src/fileIO/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DTETLIBRARY -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-release-2.8/ -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 `wx-config --cxxflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


