################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/API.cpp \
../src/BufferManager.cpp \
../src/CatalogManager.cpp \
../src/IndexManager.cpp \
../src/RecordManager.cpp \
../src/interpreter.cpp \
../src/main.cpp \
../src/test.cpp 

OBJS += \
./src/API.o \
./src/BufferManager.o \
./src/CatalogManager.o \
./src/IndexManager.o \
./src/RecordManager.o \
./src/interpreter.o \
./src/main.o \
./src/test.o 

CPP_DEPS += \
./src/API.d \
./src/BufferManager.d \
./src/CatalogManager.d \
./src/IndexManager.d \
./src/RecordManager.d \
./src/interpreter.d \
./src/main.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


