require("lspconfig").clangd.setup({
	capabilities = require("cmp_nvim_lsp").default_capabilities(vim.lsp.protocol.make_client_capabilities()),
	cmd = {
		os.getenv("HOME") .. "/esp-clang/bin/clangd",
		"--query-driver="
			.. os.getenv("HOME")
			.. "/.espressif/tools/xtensa-esp-elf/**/xtensa-esp-elf/bin/xtensa-*-elf-*,"
			.. os.getenv("HOME")
			.. "/tools/riscv32-esp-elf/**/riscv32-esp-elf/bin/riscv32-esp-elf-*",
		"--background-index",
		"--import-insertions",
		"--all-scopes-completion",
	},
	on_attach = require("cmp_nvim_lsp").on_attach,
	lsp_flags = require("cmp_nvim_lsp").lsp_flags,
	filetypes = { "c", "cpp", "objc", "objcpp", "cuda", "proto" },
})
