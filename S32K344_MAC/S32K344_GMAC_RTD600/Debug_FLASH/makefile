################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include third_party/lwip/src/netif/subdir.mk
-include third_party/lwip/src/core/ipv4/subdir.mk
-include third_party/lwip/src/core/subdir.mk
-include src/middleware/lwip/subdir.mk
-include src/app/echo/subdir.mk
-include src/app/subdir.mk
-include generate/src/subdir.mk
-include board/subdir.mk
-include RTD/src/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
S32K344_GMAC_RTD600.siz \


# All Target
all: S32K344_GMAC_RTD600.elf secondary-outputs

# Tool invocations
S32K344_GMAC_RTD600.elf: $(OBJS) D:/Project_Source_Code/M50K/miniLED_Ethernet_demo/S32K344_MAC/S32K344_GMAC_RTD600/Project_Settings/Linker_Files/linker_flash_s32k344.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "S32K344_GMAC_RTD600.elf" "@S32K344_GMAC_RTD600.args"  $(USER_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

S32K344_GMAC_RTD600.siz: S32K344_GMAC_RTD600.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley S32K344_GMAC_RTD600.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
