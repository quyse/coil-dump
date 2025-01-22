#include <coil/entrypoint.hpp>
#include <coil/util.hpp>

import coil.core.appidentity;
import coil.core.assets.structs;
import coil.core.assets;
import coil.core.base;
import coil.core.fonts.cache;
import coil.core.fonts.fthb;
import coil.core.fonts;
import coil.core.fs;
import coil.core.graphics;
import coil.core.input;
import coil.core.json;
import coil.core.localization;
import coil.core.math;
import coil.core.platform;
import coil.core.render.fonts;
import coil.core.sdl.vulkan;
import coil.core.sdl;
import coil.core.tasks;
import coil.core.vulkan;

using namespace Coil;

COIL_META_STRUCT(Assets)
{
  COIL_META_STRUCT_FIELD(FontSource*, fontArabic);
  COIL_META_STRUCT_FIELD(FontSource*, fontDevanagari);
  COIL_META_STRUCT_FIELD(FontSource*, fontHan);
  COIL_META_STRUCT_FIELD(FontSource*, fontHebrew);
  COIL_META_STRUCT_FIELD(FontSource*, fontLatinGreekCyrillic);
  COIL_META_STRUCT_FIELD(FontSource*, fontThai);
};

int COIL_ENTRY_POINT(std::vector<std::string> args)
{
  AppIdentity::GetInstance().Name() = "coil_dump_example_render_fonts";

  Book book;

  SdlVulkanSystem::Init();
  auto& windowSystem = book.Allocate<SdlWindowSystem>();

  Window& window = windowSystem.CreateWindow(book, "coil_dump_example_render_fonts", { 1024, 768 });
  window.SetLoopOnlyVisible(true);

  GraphicsSystem& graphicsSystem = VulkanSystem::Create(book, window,
  {
    .render = true,
  });
  GraphicsDevice& graphicsDevice = graphicsSystem.CreateDefaultDevice(book);

  TaskEngine::GetInstance().AddThread();
  AssetManager assetManager =
  {
    FileAssetLoader(),
    FtHbAssetLoader(),
  };
  assetManager.SetJsonContext(JsonFromBuffer(File::MapRead(book, "example_render_fonts.json")));
  Assets<AssetStructAdapter> assets;
  assets.SelfLoad(book, assetManager).Get();

  Book& graphicsBook = graphicsDevice.GetBook();

  GraphicsPool& pool = graphicsDevice.CreatePool(graphicsBook, 16 * 1024 * 1024);

  GraphicsPass* pPass = nullptr;
  FontRenderer fontRenderer(graphicsDevice);
  fontRenderer.Init(graphicsBook, pool);

  std::vector<GraphicsFramebuffer*> pFramebuffers;

  GraphicsPresenter& graphicsPresenter = graphicsDevice.CreateWindowPresenter(book, pool, window,
    [&](GraphicsPresentConfig const& presentConfig, uint32_t imagesCount)
    {
      {
        GraphicsPassConfig passConfig;
        auto colorRef = passConfig.AddAttachment(GraphicsPassConfig::ColorAttachmentConfig
        {
          .format = presentConfig.pixelFormat,
          .clearColor = { 0, 0, 0, 0 },
        });
        colorRef->keepAfter = true;
        auto subPass = passConfig.AddSubPass();
        subPass->UseColorAttachment(colorRef, 0);
        pPass = &graphicsDevice.CreatePass(presentConfig.book, passConfig);
      }

      fontRenderer.InitPipeline(presentConfig.book, presentConfig.size, *pPass, 0);

      pFramebuffers.assign(imagesCount, nullptr);
    },
    [&](GraphicsPresentConfig const& presentConfig, uint32_t imageIndex, GraphicsImage& image)
    {
      GraphicsImage* images[] = { &image };
      pFramebuffers[imageIndex] = &graphicsDevice.CreateFramebuffer(presentConfig.book, *pPass, images, presentConfig.size);
    });

  int32_t const fontSize = 16;

  auto& fontLatinGreekCyrillic = assets.fontLatinGreekCyrillic->CreateFont(book, fontSize);
  auto& fontHan = assets.fontHan->CreateFont(book, fontSize);
  auto& fontArabic = assets.fontArabic->CreateFont(book, fontSize);
  auto& fontHebrew = assets.fontHebrew->CreateFont(book, fontSize);
  auto& fontDevanagari = assets.fontDevanagari->CreateFont(book, fontSize);
  auto& fontThai = assets.fontThai->CreateFont(book, fontSize);

  std::tuple<LanguageInfo const&, char const*, Font&> texts[] =
  {
    {
      Localization<Language::Arabic>::info,
      "يولد جميع الناس أحراراً متساوين في الكرامة والحقوق. وقد وهبوا عقلاً وضميراً وعليهم ان يعامل بعضهم بعضاً بروح اﻹخاء.",
      fontArabic,
    },
    {
      Localization<Language::Belarusian>::info,
      "Усе людзі нараджаюцца свабоднымі і роўнымі ў сваёй годнасці і правах. Яны надзелены розумам і сумленнем і павінны ставіцца адзін да аднаго ў духу брацтва.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Bulgarian>::info,
      "Bсички хора се раждат свободни и равни по достойнство и права. Tе са надарени с разум и съвест и следва да се отнасят помежду си в дух на братство.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::ChineseSimplified>::info,
      "人人生而自由,在尊严和权利上一律平等。他们赋有理性和良心,并应以兄弟关系的精神互相对待。",
      fontHan,
    },
    {
      Localization<Language::ChineseTraditional>::info,
      "人人生而自由﹐在尊嚴和權利上一律平等。他們賦有理性和良心﹐並應以兄弟關係的精神互相對待。",
      fontHan,
    },
    {
      Localization<Language::Czech>::info,
      "Všichni lidé rodí se svobodní a sobě rovní co do důstojnosti a práv. Jsou nadáni rozumem a svědomím a mají spolu jednat v duchu bratrství.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Danish>::info,
      "Alle mennesker er født frie og lige i værdighed og rettigheder. De er udstyret med fornuft og samvittighed, og de bør handle mod hverandre i en broderskabets ånd.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Dutch>::info,
      "Alle mensen worden vrij en gelijk in waardigheid en rechten geboren. Zij zijn begiftigd met verstand en geweten, en behoren zich jegens elkander in een geest van broederschap te gedragen.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::English>::info,
      "All human beings are born free and equal in dignity and rights. They are endowed with reason and conscience and should act towards one another in a spirit of brotherhood.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Finnish>::info,
      "Kaikki ihmiset syntyvät vapaina ja tasavertaisina arvoltaan ja oikeuksiltaan. Heille on annettu järki ja omatunto, ja heidän on toimittava toisiaan kohtaan veljeyden hengessä.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::French>::info,
      "Tous les êtres humains naissent libres et égaux en dignité et en droits. Ils sont doués de raison et de conscience et doivent agir les uns envers les autres dans un esprit de fraternité.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::German>::info,
      "Alle Menschen sind frei und gleich an Würde und Rechten geboren. Sie sind mit Vernunft und Gewissen begabt und sollen einander im Geist der Brüderlichkeit begegnen.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Greek>::info,
      "Όλοι οι άνθρωποι γεννιούνται ελεύθεροι και ίσοι στην αξιοπρέπεια και τα δικαιώματα. Είναι προικισμένοι με λογική και συνείδηση, και οφείλουν να συμπεριφέρονται μεταξύ τους με πνεύμα αδελφοσύνης.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Hebrew>::info,
      "כל בני האדם נולדו בני חורין ושווים בערכם ובזכויותיהם. כולם חוננו בתבונה ובמצפון, לפיכך חובה עליהם לנהוג איש ברעהו ברוח של אחווה.",
      fontHebrew,
    },
    {
      Localization<Language::Hindi>::info,
      "सभी मनुष्यों को गौरव और अधिकारों के मामले में जन्मजात स्वतन्त्रता और समानता प्राप्त है। उन्हें बुद्धि और अन्तरात्मा की देन प्राप्त है और परस्पर उन्हें भाईचारे के भाव से बर्ताव करना चाहिए।",
      fontDevanagari,
    },
    {
      Localization<Language::Hungarian>::info,
      "Minden emberi lény szabadon születik és egyenlő méltósága és joga van. Az emberek, ésszel és lelkiismerettel bírván, egymással szemben testvéri szellemben kell hogy viseltessenek.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Italian>::info,
      "Tutti gli esseri umani nascono liberi ed eguali in dignità e diritti. Essi sono dotati di ragione e di coscienza e devono agire gli uni verso gli altri in spirito di fratellanza.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Japanese>::info,
      "すべての人間は、生まれながらにして自由であり、かつ、尊厳と権利とについて平等である。人間は、理性と良心を授けられてあり、互いに同胞の精神をもって行動しなければならない。",
      fontHan,
    },
    {
      Localization<Language::Kazakh>::info,
      "Барлық адамдар тумысынан азат және қадір-қасиеті мен кұқықтары тең болып дүниеге келеді. Адамдарға ақыл-парасат, ар-ождан берілген, сондықтан олар бір-бірімен туыстық, бауырмалдық қарым-қатынас жасаулары тиіс.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Korean>::info,
      "모든 인간은 태어날 때부터 자유로우며 그 존엄과 권리에 있어 동등하다. 인간은 천부적으로 이성과 양심을 부여받았으며 서로 형제애의 정신으로 행동하여야 한다.",
      fontHan,
    },
    {
      Localization<Language::Persian>::info,
      "همه‌ی افراد بشر آزاد به دنیا می‌آیند و حیثیت و حقوق‌شان با هم برابر است، همه اندیشه و وجدان دارند و باید در برابر یکدیگر با روح برادری رفتار کنند.",
      fontArabic,
    },
    {
      Localization<Language::Polish>::info,
      "Wszyscy ludzie rodzą się wolni i równi pod względem swej godności i swych praw. Są oni obdarzeni rozumem i sumieniem i powinni postępować wobec innych w duchu braterstwa.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Portuguese>::info,
      "Todos os seres humanos nascem livres e iguais em dignidade e em direitos. Dotados de razão e de consciência, devem agir uns para com os outros em espírito de fraternidade.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Russian>::info,
      "Все люди рождаются свободными и равными в своем достоинстве и правах. Они наделены разумом и совестью и должны поступать в отношении друг друга в духе братства.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Serbian>::info,
      "Сва људска бића рађају се слободна и једнака у достојанству и правима. Она су обдарена разумом и свешћу и треба једни према другима да поступају у духу братства.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Spanish>::info,
      "Todos los seres humanos nacen libres e iguales en dignidad y derechos y, dotados como están de razón y conciencia, deben comportarse fraternalmente los unos con los otros.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Swedish>::info,
      "Alla människor äro födda fria och lika i värde och rättigheter. De äro utrustade med förnuft och samvete och böra handla gentemot varandra i en anda av broderskap.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Thai>::info,
      "เราทุกคนเกิดมาอย่างอิสระ เราทุกคนมีความคิดและความเข้าใจเป็นของเราเอง เราทุกคนควรได้รับการปฏิบัติในทางเดียวกัน.",
      fontThai,
    },
    {
      Localization<Language::Turkish>::info,
      "Bütün insanlar hür, haysiyet ve haklar bakımından eşit doğarlar. Akıl ve vicdana sahiptirler ve birbirlerine karşı kardeşlik zihniyeti ile hareket etmelidirler.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Ukrainian>::info,
      "Всі люди народжуються вільними і рівними у своїй гідності та правах. Вони наділені розумом і совістю і повинні діяти у відношенні один до одного в дусі братерства.",
      fontLatinGreekCyrillic,
    },
    {
      Localization<Language::Vietnamese>::info,
      "Tất cả mọi người sinh ra đều được tự do và bình đẳng về nhân phẩm và quyền lợi. Mọi con người đều được tạo hóa ban cho lý trí và lương tâm và cần phải đối xử với nhau trong tình anh em.",
      fontLatinGreekCyrillic,
    },
  };

  int32_t const precision = 4;
  FontGlyphCache cache({ precision, precision });

  InputManager& inputManager = window.GetInputManager();

  FontRenderer::Cache fontRenderCache;

  window.Run([&]()
  {
    auto& inputFrame = inputManager.GetCurrentFrame();
    while(auto const* event = inputFrame.NextEvent())
    {
      std::visit([&]<typename E1>(E1 const& event)
      {
        if constexpr(std::same_as<E1, InputKeyboardEvent>)
        {
          std::visit([&]<typename E2>(E2 const& event)
          {
            if constexpr(std::same_as<E2, InputKeyboardKeyEvent>)
            {
              if(event.isPressed)
              {
                switch(event.key)
                {
                case InputKey::Escape:
                  window.Stop();
                  break;
                default:
                  break;
                }
              }
            }
          }, event);
        }
      }, *event);
    }

    GraphicsFrame& frame = graphicsPresenter.StartFrame();
    fontRenderer.InitFrame(frame.GetContext());
    frame.Pass(*pPass, *pFramebuffers[frame.GetImageIndex()], [&](GraphicsSubPassId subPassId, GraphicsContext& context)
    {
      for(size_t i = 0; i < sizeof(texts) / sizeof(texts[0]); ++i)
      {
        auto const& text = texts[i];
        fontRenderer.Render(fontRenderCache,
          std::get<2>(text), std::get<1>(text), std::get<0>(text), vec2(1, 2 + i * 1.5f) * float(fontSize),
          vec4(1, 1, 1, 1)
        );
      }

      fontRenderCache.Flush(context);
    });
    frame.EndFrame();
  });

  return 0;
}
