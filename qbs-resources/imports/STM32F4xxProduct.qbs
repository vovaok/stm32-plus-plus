import qbs

STM32BaseProduct {
	MCUFamily: "STM32F4xx"

	cpp.positionIndependentCode: false

	cpp.commonCompilerFlags: [
		"-mcpu=cortex-m4",
		"-mthumb",
		"-mfpu=fpv4-sp-d16",
		"-mfloat-abi=softfp"
    	]

	cpp.linkerFlags: [
		"-mcpu=cortex-m4",
		"-mthumb",
		"-mfpu=fpv4-sp-d16",
		"-mfloat-abi=softfp"
	]
 
	cpp.defines: {
		base.push("STM32F4XX")
		base.push("ARM_MATH_CM4")
		base.push("STM32F40_41xxx")
		base.push("HSE_VALUE=168000000")
		base.push("__FPU_PRESENT")
		base.push("USE_STDPERIPH_DRIVER")
		if(qbs.buildVariant == "debug")
			base.push("USE_FULL_ASSERT")
		return base
    	}

	cpp.linkerScripts: [
		stm32LinkerScripts + "/libs.ld",
		//stm32LinkerScripts + "/mem.ld",
		//stm32LinkerScripts + "/sections.ld",
		stm32LinkerScripts + "/stm32_flash.ld"
	]

}
