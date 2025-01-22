#include <coil/entrypoint.hpp>
#include <coil/util.hpp>

import coil.core.assets.structs;
import coil.core.assets;
import coil.core.base;
import coil.core.fs;
import coil.core.image.format;
import coil.core.image.png;
import coil.core.json;
import coil.core.media.webm;
import coil.core.tasks;
import coil.core.video.av1;
import coil.core.video;

using namespace Coil;

COIL_META_STRUCT(Assets)
{
  COIL_META_STRUCT_FIELD(VideoStreamSource*, video);
};

int COIL_ENTRY_POINT(std::vector<std::string> args)
{
  Book book;
  TaskEngine::GetInstance().AddThread();

  AssetManager assetManager =
  {
    FileAssetLoader(),
    WebmTrackAssetLoader(),
    Av1AssetLoader(),
  };
  assetManager.SetJsonContext(JsonFromBuffer(File::MapRead(book, "assets/test_webm.json")));
  Assets<AssetStructAdapter> assets;
  assets.SelfLoad(book, assetManager).Get();

  auto& stream = assets.video->CreateStream(book);

  size_t const n = 100;
  for(size_t i = 0; i < n; ++i)
  {
    auto frame = stream.ReadFrame();
    if(!frame) break;
    if(i == n - 1)
    {
      auto image = frame.GetImage();
      Book tmpBook;
      SavePngImage(FileOutputStream::Open(tmpBook, "frame.png"), ImageBuffer
      {
        .format =
        {
          .format = PixelFormats::uintRGB24,
          .width = image.size(0),
          .height = image.size(1),
        },
        .buffer = Buffer(image.pixels, image.pitch(1) * image.size(1) * 3),
      });
    }
  }

  return 0;
}
