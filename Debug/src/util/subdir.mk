################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/CCommon.cpp \
../src/util/CConfig.cpp \
../src/util/CIniFile.cpp \
../src/util/CLog.cpp 

OBJS += \
./src/util/CCommon.o \
./src/util/CConfig.o \
./src/util/CIniFile.o \
./src/util/CLog.o 

CPP_DEPS += \
./src/util/CCommon.d \
./src/util/CConfig.d \
./src/util/CIniFile.d \
./src/util/CLog.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -I../src/data -I../src/net -I../src/thread -I../src/util -I../src/openssl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


