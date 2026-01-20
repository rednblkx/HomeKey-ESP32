{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    esp-dev.url = "github:mirrexagon/nixpkgs-esp-dev";
  };
  outputs = {
    self,
    nixpkgs,
    esp-dev,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
      inherit system;
    };
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = with pkgs; [
        bun
        esp-dev.packages.${system}.esp-idf-full
      ];

      shellHook = ''
        export CLANGD_FLAGS="--query-driver=`which riscv32-esp-elf-g++`,`which riscv32-esp-elf-gcc`,`which xtensa-esp32-elf-g++`,`which xtensa-esp32-elf-gcc` --clang-tidy --background-index --suggest-missing-includes"
        export IDF_CCACHE_ENABLE=1
      '';
    };
  };
}
