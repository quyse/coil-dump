#include <coil/asset.hpp>
#include <coil/asset_struct.hpp>
#include <coil/av1.hpp>
#include <coil/entrypoint.hpp>
#include <coil/fs.hpp>
#include <coil/image_png.hpp>
#include <coil/webm.hpp>

using namespace Coil;

COIL_META_STRUCT(Assets)
{
  COIL_META_STRUCT_FIELD(VideoStreamSource*, video);
};

int COIL_ENTRY_POINT(std::vector<std::string>&& args)
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
