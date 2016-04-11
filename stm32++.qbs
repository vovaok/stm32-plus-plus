import qbs

STM32F4xxStaticLib {
    Depends{ name: "cpp" }
    Depends { name: "stm32" }

    name : "stm32++" + libPostfix

    cpp.cxxLanguageVersion: "c++11"

    cpp.includePaths : {
        base.push("src/")
        return base
    }

    Group {
        name : "AGGRESSIVE OPTIMIZATION!"
        files : [
            "src/rcc.cpp"
        ]
        cpp.optimization : "aggressive"
    }

    Group {
        name : "headers"
        prefix : "src/**/"
        files : [ "*.h" ]
        excludeFiles: [
            "objnetold.h",
            //"objnet/*.h",
            //"audio/*.h",
            //"serial/*.h",
            "*/shit/*.h",
            "objnet_backup/*.h",
            "usb/otgDriver/L00k @ my H0r53 my h0R53 15 4m4Z1n9/*.h"
        ]
        qbs.install : false
        qbs.installDir : "include"
    }

    Group {
        name : "sources"
        prefix : "src/**/"
        files : [ "*.c", "*.cpp" ]
        excludeFiles: [
            "rcc.cpp", // этот файл копилится отдельно и по полной ЖЕСТИ
            //"objnet/*.cpp",
            //"audio/*.cpp",
            //"serial/*.cpp",
            "objnetold.cpp",
            "can_old.cpp",
            "*/shit/*.cpp",
            "objnet_backup/*.cpp",
            "usb/otgDriver/L00k @ my H0r53 my h0R53 15 4m4Z1n9/*.c"
        ]
        cpp.optimization : "none"
    }

}
