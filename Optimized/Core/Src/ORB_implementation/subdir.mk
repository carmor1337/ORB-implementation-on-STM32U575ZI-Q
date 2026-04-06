################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ORB_implementation/Benchmarking.c \
../Core/Src/ORB_implementation/Benchmarking_map.c \
../Core/Src/ORB_implementation/FAST.c \
../Core/Src/ORB_implementation/Feature_matching.c \
../Core/Src/ORB_implementation/Harris_corner_detection.c \
../Core/Src/ORB_implementation/Non_maximum_supression.c \
../Core/Src/ORB_implementation/ORB.c \
../Core/Src/ORB_implementation/rBrief.c 

OBJS += \
./Core/Src/ORB_implementation/Benchmarking.o \
./Core/Src/ORB_implementation/Benchmarking_map.o \
./Core/Src/ORB_implementation/FAST.o \
./Core/Src/ORB_implementation/Feature_matching.o \
./Core/Src/ORB_implementation/Harris_corner_detection.o \
./Core/Src/ORB_implementation/Non_maximum_supression.o \
./Core/Src/ORB_implementation/ORB.o \
./Core/Src/ORB_implementation/rBrief.o 

C_DEPS += \
./Core/Src/ORB_implementation/Benchmarking.d \
./Core/Src/ORB_implementation/Benchmarking_map.d \
./Core/Src/ORB_implementation/FAST.d \
./Core/Src/ORB_implementation/Feature_matching.d \
./Core/Src/ORB_implementation/Harris_corner_detection.d \
./Core/Src/ORB_implementation/Non_maximum_supression.d \
./Core/Src/ORB_implementation/ORB.d \
./Core/Src/ORB_implementation/rBrief.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ORB_implementation/%.o Core/Src/ORB_implementation/%.su Core/Src/ORB_implementation/%.cyclo: ../Core/Src/ORB_implementation/%.c Core/Src/ORB_implementation/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g -DUSE_NUCLEO_144 -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32U5xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -O2 -ffunction-sections -fdata-sections -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ORB_implementation

clean-Core-2f-Src-2f-ORB_implementation:
	-$(RM) ./Core/Src/ORB_implementation/Benchmarking.cyclo ./Core/Src/ORB_implementation/Benchmarking.d ./Core/Src/ORB_implementation/Benchmarking.o ./Core/Src/ORB_implementation/Benchmarking.su ./Core/Src/ORB_implementation/Benchmarking_map.cyclo ./Core/Src/ORB_implementation/Benchmarking_map.d ./Core/Src/ORB_implementation/Benchmarking_map.o ./Core/Src/ORB_implementation/Benchmarking_map.su ./Core/Src/ORB_implementation/FAST.cyclo ./Core/Src/ORB_implementation/FAST.d ./Core/Src/ORB_implementation/FAST.o ./Core/Src/ORB_implementation/FAST.su ./Core/Src/ORB_implementation/Feature_matching.cyclo ./Core/Src/ORB_implementation/Feature_matching.d ./Core/Src/ORB_implementation/Feature_matching.o ./Core/Src/ORB_implementation/Feature_matching.su ./Core/Src/ORB_implementation/Harris_corner_detection.cyclo ./Core/Src/ORB_implementation/Harris_corner_detection.d ./Core/Src/ORB_implementation/Harris_corner_detection.o ./Core/Src/ORB_implementation/Harris_corner_detection.su ./Core/Src/ORB_implementation/Non_maximum_supression.cyclo ./Core/Src/ORB_implementation/Non_maximum_supression.d ./Core/Src/ORB_implementation/Non_maximum_supression.o ./Core/Src/ORB_implementation/Non_maximum_supression.su ./Core/Src/ORB_implementation/ORB.cyclo ./Core/Src/ORB_implementation/ORB.d ./Core/Src/ORB_implementation/ORB.o ./Core/Src/ORB_implementation/ORB.su ./Core/Src/ORB_implementation/rBrief.cyclo ./Core/Src/ORB_implementation/rBrief.d ./Core/Src/ORB_implementation/rBrief.o ./Core/Src/ORB_implementation/rBrief.su

.PHONY: clean-Core-2f-Src-2f-ORB_implementation

