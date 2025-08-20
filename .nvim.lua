require("lspconfig").clangd.setup({
	cmd = {
		os.getenv("HOME") .. "/.espressif/tools/esp-clang/esp-19.1.2_20250312/esp-clang/bin/clangd",
		"--header-insertion=iwyu",
		"--query-driver="
			.. os.getenv("HOME")
			.. "/.espressif/tools/xtensa-esp-elf/**/xtensa-esp-elf/bin/xtensa-*-elf-*,"
			.. os.getenv("HOME")
			.. "/.espressif/tools/riscv32-esp-elf/**/riscv32-esp-elf/bin/riscv32-esp-elf-*",
		"--background-index",
		"--suggest-missing-includes",
		"--clang-tidy",
		"--completion-style=detailed",
		"--function-arg-placeholders",
		"--fallback-style=llvm",
	},
	filetypes = { "c", "cpp", "objc", "objcpp", "cuda", "proto" },
})
