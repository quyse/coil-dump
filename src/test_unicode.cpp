#include <coil/base_meta.hpp>
#include <iostream>

import coil.core.assets.structs;
import coil.core.assets;
import coil.core.base;
import coil.core.fs;
import coil.core.json;
import coil.core.unicode;

using namespace Coil;

COIL_META_STRUCT(Assets)
{
  COIL_META_STRUCT_FIELD(Buffer, utf8);
  COIL_META_STRUCT_FIELD(Buffer, utf16);
  COIL_META_STRUCT_FIELD(Buffer, utf32);
};

static bool IsBufferEqual(Buffer const& a, Buffer const& b)
{
  std::cout << "comparing buffers: " << a.size << " " << b.size << "\n";
  if(a.size != b.size)
  {
    File::Write("out.txt", a);
    return false;
  }
  int r = memcmp(a.data, b.data, a.size);
  if(r != 0)
  {
    File::Write("out.txt", a);
  }
  return r == 0;
}

int COIL_ENTRY_POINT(std::vector<std::string> args)
{
  Book book;

  AssetManager assetManager =
  {
    FileAssetLoader(),
  };
  assetManager.SetJsonContext(JsonFromBuffer(File::MapRead(book, "assets/test_unicode.json")));
  Assets<AssetStructAdapter> assets;
  assets.SelfLoad(book, assetManager);

  bool globalOk = true;

  auto test = [&]<typename From, typename To>(Buffer const& from, Buffer const& to, char const* name)
  {
    std::vector<To> s;
    Unicode::Convert<From, To>((From const*)from.data, (From const*)((uint8_t const*)from.data + from.size), s);
    if(s[s.size() - 1] == 0) s.pop_back();
    bool ok = IsBufferEqual(s, to);
    globalOk = globalOk && ok;
    std::cout << name << ": " << (ok ? "OK" : "FAIL") << "\n";
  };

  test.operator()<char32_t, char>(assets.utf32, assets.utf8, "UTF32->UTF8");
  test.operator()<char, char32_t>(assets.utf8, assets.utf32, "UTF8->UTF32");
  test.operator()<char32_t, char16_t>(assets.utf32, assets.utf16, "UTF32->UTF16");
  test.operator()<char16_t, char32_t>(assets.utf16, assets.utf32, "UTF16->UTF32");

  return globalOk ? 0 : 1;
}
