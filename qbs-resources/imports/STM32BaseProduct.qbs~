import qbs

Product {
	Depends { name: "cpp" }
	Depends { name: "stm32" }

	property string MCUFamily: ""

	property string libPostfix : qbs.buildVariant == "debug" ? "_dbg" : ""

	property string stm32LinkerScripts: stm32.repoRoot + "/ldscripts/" + MCUFamily

	cpp.includePaths: [
		stm32.includes + "/CMSIS/Include",
        	stm32.includes + "/CMSIS/ST/" + MCUFamily + "/Include"
	]

	cpp.defines: []
}
