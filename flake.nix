{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    esp-dev.url = "github:rednblkx/nixpkgs-esp-dev";
  };
  outputs = {
    self,
    nixpkgs,
    esp-dev,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
      inherit system;
      overlays = [ esp-dev.overlays.default ];
      # The Python library ecdsa is marked as insecure, but we need it for
      # esptool. See https://github.com/mirrexagon/nixpkgs-esp-dev/issues/109
      config.permittedInsecurePackages = [
        "python3.13-ecdsa-0.19.1"
      ];
    };
    esp-idf-v5_5_5 = esp-dev.packages.${system}.esp-idf-full.override {
      rev = "b774170ff46c393eeb5e495ea37936038d3f4f4f";
      sha256 = "1ywzyw34qrj147lgqy89s39m3sg604mazjk7485m6p36qyx6q5gz";
    };
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = with pkgs; [
        bun
        esp-idf-v5_5_5
      ];

      shellHook = ''
        export CLANGD_FLAGS="--query-driver=`which riscv32-esp-elf-g++`,`which riscv32-esp-elf-gcc`,`which xtensa-esp32-elf-g++`,`which xtensa-esp32-elf-gcc` --clang-tidy --background-index --suggest-missing-includes"
        export IDF_CCACHE_ENABLE=1
      '';
    };
  };
}
