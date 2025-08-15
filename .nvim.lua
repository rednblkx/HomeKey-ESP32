require("lspconfig").clangd.setup({
	cmd = {
		os.getenv("HOME") .. "/.espressif/tools/esp-clang/esp-18.1.2_20240912/esp-clang/bin/clangd",
		"--pretty",
		"--header-insertion=iwyu",
		"--query-driver="
			.. os.getenv("HOME")
			.. "/.espressif/tools/xtensa-esp-elf/**/xtensa-esp-elf/bin/xtensa-*-elf-*,"
			.. os.getenv("HOME")
			.. "/.espressif/tools/riscv32-esp-elf/**/riscv32-esp-elf/bin/riscv32-esp-elf-*",
		"--background-index",
		"--suggest-missing-includes",
		"-j=40",
		"--pch-storage=memory",
		"--clang-tidy",
	},
	filetypes = { "c", "cpp", "objc", "objcpp", "cuda", "proto" },
})
