################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/messages-bitcoin.pb.cc \
../src/messages-common.pb.cc \
../src/messages-management.pb.cc \
../src/messages.pb.cc 

CPP_SRCS += \
../src/trezor.cpp 

C_SRCS += \
../src/messenger.c 

CC_DEPS += \
./src/messages-bitcoin.pb.d \
./src/messages-common.pb.d \
./src/messages-management.pb.d \
./src/messages.pb.d 

OBJS += \
./src/messages-bitcoin.pb.o \
./src/messages-common.pb.o \
./src/messages-management.pb.o \
./src/messages.pb.o \
./src/messenger.o \
./src/trezor.o 

CPP_DEPS += \
./src/trezor.d 

C_DEPS += \
./src/messenger.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/peter/eclipse-workspace/trezor/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DPB_FIELD_32BIT -I"/home/peter/eclipse-workspace/trezor/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/peter/eclipse-workspace/trezor/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


