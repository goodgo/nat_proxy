################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/net/CAccepter.cpp \
../src/net/CBuffer.cpp \
../src/net/CEpoller.cpp \
../src/net/CNetEvent.cpp \
../src/net/CNetSpeeder.cpp \
../src/net/CSocket.cpp \
../src/net/CTcpConnect.cpp \
../src/net/CUdpChannel.cpp 

OBJS += \
./src/net/CAccepter.o \
./src/net/CBuffer.o \
./src/net/CEpoller.o \
./src/net/CNetEvent.o \
./src/net/CNetSpeeder.o \
./src/net/CSocket.o \
./src/net/CTcpConnect.o \
./src/net/CUdpChannel.o 

CPP_DEPS += \
./src/net/CAccepter.d \
./src/net/CBuffer.d \
./src/net/CEpoller.d \
./src/net/CNetEvent.d \
./src/net/CNetSpeeder.d \
./src/net/CSocket.d \
./src/net/CTcpConnect.d \
./src/net/CUdpChannel.d 


# Each subdirectory must supply rules for building sources it contributes
src/net/%.o: ../src/net/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../src -I../src/data -I../src/net -I../src/thread -I../src/util -I../src/openssl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


