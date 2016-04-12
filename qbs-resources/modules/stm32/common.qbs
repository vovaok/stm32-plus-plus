import qbs

Module {
	property string repoRoot: qbs.getEnv("ARM_REPO")

	property string includes: repoRoot + "/include"

	property string libs: repoRoot + "/lib/GCC"
}
