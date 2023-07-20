{ pkgs
, coil
}:

rec {
  coil-dump-nixos = (pkgs.callPackage ./coil-dump.nix {
    inherit (coil.core.nixos-pkgs) coil-core;
  }).overrideAttrs (attrs: {
    cmakeFlags = (attrs.cmakeFlags or []) ++ [
      "-DCMAKE_CXX_COMPILER=clang++"
      "-DCMAKE_C_COMPILER=clang"
    ];
    nativeBuildInputs = attrs.nativeBuildInputs ++ [
      coil.core.nixos-pkgs.clang
    ];
  });

  coil-dump-ubuntu = pkgs.vmTools.runInLinuxImage ((pkgs.callPackage ./coil-dump.nix {
    inherit (coil.core.ubuntu-pkgs) coil-core;
  }).overrideAttrs (attrs: {
    cmakeFlags = (attrs.cmakeFlags or []) ++ [
      # force clang
      "-DCMAKE_CXX_COMPILER=clang++-${coil.core.ubuntu-pkgs.clangVersion}"
      "-DCMAKE_C_COMPILER=clang-${coil.core.ubuntu-pkgs.clangVersion}"
    ];
    inherit (coil.core.ubuntu-pkgs) diskImage;
    diskImageFormat = "qcow2";
    memSize = 2048;
  }));

  windows-pkgs = coil.core.windows-pkgs.extend (self: super: with self; {
    coil-dump = mkCmakePkg {
      name = "coil-dump";
      inherit (coil-dump-nixos) src;
      buildInputs = [
        nlohmann_json
        vulkan-headers
        vulkan-loader
        spirv-headers
        zstd
        SDL2
        zlib
        libpng
        sqlite
        freetype
        harfbuzz
        ogg
        opus
        coil-core
      ];
    };
  });
  coil-dump-windows = windows-pkgs.coil-dump;

  assets = let
    font = file: {
      loader = "fthb";
      buffer = {
        loader = "file";
        path = file;
      };
    };
  in {
    example_render_fonts = pkgs.writeText "example_render_fonts.json" (builtins.toJSON {
      fontArabic = font "${coil.stuff.fonts.vazirmatn}/share/fonts/Vazirmatn[wght].ttf";
      fontDevanagari = font "${coil.stuff.fonts.poppins}/share/fonts/Poppins-VariableFont_wght.otf";
      fontHan = font "${coil.stuff.fonts.source_han_sans}/share/fonts/SourceHanSans-VF.otf";
      fontHebrew = font "${coil.stuff.fonts.noto_sans_hebrew}/share/fonts/NotoSansHebrew[wght].ttf";
      fontLatinGreekCyrillic = font "${coil.stuff.fonts.roboto_flex}/share/fonts/RobotoFlex[GRAD,XOPQ,XTRA,YOPQ,YTAS,YTDE,YTFI,YTLC,YTUC,opsz,slnt,wdth,wght].ttf";
      fontThai = font "${coil.stuff.fonts.noto_sans_thai}/share/fonts/NotoSansThai[wght].ttf";
    });
  };

  touch = {
    inherit coil-dump-nixos coil-dump-ubuntu coil-dump-windows;
  } // assets;
}
