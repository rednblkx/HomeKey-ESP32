require 'lspconfig'.clangd.setup {
  capabilities = require('cmp_nvim_lsp').default_capabilities(vim.lsp.protocol.make_client_capabilities()), 
  cmd = {
    os.getenv("HOME") .. "/Downloads/esp-clang/bin/clangd",
    "--background-index",
    "--query-driver=$HOME/.platformio/packages/toolchain-xtensa-esp32/bin/xtensa-esp32-elf-gcc*"
  },
  on_attach = require("cmp_nvim_lsp").on_attach,
  lsp_flags = require("cmp_nvim_lsp").lsp_flags,
  filetypes = { "c", "cpp", "objc", "objcpp", "cuda", "proto" },
} 
