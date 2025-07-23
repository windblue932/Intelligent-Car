################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../code/HANA_math.c" \
"../code/all_init.c" \
"../code/beep.c" \
"../code/fan.c" \
"../code/image.c" \
"../code/motor.c" \
"../code/pidpid.c" \
"../code/send.c" \
"../code/ui.c" \
"../code/variables.c" 

COMPILED_SRCS += \
"code/HANA_math.src" \
"code/all_init.src" \
"code/beep.src" \
"code/fan.src" \
"code/image.src" \
"code/motor.src" \
"code/pidpid.src" \
"code/send.src" \
"code/ui.src" \
"code/variables.src" 

C_DEPS += \
"./code/HANA_math.d" \
"./code/all_init.d" \
"./code/beep.d" \
"./code/fan.d" \
"./code/image.d" \
"./code/motor.d" \
"./code/pidpid.d" \
"./code/send.d" \
"./code/ui.d" \
"./code/variables.d" 

OBJS += \
"code/HANA_math.o" \
"code/all_init.o" \
"code/beep.o" \
"code/fan.o" \
"code/image.o" \
"code/motor.o" \
"code/pidpid.o" \
"code/send.o" \
"code/ui.o" \
"code/variables.o" 


# Each subdirectory must supply rules for building sources it contributes
"code/HANA_math.src":"../code/HANA_math.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/HANA_math.o":"code/HANA_math.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/all_init.src":"../code/all_init.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/all_init.o":"code/all_init.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/beep.src":"../code/beep.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/beep.o":"code/beep.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/fan.src":"../code/fan.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/fan.o":"code/fan.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/image.src":"../code/image.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/image.o":"code/image.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/motor.src":"../code/motor.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/motor.o":"code/motor.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/pidpid.src":"../code/pidpid.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/pidpid.o":"code/pidpid.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/send.src":"../code/send.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/send.o":"code/send.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/ui.src":"../code/ui.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/ui.o":"code/ui.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"code/variables.src":"../code/variables.c" "code/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2004 -D__CPU__=tc26xb "-fW:/pre_match/FRONT_ALL_ELEMENT_170/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<"
"code/variables.o":"code/variables.src" "code/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-code

clean-code:
	-$(RM) ./code/HANA_math.d ./code/HANA_math.o ./code/HANA_math.src ./code/all_init.d ./code/all_init.o ./code/all_init.src ./code/beep.d ./code/beep.o ./code/beep.src ./code/fan.d ./code/fan.o ./code/fan.src ./code/image.d ./code/image.o ./code/image.src ./code/motor.d ./code/motor.o ./code/motor.src ./code/pidpid.d ./code/pidpid.o ./code/pidpid.src ./code/send.d ./code/send.o ./code/send.src ./code/ui.d ./code/ui.o ./code/ui.src ./code/variables.d ./code/variables.o ./code/variables.src

.PHONY: clean-code

