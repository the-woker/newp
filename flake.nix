{
  description = "C++ project";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/25.11";

  outputs = {
    self,
    nixpkgs,
  }: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
    cross = pkgs.pkgsCross.mingwW64;
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      pname = "newp";
      version = "1.0";
      src = pkgs.lib.fileset.toSource {
        root = ./.;
        fileset = pkgs.lib.fileset.unions [
          ./CMakeLists.txt
          ./src
        ];
      };

      nativeBuildInputs = with pkgs; [
        cmake
      ];
      cmakeFlags = [
        "-DCMAKE_BUILD_TYPE=Release"
      ];
    };

    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        clang-tools
        cmake
        gcc
      ];

      shellHook = ''
        if [ ! -f compile_commands.json ]; then
          cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
          ln -sf build/compile_commands.json .
        fi

        printf "CompileFlags:\n  Add:\n" > .clangd
        g++ -E -x c++ - -v < /dev/null 2>&1 \
          | awk '/#include <...> search starts here:/{flag=1;next}/End of search list./{flag=0}flag' \
          | sed 's/^ *//' \
          | while read -r dir; do
              printf "    - \"-isystem\"\n    - \"%s\"\n" "$dir" >> .clangd
            done
      '';
    };
  };
}
