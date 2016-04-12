import qbs

STM32F4xxProduct {
	Depends { name: "stm32" }
	type: "application"
	consoleApplication: true

	cpp.linkerFlags: {
		base.push("-Xlinker")
		base.push("--gc-sections")
		return base
	}

	cpp.staticLibraries: [
		stm32.libs + "/" + MCUFamily + "/libStdPeriphDriver" + libPostfix + ".a"
	]

	cpp.includePaths: {
		base.push(stm32.includes + "/StdPeriphDriver/" + MCUFamily)
		return base
	}

}
