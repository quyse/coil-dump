{ stdenv
, cmake
, ninja
, pkg-config
, coil-core
}:

stdenv.mkDerivation {
  name = "coil-dump";
  src = ./src;
  nativeBuildInputs = [
    cmake
    ninja
    pkg-config
  ];
  buildInputs = [
    coil-core
  ];
  doCheck = true;
}
