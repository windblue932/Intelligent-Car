################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.c" 

COMPILED_SRCS += \
"libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.src" 

C_DEPS += \
"./libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.d" 

OBJS += \
"libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.o" 


# Each subdirectory must supply rules for building sources it contributes
"libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.src":"../libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.c" "libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fD:/files_works/ads/BACK_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.o":"libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.src" "libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Qspi-2f-Std

clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Qspi-2f-Std:
	-$(RM) ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.d ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.o ./libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/IfxQspi.src

.PHONY: clean-libraries-2f-infineon_libraries-2f-iLLD-2f-TC26B-2f-Tricore-2f-Qspi-2f-Std

