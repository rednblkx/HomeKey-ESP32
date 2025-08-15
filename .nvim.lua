local path = os.getenv("HOME") .. "/.espressif/tools/esp-clang"
local clang_version
for file in io.popen("ls -a " .. path):lines() do
	if file ~= "." and file ~= ".." then
		clang_version = file
		break
	end
end

require("lspconfig").clangd.setup({
	cmd = {
		os.getenv("HOME") .. "/.espressif/tools/esp-clang/" .. clang_version .. "/esp-clang/bin/clangd",
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
