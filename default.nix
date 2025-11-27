let
  sources = import ./npins;
  pkgs = import sources.nixpkgs { };

  inherit (pkgs) lib;

  # Major hack to get Nixpkgs to support riscv32i: we patch Nixpkgs
  # itself to add a couple of arguments on to the end of gcc's build
  # script. TODO: figure out if there's a better way to do this
  pkgsRiscV = import (pkgs.runCommand "nixpkgs-with-riscv32i" { } ''
    cp -r ${pkgs.path} $out
    chmod -R +w $out
    echo '++ lib.optionals targetPlatform.isRiscV [ "--with-arch=rv32i" "--with-abi=ilp32" ]' \
        >> $out/pkgs/development/compilers/gcc/common/configure-flags.nix
  '') {
    crossSystem.config = "riscv32-unknown-none-elf";
  };
in
{
  kernel = pkgsRiscV.stdenv.mkDerivation {
    name = "kernel";
    src = lib.cleanSource ./kernel;
  };
  emulator = pkgs.stdenv.mkDerivation {
    name = "emulator";
    src = lib.cleanSource ./emulator;
  };
}
