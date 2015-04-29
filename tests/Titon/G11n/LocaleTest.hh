<?hh
namespace Titon\G11n;

use Titon\Test\TestCase;
use Titon\Utility\Config;

/**
 * @property \Titon\G11n\Locale $object
 */
class LocaleTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        Config::add('titon.paths.resources', __DIR__);

        $this->object = new Locale('en-us');
    }

    public function testCodeIsCanonicalized(): void {
        $this->assertEquals('en_US', $this->object->getCode());
    }

    public function testBundlesAreAutoInitialized(): void {
        $this->assertInstanceOf('Titon\G11n\LocaleBundle', $this->object->getLocaleBundle());
        $this->assertInstanceOf('Titon\G11n\MessageBundle', $this->object->getMessageBundle());
    }

    public function testParentIsAutoInitialized(): void {
        $this->assertInstanceOf('Titon\G11n\Locale', $this->object->getParentLocale());
        $this->assertEquals('en', $this->object->getParentLocale()->getCode());
    }

    public function testPathsAreInherited(): void {
        $this->assertEquals(Map {
            'core' => Vector { __DIR__ . '/locales/en_US/' }
        }, $this->object->getLocaleBundle()->getPaths());

        $this->assertEquals(Map {
            'core' => Vector { __DIR__ . '/messages/en_US/' }
        }, $this->object->getMessageBundle()->getPaths());
    }

    public function testAddResourcePath(): void {
        $this->object->addResourcePath('foo', __DIR__);

        $this->assertEquals(Map {
            'core' => Vector { __DIR__ . '/locales/en_US/' },
            'foo' => Vector { __DIR__ . '/locales/en_US/' }
        }, $this->object->getLocaleBundle()->getPaths());

        $this->assertEquals(Map {
            'core' => Vector { __DIR__ . '/messages/en_US/' },
            'foo' => Vector { __DIR__ . '/messages/en_US/' }
        }, $this->object->getMessageBundle()->getPaths());
    }

    public function testAddResourcePaths(): void {
        $this->object->addResourcePaths('bar', Vector {
            __DIR__,
            dirname(__DIR__)
        });

        $this->assertEquals(Map {
            'core' => Vector { __DIR__ . '/locales/en_US/' },
            'bar' => Vector { __DIR__ . '/locales/en_US/', dirname(__DIR__) . '/locales/en_US/' }
        }, $this->object->getLocaleBundle()->getPaths());

        $this->assertEquals(Map {
            'core' => Vector { __DIR__ . '/messages/en_US/' },
            'bar' => Vector { __DIR__ . '/messages/en_US/', dirname(__DIR__) . '/messages/en_US/' }
        }, $this->object->getMessageBundle()->getPaths());
    }

    public function testCanonicalize(): void {
        $this->assertEquals('en-us', Locale::canonicalize('en-us', Locale::FORMAT_1));
        $this->assertEquals('en-US', Locale::canonicalize('en-us', Locale::FORMAT_2));
        $this->assertEquals('en_US', Locale::canonicalize('en-us', Locale::FORMAT_3));
        $this->assertEquals('enUS', Locale::canonicalize('en-us', Locale::FORMAT_4));

        $this->assertEquals('en-us', Locale::canonicalize('en-US', Locale::FORMAT_1));
        $this->assertEquals('en-US', Locale::canonicalize('en-US', Locale::FORMAT_2));
        $this->assertEquals('en_US', Locale::canonicalize('en-US', Locale::FORMAT_3));
        $this->assertEquals('enUS', Locale::canonicalize('en-US', Locale::FORMAT_4));

        $this->assertEquals('en-us', Locale::canonicalize('en_US', Locale::FORMAT_1));
        $this->assertEquals('en-US', Locale::canonicalize('en_US', Locale::FORMAT_2));
        $this->assertEquals('en_US', Locale::canonicalize('en_US', Locale::FORMAT_3));
        $this->assertEquals('enUS', Locale::canonicalize('en_US', Locale::FORMAT_4));
    }

    public function testCompose(): void {
        $this->assertEquals('en', Locale::compose(Map {
            'language' => 'en'
        }));

        $this->assertEquals('en_US', Locale::compose(Map {
            'language' => 'en',
            'region' => 'US'
        }));

        $this->assertEquals('en_Hans_US', Locale::compose(Map {
            'language' => 'en',
            'region' => 'US',
            'script' => 'Hans'
        }));

        $this->assertEquals('en_Hans_US_NEDIS_x_prv1', Locale::compose(Map {
            'language' => 'en',
            'region' => 'US',
            'script' => 'Hans',
            'variant0' => 'NEDIS',
            'private0' => 'prv1'
        }));
    }

    public function testDecompose(): void {
        $this->assertEquals(Map {
            'language' => 'en'
        }, Locale::decompose('en'));

        $this->assertEquals(Map {
            'language' => 'en',
            'region' => 'US'
        }, Locale::decompose('en_US'));

        $this->assertEquals(Map {
            'language' => 'en',
            'region' => 'US',
            'script' => 'Hans'
        }, Locale::decompose('en_Hans_US'));

        $this->assertEquals(Map {
            'language' => 'en',
            'script' => 'Hans',
            'region' => 'US',
            'variant0' => 'NEDIS',
            'private0' => 'prv1'
        }, Locale::decompose('en_Hans_US_nedis_x_prv1'));
    }

    public function testGetCode(): void {
        $this->assertEquals('en_US', $this->object->getCode());
        $this->assertEquals('en', $this->object->getParentLocale()->getCode());
    }

    public function testGetParentLocale(): void {
        $this->assertInstanceOf('Titon\G11n\Locale', $this->object->getParentLocale());
        $this->assertEquals(null, $this->object->getParentLocale()->getParentLocale());
    }

}
