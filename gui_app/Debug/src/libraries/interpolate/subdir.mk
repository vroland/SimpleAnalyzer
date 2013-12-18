################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/libraries/interpolate/GeometryClasses.cpp \
../src/libraries/interpolate/Interpolator.cpp 

OBJS += \
./src/libraries/interpolate/GeometryClasses.o \
./src/libraries/interpolate/Interpolator.o 

CPP_DEPS += \
./src/libraries/interpolate/GeometryClasses.d \
./src/libraries/interpolate/Interpolator.d 


# Each subdirectory must supply rules for building sources it contributes
src/libraries/interpolate/%.o: ../src/libraries/interpolate/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DTETLIBRARY -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-release-2.8/ -O0 -g3 -Wall -std=c++0x -c -fmessage-length=0 `wx-config --cxxflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


