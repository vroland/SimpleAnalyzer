################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GUI/GUIAnalyzeOutputWindow.cpp \
../src/GUI/GUIAnalyzePointWindow.cpp \
../src/GUI/GUICutRenderWindow.cpp \
../src/GUI/GUIGLCanvas.cpp \
../src/GUI/GUIMainWindow.cpp \
../src/GUI/GUIRenderCutCanvas.cpp \
../src/GUI/GUITimeline.cpp \
../src/GUI/PropertiesBox.cpp \
../src/GUI/Renderer.cpp \
../src/GUI/ViewpropBox.cpp \
../src/GUI/events.cpp 

OBJS += \
./src/GUI/GUIAnalyzeOutputWindow.o \
./src/GUI/GUIAnalyzePointWindow.o \
./src/GUI/GUICutRenderWindow.o \
./src/GUI/GUIGLCanvas.o \
./src/GUI/GUIMainWindow.o \
./src/GUI/GUIRenderCutCanvas.o \
./src/GUI/GUITimeline.o \
./src/GUI/PropertiesBox.o \
./src/GUI/Renderer.o \
./src/GUI/ViewpropBox.o \
./src/GUI/events.o 

CPP_DEPS += \
./src/GUI/GUIAnalyzeOutputWindow.d \
./src/GUI/GUIAnalyzePointWindow.d \
./src/GUI/GUICutRenderWindow.d \
./src/GUI/GUIGLCanvas.d \
./src/GUI/GUIMainWindow.d \
./src/GUI/GUIRenderCutCanvas.d \
./src/GUI/GUITimeline.d \
./src/GUI/PropertiesBox.d \
./src/GUI/Renderer.d \
./src/GUI/ViewpropBox.d \
./src/GUI/events.d 


# Each subdirectory must supply rules for building sources it contributes
src/GUI/%.o: ../src/GUI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -DTETLIBRARY -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-release-2.8/ -O0 -g3 -Wall -c -fmessage-length=0 `wx-config --cxxflags` -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


