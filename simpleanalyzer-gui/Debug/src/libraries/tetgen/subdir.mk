################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CXX_SRCS += \
../src/libraries/tetgen/predicates.cxx \
../src/libraries/tetgen/tetgen.cxx 

OBJS += \
./src/libraries/tetgen/predicates.o \
./src/libraries/tetgen/tetgen.o 

CXX_DEPS += \
./src/libraries/tetgen/predicates.d \
./src/libraries/tetgen/tetgen.d 


# Each subdirectory must supply rules for building sources it contributes
src/libraries/tetgen/%.o: ../src/libraries/tetgen/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DTETLIBRARY -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-debug-2.8 -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 `wx-config --cxxflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


